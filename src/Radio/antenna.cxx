// antenna.cxx -- implementation of FGRadioAntenna
// Class to represent a virtual radio antenna properties
// Written by Adrian Musceac YO8RZZ, started December 2011.
//
// SPDX-FileCopyrightText: 2011 Adrian Musceac YO8RZZ
// SPDX-License-Identifier: GPL-2.0-or-later


#include <config.h>

#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <Scenery/scenery.hxx>
#include <simgear/io/iostreams/sgstream.hxx>

#include "antenna.hxx"

using namespace std;

FGRadioAntenna::FGRadioAntenna(string type) {
    _mirror_y = 1; // normally we want to mirror these axis because the pattern is symmetric
    _mirror_z = 1;
    _invert_ground = 0; // TODO: use for inverting the antenna ground, for instance aircraft body reflection
    load_NEC_antenna_pattern(type);
}

FGRadioAntenna::~FGRadioAntenna() {
	for (unsigned i =0; i < _pattern.size(); i++) {
		AntennaGain *point_gain = _pattern[i];
		delete point_gain;
	}
	_pattern.clear();
}

// WIP
double FGRadioAntenna::calculate_gain(double bearing, double angle) {

	// TODO: what if the pattern is assimetric?
	bearing = fabs(bearing);
	if (bearing > 180)
		bearing = 360 - bearing;
	// for plots with 2 degrees resolution:
	int azimuth = (int)floor(bearing);
	azimuth += azimuth % 2;
	int elevation = (int)floor(angle);
	elevation += elevation % 2;
	//cerr << "Bearing: " << bearing << " angle: " << angle << " azimuth: " << azimuth << " elevation: " << elevation << endl;
	for (unsigned i =0; i < _pattern.size(); i++) {
		AntennaGain *point_gain = _pattern[i];

		if ( (azimuth == point_gain->azimuth) && (elevation == point_gain->elevation)) {
			return point_gain->gain;
		}
	}

	return 0;
}


void FGRadioAntenna::load_NEC_antenna_pattern(string type) {

	//SGPath pattern_file(globals->get_fg_home());
	SGPath pattern_file(globals->get_fg_root());
	pattern_file.append("Navaids/Antennas");
	pattern_file.append(type + ".txt");
	if (!pattern_file.exists()) {
		return;
	}
	sg_ifstream file_in(pattern_file);
	int heading, elevation;
	double gain;
	while(!file_in.eof()) {
		file_in >> heading >> elevation >> gain;
		if( (_mirror_y == 1) && (heading > 180) ) {
			continue;
		}
		if ( (_mirror_z == 1) && (elevation < 0) ) {
			continue;
		}
		//cerr << "head: " << heading << " elev: " << elevation << " gain: " << gain << endl;
		AntennaGain *datapoint = new AntennaGain;
		datapoint->azimuth = heading;
		datapoint->elevation = 90.0 - abs(elevation);
		datapoint->gain = gain;
		_pattern.push_back(datapoint);
	}
}
