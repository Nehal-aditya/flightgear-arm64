// Build a cloud layer based on metar
//
// Written by Harald JOHNSEN, started April 2005.
//
// SPDX-FileCopyrightText: 2005 Harald JOHNSEN <hjohnsen@evc.net>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "config.h"

#include "fgclouds.hxx"

#include <cstring>
#include <cstdio>
#include <Main/fg_props.hxx>

#include <osg/Image>

#include <simgear/constants.h>
#include <simgear/math/sg_geodesy.hxx>
#include <simgear/sound/soundmgr.hxx>
#include <simgear/scene/sky/newcloud.hxx>
#include <simgear/scene/sky/sky.hxx>
//#include <simgear/environment/visual_enviro.hxx>
#include <simgear/scene/sky/cloudfield.hxx>
#include <simgear/scene/util/StateAttributeFactory.hxx>
#include <simgear/structure/commands.hxx>
#include <simgear/props/props_io.hxx>

#include <Main/globals.hxx>
#include <Main/util.hxx>
#include <Viewer/renderer.hxx>
#include <Viewer/view.hxx>
#include <Airports/airport.hxx>
#include <Time/light.hxx>

// RNG seed to ensure cloud synchronization across multi-process
// deployments
static mt seed;

FGClouds::FGClouds() :
    index(0)
{
    update_event = 0;
}

FGClouds::~FGClouds()
{
    globals->get_commands()->removeCommand("add-cloud");
    globals->get_commands()->removeCommand("del-cloud");
    globals->get_commands()->removeCommand("move-cloud");
}

int FGClouds::get_update_event(void) const {
    return update_event;
}

void FGClouds::set_update_event(int count) {
    update_event = count;
    buildCloudLayers();
}

void FGClouds::Init(void)
{
    mt_init_time_10(&seed);

    globals->get_commands()->addCommand("add-cloud", this, &FGClouds::add3DCloud);
    globals->get_commands()->addCommand("del-cloud", this, &FGClouds::delete3DCloud);
    globals->get_commands()->addCommand("move-cloud", this, &FGClouds::move3DCloud);

    // Build and assign the voxel fields
    auto cloudsProp = globals->get_props()->getChild("/sim/rendering/hdr/clouds/");

    _roughFieldWidth        = cloudsProp->getIntValue("rough-voxel-field-width", 128);
    _roughFieldHeight       = cloudsProp->getIntValue("rough-voxel-field-height", 128);
    _roughFieldVoxelSize    = cloudsProp->getIntValue("rough-voxel-size-m", 800);
    _detailedFieldWidth     = cloudsProp->getIntValue("detailed-voxel-field-width", 128);
    _detailedFieldHeight    = cloudsProp->getIntValue("detailed-voxel-field-height", 128);
    _detailedFieldVoxelSize = cloudsProp->getIntValue("detailed-voxel-size-m", 200);

    _fieldDirty = true;
    rebuildField();
}

// Build an individual cloud. Returns the extents of the cloud for coverage calculations
double FGClouds::buildCloud(SGPropertyNode *cloud_def_root, SGPropertyNode *box_def_root,
                            const std::string& name, double grid_z_rand, SGCloudField *layer)
{
    SGPropertyNode* box_def = NULL;
    SGPropertyNode* cld_def = NULL;
    double extent = 0.0;

    SGPath texture_root = globals->get_fg_root();
    texture_root.append("Textures");
    texture_root.append("Sky");

    box_def = box_def_root->getChild(name.c_str());

    string base_name = name.substr(0, 2);
    if (!box_def) {
        if (name[2] == '-') {
            box_def = box_def_root->getChild(base_name.c_str());
        }
        if (!box_def)
            return 0.0;
    }

    float roughFieldWidthM = (float) _roughFieldWidth * (float) _roughFieldVoxelSize;
    double x = mt_rand(&seed) * roughFieldWidthM - (roughFieldWidthM / 2.0);
    double y = mt_rand(&seed) * roughFieldWidthM - (roughFieldWidthM / 2.0);
    double z = grid_z_rand * (mt_rand(&seed) - 0.5);

    float lon = fgGetNode("/position/longitude-deg", false)->getFloatValue();
    float lat = fgGetNode("/position/latitude-deg", false)->getFloatValue();

    SGVec3f pos(x, y, z);

    for (int i = 0; i < box_def->nChildren(); i++) {
        SGPropertyNode* abox = box_def->getChild(i);
        if (abox->getNameString() == "box") {
            string type = abox->getStringValue("type", "cu-small");
            cld_def = cloud_def_root->getChild(type.c_str());
            if (!cld_def) return 0.0;

            double w = abox->getDoubleValue("width", 1000.0);
            double h = abox->getDoubleValue("height", 1000.0);
            int hdist = abox->getIntValue("hdist", 1);
            int vdist = abox->getIntValue("vdist", 1);

            double c = abox->getDoubleValue("count", 5);
            int count = (int)(c + (mt_rand(&seed) - 0.5) * c);

            extent = std::max(w * w, extent);

            for (int j = 0; j < count; j++) {
                // Locate the clouds randomly in the defined space. The hdist and
                // vdist values control the horizontal and vertical distribution
                // by simply summing random components.
                double x = 0.0;
                double y = 0.0;
                double z = 0.0;

                for (int k = 0; k < hdist; k++) {
                    x += (mt_rand(&seed) / hdist);
                    y += (mt_rand(&seed) / hdist);
                }

                for (int k = 0; k < vdist; k++) {
                    z += (mt_rand(&seed) / vdist);
                }

                x = w * (x - 0.5) + pos[0]; // N/S
                y = w * (y - 0.5) + pos[1]; // E/W
                z = h * z + pos[2];         // Up/Down. pos[2] is the cloudbase

                SGNewCloud cld(cld_def, &seed);
                addCloud(cld, index++, lon, lat, z, x, y);
            }
        }
    }

    // Return the maximum extent of the cloud
    return extent;
}

void FGClouds::buildLayer(int iLayer, const string& name, double coverage) {
    struct {
        string name;
        double count;
    } tCloudVariety[20];
    int CloudVarietyCount = 0;
    double totalCount = 0.0;

    SGSky* thesky = globals->get_renderer()->getSky();

    SGPropertyNode* cloud_def_root = fgGetNode("/environment/cloudlayers/clouds", false);
    SGPropertyNode* box_def_root   = fgGetNode("/environment/cloudlayers/boxes", false);
    SGPropertyNode* layer_def_root = fgGetNode("/environment/cloudlayers/layers", false);
    SGCloudField* layer = thesky->get_cloud_layer(iLayer)->get_layer3D();
    layer->clear();

    // If we don't have the required properties, then render the cloud in 2D
    if (coverage == 0.0 || layer_def_root == NULL || cloud_def_root == NULL || box_def_root == NULL) {
        thesky->get_cloud_layer(iLayer)->set_enable3dClouds(false);
        return;
    }

    // If we can't find a definition for this cloud type, then render the cloud in 2D
    SGPropertyNode* layer_def = NULL;
    layer_def = layer_def_root->getChild(name.c_str());
    if (!layer_def) {
        if (name[2] == '-') {
            string base_name = name.substr(0, 2);
            layer_def = layer_def_root->getChild(base_name.c_str());
        }
        if (!layer_def) {
            thesky->get_cloud_layer(iLayer)->set_enable3dClouds(false);
            return;
        }
    }

    // At this point, we know we've got some 3D clouds to generate.
    thesky->get_cloud_layer(iLayer)->set_enable3dClouds(true);

    double grid_z_rand = layer_def->getDoubleValue("grid-z-rand");

    for (int i = 0; i < layer_def->nChildren(); i++) {
        SGPropertyNode* acloud = layer_def->getChild(i);
        if (acloud->getNameString() == "cloud") {
            string cloud_name = acloud->getStringValue("name");
            tCloudVariety[CloudVarietyCount].name = cloud_name;
            double count = acloud->getDoubleValue("count", 1.0);
            tCloudVariety[CloudVarietyCount].count = count;
            int variety = 0;
            char variety_name[50];
            do {
                variety++;
                snprintf(variety_name, sizeof(variety_name) - 1, "%s-%d", cloud_name.c_str(), variety);
            } while (box_def_root->getChild(variety_name, 0, false));

            totalCount += count;
            if (CloudVarietyCount < 20)
                CloudVarietyCount++;
        }
    }
    totalCount = 1.0 / totalCount;

    // Determine how much cloud coverage we need in m^2.
    float roughFieldWidthM = (float) _roughFieldWidth * (float) _roughFieldVoxelSize;
    double cov = coverage * roughFieldWidthM * roughFieldWidthM;

    while (cov > 0.0f) {
        double choice = mt_rand(&seed);

        for (int i = 0; i < CloudVarietyCount; i++) {
            choice -= tCloudVariety[i].count * totalCount;
            if (choice <= 0.0) {
                cov -= buildCloud(cloud_def_root,
                                  box_def_root,
                                  tCloudVariety[i].name,
                                  grid_z_rand,
                                  layer);
                break;
            }
        }
    }
}

void FGClouds::buildCloudLayers(void) {
    SGPropertyNode* metar_root = fgGetNode("/environment", true);

    //double wind_speed_kt	 = metar_root->getDoubleValue("wind-speed-kt");
    double temperature_degc = metar_root->getDoubleValue("temperature-sea-level-degc");
    double dewpoint_degc = metar_root->getDoubleValue("dewpoint-sea-level-degc");
    double pressure_mb = metar_root->getDoubleValue("pressure-sea-level-inhg") * SG_INHG_TO_PA / 100.0;
    double rel_humidity = metar_root->getDoubleValue("relative-humidity");

    // formula d'Epsy, base d'un cumulus
    double cumulus_base = 122.0 * (temperature_degc - dewpoint_degc);
    double stratus_base = 100.0 * (100.0 - rel_humidity) * SG_FEET_TO_METER;

    SGSky* thesky = globals->get_renderer()->getSky();
    for (int iLayer = 0; iLayer < thesky->get_cloud_layer_count(); iLayer++) {
        SGPropertyNode* cloud_root = fgGetNode("/environment/clouds/layer", iLayer, true);

        double alt_ft = cloud_root->getDoubleValue("elevation-ft");
        double alt_m = alt_ft * SG_FEET_TO_METER;
        string coverage = cloud_root->getStringValue("coverage");

        double coverage_norm = 0.0;
        if (coverage == "few")
            coverage_norm = 2.0 / 8.0; // <1-2
        else if (coverage == "scattered")
            coverage_norm = 4.0 / 8.0; // 3-4
        else if (coverage == "broken")
            coverage_norm = 6.0 / 8.0; // 5-7
        else if (coverage == "overcast")
            coverage_norm = 8.0 / 8.0; // 8

        string layer_type = "nn";

        if (coverage == "cirrus") {
            layer_type = "ci";
        } else if (alt_ft > 16500) {
            //			layer_type = "ci|cs|cc";
            layer_type = "ci";
        } else if (alt_ft > 6500) {
            //			layer_type = "as|ac|ns";
            layer_type = "ac";
            if (pressure_mb < 1005.0 && coverage_norm >= 0.5)
                layer_type = "ns";
        } else {
            //			layer_type = "st|cu|cb|sc";
            if (cumulus_base * 0.80 < alt_m && cumulus_base * 1.20 > alt_m) {
                // +/- 20% from cumulus probable base
                layer_type = "cu";
            } else if (stratus_base * 0.80 < alt_m && stratus_base * 1.40 > alt_m) {
                // +/- 20% from stratus probable base
                layer_type = "st";
            } else {
                // above formulae is far from perfect
                if (alt_ft < 2000)
                    layer_type = "st";
                else if (alt_ft < 4500)
                    layer_type = "cu";
                else
                    layer_type = "sc";
            }
        }

        cloud_root->setStringValue("layer-type", layer_type);
        buildLayer(iLayer, layer_type, coverage_norm);
    }

    rebuildField();
}

/**
 * Adds a 3D cloud to a cloud layer.
 *
 * Property arguments
 * layer - the layer index to add this cloud to. (Defaults to 0)
 * index - the index for this cloud (to be used later)
 * lon/lat/alt - the position for the cloud
 * (Various) - cloud definition properties. See README.3DClouds
 *
 */
 bool FGClouds::add3DCloud(const SGPropertyNode *arg, SGPropertyNode * root)
 {
   int index = arg->getIntValue("index", 0);
   float lon = arg->getFloatValue("lon-deg", 0.0f);
   float lat = arg->getFloatValue("lat-deg", 0.0f);
   float alt = arg->getFloatValue("alt-ft", 0.0f);
   float x = arg->getFloatValue("x-offset-m", 0.0f);
   float y = arg->getFloatValue("y-offset-m", 0.0f);

   SGNewCloud cld(arg, &seed);
   bool success = addCloud(cld, index, lon, lat, alt, x, y);
   return success;
 }

 /**
  * Removes a 3D cloud from a cloud layer
  *
  * Property arguments
  *
  * layer - the layer index to remove this cloud from. (defaults to 0)
  * index - the cloud index
  *
  */
 bool FGClouds::delete3DCloud(const SGPropertyNode *arg, SGPropertyNode * root)
 {
   int i = arg->getIntValue("index", 0);
   return removeCloud(i);
 }

/**
 * Move a cloud within a 3D layer
 *
 * Property arguments
 * layer - the layer index to add this cloud to. (Defaults to 0)
 * index - the cloud index to move.
 * lon/lat/alt - the position for the cloud
 *
 */
bool FGClouds::move3DCloud(const SGPropertyNode *arg, SGPropertyNode * root)
 {
    int i = arg->getIntValue("index", 0);
    float lon = arg->getFloatValue("lon-deg", 0.0f);
    float lat = arg->getFloatValue("lat-deg", 0.0f);
    float alt = arg->getFloatValue("alt-ft", 0.0f);
    float x = arg->getFloatValue("x-offset-m", 0.0f);
    float y = arg->getFloatValue("y-offset-m", 0.0f);
    return repositionCloud(i, lon, lat, alt, x, y);
 }

 bool FGClouds::addCloud(SGNewCloud cloud, int index, float lon, float lat, float alt) {
  return addCloud(cloud, index, lon, lat, alt, 0.0f, 0.0f);
}

bool FGClouds::addCloud(SGNewCloud cloud, int index, float lon, float lat, float alt, float x, float y) {
    SGGeod loc = SGGeod::fromDegFt(lon, lat, alt);
    return addCloud(cloud, index, loc, x, y);
}

bool FGClouds::removeCloud(int index)
{
    if (_cloudPlacementMap.erase(index) > 0) {
        _fieldDirty = true;
        return true;
    } else {
        return false;
    }
}

bool FGClouds::addCloud(SGNewCloud cloud, int index, SGGeod loc, float x, float y) {

    // If this cloud index already exists, don't replace it.
    if (_cloudPlacementMap.contains(index)) return false;

    float alt = loc.getElevationFt();
    // Determine any shift by x/y
    if ((x != 0.0f) || (y != 0.0f)) {
        double crs = 90.0 - SG_RADIANS_TO_DEGREES * atan2(y, x);
        double dst = sqrt(x*x + y*y);
        double endcrs;

        SGGeod base_pos = SGGeod::fromGeodFt(loc, 0.0f);
        SGGeodesy::direct(base_pos, crs, dst, loc, endcrs);
    }

    // The direct call provides the position at 0 alt, so adjust as required.
    loc.setElevationFt(alt);

    // Work out where this cloud should go in OSG coordinates.
    SGVec3<double> cart;
    SGGeodesy::SGGeodToCart(loc, cart);
    osg::Vec3f pos = toOsg(cart);
    CloudPlacement cl = std::make_tuple(cloud, pos);
    _cloudPlacementMap.insert({index, cl});
    _fieldDirty = true;

    return true;
}

bool FGClouds::repositionCloud(int index, float lon, float lat, float alt) {
    return repositionCloud(index, lon, lat, alt, 0.0f, 0.0f);
}

bool FGClouds::repositionCloud(int index, float lon, float lat, float alt, float x, float y) {

    if (_cloudPlacementMap.contains(index)) {
        CloudPlacement cp = _cloudPlacementMap.at(index);
        SGNewCloud c = std::get<0>(cp);
        removeCloud(index);
        addCloud(c, index, lon, lat, alt, x, y);
        return true;
    } else {
        return false;
    }
}

// Build the cloud field centered on the current location.
void FGClouds::rebuildField() {

    if (!_fieldDirty) return;

    // Save off the current location, which will be used in transforms.
    // We will determine the altitude later, so make sure it's 0 for
    // the various coversions between ECF and local coordinates.
    SGGeod geod = globals->get_view_position();
    geod.setElevationM(0);

    SGGeodesy::SGGeodToCart(geod, _centerCart);    
    _cloudPosMatrix = makeZUpFrameRelative(globals->get_view_position());
    SG_LOG(SG_GENERAL, SG_ALERT, "Rebuilding field at " << geod.getLatitudeDeg() << " " << geod.getLongitudeDeg());

    fgSetDouble("/sim/rendering/hdr/clouds/cloud-center-x",  _centerCart.x());
    fgSetDouble("/sim/rendering/hdr/clouds/cloud-center-y",  _centerCart.y());
    fgSetDouble("/sim/rendering/hdr/clouds/cloud-center-z",  _centerCart.z());

    if (_cloudPlacementMap.empty()) {
        // Nothing to generate.
        //simgear::StateAttributeFactory::instance()->setCloudVoxelImage(roughVoxelData);
        SG_LOG(SG_GENERAL, SG_ALERT, "rebuildField - No cloud data to build");
        _fieldDirty = false;
        return;
    }

    osg::ref_ptr<osg::Image> roughVoxelData = new osg::Image();
    roughVoxelData->allocateImage(_roughFieldWidth, _roughFieldWidth, _roughFieldHeight, GL_RGBA, GL_FLOAT);

    osg::ref_ptr<osg::Image> detailedVoxelData = new osg::Image();
    detailedVoxelData->allocateImage(_detailedFieldWidth, _detailedFieldWidth, _detailedFieldHeight, GL_RGBA, GL_FLOAT);

    osg::ref_ptr<osg::Image> voxelShadeData = new osg::Image();
    voxelShadeData->allocateImage(_roughFieldWidth, _roughFieldWidth, _roughFieldWidth, GL_RGB, GL_FLOAT);

    std::vector<CloudPlacement> roughFieldList;
    std::vector<CloudPlacement> detailedFieldList;

    for (const auto& [key, value] : _cloudPlacementMap) {
        const CloudPlacement cl = value;

        const SGNewCloud c = std::get<0>(cl);
        osg::Vec3f p = std::get<1>(cl) - toOsg(_centerCart);
        // Transform to Z-up coordinates
        p = _cloudPosMatrix * p;

        // Now check if any part is within the X/Y bounds for each of the voxelMaps.
        if ((abs(p.x()) - c._maxWidth < (float) _roughFieldWidth / 2) && (abs(p.y()) - c._maxWidth < (float)_roughFieldWidth / 2)) {
            // Local coordinate cloud placement
            CloudPlacement localCloud = std::make_tuple(c, p);
            if ((abs(p.x()) - c._maxWidth < (float) _detailedFieldWidth / 2) && (abs(p.y()) - c._maxWidth < (float) _detailedFieldWidth / 2)) {
                detailedFieldList.push_back(localCloud);
            }

            roughFieldList.push_back(localCloud);
        }
    }

    if (roughFieldList.empty() && detailedFieldList.empty()) {
        // Nothing to display, so clean up and return early.
        simgear::StateAttributeFactory::instance()->setCloudVoxelImage(detailedVoxelData, voxelShadeData);
        SG_LOG(SG_GENERAL, SG_ALERT, "rebuildField - No cloud data in range");
        _fieldDirty = false;
        return;
    }

    // Now generate the voxelMaps suitable for these bounds.  Note that the voxel map is symmetrical on X

    // The alpha value is use for a Signed Distance Field, and indicates the maximum distance that can be travelled
    // before hitting something in UV coordinates.  We default to 1 pixel, but have to take into account that our
    // voxel space isn't a cube.  This makes it more conservative than it needs to be.
    const float roughSDFMin = 1.0 / (float) std::max(_roughFieldHeight, _roughFieldWidth);

    for (unsigned int k = 0U; k < _roughFieldHeight; ++k) {
        for (unsigned int j = 0U; j < _roughFieldWidth; ++j) {
            for (unsigned int i = 0U; i < _roughFieldWidth; ++i) {
                roughVoxelData->setColor(osg::Vec4f(0.0f,0.0f,0.0f,roughSDFMin), i,j,k);
            }
        }
    }

    const float detailedSDFMin = 1.0 / (float) std::max(_detailedFieldHeight, _detailedFieldWidth);

    for (unsigned int k = 0U; k < _detailedFieldHeight; ++k) {
        for (unsigned int j = 0U; j < _detailedFieldWidth; ++j) {
            for (unsigned int i = 0U; i < _detailedFieldWidth; ++i) {
                detailedVoxelData->setColor(osg::Vec4f(0.0f,0.0f,0.0f,detailedSDFMin), i,j,k);
            }
        }
    }

    SGVec3f clouds[6] = {
        SGVec3f(float(_detailedFieldWidth * 2 / 8), float(_detailedFieldWidth * 2 / 8), 24),
        SGVec3f(float(_detailedFieldWidth * 6 / 8), float(_detailedFieldWidth * 2 / 8), 24),
        SGVec3f(float(_detailedFieldWidth * 2 / 8), float(_detailedFieldWidth * 6 / 8), 24),
        SGVec3f(float(_detailedFieldWidth * 6 / 8), float(_detailedFieldWidth * 6 / 8), 24),
        SGVec3f(float(_detailedFieldWidth * 4 / 8), float(_detailedFieldWidth * 4 / 8), 10),
        SGVec3f(float(_detailedFieldWidth * 4 / 8), float(_detailedFieldWidth * 4 / 8), 50)
    };

    float cloudHeight = fgGetDouble("/sim/rendering/hdr/clouds/debug/height", 6.0) * 0.5;
    float cloudWidth = fgGetDouble("/sim/rendering/hdr/clouds/debug/width", 6.0) * 0.5;
    float cloudBottomType = fgGetDouble("/sim/rendering/hdr/clouds/debug/bottom-type", 1.0);
    float cloudTopType = fgGetDouble("/sim/rendering/hdr/clouds/debug/bottom-type", 1.0);
    float cloudDensity = fgGetDouble("/sim/rendering/hdr/clouds/debug/density", 1.0);
    float erosion = fgGetDouble("/sim/rendering/hdr/clouds/debug/erosion", 0.0);

    mt seed;
    mt_init(&seed, 123);

    for (unsigned int j = 0; j < _detailedFieldWidth; ++j) {
        for (unsigned int i = 0; i < _detailedFieldWidth; ++i) {
            for (unsigned int k = 0; k < _detailedFieldHeight; ++k) {
                for (SGVec3f cloudCentre : clouds) {
                    SGVec3f p = SGVec3f(i,j,k) - cloudCentre;
                    p.z() = p.z() * cloudWidth / cloudHeight;

                    float erode = erosion * float(mt_rand(&seed));
                    float dist = length(p);
                    float depth = p.z() / cloudHeight;
                    float cloudType = depth < -0.5 ? cloudBottomType : cloudTopType;

                    // Erode randomly by making the distance greater than calculated and therefore perhaps outside of the spheriod
                    if (dist + erode < cloudWidth) {
                        float cloudDimension = 1.0 - (dist / std::max(cloudHeight, cloudWidth));
                        detailedVoxelData->setColor(osg::Vec4f(cloudDimension,cloudType,cloudDensity,-roughSDFMin), i,j,k);
                    }
                }
            }
        }
    }

    // Now build the shade image.  The R channel is the summed density towards the Sun.  The G channel the summed vertical density.
    // We just do a single image covering both voxel spaces.
    osg::ref_ptr<osg::Image> shadeVoxelData = new osg::Image();
    shadeVoxelData->allocateImage(_detailedFieldWidth, _detailedFieldWidth, _detailedFieldHeight, GL_RGBA, GL_FLOAT);

    // Get the Sun direction and transform into the Z-up X-north coordinates
    auto l = globals->get_subsystem<FGLight>();
    const osg::Vec4f sunDirection(l->sun_vec_inv()[0], l->sun_vec_inv()[1], l->sun_vec_inv()[2], 0.0);

    const SGGeod cameraPosGeod = globals->get_current_view()->getPosition();
    const osg::Matrixf cameraZUp = makeZUpFrameRelative(cameraPosGeod);
    osg::Vec4f s = cameraZUp * (- sunDirection);
    s.normalize();
    const osg::Vec3f sunDirZUp(s.x() / _detailedFieldWidth, s.y() / _detailedFieldWidth, s.z() / _detailedFieldHeight);

    SG_LOG(SG_GENERAL, SG_DEBUG, "Sun Direction Z-Up: " << sunDirZUp.x() << ", " << sunDirZUp.y() << ", " << sunDirZUp.z());

    // Build up the shadow space.  By starting from the top we can make some efficiencies by using previously calculated values
    // from further up the stack.
    for (unsigned int k = _detailedFieldHeight - 1; k > 0; --k) {
        for (unsigned int i = 0; i < _detailedFieldWidth; ++i) {
            for (unsigned int j = 0; j < _detailedFieldWidth; ++j) {
                const osg::Vec3f start((float) i / _detailedFieldWidth, (float) j / _detailedFieldWidth, (float) k / _detailedFieldHeight);
                float d = 1.0f;
                float sunDensity = 0.0f;

                osg::Vec3f p = start + sunDirZUp * d;
                while (p.x() > 0.0f && p.x() < 1.0f && 
                       p.y() > 0.0f && p.y() < 1.0f && 
                       p.z() > 0.0f && p.z() < 1.0f    ) {                        

                    if (p.z() > (float) (k + 1) / _detailedFieldHeight) {
                        // Use the pre-calculated for the voxel above
                        sunDensity += shadeVoxelData->getColor(p).r();
                        break;
                    } else {
                        sunDensity += detailedVoxelData->getColor(p).z();
                        d += 1.0f;
                        p = start + sunDirZUp * d;
                    }
                }

                d = 1.0;
                float verticalDensity = 0.0;

                p = start + osg::Vec3f(0.0, 0.0, 1.0f / _detailedFieldHeight) * d;
                while (p.x() > 0.0 && p.x() < 1.0 && 
                       p.y() > 0.0 && p.y() < 1.0 && 
                       p.z() > 0.0 && p.z() < 1.0    ) {

                    if (p.z() > (float) (k + 1) / _detailedFieldHeight) {
                        // Use the pre-calculated for the voxel above
                        verticalDensity += shadeVoxelData->getColor(p).g();
                        break;
                    } else {
                        verticalDensity += detailedVoxelData->getColor(p).z();
                        d += 1.0;
                        p = start + osg::Vec3f(0.0,0.0,1.0f / _detailedFieldHeight) * d;
                    }
                }

                shadeVoxelData->setColor(osg::Vec4f(sunDensity, verticalDensity, 0.0f, 0.0f), i, j, k);
            }
        }
    }




    simgear::StateAttributeFactory::instance()->setCloudVoxelImage(detailedVoxelData, shadeVoxelData);
    _fieldDirty = false;
}