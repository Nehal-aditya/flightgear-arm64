//
// AircraftDirVisitorBase.hxx - helper to traverse a hierarchy containing
// aircraft dirs
//
// Written by Curtis Olson, started August 1997.
//
// SPDX-FileCopyrightText: 1997 Curtis L. Olson
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <simgear/misc/sg_dir.hxx>
#include <simgear/misc/sg_path.hxx>

#include <Main/globals.hxx>

class AircraftDirVistorBase
{
public:
protected:
    enum VisitResult {
        VISIT_CONTINUE = 0,
        VISIT_DONE,
        VISIT_ERROR
    };

    AircraftDirVistorBase() :
        _maxDepth(2)
    {
    }

    VisitResult visitAircraftPaths()
    {
        const simgear::PathList& paths(globals->get_aircraft_paths());
        simgear::PathList::const_iterator it = paths.begin();
        for (; it != paths.end(); ++it) {
            VisitResult vr = visitDir(*it, 0);
            if (vr != VISIT_CONTINUE) {
                return vr;
            }
        } // of aircraft paths iteration

        // if we reach this point, search the default location (always last)
        SGPath rootAircraft(globals->get_fg_root());
        rootAircraft.append("Aircraft");
        return visitDir(rootAircraft, 0);
    }

    VisitResult visitPath(const SGPath& path, unsigned int depth)
    {
        if (!path.exists()) {
            return VISIT_ERROR;
        }

        return visit(path);
    }

    VisitResult visitDir(const simgear::Dir& d, unsigned int depth)
    {
        if (!d.exists()) {
            SG_LOG(SG_GENERAL, SG_WARN, "visitDir: no such path:" << d.path());
            return VISIT_CONTINUE;
        }

        if (depth >= _maxDepth) {
            return VISIT_CONTINUE;
        }

        bool recurse = true;
        simgear::PathList setFiles(d.children(simgear::Dir::TYPE_FILE, "-set.xml"));
        simgear::PathList::iterator p;
        for (p = setFiles.begin(); p != setFiles.end(); ++p) {
            // if we found a -set.xml at this level, don't recurse any deeper
            recurse = false;
            VisitResult vr = visit(*p);
            if (vr != VISIT_CONTINUE) {
                return vr;
            }
        } // of -set.xml iteration

        if (!recurse) {
            return VISIT_CONTINUE;
        }

        simgear::PathList subdirs(d.children(simgear::Dir::TYPE_DIR | simgear::Dir::NO_DOT_OR_DOTDOT));
        for (p = subdirs.begin(); p != subdirs.end(); ++p) {
            VisitResult vr = visitDir(*p, depth + 1);
            if (vr != VISIT_CONTINUE) {
                return vr;
            }
        }

        return VISIT_CONTINUE;
    } // of visitDir method

    virtual VisitResult visit(const SGPath& path) = 0;

private:
    unsigned int _maxDepth;
};
