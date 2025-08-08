// gravity.hxx -- interface for earth gravitational model
//
// Written by Torsten Dreyer, June 2011
//
// SPDX-FileCopyrightText: 2011 Torsten Dreyer
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/math/SGMath.hxx>

namespace Environment {

class Gravity
{
public:
    virtual ~Gravity();
    virtual double getGravity( const SGGeod & position ) const = 0;

    const static Gravity * instance();

private:
    static Gravity* _instance;
};

} // namespace Environment
