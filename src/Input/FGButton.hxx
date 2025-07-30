// FGButton.hxx -- a simple button/key wrapper class
//
// Written by Torsten Dreyer, started August 2009
// Based on work from David Megginson, started May 2001.
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2001 David Megginson <david@megginson.com>
// SPDX-FileCopyrightText: 2009 Torsten Dreyer

#pragma once

#include "FGCommonInput.hxx"
#include <Main/fg_os.hxx> // for KEYMOD_MAX

class FGButton : public FGCommonInput {
public:
  FGButton();
  virtual ~FGButton();
  void init(const SGPropertyNode* node, const std::string& name,
            const std::string& module);
  void update( int modifiers, bool pressed, int x = -1, int y = -1);
  bool is_repeatable;
  float interval_sec, delay_sec, release_delay_sec;
  float last_dt;
  int last_state;
  binding_list_t bindings[KEYMOD_MAX];
};
