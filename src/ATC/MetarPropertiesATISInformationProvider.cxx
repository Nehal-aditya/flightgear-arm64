// SPDX-FileCopyrightText: 2014 Torsten Dreyer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "MetarPropertiesATISInformationProvider.hxx"
#include <Main/globals.hxx>
#include <simgear/constants.h>

using std::string;

MetarPropertiesATISInformationProvider::MetarPropertiesATISInformationProvider(SGPropertyNode_ptr metar) : _metar(metar)
{
}

MetarPropertiesATISInformationProvider::~MetarPropertiesATISInformationProvider()
{
}

bool MetarPropertiesATISInformationProvider::isValid() const
{
    return _metar->getBoolValue("valid", false);
}

string MetarPropertiesATISInformationProvider::airportId() const
{
    return _metar->getStringValue("station-id", "xxxx");
}

long MetarPropertiesATISInformationProvider::getTime() const
{
    return makeAtisTime(0,
                        _metar->getIntValue("hour") % 24,
                        _metar->getIntValue("minute") % 60);
}

int MetarPropertiesATISInformationProvider::getWindDeg() const
{
    return _metar->getIntValue("base-wind-dir-deg");
}

int MetarPropertiesATISInformationProvider::getWindMinDeg() const
{
    return _metar->getIntValue("base-wind-range-from");
}
int MetarPropertiesATISInformationProvider::getWindMaxDeg() const
{
    return _metar->getIntValue("base-wind-range-to");
}
int MetarPropertiesATISInformationProvider::getWindSpeedKt() const
{
    return _metar->getIntValue("base-wind-speed-kt");
}

int MetarPropertiesATISInformationProvider::getGustsKt() const
{
    return _metar->getIntValue("gust-wind-speed-kt");
}

int MetarPropertiesATISInformationProvider::getQnh() const
{
    return _metar->getDoubleValue("pressure-inhg") * SG_INHG_TO_PA / 100.0;
}

double MetarPropertiesATISInformationProvider::getQnhInHg() const
{
    return _metar->getDoubleValue("pressure-inhg");
}

bool MetarPropertiesATISInformationProvider::isCavok() const
{
    return _metar->getBoolValue("cavok");
}

int MetarPropertiesATISInformationProvider::getVisibilityMeters() const
{
    return _metar->getIntValue("min-visibility-m");
}

string MetarPropertiesATISInformationProvider::getPhenomena() const
{
    return _metar->getStringValue("decoded");
}

ATISInformationProvider::CloudEntries MetarPropertiesATISInformationProvider::getClouds()
{
    CloudEntries reply;

    using simgear::PropertyList;

    PropertyList layers = _metar->getNode("clouds", true)->getChildren("layer");
    for (PropertyList::iterator it = layers.begin(); it != layers.end(); ++it) {
        std::string coverage = (*it)->getStringValue("coverage", "clear");
        double elevation = (*it)->getDoubleValue("elevation-ft", -9999);
        if (elevation > 0) {
            reply[elevation] = coverage;
        }
    }

    return reply;
}

int MetarPropertiesATISInformationProvider::getTemperatureDeg() const
{
    return _metar->getIntValue("temperature-degc");
}

int MetarPropertiesATISInformationProvider::getDewpointDeg() const
{
    return _metar->getIntValue("dewpoint-degc");
}

string MetarPropertiesATISInformationProvider::getTrend() const
{
    return "nosig";
}
