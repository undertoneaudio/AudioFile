#pragma once

template <class T>
AudioHeader<T>::AudioHeader()
{
    sampleRate = 0;
    bitDepth = 0;
    numChannels = 0;
    audioFileFormat = AudioFileFormat::NotLoaded;
}

template <class T>
bool AudioHeader<T>::loadHeader(std::string filePath)
{
    std::ifstream file(filePath, std::ios::binary);

    if (!file.good())
    {
        reportError("ERROR: File doesn't exist or can't be opened: " + filePath);
        return false;
    }

    // Read the first 12 bytes to determine the file format
    char header[12];
    file.read(header, 12);
    if (file.gcount() != 12)
    {
        reportError("ERROR: Couldn't read the file header: " + filePath);
        return false;
    }

    std::string chunkID(header, 4);
    std::string format(header + 8, 4);

    if (chunkID == "RIFF" && format == "WAVE")
    {
        audioFileFormat = AudioFileFormat::Wave;
        return decodeWaveFileHeader(file);
    }
    else if (chunkID == "FORM" && (format == "AIFF" || format == "AIFC"))
    {
        audioFileFormat = AudioFileFormat::Aiff;
        return decodeAiffFileHeader(file);
    }
    else
    {
        reportError("ERROR: Unknown or unsupported audio file format: " + filePath);
        return false;
    }
}

template <class T>
bool AudioHeader<T>::decodeWaveFileHeader(std::ifstream& file)
{
    // Read chunks until 'fmt ' chunk is found
    while (file.good() && !file.eof())
    {
        char chunkId[4];
        uint32_t chunkSize = 0;

        file.read(chunkId, 4);
        file.read(reinterpret_cast<char*>(&chunkSize), 4);

        if (file.gcount() != 4)
        {
            reportError("ERROR: Unexpected end of file while reading chunk size");
            return false;
        }

        std::string id(chunkId, 4);

        if (id == "fmt ")
        {
            // Read the 'fmt ' chunk
            char* fmtChunkData = new char[chunkSize];
            file.read(fmtChunkData, chunkSize);

            if (file.gcount() != static_cast<std::streamsize>(chunkSize))
            {
                delete[] fmtChunkData;
                reportError("ERROR: Couldn't read 'fmt ' chunk");
                return false;
            }

            // Parse 'fmt ' chunk
            uint16_t audioFormat = twoBytesToInt(fmtChunkData, Endianness::LittleEndian);
            numChannels = twoBytesToInt(fmtChunkData + 2, Endianness::LittleEndian);
            sampleRate = fourBytesToInt(fmtChunkData + 4, Endianness::LittleEndian);
            bitDepth = twoBytesToInt(fmtChunkData + 14, Endianness::LittleEndian);

            delete[] fmtChunkData;
            break; // We have the necessary header info
        }
        else
        {
            // Skip this chunk
            file.seekg(chunkSize, std::ios::cur);
        }
    }

    if (numChannels == 0 || sampleRate == 0 || bitDepth == 0)
    {
        reportError("ERROR: Failed to read WAV header information");
        return false;
    }

    return true;
}

template <class T>
bool AudioHeader<T>::decodeAiffFileHeader(std::ifstream& file)
{
    return false;
    // // Read chunks until 'COMM' chunk is found
    // while (file.good() && !file.eof())
    // {
    //     char chunkId[4];
    //     uint32_t chunkSize = 0;

    //     file.read(chunkId, 4);
    //     file.read(reinterpret_cast<char*>(&chunkSize), 4);

    //     if (file.gcount() != 4)
    //     {
    //         reportError("ERROR: Unexpected end of file while reading chunk size");
    //         return false;
    //     }

    //     std::string id(chunkId, 4);

    //     if (id == "COMM")
    //     {
    //         // Read the 'COMM' chunk
    //         chunkSize = _byteswap_ulong(chunkSize); // AIFF uses big endian
    //         char* commChunkData = new char[chunkSize];
    //         file.read(commChunkData, chunkSize);

    //         if (file.gcount() != static_cast<std::streamsize>(chunkSize))
    //         {
    //             delete[] commChunkData;
    //             reportError("ERROR: Couldn't read 'COMM' chunk");
    //             return false;
    //         }

    //         // Parse 'COMM' chunk
    //         numChannels = twoBytesToInt(commChunkData, Endianness::BigEndian);
    //         uint32_t numSampleFrames = fourBytesToInt(commChunkData + 2, Endianness::BigEndian);
    //         bitDepth = twoBytesToInt(commChunkData + 6, Endianness::BigEndian);

    //         // For sample rate, AIFF uses 80-bit extended precision, which is complex to parse
    //         // For simplicity, you might set a default value or implement proper parsing

    //         delete[] commChunkData;
    //         break; // We have the necessary header info
    //     }
    //     else
    //     {
    //         // Skip this chunk
    //         chunkSize = _byteswap_ulong(chunkSize); // AIFF uses big endian
    //         file.seekg(chunkSize, std::ios::cur);
    //     }
    // }

    // if (numChannels == 0 || bitDepth == 0)
    // {
    //     reportError("ERROR: Failed to read AIFF header information");
    //     return false;
    // }

    // return true;
}

template <class T>
uint32_t AudioHeader<T>::getSampleRate() const
{
    return sampleRate;
}

template <class T>
int AudioHeader<T>::getNumChannels() const
{
    return numChannels;
}

template <class T>
int AudioHeader<T>::getBitDepth() const
{
    return bitDepth;
}

template <class T>
AudioFileFormat AudioHeader<T>::getAudioFormat() const
{
    return audioFileFormat;
}

template <class T>
void AudioHeader<T>::shouldLogErrorsToConsole(bool logErrors)
{
    logErrorsToConsole = logErrors;
}

template <class T>
void AudioHeader<T>::reportError(std::string errorMessage)
{
    if (logErrorsToConsole)
        std::cerr << errorMessage << std::endl;
}

template <class T>
uint32_t AudioHeader<T>::fourBytesToInt(char* bytes, Endianness endianness)
{
    uint32_t result;
    if (endianness == Endianness::LittleEndian)
    {
        result = (static_cast<uint8_t>(bytes[3]) << 24) | (static_cast<uint8_t>(bytes[2]) << 16) |
                 (static_cast<uint8_t>(bytes[1]) << 8) | static_cast<uint8_t>(bytes[0]);
    }
    else
    {
        result = (static_cast<uint8_t>(bytes[0]) << 24) | (static_cast<uint8_t>(bytes[1]) << 16) |
                 (static_cast<uint8_t>(bytes[2]) << 8) | static_cast<uint8_t>(bytes[3]);
    }
    return result;
}

template <class T>
uint16_t AudioHeader<T>::twoBytesToInt(char* bytes, Endianness endianness)
{
    uint16_t result;
    if (endianness == Endianness::LittleEndian)
    {
        result = (static_cast<uint8_t>(bytes[1]) << 8) | static_cast<uint8_t>(bytes[0]);
    }
    else
    {
        result = (static_cast<uint8_t>(bytes[0]) << 8) | static_cast<uint8_t>(bytes[1]);
    }
    return result;
}
