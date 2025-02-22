// Handle picking using VR controller
//
// Copyright (C) 2021  James Hogan
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "config.h"

#include "FGVRPick.hxx"

#include <Main/fg_props.hxx>
#include <Viewer/renderer.hxx>

#include <simgear/scene/material/Effect.hxx>
#include <simgear/scene/material/EffectGeode.hxx>
#include <simgear/scene/util/OsgMath.hxx>
#include <simgear/scene/util/RenderConstants.hxx>
#include <simgear/scene/util/SGReaderWriterOptions.hxx>

#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/MatrixTransform>
#include <osg/ref_ptr>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osgDB/Registry>

typedef SGSharedPtr<SGPickCallback> SGPickCallbackPtr;
typedef std::list<SGPickCallbackPtr> SGPickCallbackList;

using namespace flightgear;

// FGVRPick::Private

class FGVRPick::Private
{
public:
    void hover(FGRenderer::PickList& pickList);
    void buttonDown(unsigned int button, FGRenderer::PickList& pickList);
    void buttonUp(unsigned int button);
    void update(double dt);
    bool picking() const;

    FGVRInput* _input;
    osg::ref_ptr<osg::Switch> _pickSwitch;
    osg::ref_ptr<simgear::EffectGeode> _pickGeode;
    osg::ref_ptr<osg::ShapeDrawable> _pickGeom;
    osg::ref_ptr<osg::Capsule> _pickShape;
    osg::ref_ptr<simgear::Effect> _effectMiss;
    osg::ref_ptr<simgear::Effect> _effectHit;
    osg::ref_ptr<simgear::Effect> _effectGrab;

    struct Contact {
        //struct SGSceneryPick pick;
        osg::observer_ptr<osg::Node> rootNode;
        // FIXME save pointing...
        SGIKLink* ikLink;
        std::shared_ptr<SGIKContactSpringStatic> contact;
        // Contact distance
        double distance;

        // Normal pick callbacks
        std::map<int, SGPickCallbackList> activeCallbacks;
    };

    /// Contact point
    Contact _contact;

protected:
    SGSceneryPick _hoverPick;
    SGSceneryPick _buttonPicks[2];
};

void FGVRPick::Private::hover(FGRenderer::PickList& pickList)
{
    osg::Vec2d dummyPos(0, 0);
    for (auto& pick : pickList) {
        // Unchanged hover, no change
        if (pick.callback == _hoverPick.callback)
            return;
        // New hover, leave previously hovered pick
        if (pick.callback.valid() && pick.callback->hover(dummyPos, pick.info)) {
            if (_hoverPick.callback.valid())
                _hoverPick.callback->mouseLeave(dummyPos);
            _hoverPick = pick;
            return;
        }
    }
    // No hover, leave previously hovered pick
    if (_hoverPick.callback.valid())
        _hoverPick.callback->mouseLeave(dummyPos);
    _hoverPick.callback.reset();
}

void FGVRPick::Private::buttonDown(unsigned int button, FGRenderer::PickList& pickList)
{
    // FIXME broken, ea.getGraphicsContext()==nullptr breaks
    // simgear/scene/model/SGPickAnimation.cxx eventToWindowCoords()
    osgGA::GUIEventAdapter* ea = osgGA::GUIEventAdapter::getAccumulatedEventState().get();
    for (auto& pick : pickList) {
        if (pick.callback.valid() && pick.callback->buttonPressed(button, *ea, pick.info)) {
            _buttonPicks[button] = pick;
            return;
        }
    }
}

void FGVRPick::Private::buttonUp(unsigned int button)
{
    // FIXME broken, ea.getGraphicsContext()==nullptr breaks
    // simgear/scene/model/SGPickAnimation.cxx eventToWindowCoords()
    osgGA::GUIEventAdapter* ea = osgGA::GUIEventAdapter::getAccumulatedEventState().get();
    if (_buttonPicks[button].callback.valid())
        _buttonPicks[button].callback->buttonReleased(button, *ea, &_buttonPicks[button].info);
    _buttonPicks[button].callback.reset();
}

void FGVRPick::Private::update(double dt)
{
    int keyModState = fgGetKeyModifiers();
    for (auto& pick : _buttonPicks)
        if (pick.callback.valid())
            pick.callback->update(dt, keyModState);
}

bool FGVRPick::Private::picking() const
{
    for (auto& pick : _buttonPicks)
        if (pick.callback.valid())
            return true;
    return false;
}


// FGVRPick

FGVRPick::FGVRPick(FGVRInput* input,
                   FGVRInput::Mode* mode,
                   FGVRInput::Subaction* subaction,
                   SGPropertyNode* node,
                   SGPropertyNode* statusNode)
    : ModeProcess(mode, subaction, node, statusNode),
      _pose(mode, subaction, getInputNode("pose")),
      _grab(mode, subaction, getInputNode("grab")),
      _mouseLeft(mode, subaction, getInputNode("mouse-left-click")),
      _mouseMiddle(mode, subaction, getInputNode("mouse-middle-click")),
      _reach(node->getDoubleValue("reach", 5.0f)),
      _private(std::make_unique<Private>())
{
    // Create a geometry for the line
    osg::ShapeDrawable* pickGeom = _private->_pickGeom = new osg::ShapeDrawable;
    pickGeom->setUseDisplayList(false);

    // Create a geode for the line
    simgear::EffectGeode* pickGeode = _private->_pickGeode = new simgear::EffectGeode;
    pickGeode->addDrawable(pickGeom);

    // Get the effects
    std::string eff_file = node->getStringValue("miss-effect");
    osg::ref_ptr<simgear::SGReaderWriterOptions> options
        = simgear::SGReaderWriterOptions::copyOrCreate(osgDB::Registry::instance()->getOptions());
    if (!eff_file.empty()) {
        simgear::Effect* effect = _private->_effectMiss = makeEffect(eff_file, true, options);
        pickGeode->setEffect(effect);
    }
    eff_file = node->getStringValue("hit-effect");
    if (!eff_file.empty())
        _private->_effectHit = makeEffect(eff_file, true, options);
    eff_file = node->getStringValue("grab-effect");
    if (!eff_file.empty())
        _private->_effectGrab = makeEffect(eff_file, true, options);

    // Switch on and off
    osg::Switch* sw = _private->_pickSwitch = new osg::Switch();
    sw->setNodeMask(~simgear::PICK_BIT);
    sw->addChild(pickGeode);

    // Add it to the local space group
    _private->_input = input;
    input->getLocalSpaceGroup()->addChild(sw);
}

FGVRPick::~FGVRPick()
{
    // Remove the line from the local space group
    _private->_input->getLocalSpaceGroup()->removeChild(_private->_pickSwitch);
}

void FGVRPick::postinit(SGPropertyNode* node,
                        const std::string& module)
{
}

void FGVRPick::update(double dt)
{
    //FGRenderer::PickList pickList;
    osgXR::ActionPose::Location pose;

    bool active = _pose.getPoseValue(pose) &&
                  pose.isPositionValid() && pose.isOrientationValid();
    _private->_pickSwitch->setValue(0, active);
    if (active) {
#if 0
        // Handle hovering
        _private->hover(pickList);
#endif
        // Calculate pick line segment in local space
        auto& localMatrix = _private->_input->getLocalSpaceGroup()->getMatrix();
        osg::Vec3d startLocal = pose.getPosition();
        osg::Vec3d aimVecLocal = pose.getOrientation() * osg::Vec3d(0.0, 0.0, -1.0);
        double pickLength = _reach;
        osg::Vec3d endLocal = startLocal + aimVecLocal * pickLength;

        // If grab in progress, just update contact point
        bool grab;
        bool grabChanged;
        _grab.getBoolValue(grab, &grabChanged);
        if (grab && !grabChanged && _private->_contact.contact &&
                _private->_contact.rootNode.valid()) {
            //std::cout << "Ongoing grab" << std::endl;
            auto nodePaths = _private->_contact.rootNode->getParentalNodePaths();
            if (!nodePaths.empty()) {
                nodePaths.front().pop_back();
                auto rootMatrix = computeWorldToLocal(nodePaths.front());

                // Update spring destination relative to IK root, using new aim
                // pose
                pickLength = _private->_contact.distance;
                endLocal = startLocal + aimVecLocal * pickLength;
                osg::Vec3d endGlobal = endLocal * localMatrix;
                osg::Vec3d endRoot = endGlobal * rootMatrix;
                /*
                   std::cout << "  endLocal " << endLocal.x() << "," << endLocal.y() << "," << endLocal.z() << std::endl;
                   std::cout << "  endGlobal " << endGlobal.x() << "," << endGlobal.y() << "," << endGlobal.z() << std::endl;
                   std::cout << "  endRoot " << endRoot.x() << "," << endRoot.y() << "," << endRoot.z() << std::endl;
                   */
                _private->_contact.contact->setSpringPositionRoot(endRoot);
            }
        } else {
            // Get line segment in global scene space
            osg::Vec3d endGlobal = endLocal * localMatrix;
            osg::Vec3d startGlobal = startLocal * localMatrix;

            // Perform the pick
            auto pickLinks = globals->get_renderer()->pickLinks(startGlobal, endGlobal);
            if (pickLinks.rootNode) {
                //std::cout << "Picking: hit " << grab << std::endl;
                // It hits something
                _private->_pickGeode->setEffect(_private->_effectHit);

                // Update line segment to stop at first item
                auto& localMatrixInv = _private->_input->getLocalSpaceGroup()->getInverseMatrix();
                endGlobal = pickLinks.wgs84;
                endLocal = pickLinks.wgs84 * localMatrixInv;
                pickLength = (endLocal - startLocal).length();
            } else {
                //std::cout << "Picking: miss " << grab << std::endl;
                _private->_pickGeode->setEffect(_private->_effectMiss);
                // FIXME maybe wgs84 still valid?
            }

            // Start grabbing something reversible
            if (grab && grabChanged && pickLinks.reversible) {
                auto* ik = pickLinks.linkPath.back().link;
                // If top link is different to last time, clear contact and update
                if (ik != _private->_contact.ikLink) {
                    if (_private->_contact.contact)
                        _private->_contact.contact->setStale();
                    _private->_contact.contact = nullptr;
                    _private->_contact.ikLink = ik;
                }
                // Create a new spring contact
                if (!_private->_contact.contact) {
                    _private->_contact.contact = std::make_shared<SGIKContactSpringStatic>();
                    std::cout << "Creating contact " << _private->_contact.contact << std::endl;
                    ik->addContact(_private->_contact.contact, pickLinks.linkPath);
                } else {
                    std::cout << "Updating contact " << _private->_contact.contact << std::endl;
                }

                // Set contact position relative to IK tip (for IK calculations)
                auto contactPosTip = pickLinks.wgs84 * pickLinks.tipMatrix;
                _private->_contact.contact->setContactPositionTip(contactPosTip);

                // Set spring destination to match
                osg::Vec3d contactPosRoot = endGlobal * pickLinks.rootMatrix;
                _private->_contact.contact->setSpringPositionRoot(contactPosRoot);
                _private->_contact.contact->setForce(10.0f, 1.0f);

                // Update root node pointer
                _private->_contact.rootNode = pickLinks.rootNode;
                _private->_contact.distance = pickLength;

                _private->_pickGeode->setEffect(_private->_effectGrab);
            } else if (grabChanged && !grab) {
                // No contact, make existing contact stale
                if (_private->_contact.contact) {
                    std::cout << "Dropping contact " << _private->_contact.contact << std::endl;
                    _private->_contact.contact->setStale();
                    _private->_contact.contact = nullptr;
                    _private->_contact.ikLink = nullptr;
#if 0
                } else if (!grab && grabsChanged &&
                           grabNodes[grab] && grabPositions[grab]) {
                    // If grab finished, execute the mouse up event
                    _private->finishPick(grab, 0, *grabNodes[grab], *grabPositions[grab]);
#endif
                }
            }

#if 0
            // If grab started
            if (grab && grabChanged &&
                grabNodes[grab] && grabPositions[grab]) {
                // Fall back to normal mouse clicks
            }
#endif

#if 0
            // Fall back to mouse emulation
            if (_mouseLeft.getBoolValue(value, &changed) && changed) {
                if (value)
                    _private->buttonDown(0, pickList);
                else
                    _private->buttonUp(0);
            }
            if (_mouseMiddle.getBoolValue(value, &changed) && changed) {
                if (value)
                    _private->buttonDown(1, pickList);
                else
                    _private->buttonUp(1);
            }

            // Update active pick callbacks
            _private->update(dt);

            // Use grab effect if any pick callbacks in use
            if (_private->picking())
                _private->_pickGeode->setEffect(_private->_effectGrab);
#endif
        }

        // Create/update the capsule for the pick ray
        osg::Capsule* pickShape = _private->_pickShape = new osg::Capsule(startLocal + (endLocal - startLocal)/2,
                                                                          0.001f, pickLength);
        pickShape->setRotation(pose.getOrientation());
        _private->_pickGeom->setShape(pickShape);
    }
}

void FGVRPick::deactivate()
{
    _pose.deactivate();
    _private->_pickSwitch->setValue(0, false);

    if (_grab.getLastBoolValue()) {
        _grab.deactivate();
        // FIXME release
    }

    if (_mouseLeft.getLastBoolValue()) {
        _mouseLeft.deactivate();
        _private->buttonUp(0);
    }
    if (_mouseMiddle.getLastBoolValue()) {
        _mouseMiddle.deactivate();
        _private->buttonUp(1);
    }
}
