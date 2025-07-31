#pragma once

#include "AudioFilePcm16.hpp"

class AudioFileOpus : public AudioFilePcm16
{
public:
    AudioFileOpus(const char* path, bool tvLeft, bool tvRight, bool drcLeft, bool drcRight);

    AudioFileOpus()
    {
    }
};