// soundgenerator.cxx -- simple sound generation
//
// Written by Curtis Olson, started March 2001.
//
// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2001 Curtis L. Olson

#include "config.h"

#include "soundgenerator.hxx"
#include <simgear/constants.h>

FGSoundGenerator::~FGSoundGenerator() = default;

// Make a tone of specified freq and total_len with trans_len ramp in
// and out and only the first len bytes with sound, the rest with
// silence
void FGSoundGenerator::make_tone(unsigned char* buf, int freq,
                                 int len, int total_len, int trans_len)
{
    int i, j;

    for ( i = 0; i < trans_len; ++i ) {
        float level = ( sin( (double) i * SGD_2PI / (BYTES_PER_SECOND / freq) ) )
            * ((double)i / trans_len) / 2.0 + 0.5;

        /* Convert to unsigned byte */
        buf[ i ] = (unsigned char) ( level * 255.0 ) ;
    }

    for ( i = trans_len; i < len - trans_len; ++i ) {
        float level = ( sin( (double) i * SGD_2PI / (BYTES_PER_SECOND / freq) ) )
            / 2.0 + 0.5;

        /* Convert to unsigned byte */
        buf[ i ] = (unsigned char) ( level * 255.0 ) ;
    }
    j = trans_len;
    for ( i = len - trans_len; i < len; ++i ) {
        float level = ( sin( (double) i * SGD_2PI / (BYTES_PER_SECOND / freq) ) )
            * ((double)j / trans_len) / 2.0 + 0.5;
        --j;

        /* Convert to unsigned byte */
        buf[ i ] = (unsigned char) ( level * 255.0 ) ;
    }
    for ( i = len; i < total_len; ++i ) {
        buf[ i ] = (unsigned char) ( 0.5 * 255.0 ) ;
    }
}
