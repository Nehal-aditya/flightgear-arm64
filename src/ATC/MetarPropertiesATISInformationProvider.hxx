// SPDX-FileCopyrightText: 2014 Torsten Dreyer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/* ATIS encoder from metarproperties */

#include <simgear/props/props.hxx>

#include "ATISEncoder.hxx"
#include <string>

class MetarPropertiesATISInformationProvider : public ATISInformationProvider
{
public:
    explicit MetarPropertiesATISInformationProvider(SGPropertyNode_ptr metar);
    virtual ~MetarPropertiesATISInformationProvider();

protected:
    virtual bool isValid() const override;
    virtual std::string airportId() const override;
    virtual long getTime() const override;
    virtual int getWindDeg() const override;
    virtual int getWindMinDeg() const override;
    virtual int getWindMaxDeg() const override;
    virtual int getWindSpeedKt() const override;
    virtual int getGustsKt() const override;
    virtual int getQnh() const override;
    virtual double getQnhInHg() const override;
    virtual bool isCavok() const override;
    virtual int getVisibilityMeters() const override;
    virtual std::string getPhenomena() const override;
    virtual CloudEntries getClouds();
    virtual int getTemperatureDeg() const override;
    virtual int getDewpointDeg() const override;
    virtual std::string getTrend() const override;

private:
    SGPropertyNode_ptr _metar;
};
