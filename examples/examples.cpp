#include <iostream>
#include <cmath>
#include <AudioFile.h>
#include <AudioHeader.h>

#include "MemoryMappedFile.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

void loadFileToMemory(const std::string& filePath, std::vector<uint8_t>& buffer)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("ERROR: Could not open file for reading: " + filePath);
    }

    file.unsetf(std::ios::skipws);
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    if (fileSize <= 0)
    {
        throw std::runtime_error("ERROR: File is empty or error reading file size: " + filePath);
    }
    file.seekg(0, std::ios::beg);

    buffer.reserve(static_cast<size_t>(fileSize));
    buffer.insert(buffer.begin(),
                  std::istream_iterator<uint8_t>(file),
                  std::istream_iterator<uint8_t>());
}


 //=======================================================================
 namespace examples
 {
     void writeSineWaveToAudioFile();
     void loadAudioFileAndPrintSummary();
     void loadAudioHeaderAndPrintSummary();
     void loadAudioFileAndProcessSamples();
     void memoryMappedFile();
 }

//=======================================================================
int main()
{
    examples::memoryMappedFile();

    //---------------------------------------------------------------
    /** Writes a sine wave to an audio file */
    examples::writeSineWaveToAudioFile();
    
    //---------------------------------------------------------------
    /** Loads an audio header and prints key details to the console*/
    examples::loadAudioHeaderAndPrintSummary();
    
    //---------------------------------------------------------------
    /** Loads an audio file and prints key details to the console*/
    examples::loadAudioFileAndPrintSummary();
    
    //---------------------------------------------------------------
    /** Loads an audio file and processess the samples */
    examples::loadAudioFileAndProcessSamples();
        
    return 0;
}

//=======================================================================
namespace examples
{
    void memoryMappedFile()
    {
        const std::string filePath = "../blob.txt";
        const size_t chunkSize = 8192; // Set the size of compared data chunk in bytes
        const int numIterations = 10000; // Number of random points to compare

        try
        {
            // Load file to memory
            std::vector<uint8_t> memoryData;
            auto startLoadMemory = std::chrono::high_resolution_clock::now();
            loadFileToMemory(filePath, memoryData);
            auto endLoadMemory = std::chrono::high_resolution_clock::now();
            std::cout << "Loaded file into memory. Size: " << memoryData.size() << " bytes" << std::endl;

            // Map file using MemoryMappedFile
            MemoryMappedFile mappedFile;
            auto startMap = std::chrono::high_resolution_clock::now();
            if (!mappedFile.open(filePath))
            {
                std::cerr << "Failed to memory-map the file." << std::endl;
                return;
            }
            auto endMap = std::chrono::high_resolution_clock::now();
            std::cout << "Memory-mapped file. Size: " << mappedFile.size() << " bytes" << std::endl;

            // Ensure sizes match
            if (memoryData.size() != mappedFile.size())
            {
                std::cerr << "ERROR: File sizes do not match." << std::endl;
                return;
            }

            // Set up random number generator
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<size_t> dis(0, memoryData.size() - chunkSize);

            // Vectors to store access and comparison times
            std::vector<double> timesMemoryAccess;
            std::vector<double> timesMappedAccess;
            std::vector<double> timesComparison;

            // Start comparing random chunks
            bool allMatch = true;
            for (int i = 0; i < numIterations; ++i)
            {
                size_t offset = dis(gen);

                // Access in-memory data
                auto startMemoryAccess = std::chrono::high_resolution_clock::now();
                const uint8_t* memoryPtr = &memoryData[offset];
                auto endMemoryAccess = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::nano> durationMemoryAccess = endMemoryAccess - startMemoryAccess;
                timesMemoryAccess.push_back(durationMemoryAccess.count());

                // Access memory-mapped data
                auto startMappedAccess = std::chrono::high_resolution_clock::now();
                const uint8_t* mappedPtr = mappedFile.data() + offset;
                auto endMappedAccess = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::nano> durationMappedAccess = endMappedAccess - startMappedAccess;
                timesMappedAccess.push_back(durationMappedAccess.count());

                // Compare the chunks
                auto startComparison = std::chrono::high_resolution_clock::now();
                int cmpResult = std::memcmp(memoryPtr, mappedPtr, chunkSize);
                auto endComparison = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::nano> durationComparison = endComparison - startComparison;
                timesComparison.push_back(durationComparison.count());

                if (cmpResult != 0)
                {
                    std::cerr << "ERROR: Data mismatch at offset " << offset << std::endl;
                    allMatch = false;
                    break;
                }
            }

            if (allMatch)
            {
                std::cout << "All data chunks match." << std::endl;
            }

            // Function to compute statistics
            auto computeStats = [](const std::vector<double>& times, const std::string& description)
            {
                double avg = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
                double minTime = *std::min_element(times.begin(), times.end());
                double maxTime = *std::max_element(times.begin(), times.end());
                std::cout << description << " Times (nanoseconds):" << std::endl;
                std::cout << "Average: " << avg << std::endl;
                std::cout << "Minimum: " << minTime << std::endl;
                std::cout << "Maximum: " << maxTime << std::endl;
            };

            computeStats(timesMemoryAccess, "In-memory Access");
            computeStats(timesMappedAccess, "Memory-mapped Access");
            computeStats(timesComparison, "Comparison");

            // Print load times
            std::chrono::duration<double, std::milli> durationLoadMemory = endLoadMemory - startLoadMemory;
            std::chrono::duration<double, std::milli> durationMap = endMap - startMap;
            std::cout << "Time to load into memory: " << durationLoadMemory.count() << " ms" << std::endl;
            std::cout << "Time to memory-map file: " << durationMap.count() << " ms" << std::endl;

            // Close the memory-mapped file
            mappedFile.close();
        }
        catch (const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
            return;
        }
    }

    //=======================================================================
    void writeSineWaveToAudioFile()
    {
        //---------------------------------------------------------------
        std::cout << "**********************" << std::endl;
        std::cout << "Running Example: Write Sine Wave To Audio File" << std::endl;
        std::cout << "**********************" << std::endl << std::endl;
        
        //---------------------------------------------------------------
        // 1. Let's setup our AudioFile instance
        
        AudioFile<float> a;
        a.setNumChannels (2);
        a.setNumSamplesPerChannel (44100);

        //---------------------------------------------------------------
        // 2. Create some variables to help us generate a sine wave
        
        const float sampleRate = 44100.f;
        const float frequencyInHz = 440.f;
        
        //---------------------------------------------------------------
        // 3. Write the samples to the AudioFile sample buffer
        
        for (int i = 0; i < a.getNumSamplesPerChannel(); i++)
        {
            for (int channel = 0; channel < a.getNumChannels(); channel++)
            {
                a.samples[channel][i] = sin ((static_cast<float> (i) / sampleRate) * frequencyInHz * 2.f * (float)M_PI);
            }
        }
        
        //---------------------------------------------------------------
        // 4. Save the AudioFile
        
        std::string filePath = "sine-wave.wav"; // change this to somewhere useful for you
        a.save ("sine-wave.wav", AudioFileFormat::Wave);
    }
    
    //=======================================================================
    void loadAudioHeaderAndPrintSummary()
    {
        //---------------------------------------------------------------
        std::cout << "**********************" << std::endl;
        std::cout << "Running Example: Load Audio File and Print Summary" << std::endl;
        std::cout << "**********************" << std::endl << std::endl;
        
        //---------------------------------------------------------------
        // 1. Set a file path to an audio file on your machine
        const std::string filePath = std::string (PROJECT_BINARY_DIR) + "/test-audio.wav";
        
        //---------------------------------------------------------------
        // 2. Create an AudioFile object and load the audio file
        
        AudioHeader<float> a;
        bool loadedOK = a.loadHeader(filePath);
        
        /** If you hit this assert then the file path above
         probably doesn't refer to a valid audio file */
        assert (loadedOK);
        
        //---------------------------------------------------------------
        // 3. Let's print out some key details
        
        std::cout << "Bit Depth: " << a.getBitDepth() << std::endl;
        std::cout << "Sample Rate: " << a.getSampleRate() << std::endl;
        std::cout << "Num Channels: " << a.getNumChannels() << std::endl;
        std::cout << std::endl;
    }

    //=======================================================================
    void loadAudioFileAndPrintSummary()
    {
        //---------------------------------------------------------------
        std::cout << "**********************" << std::endl;
        std::cout << "Running Example: Load Audio File and Print Summary" << std::endl;
        std::cout << "**********************" << std::endl << std::endl;
        
        //---------------------------------------------------------------
        // 1. Set a file path to an audio file on your machine
        const std::string filePath = std::string (PROJECT_BINARY_DIR) + "/test-audio.wav";
        
        //---------------------------------------------------------------
        // 2. Create an AudioFile object and load the audio file
        
        AudioFile<float> a;
        bool loadedOK = a.load (filePath);
        
        /** If you hit this assert then the file path above
         probably doesn't refer to a valid audio file */
        assert (loadedOK);
        
        //---------------------------------------------------------------
        // 3. Let's print out some key details
        
        std::cout << "Bit Depth: " << a.getBitDepth() << std::endl;
        std::cout << "Sample Rate: " << a.getSampleRate() << std::endl;
        std::cout << "Num Channels: " << a.getNumChannels() << std::endl;
        std::cout << "Length in Seconds: " << a.getLengthInSeconds() << std::endl;
        std::cout << std::endl;
    }
    
    //=======================================================================
    void loadAudioFileAndProcessSamples()
    {
        //---------------------------------------------------------------
        std::cout << "**********************" << std::endl;
        std::cout << "Running Example: Load Audio File and Process Samples" << std::endl;
        std::cout << "**********************" << std::endl << std::endl;
        
        //---------------------------------------------------------------
        // 1. Set a file path to an audio file on your machine
        const std::string inputFilePath = std::string (PROJECT_BINARY_DIR) + "/test-audio.wav";
        
        //---------------------------------------------------------------
        // 2. Create an AudioFile object and load the audio file
        
        AudioFile<float> a;
        bool loadedOK = a.load (inputFilePath);
        
        /** If you hit this assert then the file path above
         probably doesn't refer to a valid audio file */
        assert (loadedOK);
        
        //---------------------------------------------------------------
        // 3. Let's apply a gain to every audio sample
        
        float gain = 0.5f;

        for (int i = 0; i < a.getNumSamplesPerChannel(); i++)
        {
            for (int channel = 0; channel < a.getNumChannels(); channel++)
            {
                a.samples[channel][i] = a.samples[channel][i] * gain;
            }
        }
        
        //---------------------------------------------------------------
        // 4. Write audio file to disk
        
        std::string outputFilePath = "quieter-audio-file.wav"; // change this to somewhere useful for you
        a.save (outputFilePath, AudioFileFormat::Aiff);
    }
}
