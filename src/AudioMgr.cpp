// AudioMgr.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "AudioMgr.hpp"
#include "Util.hpp"
#include <coreinit/cache.h>

AudioMgr* AudioMgr::s_instance = nullptr;

AudioMgr::AudioMgr()
  : CThread(
      CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff, 16, 0x80000)
  , m_voices{0}

{
    s_instance = this;

    LOG(LogAudio, "Starting up AudioMgr");

    OSInitMessageQueueEx(
      &m_frameCbQueue, m_frameCbMsg, 8, "AudioMgr::m_frameCbQueue");

    OSInitMutexEx(&m_acquireVoiceMutex, "AudioMgr::m_acquireVoiceMutex");

    resumeThread();
}

AudioMgr::~AudioMgr()
{
    LOG(LogAudio, "Shutting down AudioMgr");

    OSMessage msg = {
      .message = nullptr,
      .args = {MSG_SHUTDOWN},
    };

    if (!OSSendMessage(&m_frameCbQueue, &msg, OS_MESSAGE_FLAGS_BLOCKING)) {
        PANIC("OSSendMessage failed");
    }
}

int AudioMgr::AcquireVoice(u32 volume, bool tvLeft, bool tvRight, bool drcLeft,
  bool drcRight, u32 sampleRate, bool streamed, bool looped)
{
    if (m_shutdown)
        return -1;

    Lock l(m_acquireVoiceMutex);

    for (u32 i = 0; i < m_voices.size(); i++) {
        if (!m_voices[i].m_inUse && m_voices[i].m_voice == nullptr) {
            m_voices[i].m_inUse = true;

            OSInitMessageQueueEx(&m_voices[i].m_bufferInQueue,
              m_voices[i].m_bufferInMsg, 8, "AudioMgr::Voice::m_bufferInQueue");

            OSInitMessageQueueEx(&m_voices[i].m_ctrlQueue,
              m_voices[i].m_ctrlMsg, 8, "AudioMgr::Voice::m_ctrlQueue");

            OSInitMessageQueueEx(&m_voices[i].m_ctrlRespQueue,
              m_voices[i].m_ctrlRespMsg, 8, "AudioMgr::Voice::m_ctrlRespQueue");

            OSMessage msg = {
              .message = nullptr,
              .args =
                {
                  Voice::MSG_INIT,
                },
            };

            m_voices[i].m_initArgs = {};
            m_voices[i].m_initArgs = {
              .volume = volume,
              .tvLeft = tvLeft,
              .tvRight = tvRight,
              .drcLeft = drcLeft,
              .drcRight = drcRight,
            };

            m_voices[i].m_sampleRate = sampleRate;
            m_voices[i].m_streamed = streamed;
            m_voices[i].m_looped = looped;

            auto ret = OSSendMessage(
              &m_voices[i].m_ctrlQueue, &msg, OS_MESSAGE_FLAGS_BLOCKING);
            assert(ret == TRUE);
            return i;
        }
    }

    LOG(LogAudio, "Failed to find free voice");
    return -1;
}

void AudioMgr::FreeVoice(int voice)
{
    if (m_shutdown)
        return;

    Lock l(m_acquireVoiceMutex);

    if (!m_voices[voice].m_inUse) {
        LOG(LogAudio, "Attempt to free a voice that's not in use (%d)", voice);
        return;
    }

    OSMessage msg = {
      .message = nullptr,
      .args =
        {
          Voice::MSG_FREE,
        },
    };

    auto ret = OSSendMessage(
      &m_voices[voice].m_ctrlQueue, &msg, OS_MESSAGE_FLAGS_BLOCKING);
    assert(ret == TRUE);

    ret = OSReceiveMessage(
      &m_voices[voice].m_ctrlRespQueue, &msg, OS_MESSAGE_FLAGS_BLOCKING);
    assert(ret == TRUE);

    m_voices[voice] = {};
}

void AudioMgr::PushBuffer(int voice, u16* buffer, u32 sampleCount, u32 marker)
{
    if (m_shutdown)
        return;

    if (!m_voices[voice].m_inUse) {
        LOG(LogAudio,
          "Attempt to push buffer to a voice that's not in use (%d)", voice);
        return;
    }

    OSMessage msg = {
      .message = buffer,
      .args =
        {
          sampleCount,
          marker,
        },
    };

    auto ret = OSSendMessage(
      &m_voices[voice].m_bufferInQueue, &msg, OS_MESSAGE_FLAGS_BLOCKING);
    assert(ret == TRUE);
}

void AudioMgr::SetBufferOutQueue(int voice, OSMessageQueue* queue)
{
    if (m_shutdown)
        return;

    if (!m_voices[voice].m_inUse) {
        LOG(LogAudio,
          "Attempt to set the buffer out queue in a voice that's not in use "
          "(%d)",
          voice);
        return;
    }

    m_voices[voice].m_bufferOutQueue = queue;
}

void AudioMgr::Start(int voice)
{
    if (m_shutdown)
        return;

    if (!m_voices[voice].m_inUse) {
        LOG(LogAudio, "Attempt to start a voice that's not in use (%d)", voice);
        return;
    }

    OSMessage msg = {
      .message = nullptr,
      .args =
        {
          Voice::MSG_START,
        },
    };

    auto ret = OSSendMessage(
      &m_voices[voice].m_ctrlQueue, &msg, OS_MESSAGE_FLAGS_BLOCKING);
    assert(ret == TRUE);
}

void AudioMgr::Stop(int voice)
{
    if (m_shutdown)
        return;

    if (!m_voices[voice].m_inUse) {
        LOG(LogAudio, "Attempt to stop a voice that's not in use (%d)", voice);
        return;
    }

    OSMessage msg = {
      .message = nullptr,
      .args =
        {
          Voice::MSG_STOP,
        },
    };

    auto ret = OSSendMessage(
      &m_voices[voice].m_ctrlQueue, &msg, OS_MESSAGE_FLAGS_BLOCKING);
    assert(ret == TRUE);
}

void AudioMgr::Restart(int voice)
{
    if (m_shutdown)
        return;

    if (!m_voices[voice].m_inUse) {
        LOG(
          LogAudio, "Attempt to restart a voice that's not in use (%d)", voice);
        return;
    }

    OSMessage msg = {
      .message = nullptr,
      .args =
        {
          Voice::MSG_RESTART,
        },
    };

    auto ret = OSSendMessage(
      &m_voices[voice].m_ctrlQueue, &msg, OS_MESSAGE_FLAGS_BLOCKING);
    assert(ret == TRUE);
}

void AudioMgr::ChangeMarker(int voice, u32 marker)
{
    if (m_shutdown)
        return;

    if (!m_voices[voice].m_inUse) {
        LOG(
          LogAudio, "Attempt to restart a voice that's not in use (%d)", voice);
        return;
    }

    OSMessage msg = {
      .message = nullptr,
      .args =
        {
          Voice::MSG_CHANGE_MARKER,
          marker,
        },
    };

    auto ret = OSSendMessage(
      &m_voices[voice].m_ctrlQueue, &msg, OS_MESSAGE_FLAGS_BLOCKING);
    assert(ret == TRUE);
}

void AudioMgr::InternalInitVoice(
  int voice, u32 volume, bool tvLeft, bool tvRight, bool drcLeft, bool drcRight)
{
    assert(u32(voice) < m_voices.size());

    auto vc = &m_voices[voice];
    vc->m_voice = AXAcquireVoice(1, nullptr, 0);
    assert(vc->m_voice != nullptr);

    LOG(LogAudio, "Acquired voice %d", voice);

    AXVoiceBegin(vc->m_voice);
    AXSetVoiceType(vc->m_voice, 0);

    AXVoiceVeData veData = {};
    veData = {
      .volume = volume >> 16,
      .delta = volume & 0xFFFF,
    };

    AXSetVoiceVe(vc->m_voice, &veData);

    AXVoiceDeviceMixData mix[6] = {};

    mix[0].bus[0] = {
      .volume = tvLeft ? 0x8000 : 0,
      .delta = 0,
    };

    mix[1].bus[0] = {
      .volume = tvRight ? 0x8000 : 0,
      .delta = 0,
    };

    AXSetVoiceDeviceMix(vc->m_voice, 0, 0, mix);

    mix[0].bus[0] = {
      .volume = drcLeft ? 0x8000 : 0,
      .delta = 0,
    };

    mix[1].bus[0] = {
      .volume = drcRight ? 0x8000 : 0,
      .delta = 0,
    };

    AXSetVoiceDeviceMix(vc->m_voice, 1, 0, mix);
    AXVoiceEnd(vc->m_voice);
}

void AudioMgr::InternalVoiceSetOffsets(Voice* vc, bool loop)
{
    assert(vc != nullptr);

    vc->m_voiceOffsets = {};
    vc->m_voiceOffsets = {
      .dataType = AX_VOICE_FORMAT_LPCM16,
      .loopingEnabled = loop ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED,
      .loopOffset =
        loop && vc->m_nextBuffer ? vc->m_nextBuffer - vc->m_buffer : 0,
      .endOffset = vc->m_bufferSampleCount - 1,
      .currentOffset = 0,
      .data = vc->m_buffer,
    };

    u32 samplesPerSec = AXGetInputSamplesPerSec();

    vc->m_voiceSrc = {};
    vc->m_voiceSrc.ratio =
      u32(0x00010000 * (float(vc->m_sampleRate) / float(samplesPerSec)));

    AXSetVoiceOffsets(vc->m_voice, &vc->m_voiceOffsets);
    AXSetVoiceSrc(vc->m_voice, &vc->m_voiceSrc);
    AXSetVoiceSrcType(vc->m_voice, 1);
}

void AudioMgr::executeThread()
{
    if (AXIsInit() == TRUE) {
        LOG(LogAudio, "AX is already initialized, restarting...");
        AXQuit();
    }

    AXInitParams params = {};
    params = {
      .renderer = AX_INIT_RENDERER_48KHZ,
      .pipeline = AX_INIT_PIPELINE_SINGLE,
    };

    AXInitWithParams(&params);

    s32 ret = AXRegisterAppFrameCallback(AXFrameCallbackFunc);

    if (ret != AX_RESULT_SUCCESS) {
        PANIC("AXRegisterAppFrameCallback failed: %d", ret);
    }

    while (true) {
        // Wait for AX frame callback
        OSMessage msg;
        if (!OSReceiveMessage(
              &m_frameCbQueue, &msg, OS_MESSAGE_FLAGS_BLOCKING)) {
            PANIC("OSReceiveMessage failed");
        }

        if (msg.args[0] == MSG_SHUTDOWN) {
            LOG(LogAudio, "Shutting down AudioMgr");

            for (u32 i = 0; i < m_voices.size(); i++) {
                if (m_voices[i].m_voice != nullptr)
                    AXFreeVoice(m_voices[i].m_voice);
            }

            AXQuit();
            return;
        }

        assert(msg.args[0] == MSG_AXFRAME);

        for (u32 i = 0; i < m_voices.size(); i++) {
            auto vc = &m_voices[i];

            auto sendBufferBack = [&](void* sbbBuffer, u32 sbbSampleCount) {
                if (vc->m_bufferOutQueue != nullptr) {
                    msg = {
                      .message = sbbBuffer,
                      .args =
                        {
                          sbbSampleCount,
                          i,
                        },
                    };

                    auto ret = OSSendMessage(
                      vc->m_bufferOutQueue, &msg, OS_MESSAGE_FLAGS_BLOCKING);
                    assert(ret == TRUE);
                } else {
                    // There is no situation where any audio player can
                    // get this buffer back, so I guess just attempt to
                    // free it?
                    free(vc->m_buffer);
                }
            };

            if (vc->m_inUse && vc->m_voice != nullptr) {
                // Check if we need a new buffer
                u32 loopCount = AXGetVoiceLoopCount(vc->m_voice);

                if (vc->m_streamed && vc->m_lastLoopCount != loopCount) {
                    vc->m_lastLoopCount = loopCount;
                    // Disable looping
                    AXSetVoiceLoop(vc->m_voice, AX_VOICE_LOOP_DISABLED);
                    if (vc->m_nextBuffer != nullptr) {
                        // Now we need to set the next end offset
                        AXSetVoiceEndOffset(
                          vc->m_voice, vc->m_voiceOffsets.loopOffset +
                                         vc->m_nextBufferSampleCount - 1);

                        sendBufferBack(vc->m_buffer, vc->m_bufferSampleCount);

                        vc->m_buffer = vc->m_nextBuffer;
                        vc->m_bufferSampleCount = vc->m_nextBufferSampleCount;
                        vc->m_nextBuffer = nullptr;
                        vc->m_nextBufferSampleCount = 0;
                    }
                }

                // For when a marker is changed.
                if (vc->m_streamed && vc->m_buffer == nullptr &&
                    vc->m_nextBuffer != nullptr) {
                    // Check if marker is less than current one and immediately
                    // send it back.
                    if (vc->m_marker > vc->m_nextBufferMarker) {
                        sendBufferBack(
                          vc->m_nextBuffer, vc->m_nextBufferSampleCount);
                        vc->m_nextBuffer = nullptr;
                    } else if (vc->m_marker == vc->m_nextBufferMarker) {
                        vc->m_buffer = vc->m_nextBuffer;
                        vc->m_bufferSampleCount = vc->m_nextBufferSampleCount;
                        vc->m_nextBuffer = nullptr;
                        vc->m_nextBufferSampleCount = 0;
                        InternalVoiceSetOffsets(vc, false);
                        if (vc->m_state == Voice::State::WaitingOnMarker) {
                            vc->m_state = Voice::State::Playing;
                            AXSetVoiceState(
                              vc->m_voice, AX_VOICE_STATE_PLAYING);
                        }
                    }
                }

                if (!vc->m_streamed ||
                    (vc->m_streamed && vc->m_nextBuffer == nullptr)) {
                    if (OSReceiveMessage(
                          &vc->m_bufferInQueue, &msg, OS_MESSAGE_FLAGS_NONE)) {
                        assert(msg.message != nullptr);

                        // Check if marker is less than current one and
                        // immediately send it back.
                        if (msg.args[1] < vc->m_marker) {
                            sendBufferBack(msg.message, msg.args[0]);
                        } else {
                            DCFlushRange(msg.message, msg.args[0] * 2);

                            if ((vc->m_buffer == nullptr || !vc->m_streamed) &&
                                vc->m_marker == msg.args[1]) {
                                vc->m_buffer = (u16*) msg.message;
                                vc->m_bufferSampleCount = msg.args[0];
                                InternalVoiceSetOffsets(vc, false);
                                if (vc->m_state ==
                                      Voice::State::WaitingOnMarker ||
                                    (vc->m_state == Voice::State::Playing &&
                                      vc->m_voice->state ==
                                        AX_VOICE_STATE_STOPPED)) {
                                    vc->m_state = Voice::State::Playing;
                                    AXSetVoiceState(
                                      vc->m_voice, AX_VOICE_STATE_PLAYING);
                                }
                            } else {
                                vc->m_nextBuffer = (u16*) msg.message;
                                vc->m_nextBufferSampleCount = msg.args[0];
                                vc->m_nextBufferMarker = msg.args[1];
                                vc->m_voiceOffsets.loopOffset =
                                  vc->m_nextBuffer -
                                  (u16*) vc->m_voiceOffsets.data;
                                if (vc->m_nextBufferMarker != vc->m_marker) {
                                    AXSetVoiceLoop(
                                      vc->m_voice, AX_VOICE_LOOP_DISABLED);
                                } else {
                                    AXSetVoiceLoop(
                                      vc->m_voice, AX_VOICE_LOOP_ENABLED);
                                    AXSetVoiceLoopOffset(vc->m_voice,
                                      vc->m_voiceOffsets.loopOffset);
                                }
                            }
                        }
                    }
                }
            }

            if (OSReceiveMessage(
                  &vc->m_ctrlQueue, &msg, OS_MESSAGE_FLAGS_NONE)) {
                switch (msg.args[0]) {
                case Voice::MSG_INIT:
                    vc->m_state = Voice::State::Stopped;
                    InternalInitVoice(i, vc->m_initArgs.volume,
                      vc->m_initArgs.tvLeft, vc->m_initArgs.tvRight,
                      vc->m_initArgs.drcLeft, vc->m_initArgs.drcRight);
                    break;

                case Voice::MSG_FREE: {
                    if (!vc->m_inUse || !vc->m_voice)
                        break;

                    AXSetVoiceState(vc->m_voice, AX_VOICE_STATE_STOPPED);
                    AXFreeVoice(vc->m_voice);

                    // Clear buffer in queue
                    while (OSReceiveMessage(
                      &vc->m_bufferInQueue, &msg, OS_MESSAGE_FLAGS_NONE)) {
                        if (vc->m_bufferOutQueue != nullptr)
                            OSSendMessage(vc->m_bufferOutQueue, &msg,
                              OS_MESSAGE_FLAGS_BLOCKING);
                    }

                    vc->m_voice = nullptr;

                    msg = {};
                    auto ret = OSSendMessage(
                      &vc->m_ctrlRespQueue, &msg, OS_MESSAGE_FLAGS_BLOCKING);
                    assert(ret);
                    break;
                }

                case Voice::MSG_START:
                    assert(vc->m_inUse && vc->m_voice);
                    vc->m_state = Voice::State::Playing;
                    AXSetVoiceState(vc->m_voice, AX_VOICE_STATE_PLAYING);
                    break;

                case Voice::MSG_STOP:
                    assert(vc->m_inUse && vc->m_voice);
                    vc->m_state = Voice::State::Stopped;
                    AXSetVoiceState(vc->m_voice, AX_VOICE_STATE_STOPPED);
                    break;

                case Voice::MSG_RESTART:
                    assert(vc->m_inUse && vc->m_voice);
                    AXSetVoiceCurrentOffset(vc->m_voice, 0);
                    break;

                case Voice::MSG_CHANGE_MARKER: {
                    assert(vc->m_inUse && vc->m_voice);
                    vc->m_state = Voice::State::WaitingOnMarker;
                    AXSetVoiceState(vc->m_voice, AX_VOICE_STATE_STOPPED);

                    u32 newMarker = msg.args[1];
                    if (newMarker <= vc->m_marker)
                        break;

                    if (vc->m_buffer != nullptr) {
                        sendBufferBack(vc->m_buffer, vc->m_bufferSampleCount);
                        vc->m_buffer = nullptr;
                    }

                    if (vc->m_nextBuffer != nullptr &&
                        vc->m_nextBufferMarker < newMarker) {
                        sendBufferBack(
                          vc->m_nextBuffer, vc->m_nextBufferSampleCount);

                        vc->m_nextBuffer = nullptr;

                        while (OSReceiveMessage(
                          &vc->m_bufferInQueue, &msg, OS_MESSAGE_FLAGS_NONE)) {
                            assert(msg.message != nullptr);
                            if (msg.args[1] < newMarker) {
                                sendBufferBack(msg.message, msg.args[0]);
                            } else if (msg.args[1] > newMarker) {
                                PANIC("We don't allow skipping markers!!!");
                            } else {
                                vc->m_nextBuffer = (u16*) msg.message;
                                vc->m_nextBufferSampleCount = msg.args[0];
                                vc->m_nextBufferMarker = msg.args[1];
                            }
                        }
                    }

                    vc->m_marker = newMarker;
                }
                }
            }
        }
    }
}

void AudioMgr::AXFrameCallbackFunc()
{
    OSMessage msg = {
      .message = nullptr,
      .args = {MSG_AXFRAME},
    };

    auto obj = AudioMgr::s_instance;
    assert(obj != nullptr);

    auto ret =
      OSSendMessage(&obj->m_frameCbQueue, &msg, OS_MESSAGE_FLAGS_BLOCKING);
    assert(ret == TRUE);
}
