// replay.hxx - a system to record and replay FlightGear flights
//
// Written by Curtis Olson, started July 2003.
//
// SPDX-FileCopyrightText: Copyright (C) 2003  Curtis L. Olson
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/structure/subsystem_mgr.hxx>
#include <simgear/io/iostreams/gzcontainerfile.hxx>
#include <simgear/io/HTTPFileRequest.hxx>

#include <MultiPlayer/multiplaymgr.hxx>


/* A recording/replay module for FlightGear flights. */
struct FGReplay : SGSubsystem
{
    FGReplay ();
    virtual ~FGReplay();

    /* Subsystem API. */
    void bind() override;
    void init() override;
    void reinit() override;
    void unbind() override;
    void update(double dt) override;

    /* Subsystem identification. */
    static const char* staticSubsystemClassId() { return "replay"; }

    /* For built-in 'replay' command - replay using in-memory Normal recording.

        new_tape: If true, we start at beginning of tape, otherwise we start at
        loop interval.
    */
    bool start(bool new_tape=false);

    /* For save and load tape operations from Flightgear GUI. */
    bool saveTape(const SGPropertyNode* ConfigData);
    bool loadTape(const SGPropertyNode* ConfigData);

    /* Attempts to load Continuous recording header properties into
    <properties>. If in is null we use internal std::fstream, otherwise we use
    *in.

    Returns 0 on success, +1 if we may succeed after further download, or -1 if
    recording is not a Continuous recording.

    For command line --load-tape=...
    */
    static int loadContinuousHeader(const std::string& path, std::istream* in, SGPropertyNode* properties);

    /* Start replaying a flight recorder tape from disk.
        filename
            Path of recording.
        preview
            If true we read the header (and return it in <meta_meta> but do not
            start replaying.
        create_video
            If true we automatically encode a video while replaying.
        fixed_dt
            If non-zero we set /sim/time/fixed-dt while replaying.
        meta_meta
            Filled in with contents of recording header's "meta" tree.
        filerequest
            If not null we use this to get called back as download of file
            progresses, so that we can index the recording. Only useful for
            Continuous recordings.
    */
    bool loadTape(
            const SGPath& filename,
            bool preview,
            bool create_video,
            double fixed_dt,
            SGPropertyNode& meta_meta,
            simgear::HTTP::FileRequestRef file_request=nullptr
            );

    /* Prepends /sim/replay/tape-directory and/or appends .fgtape etc.

     For command line --load-tape=... */
    static std::string  makeTapePath(const std::string& tape_name);

    /* Resets out static property nodes; to be called by fgStartNewReset(). */
    static void resetStatisticsProperties();

    std::unique_ptr<struct FGReplayInternal>    m_internal;
};
