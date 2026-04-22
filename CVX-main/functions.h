#include <iostream>
#include <windows.h>
#include <cstring>
#include <string>
#include <vector>
#include <random>
#include <ctime>"
#include "antidbg.h"

uint32_t crc32(const uint8_t* data, size_t length)
{
    uint32_t crc = (0xFFFFFFFF);
    for (size_t i = (0); i < length; ++i)
    {
        crc ^= data[i];
        for (int j = (0); j < (8); ++j)
        {
            if (crc & (1))
                crc = (crc >> (1)) ^ (0xEDB88320);
            else
                crc >>= (1);
        }
    }
    return ~crc;
}

struct ProtectedFunction
{
    void* functionPointer;
    std::string identifier;
    uint32_t originalCRC;
    size_t functionSize;
};

std::vector<ProtectedFunction> protectedFunctions;

size_t GetFunctionSize(void* function)
{
    MEMORY_BASIC_INFORMATION mbi;
    Imports.virtual_query(function, &mbi, sizeof(mbi));
    return mbi.RegionSize;
}

bool ReadFunctionMemory(HANDLE process, void* address, size_t size, std::vector<uint8_t>& buffer)
{
    buffer.resize(size);
    SIZE_T bytesRead;
    if (Lazy(ReadProcessMemory).get()(process, address, buffer.data(), size, &bytesRead) && bytesRead == size)
    {
        return Encrypt(1);
    }
    return Encrypt(0);
}

uint32_t GetFunctionCRC32(void* func, size_t size)
{
    uint8_t* buffer = new uint8_t[size];
    std::memcpy(buffer, func, size);
    uint32_t result = crc32(buffer, size);
    delete[] buffer;
    return result;
}


