// FGKeyboardInput.hxx -- handle user input from keyboard devices
//
// Written by Torsten Dreyer, started August 2009
// Based on work from David Megginson, started May 2001.
//
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2001 David Megginson <david@megginson.com>
// SPDX-FileCopyrightText: 2009 Torsten Dreyer

#pragma once

#include "FGCommonInput.hxx"
#include "FGButton.hxx"
#include <simgear/structure/subsystem_mgr.hxx>
#include <simgear/props/tiedpropertylist.hxx>

////////////////////////////////////////////////////////////////////////
// The Keyboard Input Class
////////////////////////////////////////////////////////////////////////
class FGKeyboardInput : public SGSubsystem,
                        FGCommonInput
{
public:
    FGKeyboardInput();
    virtual ~FGKeyboardInput();

    // Subsystem API.
    void bind() override;
    void init() override;
    void postinit() override;
    void unbind() override;
    void update(double dt) override;

    // Subsystem identification.
    static const char* staticSubsystemClassId() { return "input-keyboard"; }

    static const int MAX_KEYS = 1024;

private:
    const binding_list_t& _find_key_bindings (unsigned int k, int modifiers);
    void doKey (int k, int modifiers, int x, int y);

    static void keyHandler(int key, int keymod, int mousex, int mousey);
    static FGKeyboardInput * keyboardInput;
    FGButton bindings[MAX_KEYS];
    SGPropertyNode_ptr _key_event;
    int  _key_code;
    int  _key_modifiers;
    bool _key_pressed;
    bool _key_shift;
    bool _key_ctrl;
    bool _key_alt;
    bool _key_meta;
    bool _key_super;
    bool _key_hyper;
    simgear::TiedPropertyList _tiedProperties;
};
