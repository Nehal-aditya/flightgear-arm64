// metarproperties.hxx -- Parse a METAR and write properties
//
// Written by David Megginson, started May 2002.
// Rewritten by Torsten Dreyer, August 2010
//
// SPDX-FileCopyrightText: 2002 David Megginson <david@megginson.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <Airports/airport.hxx>
#include <simgear/props/props.hxx>
#include <simgear/props/tiedpropertylist.hxx>

class FGMetar;

namespace Environment {

class MagneticVariation;

class MetarProperties : public SGReferenced
{
public:
    MetarProperties( SGPropertyNode_ptr rootNode );
    virtual ~MetarProperties();

    SGPropertyNode_ptr get_root_node() const { return _rootNode; }
    virtual bool isValid() const { return _metarValidNode->getBoolValue(); }
    virtual const std::string & getStationId() const { return _station_id; }
    virtual void setStationId( const std::string & value );
    virtual void setMetar(SGSharedPtr<FGMetar> m);
    virtual void invalidate();

private:
    const char * get_metar() const;
    void set_metar( const char * metar );

    const char * get_station_id() const { return _station_id.c_str(); }
    void set_station_id( const char * value );
    const char * get_decoded() const { return _decoded.c_str(); }
    const char * get_description() const { return _description.c_str(); }
    double get_magnetic_variation_deg() const;
    double get_magnetic_dip_deg() const;
    double get_wind_from_north_fps() const { return _wind_from_north_fps; }
    double get_wind_from_east_fps() const { return _wind_from_east_fps; }
    double get_base_wind_dir() const { return _base_wind_dir; }
    double get_wind_speed() const { return _wind_speed; }
    void set_wind_from_north_fps( double value );
    void set_wind_from_east_fps( double value );
    void set_base_wind_dir( double value );
    void set_wind_speed( double value );

    SGSharedPtr<FGMetar> _metar;
    SGPropertyNode_ptr _rootNode;
    SGPropertyNode_ptr _metarValidNode;

    std::string _metarData;

    std::string _station_id;
    double _station_elevation;
    double _station_latitude;
    double _station_longitude;
    double _min_visibility;
    double _max_visibility;
    int _base_wind_dir;
    int _base_wind_range_from;
    int _base_wind_range_to;
    double _wind_speed;
    double _wind_from_north_fps;
    double _wind_from_east_fps;
    double _gusts;
    double _temperature;
    double _dewpoint;
    double _humidity;
    double _pressure;
    double _sea_level_temperature;
    double _sea_level_dewpoint;
    double _sea_level_pressure;
    double _rain;
    double _hail;
    double _snow;
    bool _snow_cover;
    std::string _decoded;
    int _day;
    int _hour;
    int _minute;
    bool _cavok;
    std::string _description;
protected:
    simgear::TiedPropertyList _tiedProperties;
    MagneticVariation * _magneticVariation;
};

inline void MetarProperties::set_station_id( const char * value )
{
    _station_id = value;
}

} // namespace Environment
