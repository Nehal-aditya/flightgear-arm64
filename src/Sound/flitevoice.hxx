// speech synthesis interface subsystem
//
// Written by Torsten Dreyer, started April 2014
//
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2014 Torsten Dreyer

#pragma once

#include "voice.hxx"
#include <simgear/sound/soundmgr.hxx>
#include <simgear/sound/sample.hxx>
#include <simgear/threads/SGQueue.hxx>

class VoiceSynthesizer;

class FGFLITEVoice: public FGVoiceMgr::FGVoice {
public:
  FGFLITEVoice(FGVoiceMgr *, const SGPropertyNode_ptr, const char * sampleGroupRefName = "flite-voice");
  virtual ~FGFLITEVoice();
  virtual void speak(const std::string & msg);
  virtual void update(double dt);

private:
  FGFLITEVoice(const FGFLITEVoice & other);
  FGFLITEVoice & operator =(const FGFLITEVoice & other);

  SGSharedPtr<SGSampleGroup> _sgr;
  VoiceSynthesizer * _synthesizer;
  SGLockedQueue<SGSharedPtr<SGSoundSample> > _sampleQueue;
  std::string _sampleName;
  double _seconds_to_run;
};
