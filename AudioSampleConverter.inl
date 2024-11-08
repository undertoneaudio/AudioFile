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
template <typename SignedType>
typename std::make_unsigned<SignedType>::type convertSignedToUnsigned (SignedType signedValue)
{
    static_assert (std::is_signed<SignedType>::value, "The input value must be signed");
    
    typename std::make_unsigned<SignedType>::type unsignedValue = static_cast<typename std::make_unsigned<SignedType>::type> (1) + std::numeric_limits<SignedType>::max();
    
    unsignedValue += signedValue;
    return unsignedValue;
}


//=============================================================
template <class T>
T AudioSampleConverter<T>::thirtyTwoBitIntToSample (int32_t sample)
{
    if (std::is_floating_point<T>::value)
    {
        return static_cast<T> (sample) / static_cast<T> (std::numeric_limits<int32_t>::max());
    }
    else if (std::numeric_limits<T>::is_integer)
    {
        if (std::is_signed<T>::value)
            return static_cast<T> (sample);
        else
            return static_cast<T> (clamp (static_cast<T> (sample + 2147483648), 0, 4294967295));
    }
}

//=============================================================
template <class T>
int32_t AudioSampleConverter<T>::sampleToThirtyTwoBitInt (T sample)
{
    if (std::is_floating_point<T>::value)
    {
        // multiplying a float by a the max int32_t is problematic because
        // of roundng errors which can cause wrong values to come out, so
        // we use a different implementation here compared to other types
        if (std::is_same<T, float>::value)
        {
            if (sample >= 1.f)
                return std::numeric_limits<int32_t>::max();
            else if (sample <= -1.f)
                return std::numeric_limits<int32_t>::lowest() + 1; // starting at 1 preserves symmetry
            else
                return static_cast<int32_t> (sample * std::numeric_limits<int32_t>::max());
        }
        else
        {
            return static_cast<int32_t> (clamp (sample, -1., 1.) * std::numeric_limits<int32_t>::max());
        }
    }
    else
    {
        if (std::is_signed<T>::value)
            return static_cast<int32_t> (clamp (sample, -2147483648LL, 2147483647LL));
        else
            return static_cast<int32_t> (clamp (sample, 0, 4294967295) - 2147483648);
    }
}

//=============================================================
template <class T>
T AudioSampleConverter<T>::twentyFourBitIntToSample (int32_t sample)
{
    if (std::is_floating_point<T>::value)
    {
        return static_cast<T> (sample) / static_cast<T> (8388607.);
    }
    else if (std::numeric_limits<T>::is_integer)
    {
        if (std::is_signed<T>::value)
            return static_cast<T> (clamp (sample, SignedInt24_Min, SignedInt24_Max));
        else
            return static_cast<T> (clamp (sample + 8388608, UnsignedInt24_Min, UnsignedInt24_Max));
    }
}

//=============================================================
template <class T>
int32_t AudioSampleConverter<T>::sampleToTwentyFourBitInt (T sample)
{
    if (std::is_floating_point<T>::value)
    {
        sample = clamp (sample, -1., 1.);
        return static_cast<int32_t> (sample * 8388607.);
    }
    else
    {
        if (std::is_signed<T>::value)
            return static_cast<int32_t> (clamp (sample, SignedInt24_Min, SignedInt24_Max));
        else
            return static_cast<int32_t> (clamp (sample, UnsignedInt24_Min, UnsignedInt24_Max) + SignedInt24_Min);
    }
}

//=============================================================
template <class T>
T AudioSampleConverter<T>::sixteenBitIntToSample (int16_t sample)
{
    if (std::is_floating_point<T>::value)
    {
        return static_cast<T> (sample) / static_cast<T> (32767.);
    }
    else if (std::numeric_limits<T>::is_integer)
    {
        if (std::is_signed<T>::value)
            return static_cast<T> (sample);
        else
            return static_cast<T> (convertSignedToUnsigned<int16_t> (sample));
    }
}

//=============================================================
template <class T>
int16_t AudioSampleConverter<T>::sampleToSixteenBitInt (T sample)
{
    if (std::is_floating_point<T>::value)
    {
        sample = clamp (sample, -1., 1.);
        return static_cast<int16_t> (sample * 32767.);
    }
    else
    {
        if (std::is_signed<T>::value)
            return static_cast<int16_t> (clamp (sample, SignedInt16_Min, SignedInt16_Max));
        else
            return static_cast<int16_t> (clamp (sample, UnsignedInt16_Min, UnsignedInt16_Max) + SignedInt16_Min);
    }
}

//=============================================================
template <class T>
uint8_t AudioSampleConverter<T>::sampleToUnsignedByte (T sample)
{
    if (std::is_floating_point<T>::value)
    {
        sample = clamp (sample, -1., 1.);
        sample = (sample + 1.) / 2.;
        return static_cast<uint8_t> (1 + (sample * 254));
    }
    else
    {
        if (std::is_signed<T>::value)
            return static_cast<uint8_t> (clamp (sample, -128, 127) + 128);
        else
            return static_cast<uint8_t> (clamp (sample, 0, 255));
    }
}

//=============================================================
template <class T>
int8_t AudioSampleConverter<T>::sampleToSignedByte (T sample)
{
    if (std::is_floating_point<T>::value)
    {
        sample = clamp (sample, -1., 1.);
        return static_cast<int8_t> (sample * (T)0x7F);
    }
    else
    {
        if (std::is_signed<T>::value)
            return static_cast<int8_t> (clamp (sample, -128, 127));
        else
            return static_cast<int8_t> (clamp (sample, 0, 255) - 128);
    }
}

//=============================================================
template <class T>
T AudioSampleConverter<T>::unsignedByteToSample (uint8_t sample)
{
    if (std::is_floating_point<T>::value)
    {
        return static_cast<T> (sample - 128) / static_cast<T> (127.);
    }
    else if (std::numeric_limits<T>::is_integer)
    {
        if (std::is_unsigned<T>::value)
            return static_cast<T> (sample);
        else
            return static_cast<T> (sample - 128);
    }
}

//=============================================================
template <class T>
T AudioSampleConverter<T>::signedByteToSample (int8_t sample)
{
    if (std::is_floating_point<T>::value)
    {
        return static_cast<T> (sample) / static_cast<T> (127.);
    }
    else if (std::numeric_limits<T>::is_integer)
    {
        if (std::is_signed<T>::value)
            return static_cast<T> (sample);
        else
            return static_cast<T> (convertSignedToUnsigned<int8_t> (sample));
    }
}

//=============================================================
template <class T>
T AudioSampleConverter<T>::clamp (T value, T minValue, T maxValue)
{
    value = std::min (value, maxValue);
    value = std::max (value, minValue);
    return value;
}

#if defined (_MSC_VER)
    __pragma(warning (pop))
#elif defined (__GNUC__)
    _Pragma("GCC diagnostic pop")
#endif

