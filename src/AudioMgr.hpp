// AudioMgr.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <array>
#include <coreinit/messagequeue.h>
#include <coreinit/mutex.h>
#include <gctypes.h>
#include <gui/system/CThread.h>
#include <sndcore2/core.h>
#include <sndcore2/voice.h>

class AudioMgr : public CThread
{
public:
    static AudioMgr* s_instance;

    static constexpr u32 MaxVoiceCount = 8;

    AudioMgr();
    ~AudioMgr();

    /**
     * Create a new voice to be managed by the caller.
     * @returns Voice handle, or -1 on error.
     */
    int AcquireVoice(u32 volume, bool tvLeft, bool tvRight, bool drcLeft,
      bool drcRight, u32 sampleRate, bool streamed, bool looped);

    /**
     * Free a voice.
     */
    void FreeVoice(int voice);

    /**
     * Push a 16-bit PCM buffer to the queue.
     */
    void PushBuffer(int voice, u16* buffer, u32 sampleCount, u32 marker);

    /**
     * Set a voice's buffer out queue.
     */
    void SetBufferOutQueue(int voice, OSMessageQueue* queue);

    void Start(int voice);

    void Stop(int voice);

    void Restart(int voice);

    void ChangeMarker(int voice, u32 marker);

    void Shutdown()
    {
        m_shutdown = true;
    }

protected:
    enum FrameCBValue {
        MSG_AXFRAME,
        MSG_SHUTDOWN,
    };

    OSMessage m_frameCbMsg[8];
    OSMessageQueue m_frameCbQueue;

    OSMutex m_acquireVoiceMutex;

    bool m_shutdown = false;

    struct Voice {
        bool m_inUse;

        enum VoiceMsgValue {
            MSG_INIT,
            MSG_FREE,
            MSG_START,
            MSG_STOP,
            MSG_RESTART,
            MSG_CHANGE_MARKER,
        };

        struct InitArgs {
            u32 volume;
            bool tvLeft;
            bool tvRight;
            bool drcLeft;
            bool drcRight;
        };

        enum class State {
            Stopped,
            WaitingOnMarker,
            Playing,
        };

        InitArgs m_initArgs;

        AXVoice* m_voice;
        AXVoiceSrc m_voiceSrc;
        AXVoiceOffsets m_voiceOffsets;

        State m_state;
        u32 m_marker;

        OSMessage m_bufferInMsg[8];
        OSMessageQueue m_bufferInQueue;
        OSMessageQueue* m_bufferOutQueue;

        OSMessage m_ctrlMsg[8];
        OSMessageQueue m_ctrlQueue;

        OSMessage m_ctrlRespMsg[8];
        OSMessageQueue m_ctrlRespQueue;

        bool m_streamed;
        bool m_looped;
        u32 m_sampleRate;

        // Used to determine when one buffer ends and we move onto the next.
        u32 m_lastLoopCount;
        bool m_needsBuffer;

        u16* m_buffer;
        u32 m_bufferSampleCount;
        u16* m_nextBuffer;
        u32 m_nextBufferSampleCount;
        u32 m_nextBufferMarker;
    };

    std::array<Voice, MaxVoiceCount> m_voices;

    void InternalInitVoice(int voice, u32 volume, bool tvLeft, bool tvRight,
      bool drcLeft, bool drcRight);

    void InternalVoiceSetOffsets(Voice* vc, bool loop);

    /**
     * Sound thread entry.
     */
    void executeThread();

    /**
     * AX frame callback.
     */
    static void AXFrameCallbackFunc();
};
