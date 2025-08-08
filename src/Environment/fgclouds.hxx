// Build a cloud layer based on metar
//
// Written by Harald JOHNSEN, started April 2005.
//
// SPDX-FileCopyrightText: 2005 Harald JOHNSEN <hjohnsen@evc.net>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>

// forward decls
class SGPropertyNode;
class SGCloudField;

class FGClouds {

private:
    double buildCloud(SGPropertyNode* cloud_def_root, SGPropertyNode* box_def_root,
                      const std::string& name, double grid_z_rand, SGCloudField* layer);
    void buildLayer(int iLayer, const std::string& name, double coverage);

    void buildCloudLayers(void);

    int update_event;

    bool clouds_3d_enabled;
    int index;

    bool add3DCloud(const SGPropertyNode *arg, SGPropertyNode * root);
    bool delete3DCloud(const SGPropertyNode *arg, SGPropertyNode * root);
    bool move3DCloud(const SGPropertyNode* arg, SGPropertyNode* root);

public:
    FGClouds();
    ~FGClouds();

    void Init(void);

    int get_update_event(void) const;
    void set_update_event(int count);
    bool get_3dClouds() const;
    void set_3dClouds(bool enable);
};
