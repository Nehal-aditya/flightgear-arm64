// gravity.cxx -- interface for earth gravitational model
//
// Written by Torsten Dreyer, June 2011
//
// SPDX-FileCopyrightText: 2011 Torsten Dreyer
// SPDX-License-Identifier: GPL-2.0-or-later

#include "gravity.hxx"

#include <simgear/structure/exception.hxx>

namespace Environment {

/*
http://de.wikipedia.org/wiki/Normalschwereformel
*/
class Somigliana : public Gravity {
public:
    Somigliana();
    virtual ~Somigliana();
    virtual double getGravity( const SGGeod & position ) const;
};

Somigliana::Somigliana()
{
}

Somigliana::~Somigliana()
{
}

double Somigliana::getGravity( const SGGeod & position ) const
{
// Geodetic Reference System 1980 parameter
#define A 6378137.0 // equatorial radius of earth
#define B 6356752.3141 // semiminor axis
#define AGA (A*9.7803267715) // A times normal gravity at equator
#define BGB (B*9.8321863685) // B times normal gravity at pole
    // formula of Somigliana
    double cosphi = ::cos(position.getLatitudeRad());
    double cos2phi = cosphi * cosphi;
    double sinphi = ::sin(position.getLatitudeRad());
    double sin2phi = sinphi * sinphi;
    double g0 = (AGA * cos2phi + BGB * sin2phi) / sqrt(A * A * cos2phi + B * B * sin2phi);

    static const double k1 = 3.15704e-7;
    static const double k2 = 2.10269e-9;
    static const double k3 = 7.37452e-14;

    double h = position.getElevationM();

    return g0 * (1 - (k1 - k2 * sin2phi) * h + k3 * h * h);
}

static Somigliana _somigliana;

/* --------------------- Gravity implementation --------------------- */
Gravity * Gravity::_instance = NULL;

Gravity::~Gravity()
{
}

//double Gravity::getGravity( const SGGeoc & position ) = 0;

const Gravity * Gravity::instance()
{
    if( _instance == NULL )
        _instance = &_somigliana;

    return _instance;
}

} // namespace
