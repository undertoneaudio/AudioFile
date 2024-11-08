//=======================================================================
/** @file AudioFile.h
 *  @author Adam Stark
 *  @copyright Copyright (C) 2017  Adam Stark
 *
 * This file is part of the 'AudioFile' library
 *
 * MIT License
 *
 * Copyright (c) 2017 Adam Stark
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy 
 * of this software and associated documentation files (the "Software"), to deal 
 * in the Software without restriction, including without limitation the rights 
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
 * of the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all 
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
//=======================================================================

#pragma once
#include "AudioFileTypes.h"

#if defined (_MSC_VER)
#undef max
#undef min
#define NOMINMAX
#endif

#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <cstring>
#include <fstream>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include <limits>

// disable some warnings on Windows
#if defined (_MSC_VER)
    __pragma(warning (push))
    __pragma(warning (disable : 4244))
    __pragma(warning (disable : 4457))
    __pragma(warning (disable : 4458))
    __pragma(warning (disable : 4389))
    __pragma(warning (disable : 4996))
#elif defined (__GNUC__)
    _Pragma("GCC diagnostic push")
    _Pragma("GCC diagnostic ignored \"-Wconversion\"")
    _Pragma("GCC diagnostic ignored \"-Wsign-compare\"")
    _Pragma("GCC diagnostic ignored \"-Wshadow\"")
#endif


//=============================================================
template <typename T>
struct AudioSampleConverter
{
    //=============================================================
    /** Convert a signed 8-bit integer to an audio sample */
    static T signedByteToSample (int8_t sample);
    
    /** Convert an audio sample to an signed 8-bit representation */
    static int8_t sampleToSignedByte (T sample);
    
    //=============================================================
    /** Convert an unsigned 8-bit integer to an audio sample */
    static T unsignedByteToSample (uint8_t sample);
    
    /** Convert an audio sample to an unsigned 8-bit representation */
    static uint8_t sampleToUnsignedByte (T sample);
    
    //=============================================================
    /** Convert a 16-bit integer to an audio sample */
    static T sixteenBitIntToSample (int16_t sample);
    
    /** Convert a an audio sample to a 16-bit integer */
    static int16_t sampleToSixteenBitInt (T sample);
    
    //=============================================================
    /** Convert a 24-bit value (int a 32-bit int) to an audio sample */
    static T twentyFourBitIntToSample (int32_t sample);
    
    /** Convert a an audio sample to a 24-bit value (in a 32-bit integer) */
    static int32_t sampleToTwentyFourBitInt (T sample);
    
    //=============================================================
    /** Convert a 32-bit signed integer to an audio sample */
    static T thirtyTwoBitIntToSample (int32_t sample);
    
    /** Convert a an audio sample to a 32-bit signed integer */
    static int32_t sampleToThirtyTwoBitInt (T sample);
    
    //=============================================================
    /** Helper clamp function to enforce ranges */
    static T clamp (T v1, T minValue, T maxValue);
};


#if defined (_MSC_VER)
    __pragma(warning (pop))
#elif defined (__GNUC__)
    _Pragma("GCC diagnostic pop")
#endif

#include "AudioSampleConverter.inl"