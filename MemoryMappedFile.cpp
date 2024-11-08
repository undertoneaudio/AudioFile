#include "MemoryMappedFile.h"
#include <iostream>
#include <cstring>
#include <stdexcept>

#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

MemoryMappedFile::MemoryMappedFile()
    : mappedData(nullptr)
#if defined(_WIN32)
    , fileHandle(nullptr)
    , mappingHandle(nullptr)
#endif
    , fileSize(0)
{
}

MemoryMappedFile::~MemoryMappedFile()
{
    close();
}

bool MemoryMappedFile::open(const std::string& filePath)
{
#if defined(_WIN32)
    fileHandle = CreateFileA(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        std::cerr << "ERROR: Could not open file: " << filePath << std::endl;
        return false;
    }

    fileSize = GetFileSize(fileHandle, NULL);

    mappingHandle = CreateFileMappingA(fileHandle, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!mappingHandle)
    {
        std::cerr << "ERROR: Could not create file mapping: " << filePath << std::endl;
        CloseHandle(fileHandle);
        return false;
    }

    mappedData = static_cast<const uint8_t*>(MapViewOfFile(mappingHandle, FILE_MAP_READ, 0, 0, 0));
    if (!mappedData)
    {
        std::cerr << "ERROR: Could not map view of file: " << filePath << std::endl;
        CloseHandle(mappingHandle);
        CloseHandle(fileHandle);
        return false;
    }
#else
    fileDescriptor = ::open(filePath.c_str(), O_RDONLY);
    if (fileDescriptor < 0)
    {
        std::cerr << "ERROR: Could not open file: " << filePath << std::endl;
        return false;
    }

    struct stat statBuf;
    if (fstat(fileDescriptor, &statBuf) < 0)
    {
        std::cerr << "ERROR: Could not get file size: " << filePath << std::endl;
        ::close(fileDescriptor);
        return false;
    }

    fileSize = statBuf.st_size;

    mappedData = static_cast<const uint8_t*>(mmap(nullptr, fileSize, PROT_READ, MAP_SHARED, fileDescriptor, 0));
    if (mappedData == MAP_FAILED)
    {
        std::cerr << "ERROR: Could not memory-map file: " << filePath << std::endl;
        ::close(fileDescriptor);
        mappedData = nullptr;
        return false;
    }
#endif
    return true;
}

void MemoryMappedFile::close()
{
#if defined(_WIN32)
    if (mappedData)
    {
        UnmapViewOfFile(mappedData);
        mappedData = nullptr;
    }
    if (mappingHandle)
    {
        CloseHandle(mappingHandle);
        mappingHandle = nullptr;
    }
    if (fileHandle)
    {
        CloseHandle(fileHandle);
        fileHandle = nullptr;
    }
#else
    if (mappedData)
    {
        munmap(const_cast<uint8_t*>(mappedData), fileSize);
        mappedData = nullptr;
    }
    if (fileDescriptor >= 0)
    {
        ::close(fileDescriptor);
        fileDescriptor = -1;
    }
#endif
}

const uint8_t* MemoryMappedFile::data() const
{
    return mappedData;
}

size_t MemoryMappedFile::size() const
{
    return fileSize;
}
