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
class SGVoxelCloud;


class FGClouds {

private:
    typedef std::pair<const SGVoxelCloud*, osg::Vec3f> CloudPlacement;

    typedef std::unordered_map<int, CloudPlacement> CloudPlacementMap;

    double buildCloud(SGPropertyNode* cloud_def_root, SGPropertyNode* box_def_root,
                      const std::string& name, double altM, double grid_z_rand, SGCloudField* layer);
    void buildLayer(int iLayer, const std::string& name, double coverage, double altM);

    void buildCloudLayers(void);

    int update_event;

    int index;

    // Exception count from the fmm library
    unsigned int _FMMExceptionCount;

    size_t _roughVoxelSizeFactor;
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

    // Whether the cloud field is repeating (by simply mirroring the voxel space texture)
    bool _fieldRepeating;
    
    osg::ref_ptr<simgear::SGReaderWriterOptions> _options;

    // A node in the scenegraph purely used to ensure that the voxel data
    // is modified during the update traversal.
    osg::ref_ptr<osg::Group> _cloudUpdateNode;

    // The voxel images.
    osg::ref_ptr<osg::Image> _detailedVoxelData;
    osg::ref_ptr<osg::Image> _roughVoxelData;
    osg::ref_ptr<osg::Image> _voxelShadeData;

    bool add3DCloud(const SGPropertyNode *arg, SGPropertyNode * root);
    bool delete3DCloud(const SGPropertyNode *arg, SGPropertyNode * root);
    bool move3DCloud(const SGPropertyNode* arg, SGPropertyNode* root);

    /**
     * Add a new cloud with a given index at a specific point defined by lon/lat and an x/y offset
     */
    bool addCloud(const SGVoxelCloud* cloud, int index, float lon, float lat, float alt, float x, float y);
    bool addCloud(const SGVoxelCloud* cloud, int index, SGGeod loc, float x, float y);
    bool addCloud(const SGVoxelCloud* cloud, int index, float lon, float lat, float alt);
    bool addCloud(const SGVoxelCloud* cloud, int index, SGGeod loc);

    // add one cloud, data is not copied, ownership given
    void addCloud( SGVec3f& pos, const SGVoxelCloud* cloud);
    
    // Cloud handling functions.
    bool removeCloud(int index);
    bool repositionCloud(int index, float lon, float lat, float alt);
    bool repositionCloud(int index, float lon, float lat, float alt, float x, float y);

    void rebuildField(void);

    // Utility functions
    float getDetailedFieldRadiusM() { return (float) 0.5f * _detailedFieldWidth * _detailedFieldVoxelSize; }
    float getRoughFieldRadiusM()    { return (float) 0.5f * _roughFieldWidth * _roughFieldVoxelSize; }
    void generateSDF(osg::ref_ptr<osg::Image>);

public:
    FGClouds();
    ~FGClouds();

    void Init(void);

    int get_update_event(void) const;
    void set_update_event(int count);
    bool get_3dClouds() const;
    void set_3dClouds(bool enable);
    
    bool isDirty(void) const { return _fieldDirty; }
    void setDirty(bool dirty)  { _fieldDirty = dirty; }

    bool isCloudsRepeating(void) const { return _fieldRepeating; }
    void setCloudsRepeating(bool repeat) { _fieldRepeating = repeat; }

    void updateFromOsgTraversal();
    osg::ref_ptr<osg::Group> getCloudUpdateNode() { return _cloudUpdateNode; }
};

class FGCloudUpdateCallback : public osg::NodeCallback {
public:
    FGCloudUpdateCallback(FGClouds* clouds)
        : _clouds(clouds) {}

    void operator()(osg::Node* node, osg::NodeVisitor* nv) override {
        if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR) {
            _clouds->updateFromOsgTraversal();
        }
        traverse(node, nv);
    }

private:
    FGClouds* _clouds;
};
