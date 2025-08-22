// ATC-Main.hxx -- FGFS interface to ATC 610x hardware
//
// Written by Curtis Olson, started January 2002.
//
// SPDX-FileCopyrightText: (C) 2002  Curtis L. Olson - http://www.flightgear.org/~curt
// SPDX-License-Identifier: GPL-2.0-or-later


#pragma once

#include <config.h>

#include <simgear/misc/sg_path.hxx>

#include <Main/fg_props.hxx>

#include "protocol.hxx"

#include "ATC-Inputs.hxx"
#include "ATC-Outputs.hxx"


class FGATCMain : public FGProtocol {

    FGATCInput *input0;         // board0 input interface class
    FGATCInput *input1;         // board1 input interface class
    FGATCOutput *output0;       // board0 output interface class
    FGATCOutput *output1;       // board1 output interface class

    SGPath input0_path;
    SGPath input1_path;
    SGPath output0_path;
    SGPath output1_path;

    int lock0_fd;
    int lock1_fd;

public:
    FGATCMain() : input0(nullptr),
                  input1(nullptr),
                  output0(nullptr),
                  output1(nullptr)
    { }

    ~FGATCMain() {
        delete input0;
        delete input1;
        delete output0;
        delete output1;
    }

    // Open and initialize ATC 610x hardware
    bool open();

    void init_config();

    bool process();

    bool close();

    inline void set_path_names( const SGPath &in0, const SGPath &in1,
                                const SGPath &out0, const SGPath &out1 )
    {
        input0_path = in0;
        input1_path = in1;
        output0_path = out0;
        output1_path = out1;
    }
};
