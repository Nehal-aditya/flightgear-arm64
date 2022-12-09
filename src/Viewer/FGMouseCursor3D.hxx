// SPDX-FileCopyrightText: 2022 James Hogan <james@albanarts.com>
// SPDX-License-Identifier: GPL-2.0-or-later

/**
 * @file
 * @brief Defines 3D mouse cursor visible in the scene.
 */

#pragma once

#include <osg/MatrixTransform>

#include <simgear/math/SGVec2.hxx>
#include <simgear/math/SGVec3.hxx>

#include <GUI/MouseCursor.hxx>

#include "renderer.hxx"

#include <vector>

namespace flightgear {

/**
 * Represents a 3D mouse cursor visible in the scene.
 * It will place itself in the scene graph when the target is positioned.
 */
class FGMouseCursor3D : public osg::MatrixTransform
{
public:
    FGMouseCursor3D();
    virtual ~FGMouseCursor3D() = default;

    /// Set the cursor type.
    void setCursor(FGMouseCursor::Cursor cursor)
    {
        _cursor = cursor;
        updateModel();
    }

    /// Show the cursor.
    void showCursor()
    {
        _visible = true;
        updateModel();
    }

    /// Hide the cursor until showCursor().
    void hideCursorUntilMotion()
    {
        _visible = false;
        updateModel();
    }

    /// Set the 3D global position of the mouse cursor target.
    void setTargetGlobal(const SGVec3d& target);

protected:
    /// Update the cursor model.
    void updateModel();

protected:
    /**
     * Map of cursor types to switch indices (model IDs).
     * -1 indicates that no model exists.
     */
    std::vector<int> _modelMapping;

    /// Current cursor type.
    FGMouseCursor::Cursor _cursor = FGMouseCursor::CURSOR_NONE;

    /// Cursor currently visible.
    bool _visible = false;

    /// Current model ID.
    int _curModelId = -1;
};

} // namespace flightgear
