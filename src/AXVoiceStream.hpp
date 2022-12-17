#pragma once

#include <coreinit/messagequeue.h>
#include <gctypes.h>
#include <memory>

class AXVoiceStream
{
public:
    AXVoiceStream(u32 bufferSize, u32 bufferCount);
    ~AXVoiceStream();

    /**
     * Acquire and initialize the voice. This will automatically free the old
     * voice.
     */
    void Init(u32 volume, bool tvLeft, bool tvRight, bool drcLeft,
      bool drcRight, u32 sampleRate);

    /**
     * Receive an audio buffer.
     */
    u16* RecvBuffer(bool blocking);

    /**
     * Push an audio buffer to the voice.
     */
    void PushBuffer(u16* buffer, u32 sampleCount);

    /**
     * Force a blocking RecvBuffer to return nullptr.
     */
    void Wakeup();

    /**
     * Get the size of a single audio buffer.
     */
    u32 GetBufferSize() const;

    /**
     * Start playing audio.
     */
    void Start();

    /**
     * Stop playing audio.
     */
    void Stop();

private:
    /**
     * AX voice index.
     */
    int m_voice = -1;

    /**
     * Audio buffers for use in the voice queue.
     */
    std::unique_ptr<u16> m_data;
    u32 m_bufferSize;
    u32 m_bufferCount;

    /**
     * Voice buffer return message data.
     */
    OSMessage m_returnMsg[16];

    /**
     * Voice buffer return.
     */
    OSMessageQueue m_returnQueue;
};
