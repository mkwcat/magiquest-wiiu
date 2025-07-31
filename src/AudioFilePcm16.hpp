#pragma once

#include <gctypes.h>

/**
 * An audio file that gets preloaded into memory and can be randomly played.
 */
class AudioFilePcm16
{
public:
    AudioFilePcm16(const char* path, bool tvLeft, bool tvRight, bool drcLeft, bool drcRight);

    AudioFilePcm16(const char* path)
      : AudioFilePcm16(path, true, true, true, true)
    {
    }

    AudioFilePcm16(const char* path, bool left, bool right)
      : AudioFilePcm16(path, left, right, left, right)
    {
    }

    AudioFilePcm16(
      const void* data, u32 dataSize, bool tvLeft, bool tvRight, bool drcLeft, bool drcRight);

    AudioFilePcm16(const void* data, u32 dataSize)
      : AudioFilePcm16(data, dataSize, true, true, true, true)
    {
    }

    AudioFilePcm16(const void* data, u32 dataSize, bool left, bool right)
      : AudioFilePcm16(data, dataSize, left, right, left, right)
    {
    }

    AudioFilePcm16()
    {
    }

    ~AudioFilePcm16();

    void Play();
    void Stop();

protected:
    int m_voice = -1;
    void* m_data = nullptr;
    u32 m_dataSize = 0;
};