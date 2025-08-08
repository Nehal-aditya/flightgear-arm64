// metar interface class
//
// Written by Melchior FRANZ, started January 2005.
//
// SPDX-FileCopyrightText: 2005 Melchior FRANZ <mfranz@aon.at>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <vector>
#include <map>
#include <string>
#include <time.h>

#include <simgear/environment/metar.hxx>


class FGMetar : public SGMetar, public SGReferenced {
public:
	FGMetar(const std::string& icao);

	long	getAge_min()			const;
	time_t	getTime()			const { return _time; }
	double	getRain()			const { return _rain / 3.0; }
	double	getHail()			const { return _hail / 3.0; }
	double	getSnow()			const { return _snow / 3.0; }
	bool	getSnowCover()			const { return _snow_cover; }

private:
	time_t	_rq_time;
	time_t	_time;
	bool	_snow_cover;
};
