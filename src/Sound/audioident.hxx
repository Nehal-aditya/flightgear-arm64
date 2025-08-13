// audioident.hxx -- audible station identifiers
//
// Written by Torsten Dreyer, September 2011
//
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2011 Torsten Dreyer

#pragma once

#include <string>
#include <simgear/sound/soundmgr.hxx>

class AudioIdent {
public:
    AudioIdent( const std::string & fx_name, const double interval_secs, const int frequency );
    void init();
    void setVolumeNorm( double volumeNorm );
    void setIdent( const std::string & ident, double volumeNorm );

    void update( double dt );

private:
    void stop();
    void start();

    SGSharedPtr<SGSampleGroup> _sgr;
    std::string _fx_name;
    const int _frequency;
    std::string _ident;
    double _timer;
    double _interval;
    bool _running;
};

class DMEAudioIdent : public AudioIdent {
public:
    DMEAudioIdent( const std::string & fx_name );
};

class VORAudioIdent : public AudioIdent {
public:
    VORAudioIdent( const std::string & fx_name );
};

class LOCAudioIdent : public AudioIdent {
public:
    LOCAudioIdent( const std::string & fx_name );
};
