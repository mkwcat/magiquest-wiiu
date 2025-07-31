#include "AudioFileOpus.hpp"
#include "AudioManager.hpp"
#include "System.hpp"
#include "Util.hpp"
#include <cstring>
#include <opusfile.h>

AudioFileOpus::AudioFileOpus(
  const char* path, bool tvLeft, bool tvRight, bool drcLeft, bool drcRight)
{
    char fullPath[256];
    std::snprintf(fullPath, sizeof(fullPath), RES_ROOT "/Sound/%s.opus", path);

    u32 opusSize;
    void* opus = System::s_instance->RipFile(fullPath, &opusSize);
    assert(opus != nullptr);

    OggOpusFile* oggFile = op_open_memory(static_cast<const u8*>(opus), opusSize, nullptr);
    assert(oggFile != nullptr);

    auto sampleCount = op_pcm_total(oggFile, -1);
    LOG(LogSystem, "Opus file %s has %llu samples", path, sampleCount);
    m_dataSize = sampleCount * sizeof(u16);
    m_data = std::aligned_alloc(256, AlignUp(256, m_dataSize));
    assert(m_data != nullptr);

    opus_int16* buffer = static_cast<opus_int16*>(m_data);

    while (sampleCount > 0) {
        assert(buffer + sampleCount <=
               static_cast<opus_int16*>(m_data) + m_dataSize / sizeof(opus_int16));

        auto ret = op_read(oggFile, buffer, sampleCount, nullptr);
        assert(ret >= 0);

        buffer += ret;
        sampleCount -= ret;
    }

    std::free(opus);

    m_voice = AudioManager::s_instance->AcquireVoice(
      0x30000000, tvLeft, tvRight, drcLeft, drcRight, 48000, true, false);
    assert(m_voice != -1);

    AudioManager::s_instance->PushBuffer(
      m_voice, static_cast<u16*>(m_data), m_dataSize / sizeof(u16), false);
}