// SPDX-FileCopyrightText: 2014 Torsten Dreyer
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef __CURRENTWEATHER_ATIS_ENCODER_HXX
#define __CURRENTWEATHER_ATIS_ENCODER_HXX

/* ATIS encoder from current weather */

#include "ATISEncoder.hxx"
#include <string>

class CurrentWeatherATISInformationProvider : public ATISInformationProvider
{
public:
    explicit CurrentWeatherATISInformationProvider(const std::string& airportId);
    virtual ~CurrentWeatherATISInformationProvider();

protected:
    virtual bool isValid() const override;
    virtual std::string airportId() const override;
    virtual long getTime() const override;
    virtual int getWindDeg() const override;
    virtual int getWindMinDeg() const override { return getWindDeg(); }
    virtual int getWindMaxDeg() const override { return getWindDeg(); }
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
    std::string _airportId;
    SGPropertyNode_ptr _environment;
};

#endif
