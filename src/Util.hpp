// Util.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <coreinit/mutex.h>
#include <gctypes.h>
#include <stdlib.h>

#define CONF_LOG_VERBOSE 1
#define CONF_LOG_MASK 0xFFFFFFFF

enum LogSource {
    LogSystem = (1 << 0),
    LogMP4 = (1 << 1),
    LogWand = (1 << 2),
    LogAudio = (1 << 3),
};

#define RES_ROOT "wiiu/apps/magiquest-wiiu/content"

#if CONF_LOG_MASK != 0
#  include <whb/log.h>

#  define _STRIFY(line) #line

#  define _DO_PRINT(file, line, ...)                                           \
    WHBLogPrintf(file ":" _STRIFY(line) " > " __VA_ARGS__)

#  define LOG(source, ...)                                                     \
    if (CONF_LOG_MASK & source)                                                \
    _DO_PRINT(__FILE__, __LINE__, __VA_ARGS__)

#  define LOG_VERBOSE(source, ...)                                             \
    if (CONF_LOG_VERBOSE && (CONF_LOG_MASK & source))                          \
    _DO_PRINT(__FILE__, __LINE__, __VA_ARGS__)

#  define PANIC(...)                                                           \
    do {                                                                       \
      _DO_PRINT(__FILE__, __LINE__, __VA_ARGS__);                              \
      abort();                                                                 \
    } while (0)

#else // LOG_MASK != 0
#  define LOG(source, ...)
#  define LOG_VERBOSE(source, ...)
#  define PANIC(...) abort()
#endif // LOG_MASK != 0

template <class T>
constexpr T RoundUp(T num, unsigned int align)
{
    u32 raw = (u32) num;
    return (T) ((raw + align - 1) & -align);
}

template <class T>
constexpr T RoundDown(T num, unsigned int align)
{
    u32 raw = (u32) num;
    return (T) (raw & -align);
}

template <class T>
constexpr bool Aligned(T addr, unsigned int align)
{
    return !((u32) addr & (align - 1));
}

template <class T>
constexpr T Increment(T value, int diff = 1)
{
    return (T) ((int) (value) + diff);
}

/**
 * A lock that automatically unlocks itself.
 */
class Lock
{
    OSMutex* m_mutex;

public:
    Lock(OSMutex& mutex)
    {
        m_mutex = &mutex;
        OSLockMutex(m_mutex);
    }

    ~Lock()
    {
        OSUnlockMutex(m_mutex);
    }
};
