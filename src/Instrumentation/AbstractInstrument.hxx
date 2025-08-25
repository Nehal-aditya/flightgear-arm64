/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * SPDX-FileCopyrightText: 2019 James Turner <james@flightgear.org>
 *
 * AbstractInstrument.hxx
 *
*/

#pragma once

#include <simgear/props/props.hxx>
#include <simgear/structure/subsystem_mgr.hxx>

class AbstractInstrument : public SGSubsystem
{
public:
protected:
    void readConfig(SGPropertyNode* config,
                    std::string defaultName);

    void initServicePowerProperties(SGPropertyNode* node);

    bool isServiceableAndPowered() const;

    // build the path /instrumentation/<name>[number]
    std::string nodePath() const;

    std::string name() const { return _name; }
    int number() const { return _index; }

    void unbind() override;

    void setMinimumSupplyVolts(double v);

    /**
     * specify the default path to use to power the instrument, if it's non-
     * standard.
     */
    void setDefaultPowerSupplyPath(const std::string &p);

    virtual bool isPowerSwitchOn() const;
private:
    std::string _name;
    int _index = 0;
    std::string _powerSupplyPath;

    SGPropertyNode_ptr _serviceableNode;
    SGPropertyNode_ptr _powerButtonNode;
    double _minimumSupplyVolts;
    SGPropertyNode_ptr _powerSupplyNode;
};
