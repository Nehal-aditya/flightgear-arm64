// sample_queue.hxx -- sample queue management class
//
// Started by David Megginson, October 2001
// (Reuses some code from main.cxx, probably by Curtis Olson)
//
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2001 David Megginson

#pragma once

#include <simgear/compiler.h>

#include <queue>

#include <simgear/structure/subsystem_mgr.hxx>
#include <simgear/props/props.hxx>
#include <simgear/sound/sample_group.hxx>

class SGSoundSample;

/**
 * FlightGear sample queue class
 *
 *    This modules maintains a queue of 'message' audio files.  These
 *    are played sequentially with no overlap until the queue is finished.
 *    This second mechanisms is useful for things like tutorial messages or
 *    background ATC chatter.
 */
class FGSampleQueue : public SGSampleGroup
{

public:

    FGSampleQueue ( SGSoundMgr *smgr, const std::string &refname );
    virtual ~FGSampleQueue ();

    virtual void update (double dt);

    inline void add (SGSharedPtr<SGSoundSample> msg) { _messages.push(msg); }

private:

    std::queue< SGSharedPtr<SGSoundSample> > _messages;

    bool last_enabled;
    double last_volume;

    SGPropertyNode_ptr _enabled;
    SGPropertyNode_ptr _volume;
};
