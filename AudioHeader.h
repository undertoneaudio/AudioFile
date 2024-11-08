#pragma once

#include "AudioFileTypes.h"
#include "AudioSampleConverter.h"
#include <iostream>
#include <fstream>
#include <string>


template <class T>
class AudioHeader
{
public:
    AudioHeader();

    /** Loads only the header information from an audio file at a given file path.
     *  This function does not load the entire audio data into memory.
     *  @Returns true if the header was successfully loaded.
     */
    bool loadHeader(std::string filePath);

    /** @Returns the sample rate */
    uint32_t getSampleRate() const;

    /** @Returns the number of audio channels */
    int getNumChannels() const;

    /** @Returns the bit depth of each sample */
    int getBitDepth() const;

    /** @Returns the audio file format */
    AudioFileFormat getAudioFormat() const;

    /** Sets whether the library should log error messages to the console. By default, this is true */
    void shouldLogErrorsToConsole(bool logErrors);

private:
    // Header parsing functions
    bool decodeWaveFileHeader(std::ifstream& file);
    bool decodeAiffFileHeader(std::ifstream& file);

    // Member variables
    uint32_t sampleRate;
    int bitDepth;
    int numChannels;
    AudioFileFormat audioFileFormat;
    bool logErrorsToConsole{ true };

    // Helper functions
    void reportError(std::string errorMessage);
    uint32_t fourBytesToInt(char* bytes, Endianness endianness);
    uint16_t twoBytesToInt(char* bytes, Endianness endianness);
};

#include <AudioHeader.inl>
