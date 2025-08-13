// soundgenerator.hxx -- simple sound generation
//
// Written by Curtis Olson, started March 2001.
//
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2001 Curtis L. Olson

#pragma once

class FGSoundGenerator {

public:
    static const int BYTES_PER_SECOND = 22050;
    // static const int BEAT_LENGTH = 240; // milleseconds (5 wpm)
    static const int BEAT_LENGTH = 92;  // milleseconds (13 wpm)
    static const int TRANSITION_BYTES = BYTES_PER_SECOND/200; // aka (int)(0.005 * BYTES_PER_SECOND);
    static const int COUNT_SIZE = BYTES_PER_SECOND * BEAT_LENGTH / 1000;
    static const int DIT_SIZE = 2 * COUNT_SIZE;   // 2 counts
    static const int DAH_SIZE = 4 * COUNT_SIZE;   // 4 counts
    static const int SPACE_SIZE = 3 * COUNT_SIZE; // 3 counts
    static const int LO_FREQUENCY = 1020;	 // AIM 1-1-7 (f) specified in Hz
    static const int HI_FREQUENCY = 1350;	 // AIM 1-1-7 (f) specified in Hz

protected:
    /**
    * \relates FGMorse
    * Make a tone of specified freq and total_len with trans_len ramp in
    * and out and only the first len bytes with sound, the rest with
    * silence.
    * @param buf unsigned char pointer to sound buffer
    * @param freq desired frequency of tone
    * @param len length of tone within sound
    * @param total_len total length of sound (anything more than len is padded
    *        with silence.
    * @param trans_len length of ramp up and ramp down to avoid audio "pop"
    */
    static void make_tone(unsigned char* buf, int freq,
                          int len, int total_len, int trans_len);

public:

    virtual ~FGSoundGenerator();
};
