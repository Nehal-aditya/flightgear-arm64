// replay.cxx - a system to record and replay FlightGear flights
//
// Written by Curtis Olson, started July 2003.
// Updated by Thorsten Brehm, September 2011 and November 2012.
//
// SPDX-FileCopyrightText: Copyright (C) 2003  Curtis L. Olson
// SPDX-License-Identifier: GPL-2.0-or-later


#include "replay.hxx"
#include "replay-internal.hxx"


FGReplay::FGReplay()
:
m_internal(new FGReplayInternal)
{
}

FGReplay::~FGReplay()
{
}

void
FGReplay::init()
{
    m_internal->init();
}

void
FGReplay::reinit()
{
    m_internal->reinit();
}

void
FGReplay::bind()
{
    m_internal->bind();
}

void
FGReplay::unbind()
{
    m_internal->unbind();
}


/** Start replay session
 */
bool
FGReplay::start(bool NewTape)
{
    return m_internal->start(NewTape);
}

void
FGReplay::update( double dt )
{
    timingInfo.clear();
    stamp("begin");
    m_internal->update(dt);
}


/** Write flight recorder tape to disk. User/script command. */
bool
FGReplay::saveTape(const SGPropertyNode* Extra)
{
    return m_internal->saveTape(Extra);
}


bool
FGReplay::loadTape(
        const SGPath& filename,
        bool preview,
        bool create_video,
        double fixed_dt,
        SGPropertyNode& meta_meta,
        simgear::HTTP::FileRequestRef file_request
        )
{
    return m_internal->loadTape(filename, preview, create_video, fixed_dt, meta_meta, file_request);
}


std::string  FGReplay::makeTapePath(const std::string& tape_name)
{
    return FGReplayInternal::makeTapePath(tape_name);
}

int FGReplay::loadContinuousHeader(const std::string& path, std::istream* in, SGPropertyNode* properties)
{
    return FGReplayInternal::loadContinuousHeader(path, in, properties);
}

/** Load a flight recorder tape from disk. User/script command. */
bool
FGReplay::loadTape(const SGPropertyNode* ConfigData)
{
    return m_internal->loadTape(ConfigData);
}
void FGReplay::resetStatisticsProperties()
{
    FGReplayData::resetStatisticsProperties();
}


// Register the subsystem.
SGSubsystemMgr::Registrant<FGReplay> registrantFGReplay(
    SGSubsystemMgr::POST_FDM);
