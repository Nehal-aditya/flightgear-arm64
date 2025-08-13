/*
 * VoiceSynthesizer.hxx - wraps flite+hts_engine
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 * SPDX-FileCopyrightText: 2014 Torsten Dreyer
 */

#pragma once

#include <simgear/sound/sample.hxx>
#include <simgear/threads/SGQueue.hxx>

#include <string>
struct _Flite_HTS_Engine;

/**
 * A Voice Synthesizer Interface
 */
class VoiceSynthesizer {
public:
  virtual ~VoiceSynthesizer() {};
  virtual SGSoundSample * synthesize( const std::string & text, double volume, double speed, double pitch ) = 0;
};

class SoundSampleReadyListener {
public:
  virtual ~SoundSampleReadyListener() {}
  virtual void SoundSampleReady( SGSharedPtr<SGSoundSample> ) = 0;
};

struct SynthesizeRequest {
  SynthesizeRequest() {
    speed = 0.5;
    volume = 1.0;
    pitch = 0.5;
    listener = NULL;
  }
  SynthesizeRequest( const SynthesizeRequest & other ) {
    text = other.text;
    speed = other.speed;
    volume = other.volume;
    pitch = other.pitch;
    listener = other.listener;
  }

  SynthesizeRequest & operator = ( const SynthesizeRequest & other ) {
    text = other.text;
    speed = other.speed;
    volume = other.volume;
    pitch = other.pitch;
    listener = other.listener;
    return *this;
  }

  // return a special marker request used to indicate the synthesis thread
  // should be exited.
  static SynthesizeRequest cancelThreadRequest();

  std::string text;
  double speed;
  double volume;
  double pitch;
  SoundSampleReadyListener * listener;
};

/**
 * A Voice Synthesizer using FLITE+HTS
 */
class FLITEVoiceSynthesizer : public VoiceSynthesizer {
public:

  typedef enum {
    CMU_US_ARCTIC_SLT = 0,
    CSTR_UK_FEMALE,

    VOICE_UNKNOWN // keep this at the end
  } voice_t;

  static std::string getVoicePath( voice_t voice );
  static std::string getVoicePath( const std::string & voice );

  FLITEVoiceSynthesizer( const std::string & voice );
  ~FLITEVoiceSynthesizer();
  virtual SGSoundSample * synthesize( const std::string & text, double volume, double speed, double pitch  );

  virtual void synthesize( SynthesizeRequest & request );
private:
  struct _Flite_HTS_Engine * _engine;

  class WorkerThread;
  WorkerThread * _worker;

  typedef SGBlockingQueue<SynthesizeRequest> SynthesizeRequestList;
  SynthesizeRequestList _requests;

  double _volume;
};
