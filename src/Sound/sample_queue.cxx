// _samplequeue.cxx -- Sound effect management class implementation
//
// Started by David Megginson, October 2001
// (Reuses some code from main.cxx, probably by Curtis Olson)
//
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2001 David Megginson

#ifdef _MSC_VER
#pragma warning (disable: 4786)
#endif

#include <config.h>

#include "sample_queue.hxx"

#include <Main/fg_props.hxx>

#include <simgear/sound/soundmgr.hxx>
#include <simgear/sound/sample.hxx>

FGSampleQueue::FGSampleQueue ( SGSoundMgr *smgr, const std::string &refname ) :
    last_enabled( true ),
    last_volume( 0.0 ),
    _enabled( fgGetNode("/sim/sound/"+refname+"/enabled", true) ),
    _volume( fgGetNode("/sim/sound/"+refname+"/volume", true) )
{
    SGSampleGroup::_smgr = smgr;
    SGSampleGroup::_smgr->add(this, refname);
    SGSampleGroup::_refname = refname;
}


FGSampleQueue::~FGSampleQueue ()
{
}


void
FGSampleQueue::update (double dt)
{
    // command sound manager
    bool new_enabled = _enabled->getBoolValue();
    if ( new_enabled != last_enabled ) {
        if ( new_enabled ) {
            resume();
        } else {
            suspend();
        }
        last_enabled = new_enabled;
    }

    if ( new_enabled ) {
        double volume = _volume->getDoubleValue();
        if ( volume != last_volume ) {
            set_volume( volume );
            last_volume = volume;
        }

        // process message queue
        const std::string msgid = "Sequential Audio Message";
        bool now_playing = false;
        if ( exists( msgid ) ) {
            now_playing = is_playing( msgid );
            if ( !now_playing ) {
                // current message finished, stop and remove
                stop( msgid );   // removes source
                remove( msgid ); // removes buffer
            }
        }

        if ( !now_playing ) {
            // message queue idle, add next sound if we have one
            if ( ! _messages.empty() ) {
                SGSampleGroup::add( _messages.front(), msgid );
                _messages.pop();
                play_once( msgid );
            }
        }

        SGSampleGroup::update(dt);
    }
}

// end of _samplequeue.cxx
