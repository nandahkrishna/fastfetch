#pragma once

#ifndef FF_INCLUDED_common_io_io
#define FF_INCLUDED_common_io_io

#include "util/FFstrbuf.h"

#ifdef _WIN32
    #include <fileapi.h>
    #include <io.h>
    typedef HANDLE FFNativeFD;
#else
    #include <unistd.h>
    typedef int FFNativeFD;
#endif

static inline FFNativeFD FFUnixFD2NativeFD(int unixfd)
{
    #ifndef _WIN32
        return unixfd;
    #else
        return (FFNativeFD) _get_osfhandle(unixfd);
    #endif
}

static inline bool ffWriteFDData(FFNativeFD fd, size_t dataSize, const void* data)
{
    #ifndef _WIN32
        return write(fd, data, dataSize) != -1;
    #else
        DWORD written;
        return WriteFile(fd, data, (DWORD) dataSize, &written, NULL) && written == dataSize;
    #endif
}

static inline bool ffWriteFDBuffer(FFNativeFD fd, const FFstrbuf* content)
{
    return ffWriteFDData(fd, content->length, content->chars);
}

bool ffWriteFileData(const char* fileName, size_t dataSize, const void* data);

static inline bool ffWriteFileBuffer(const char* fileName, const FFstrbuf* buffer)
{
    return ffWriteFileData(fileName, buffer->length, buffer->chars);
}

static inline ssize_t ffReadFDData(FFNativeFD fd, size_t dataSize, void* data)
{
    #ifndef _WIN32
        return read(fd, data, dataSize);
    #else
        DWORD readed;
        if(!ReadFile(fd, data, (DWORD)dataSize, &readed, NULL))
            return -1;

        return (ssize_t)readed;
    #endif
}

ssize_t ffReadFileData(const char* fileName, size_t dataSize, void* data);

bool ffAppendFDBuffer(FFNativeFD fd, FFstrbuf* buffer);
bool ffAppendFileBuffer(const char* fileName, FFstrbuf* buffer);

static inline bool ffReadFileBuffer(const char* fileName, FFstrbuf* buffer)
{
    ffStrbufClear(buffer);
    return ffAppendFileBuffer(fileName, buffer);
}

//Bit flags, combine with |
typedef enum FFPathType
{
    FF_PATHTYPE_REGULAR = 1,
    FF_PATHTYPE_LINK = 2,
    FF_PATHTYPE_DIRECTORY = 4
} FFPathType;

#define FF_PATHTYPE_FILE (FF_PATHTYPE_REGULAR | FF_PATHTYPE_LINK)
#define FF_PATHTYPE_ANY (FF_PATHTYPE_FILE | FF_PATHTYPE_DIRECTORY)

bool ffPathExists(const char* path, FFPathType pathType);

#ifndef _WIN32
    FF_C_SCANF(2, 3)
    void ffGetTerminalResponse(const char* request, const char* format, ...);
#endif

// Not thread safe!
bool ffSuppressIO(bool suppress);

static inline void ffUnsuppressIO(bool* suppressed)
{
    if (!*suppressed) return;
    ffSuppressIO(false);
    *suppressed = false;
}

#define FF_SUPPRESS_IO() bool __attribute__((__cleanup__(ffUnsuppressIO), __unused__)) io_suppressed__ = ffSuppressIO(true)

void ffListFilesRecursively(const char* path);

#endif // FF_INCLUDED_common_io_io
