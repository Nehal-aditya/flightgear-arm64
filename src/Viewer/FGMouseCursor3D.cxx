// SPDX-FileCopyrightText: 2022 James Hogan <james@albanarts.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Implements 3D mouse cursor visible in the scene.
 */

#include "FGMouseCursor3D.hxx"

#include "renderer.hxx"
#include "view.hxx"
#include "viewmgr.hxx"

#include <Main/fg_props.hxx>
#include <Model/acmodel.hxx>

#include <simgear/debug/ErrorReportingCallback.hxx>
#include <simgear/scene/model/placement.hxx>
#include <simgear/scene/util/OsgMath.hxx>
#include <simgear/scene/util/RenderConstants.hxx>
#include <simgear/scene/util/SGNodeMasks.hxx>
#include <simgear/scene/util/SGPickCallback.hxx>
#include <simgear/scene/util/SGReaderWriterOptions.hxx>

using namespace flightgear;

typedef FGRenderer::PickList PickList;

FGMouseCursor3D::FGMouseCursor3D()
{
    setName("3D cursor");

    osg::Switch* sw = new osg::Switch;
    addChild(sw);

    // Read cursor models from /sim/vr/config/cursors/cursor[]
    SGPropertyNode_ptr cursor3DNode = fgGetNode("/sim/vr/config/cursors", true);
    for (auto cursor : cursor3DNode->getChildren("cursor")) {
        std::string path = cursor->getStringValue("model/path", "");
        std::string cursorStr = cursor->getStringValue("cursor", "");
        unsigned int cursorId = (int)FGMouseCursor::cursorFromString(cursorStr.c_str());

        simgear::ErrorReportContext ec("cursor-model", path);

        SGPath resolvedPath = globals->resolve_aircraft_path(path);
        if (resolvedPath.isNull()) {
            simgear::reportFailure(simgear::LoadFailure::NotFound,
                                   simgear::ErrorCode::XMLModelLoad,
                                   "Failed to find 3D mouse cursor model",
                                   SGPath::fromUtf8(path));
            continue;
        }

        osg::Node* node = simgear::SGModelLib::loadModel(resolvedPath.utf8Str(),
                                                         globals->get_props());
        if (node) {
            // Add mapping from the cursor type to the model node
            if (cursorId >= _modelMapping.size())
                _modelMapping.resize(cursorId + 1, -1);
            _modelMapping[cursorId] = sw->getNumChildren();

            // And add the model node to the switch
            sw->addChild(node, false);
        }
    }

    // Don't allow the cursor to itself be picked, or it may try to pick itself!
    // Don't allow the aircraft to collide catastrophically with the cursor!
    setNodeMask(~SG_NODEMASK_PICK_BIT & ~SG_NODEMASK_TERRAIN_BIT);
}

void FGMouseCursor3D::setTargetGlobal(const SGVec3d& target)
{
    // We need the current view
    auto* view_mgr = globals->get_subsystem<FGViewMgr>();
    if (!view_mgr)
        return;
    auto* view = view_mgr->get_current_view();
    if (!view)
        return;

    // Get aircraft pose
    auto* aircraft = globals->get_subsystem<FGAircraftModel>()->get3DModel();
    auto aircraftPosition = aircraft->getPosition();
    SGQuatd aircraftOrient = aircraft->getGlobalOrientation();

    // Update the cursor position
    osg::Matrix mat;
    SGVec3d targetAircraft = aircraftOrient.transform(target - SGVec3d::fromGeod(aircraftPosition));
    mat.setTrans(toOsg(targetAircraft));

    // Match the orientation to the view
    mat.setRotate(toOsg(inverse(aircraftOrient) * view->getViewOrientation()));

    // Scale to a fixed angular size
    double scale = length(target - view->getViewPosition());
    mat.preMultScale(osg::Vec3d(scale, scale, scale));
    setMatrix(mat);

    // Add cursor to scene graph under aircraft
    if (!getNumParents())
        aircraft->add(this);
}

void FGMouseCursor3D::updateModel()
{
    // Get model switch node
    auto* sw = dynamic_cast<osg::Switch*>(getChild(0));
    assert(sw);

    FGMouseCursor::Cursor cursor = _cursor;
    if (!_visible)
        cursor = FGMouseCursor::CURSOR_NONE;

    int modelId = -1;
    if (cursor != FGMouseCursor::CURSOR_NONE) {
        if (cursor < _modelMapping.size())
            modelId = _modelMapping[cursor];
        // fall back to arrow, if that exists
        if (modelId < 0) {
            cursor = FGMouseCursor::CURSOR_ARROW;
            if (cursor < _modelMapping.size())
                modelId = _modelMapping[cursor];
        }
        // fall back to first modelId that does exist
        if (modelId < 0 && sw->getNumChildren() > 0)
            modelId = 0;
    }

    // Switch to chosen model
    if (_curModelId != modelId) {
        if (_curModelId >= 0)
            sw->setValue(_curModelId, false);
        _curModelId = modelId;
        if (_curModelId >= 0)
            sw->setValue(_curModelId, true);
    }
}
