#include "AXVoiceStream.hpp"
#include "AudioMgr.hpp"
#include <cassert>

AXVoiceStream::AXVoiceStream(u32 bufferSize, u32 bufferCount)
{
    m_bufferSize = bufferSize;
    m_bufferCount = bufferCount;

    m_data = std::unique_ptr<u16>(
      new (std::align_val_t(32)) u16[m_bufferSize * m_bufferCount]);
}

AXVoiceStream::~AXVoiceStream()
{
    if (AudioMgr::s_instance && m_voice != -1) {
        AudioMgr::s_instance->FreeVoice(m_voice);
        m_voice = -1;
    }
}

void AXVoiceStream::Init(u32 volume, bool tvLeft, bool tvRight, bool drcLeft,
  bool drcRight, u32 sampleRate)
{
    auto ax = AudioMgr::s_instance;

    if (m_voice != -1) {
        ax->FreeVoice(m_voice);
        m_voice = -1;
    }

    OSInitMessageQueueEx(
      &m_returnQueue, m_returnMsg, 16, "AXVoiceStream::m_returnQueue");

    for (u32 i = 0; i < m_bufferCount; i++) {
        OSMessage msg = {
          .message = (void*) (m_data.get() + m_bufferSize * i),
          .args =
            {
              m_bufferSize,
            },
        };

        auto ret = OSSendMessage(&m_returnQueue, &msg, OS_MESSAGE_FLAGS_NONE);
        assert(ret);
    }

    m_voice = ax->AcquireVoice(
      volume, tvLeft, tvRight, drcLeft, drcRight, sampleRate, true, false);
    assert(m_voice != -1);

    ax->SetBufferOutQueue(m_voice, &m_returnQueue);
}

u16* AXVoiceStream::RecvBuffer(bool blocking)
{
    if (m_voice == -1) {
        return nullptr;
    }

    OSMessage msg = {};
    auto ret = OSReceiveMessage(&m_returnQueue, &msg,
      blocking ? OS_MESSAGE_FLAGS_BLOCKING : OS_MESSAGE_FLAGS_NONE);
    assert(ret || !blocking);

    return ret ? (u16*) msg.message : nullptr;
}

void AXVoiceStream::PushBuffer(u16* buffer, u32 sampleCount)
{
    if (m_voice == -1) {
        return;
    }

    AudioMgr::s_instance->PushBuffer(m_voice, buffer, sampleCount, 0);
}

void AXVoiceStream::Wakeup()
{
    if (m_voice == -1) {
        return;
    }

    OSMessage msg = {};
    OSSendMessage(&m_returnQueue, &msg, OS_MESSAGE_FLAGS_NONE);
}

u32 AXVoiceStream::GetBufferSize() const
{
    return m_bufferSize;
}

void AXVoiceStream::Start()
{
    if (m_voice == -1) {
        return;
    }

    AudioMgr::s_instance->Start(m_voice);
}

void AXVoiceStream::Stop()
{
    if (m_voice == -1) {
        return;
    }

    AudioMgr::s_instance->Stop(m_voice);
}
