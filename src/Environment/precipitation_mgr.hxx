/**
 * @file precipitation_mgr.hxx
 * @author Nicolas VIVIEN
 * @date 2008-02-10
 *
 * @brief Precipitation manager
 *   This manager calculate the intensity of precipitation in function of the altitude,
 *   calculate the wind direction and velocity, then update the drawing of precipitation.
 */
// SPDX-FileCopyrightText: 2008 Nicolas Vivien
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/structure/subsystem_mgr.hxx>
#include <simgear/environment/precipitation.hxx>
#include <simgear/props/tiedpropertylist.hxx>

class FGPrecipitationMgr : public SGSubsystem
{
private:
    osg::ref_ptr<osg::MatrixTransform> transform;
    osg::ref_ptr<SGPrecipitation> precipitation;
    float getPrecipitationAtAltitudeMax(void);
    simgear::TiedPropertyList _tiedProperties;

public:
    FGPrecipitationMgr();
    virtual ~FGPrecipitationMgr();

    // Subsystem API.
    void bind() override;
    void init() override;
    void unbind() override;
    void update(double dt) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "precipitation"; }

    void setupSceneGraph(void);
    void setPrecipitationLevel(double l);
};
