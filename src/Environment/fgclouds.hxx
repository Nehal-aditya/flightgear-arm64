// Build a cloud layer based on metar
//
// Written by Harald JOHNSEN, started April 2005.
//
// SPDX-FileCopyrightText: 2005 Harald JOHNSEN <hjohnsen@evc.net>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include <osg/Image>
#include <vector>
#include <map>

#include <simgear/scene/util/SGReaderWriterOptions.hxx>

using std::vector;

// forward decls
class SGPropertyNode;
class SGCloudField;
class SGNewCloud;

class FGClouds {

private:
    typedef std::tuple<SGNewCloud, osg::Vec3f> CloudPlacement;

    typedef std::unordered_map<int, CloudPlacement> CloudPlacementMap;

    double buildCloud(SGPropertyNode* cloud_def_root, SGPropertyNode* box_def_root,
                      const std::string& name, double altM, double grid_z_rand, SGCloudField* layer);
    void buildLayer(int iLayer, const std::string& name, double coverage, double altM);

    void buildCloudLayers(void);

    int update_event;

    int index;

    // Voxel-based clouds
    osg::ref_ptr<osg::Image> _roughVoxelData;
    osg::ref_ptr<osg::Image> _detailedVoxelData;
    osg::ref_ptr<osg::Image> _noiseData;

    size_t _roughFieldWidth;
    size_t _roughFieldHeight;
    size_t _roughFieldVoxelSize;
    size_t _detailedFieldWidth;
    size_t _detailedFieldHeight;
    size_t _detailedFieldVoxelSize;

    CloudPlacementMap _cloudPlacementMap;

    // This is the ECF cartesian coordinates of the voxel field.
    SGVec3d _centerCart; 
    osg::Matrixd _cloudPosMatrix;
    
    // Whether the cloud field requires regeneration.
    bool _fieldDirty;
    
    osg::ref_ptr<simgear::SGReaderWriterOptions> _options;

    bool add3DCloud(const SGPropertyNode *arg, SGPropertyNode * root);
    bool delete3DCloud(const SGPropertyNode *arg, SGPropertyNode * root);
    bool move3DCloud(const SGPropertyNode* arg, SGPropertyNode* root);

    /**
     * Add a new cloud with a given index at a specific point defined by lon/lat and an x/y offset
     */
    bool addCloud(SGNewCloud cloud, int index, float lon, float lat, float alt, float x, float y);
    bool addCloud(SGNewCloud cloud, int index, SGGeod loc, float x, float y);
    bool addCloud(SGNewCloud cloud, int index, float lon, float lat, float alt);
    bool addCloud(SGNewCloud cloud, int index, SGGeod loc);

    // add one cloud, data is not copied, ownership given
    void addCloud( SGVec3f& pos, SGNewCloud cloud);
    
    // Cloud handling functions.
    bool removeCloud(int index);
    bool repositionCloud(int index, float lon, float lat, float alt);
    bool repositionCloud(int index, float lon, float lat, float alt, float x, float y);

    void rebuildField(void);

    // Utility functions
    float getDetailedFieldRadiusM() { return (float) 0.5f * _detailedFieldWidth * _detailedFieldVoxelSize; }
    float getRoughFieldRadiusM()    { return (float) 0.5f * _roughFieldWidth * _roughFieldVoxelSize; }

public:
    FGClouds();
    ~FGClouds();

    void Init(void);

    int get_update_event(void) const;
    void set_update_event(int count);
    bool get_3dClouds() const;
    void set_3dClouds(bool enable);
};
