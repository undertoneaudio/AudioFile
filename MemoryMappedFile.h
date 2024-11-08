#pragma once

#include <string>

class MemoryMappedFile
{
public:
    MemoryMappedFile();
    ~MemoryMappedFile();

    bool open(const std::string& filePath);
    void close();

    const uint8_t* data() const;
    size_t size() const;

private:
#if defined(_WIN32)
    // Windows-specific members
    void* fileHandle;
    void* mappingHandle;
    const uint8_t* mappedData;
#else
    // POSIX-specific members
    int fileDescriptor;
    const uint8_t* mappedData;
#endif
    size_t fileSize;
};
