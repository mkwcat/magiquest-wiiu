#include "AudioFilePcm16.hpp"
#include "AudioManager.hpp"
#include "System.hpp"

AudioFilePcm16::AudioFilePcm16(
  const char* path, bool tvLeft, bool tvRight, bool drcLeft, bool drcRight)
{
    auto ax = AudioManager::s_instance;

    m_voice = ax->AcquireVoice(0x30000000, true, true, true, true, 48000, true, false);
    assert(m_voice != -1);

    char fullPath[256];
    std::snprintf(fullPath, sizeof(fullPath), RES_ROOT "/Sound/%s.pcm16", path);

    void* m_data = System::s_instance->RipFile(fullPath, &m_dataSize);
    assert(m_data != nullptr);

    ax->PushBuffer(m_voice, static_cast<u16*>(m_data), m_dataSize / sizeof(u16), false);
}

AudioFilePcm16::AudioFilePcm16(
  const void* data, u32 dataSize, bool tvLeft, bool tvRight, bool drcLeft, bool drcRight)
{
    auto ax = AudioManager::s_instance;

    m_voice = ax->AcquireVoice(0x30000000, tvLeft, tvRight, drcLeft, drcRight, 48000, true, false);
    assert(m_voice != -1);

    ax->PushBuffer(
      m_voice, const_cast<u16*>(static_cast<const u16*>(data)), dataSize / sizeof(u16), false);
}

AudioFilePcm16::~AudioFilePcm16()
{
    if (m_voice != -1) {
        auto ax = AudioManager::s_instance;
        ax->Stop(m_voice);
        ax->FreeVoice(m_voice);
        m_voice = -1;
    }

    if (m_data != nullptr) {
        std::free(m_data);
        m_data = nullptr;
    }
}

void AudioFilePcm16::Play()
{
    auto ax = AudioManager::s_instance;
    if (m_voice != -1) {
        ax->Restart(m_voice);
        ax->Start(m_voice);
    }
}

void AudioFilePcm16::Stop()
{
    auto ax = AudioManager::s_instance;
    if (m_voice != -1) {
        ax->Stop(m_voice);
    }
}