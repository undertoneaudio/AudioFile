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
#include "AudioSampleConverter.h"

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
template <class T>
class AudioFile
{
public:
    
    //=============================================================
    typedef std::vector<std::vector<T> > AudioBuffer;
    
    //=============================================================
    /** Constructor */
    AudioFile();
    
    /** Constructor, using a given file path to load a file */
    AudioFile (std::string filePath);
        
    //=============================================================
    /** Loads an audio file from a given file path.
     * @Returns true if the file was successfully loaded
     */
    bool load (std::string filePath);

    /** Saves an audio file to a given file path.
     * @Returns true if the file was successfully saved
     */
    bool save (std::string filePath, AudioFileFormat format = AudioFileFormat::Wave);
        
    //=============================================================
    /** Loads an audio file from data in memory */
    bool loadFromMemory (std::vector<uint8_t>& fileData);
    
    //=============================================================
    /** @Returns the sample rate */
    uint32_t getSampleRate() const;
    
    /** @Returns the number of audio channels in the buffer */
    int getNumChannels() const;

    /** @Returns true if the audio file is mono */
    bool isMono() const;
    
    /** @Returns true if the audio file is stereo */
    bool isStereo() const;
    
    /** @Returns the bit depth of each sample */
    int getBitDepth() const;
    
    /** @Returns the number of samples per channel */
    int getNumSamplesPerChannel() const;
    
    /** @Returns the length in seconds of the audio file based on the number of samples and sample rate */
    double getLengthInSeconds() const;
    
    /** Prints a summary of the audio file to the console */
    void printSummary() const;
    
    //=============================================================
    
    /** Set the audio buffer for this AudioFile by copying samples from another buffer.
     * @Returns true if the buffer was copied successfully.
     */
    bool setAudioBuffer (AudioBuffer& newBuffer);
    
    /** Sets the audio buffer to a given number of channels and number of samples per channel. This will try to preserve
     * the existing audio, adding zeros to any new channels or new samples in a given channel.
     */
    void setAudioBufferSize (int numChannels, int numSamples);
    
    /** Sets the number of samples per channel in the audio buffer. This will try to preserve
     * the existing audio, adding zeros to new samples in a given channel if the number of samples is increased.
     */
    void setNumSamplesPerChannel (int numSamples);
    
    /** Sets the number of channels. New channels will have the correct number of samples and be initialised to zero */
    void setNumChannels (int numChannels);
    
    /** Sets the bit depth for the audio file. If you use the save() function, this bit depth rate will be used */
    void setBitDepth (int numBitsPerSample);
    
    /** Sets the sample rate for the audio file. If you use the save() function, this sample rate will be used */
    void setSampleRate (uint32_t newSampleRate);
    
    //=============================================================
    /** Sets whether the library should log error messages to the console. By default this is true */
    void shouldLogErrorsToConsole (bool logErrors);
    
    //=============================================================
    /** A vector of vectors holding the audio samples for the AudioFile. You can 
     * access the samples by channel and then by sample index, i.e:
     *
     *      samples[channel][sampleIndex]
     */
    AudioBuffer samples;
    
    //=============================================================
    /** An optional iXML chunk that can be added to the AudioFile. 
     */
    std::string iXMLChunk;
    
private:
        
    //=============================================================
    AudioFileFormat determineAudioFileFormat (std::vector<uint8_t>& fileData);
    bool decodeWaveFile (std::vector<uint8_t>& fileData);
    bool decodeAiffFile (std::vector<uint8_t>& fileData);
    
    //=============================================================
    bool saveToWaveFile (std::string filePath);
    bool saveToAiffFile (std::string filePath);
    
    //=============================================================
    void clearAudioBuffer();
    
    //=============================================================
    int32_t fourBytesToInt (std::vector<uint8_t>& source, int startIndex, Endianness endianness = Endianness::LittleEndian);
    int16_t twoBytesToInt (std::vector<uint8_t>& source, int startIndex, Endianness endianness = Endianness::LittleEndian);
    int getIndexOfString (std::vector<uint8_t>& source, std::string s);
    int getIndexOfChunk (std::vector<uint8_t>& source, const std::string& chunkHeaderID, int startIndex, Endianness endianness = Endianness::LittleEndian);

    //=============================================================
    uint32_t getAiffSampleRate (std::vector<uint8_t>& fileData, int sampleRateStartIndex);
    bool tenByteMatch (std::vector<uint8_t>& v1, int startIndex1, std::vector<uint8_t>& v2, int startIndex2);
    void addSampleRateToAiffData (std::vector<uint8_t>& fileData, uint32_t sampleRate);
    
    //=============================================================
    void addStringToFileData (std::vector<uint8_t>& fileData, std::string s);
    void addInt32ToFileData (std::vector<uint8_t>& fileData, int32_t i, Endianness endianness = Endianness::LittleEndian);
    void addInt16ToFileData (std::vector<uint8_t>& fileData, int16_t i, Endianness endianness = Endianness::LittleEndian);
    
    //=============================================================
    bool writeDataToFile (std::vector<uint8_t>& fileData, std::string filePath);
    
    //=============================================================
    void reportError (std::string errorMessage);
    
    //=============================================================
    AudioFileFormat audioFileFormat;
    uint32_t sampleRate;
    int bitDepth;
    bool logErrorsToConsole {true};
};


#if defined (_MSC_VER)
    __pragma(warning (pop))
#elif defined (__GNUC__)
    _Pragma("GCC diagnostic pop")
#endif

#include "AudioFile.inl"