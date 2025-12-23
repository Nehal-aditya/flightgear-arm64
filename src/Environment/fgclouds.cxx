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

#include <fast_marching_method.hpp>
#include <Main/globals.hxx>
#include <Main/util.hxx>
#include <Viewer/renderer.hxx>
#include <Viewer/view.hxx>
#include <Airports/airport.hxx>
#include <Time/light.hxx>

namespace fmm = thinks::fast_marching_method;

// RNG seed to ensure cloud synchronization across multi-process
// deployments
static mt seed;

FGClouds::FGClouds() :
    index(0)
{
    update_event = 0;
    _options = new simgear::SGReaderWriterOptions;
    _options->setObjectCacheHint(osgDB::Options::CACHE_ALL);
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
                            const std::string& name, double altFt, double grid_z_rand, SGCloudField *layer)
{
    SGPropertyNode* box_def = NULL;
    SGPropertyNode* cld_def = NULL;
    float extent = 0.0;

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

    // Note that these are all in metres
    float x = mt_rand(&seed) * roughFieldWidthM - (roughFieldWidthM / 2.0);
    float y = mt_rand(&seed) * roughFieldWidthM - (roughFieldWidthM / 2.0);
    float z = (float) altFt * SG_FEET_TO_METER + (float) grid_z_rand * (mt_rand(&seed) - 0.5);
    SGVec3f pos(x, y, z);

    float lon = fgGetNode("/position/longitude-deg", false)->getFloatValue();
    float lat = fgGetNode("/position/latitude-deg", false)->getFloatValue();

    for (int i = 0; i < box_def->nChildren(); i++) {
        SGPropertyNode* abox = box_def->getChild(i);
        if (abox->getNameString() == "box") {
            string type = abox->getStringValue("type", "cu-small");
            cld_def = cloud_def_root->getChild(type.c_str());
            if (!cld_def) return 0.0;

            float w = abox->getFloatValue("width", 1000.0);
            float h = abox->getFloatValue("height", 1000.0);
            int hdist = abox->getIntValue("hdist", 1);
            int vdist = abox->getIntValue("vdist", 1);

            float c = abox->getFloatValue("count", 5);
            int count = (int)(c + (mt_rand(&seed) - 0.5) * c);

            extent = std::max(w * w, extent);

            for (int j = 0; j < count; j++) {
                // Locate the clouds randomly in the defined space. The hdist and
                // vdist values control the horizontal and vertical distribution
                // by simply summing random components.
                float x = 0.0;
                float y = 0.0;
                float z = 0.0;

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
                addCloud(cld, index++, lon, lat, z * SG_METER_TO_FEET, x, y);
            }
        }
    }

    // Return the maximum extent of the cloud
    return extent;
}

void FGClouds::buildLayer(int iLayer, const string& name, double coverage, double altFt) {
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
                                  altFt,
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
        buildLayer(iLayer, layer_type, coverage_norm, alt_ft);
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

 bool FGClouds::addCloud(SGNewCloud cloud, int index, float lon, float lat, float altFt) {
  return addCloud(cloud, index, lon, lat, altFt, 0.0f, 0.0f);
}

bool FGClouds::addCloud(SGNewCloud cloud, int index, float lon, float lat, float altFt, float x, float y) {
    SGGeod loc = SGGeod::fromDegFt(lon, lat, altFt);
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
    _cloudPosMatrix = makeZUpFrameRelative(geod);
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
    voxelShadeData->allocateImage(_detailedFieldWidth, _detailedFieldWidth, _detailedFieldHeight, GL_RGBA, GL_FLOAT);

    std::vector<CloudPlacement> roughFieldList;
    std::vector<CloudPlacement> detailedFieldList;

    for (const auto& [key, value] : _cloudPlacementMap) {
        const CloudPlacement cl = value;
        const SGNewCloud c = std::get<0>(cl);
        // Transform to Z-up coordinates
        osg::Vec3f q = (std::get<1>(cl) - toOsg(_centerCart));
        osg::Vec3f p = _cloudPosMatrix * q;

        // Now check if any part is within the X/Y bounds for each of the voxelMaps.
        if (p.x() > -getRoughFieldRadiusM() && p.x() < getRoughFieldRadiusM() && 
            p.y() > -getRoughFieldRadiusM() && p.y() < getRoughFieldRadiusM()) {
            // Local coordinate cloud placement

            //SG_LOG(SG_GENERAL, SG_ALERT, "Adding rough cloud at " << p.x() << " " << p.y() << " " << p.z() << " d: " << p.length());
            CloudPlacement localCloud = std::make_tuple(c, p);
            roughFieldList.push_back(localCloud);

            if (p.x() > -getDetailedFieldRadiusM() && p.x() < getDetailedFieldRadiusM() && 
                p.y() > -getDetailedFieldRadiusM() && p.y() < getDetailedFieldRadiusM()) {
                SG_LOG(SG_GENERAL, SG_ALERT, "Adding detailed cloud at " << p.x() << " " << p.y() << " " << p.z() << " d: " << p.length());
                detailedFieldList.push_back(localCloud);
            }
        }
    }

    if (detailedFieldList.empty()) {
        // Nothing to display, so clean up and return early.
        simgear::StateAttributeFactory::instance()->setCloudVoxelImage(detailedVoxelData, voxelShadeData);
        SG_LOG(SG_GENERAL, SG_ALERT, "rebuildField - No cloud data in range");
        _fieldDirty = false;
        return;
    }

    // The alpha value is use for a Signed Distance Field, and indicates the maximum distance that can be travelled
    // before hitting something in UV coordinates.  We default to 1 pixel.
    const float roughSDFMin = 1.0 / (float) std::max(_roughFieldHeight, _roughFieldWidth);
    const float detailedSDFMin = 1.0 / (float) std::max(_detailedFieldHeight, _detailedFieldWidth);

    for (size_t k = 0U; k < _roughFieldHeight; ++k) {
        for (size_t j = 0U; j < _roughFieldWidth; ++j) {
            for (size_t i = 0U; i < _roughFieldWidth; ++i) {
                roughVoxelData->setColor(osg::Vec4f(0.0f,0.0f,0.0f,roughSDFMin), i,j,k);
            }
        }
    }

    for (size_t k = 0U; k < _detailedFieldHeight; ++k) {
        for (size_t j = 0U; j < _detailedFieldWidth; ++j) {
            for (size_t i = 0U; i < _detailedFieldWidth; ++i) {
                detailedVoxelData->setColor(osg::Vec4f(0.0f,0.0f,0.0f,detailedSDFMin), i,j,k);
            }
        }
    }

    // Now write the detailed clouds into the voxel space.
    for (auto cl  : detailedFieldList) {
        SGNewCloud c = std::get<0>(cl);
        osg::Vec3f p = std::get<1>(cl);

        osg::ref_ptr<osg::Image> cloudVoxels = c.getCloud(_options);

        // Now determine where to place the origin in the voxel space.
        int x = (int) (p.x() + getDetailedFieldRadiusM()) / (int) _detailedFieldVoxelSize - cloudVoxels->s() / 2;
        int y = (int) (p.y() + getDetailedFieldRadiusM()) / (int) _detailedFieldVoxelSize - cloudVoxels->t() / 2;
        int z = (int) (p.z()) / (int) _detailedFieldVoxelSize - cloudVoxels->r() / 2;

        int source_x = 0;
        int source_y = 0;
        int source_z = 0;
        int w = cloudVoxels->s();
        int d = cloudVoxels->t();
        int h = cloudVoxels->r();

        // If this cloud falls outside the edges of the voxel space, then resize the area to be copied appropriately
        if (x < 0) { source_x = w + x; w = w - source_x; x = 0; }
        if (y < 0) { source_y = d + y; d = d - source_y; y = 0; }
        if (z < 0) { source_z = h + z; h = h - source_z; z = 0; }

        if (x + w > (int) _detailedFieldWidth)  { w = (int) _detailedFieldWidth - x; }
        if (y + d > (int) _detailedFieldWidth)  { d = (int) _detailedFieldWidth - y; }
        if (z + h > (int) _detailedFieldHeight) { h = (int) _detailedFieldHeight - z; }

        for (int k = source_z; k < h; ++k) {
            for (int j = source_y; j < d; ++j) {
                for (int i = source_x; i < w; ++i) {
                    if ((i + x) < detailedVoxelData->s() && (j + y) < detailedVoxelData->t() && (k + z) < detailedVoxelData->r()) {
                        const osg::Vec4f cloudV = cloudVoxels->getColor(i, j, k);
                        const osg::Vec4f currentV = detailedVoxelData->getColor(i + x, j + y, k + z);
                        const osg::Vec4f newV = std::max(cloudV, currentV);
                        detailedVoxelData->setColor(newV, i + x, j + y, k + z);
                    }
                }
            }
        }
    }

    // Build the SDF from the voxel data.
    vector<std::array<int, 3>> cloudBoundaryIndices;
    vector<float> cloudBoundaryDistances;

    for (size_t j = 0; j < _detailedFieldWidth; ++j) {
        for (size_t i = 0; i < _detailedFieldWidth; ++i) {
            for (size_t k = 0; k < _detailedFieldHeight; ++k) {

                if (detailedVoxelData->getColor(i,j,k).b() > 0.0f) {
                    cloudBoundaryIndices.push_back(std::array<int, 3>{{(int)i, (int)j,(int)k}});
                    //cloudBoundaryDistances.push_back(- detailedVoxelData->getColor(i,j,k).r());
                    cloudBoundaryDistances.push_back(0.0f);
                }
            }
        }
    }

    if (cloudBoundaryDistances.empty()) {
        // This is an error condition 
        SG_LOG(SG_GENERAL, SG_ALERT, "No clouds in detailedVoxelData.");
        return;   
    }

    auto gridSize = std::array<size_t, 3>{{_detailedFieldWidth, _detailedFieldWidth, _detailedFieldHeight}};
    auto gridSpacing = std::array<float, 3>{{1.f/_detailedFieldWidth, 1.f/_detailedFieldWidth, 1.f/_detailedFieldHeight}};
    auto uniformSpeed = 1.f;

    SG_LOG(SG_GENERAL, SG_ALERT, "SDF calculation started.");

    auto sdf = fmm::SignedArrivalTime(
        gridSize,
        cloudBoundaryIndices,
        cloudBoundaryDistances,
        fmm::UniformSpeedEikonalSolver<float, 3>(gridSpacing, uniformSpeed));

        
    // The SDF is now calculated, so write it back to the voxel data.
    std::size_t idx = 0;
    for (std::size_t k = 0; k < _detailedFieldHeight; ++k) {
        for (std::size_t j = 0; j < _detailedFieldWidth; ++j) {
            for (std::size_t i = 0; i < _detailedFieldWidth; ++i) {
                float distance = sdf[idx++];
                osg::Vec4f c = detailedVoxelData->getColor(i,j,k);
                if (c[3] > 0.0) {
                    c[3] = distance;
                    detailedVoxelData->setColor(c, i,j,k);
                }
            }
        }
    }  

    SG_LOG(SG_GENERAL, SG_ALERT, "SDF calculation complete.");

    // Now build the shade image.  The R channel is the summed density towards the Sun.  The G channel the summed vertical density.
    // We just do a single image covering both voxel spaces.

    // Get the Sun direction and transform into the Z-up X-north coordinates
    auto l = globals->get_subsystem<FGLight>();
    const osg::Vec4f sunDirection(l->sun_vec_inv()[0], l->sun_vec_inv()[1], l->sun_vec_inv()[2], 0.0);

    const SGGeod cameraPosGeod = globals->get_current_view()->getPosition();
    const osg::Matrixf cameraZUp = makeZUpFrameRelative(cameraPosGeod);
    osg::Vec4f s = cameraZUp * (-sunDirection);
    s.normalize();
    const osg::Vec3f sunDirZUp(s.x() / _detailedFieldWidth, s.y() / _detailedFieldWidth, s.z() / _detailedFieldHeight);
    //const osg::Vec3f sunDirZUp(0.0f, 0.0f, 1.0f / _detailedFieldHeight);

    SG_LOG(SG_GENERAL, SG_ALERT, "Sun Direction Z-Up: " << sunDirZUp.x() << ", " << sunDirZUp.y() << ", " << sunDirZUp.z());

    // Build up the shadow space. 

    // Note that the height value uses a reverse iteration, and due to using unsigned int we have to do something slightly odd for the limit check.
    // By starting from the top we can make some efficiencies by using previously calculated values from further up the voxel space.
    for (size_t k = _detailedFieldHeight-1; k < _detailedFieldHeight; --k) {
        for (size_t j = 0; j < 128U; ++j) {
            for (size_t i = 0; i < 128U; ++i) {
                const osg::Vec3f start( (float) i / (float) _detailedFieldWidth, (float) j / (float) _detailedFieldWidth, (float) k / (float) _detailedFieldHeight);
                float d = 1.0f;
                float sunDensity = 0.0f;

                osg::Vec3f p = start + sunDirZUp * d;
                //SG_LOG(SG_GENERAL, SG_ALERT, "p " << p.x() << " " << p.y() <<  " " << p.z());
                while (sunDensity < 0.99f &&
                       p.x() >= 0.0f && p.x() < 1.0f && 
                       p.y() >= 0.0f && p.y() < 1.0f && 
                       p.z() >= 0.0f && p.z() < 1.0f    ) {                        

                    if (p.z() > (float) (k + 1U) / (float) _detailedFieldHeight) {
                        // Use the pre-calculated for the voxel above then stop
                        sunDensity += voxelShadeData->getColor(p).r();
                        break;
                    }

                    sunDensity += detailedVoxelData->getColor(p).z();
                    d += 1.0f;
                    p = start + sunDirZUp * d;
                }

                d = 1.0;
                float verticalDensity = 0.0f;

                p = start + osg::Vec3f(0.0f, 0.0f, 1.0f / (float) _detailedFieldHeight);
                while (verticalDensity < 0.99f &&
                       p.x() >= 0.0f && p.x() < 1.0f && 
                       p.y() >= 0.0f && p.y() < 1.0f && 
                       p.z() >= 0.0f && p.z() < 1.0f    ) {

                    if (p.z() > (float) (k + 1U) / (float) _detailedFieldHeight) {
                        // Use the pre-calculated for the voxel above
                        verticalDensity += voxelShadeData->getColor(p).g();
                        break;
                    }

                    verticalDensity += detailedVoxelData->getColor(p).z();
                    d += 1.0;
                    p = start + osg::Vec3f(0.0,0.0, 1.0f / (float) _detailedFieldHeight) * d;
                }

                osg::Vec4f c = osg::Vec4f(std::clamp(sunDensity, 0.0f, 1.0f), std::clamp(verticalDensity, 0.0f, 1.0f), 0.0f, 0.0f);
                //std::cout << c.r();
                voxelShadeData->setColor(c, i, j, k);
            }

            //std::cout << "\n";
        }

        //std::cout << "\n\n\n";
    }

    simgear::StateAttributeFactory::instance()->setCloudVoxelImage(detailedVoxelData, voxelShadeData);
    _fieldDirty = false;
}