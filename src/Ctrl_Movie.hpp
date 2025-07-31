// Ctrl_Movie.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "AudioVoiceStream.hpp"
#include "Util.hpp"
#include "minimp4.h"
#include <coreinit/messagequeue.h>
#include <cstdio>
#include <functional>
#include <gctypes.h>
#include <gui/GuiElement.h>
#include <gui/GuiImage.h>
#include <gui/GuiImageData.h>
#include <gui/system/CThread.h>
#include <h264/decode.h>
#include <memory>
#include <opusfile.h>

class Ctrl_Movie : public GuiImageData
{
public:
    Ctrl_Movie(bool audio, u32 maxWidth = 768, u32 maxHeight = 768);
    ~Ctrl_Movie();

    /**
     * GuiElement process.
     */
    void process();

    /**
     * Video decoding thread entry.
     */
    void VideoDecodeProc();

    /**
     * NV12 decoding thread entry.
     */
    void VideoYUV2RGBProc();

    /**
     * Audio decoding thread entry.
     */
    void AudioDecodeProc();

public:
    /**
     * Change the movie that is currently playing.
     */
    bool ChangeMovie(const char* path);

    /**
     * End the current movie stream.
     */
    void ForceEndStream();

private:
    /**
     * Called when the stream ends.
     */
    std::function<void(Ctrl_Movie*)> m_onEndHandler = nullptr;

    /**
     * Called every frame.
     */
    std::function<void(Ctrl_Movie*, u32)> m_onFrameHandler = nullptr;

public:
    /**
     * Set the function to call when the stream ends.
     */
    void SetOnEndHandler(std::function<void(Ctrl_Movie*)> handler);

    /**
     * Set the function to call every frame.
     */
    void SetOnFrameHandler(std::function<void(Ctrl_Movie*, u32)> handler);

private:
    class DecoderThread : public CThread
    {
    public:
        DecoderThread(s32 priority = 16, int core = -1)
          : CThread(core == 0   ? CThread::eAttributeAffCore0 | CThread::eAttributePinnedAff
                    : core == 1 ? CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff
                    : core == 2 ? CThread::eAttributeAffCore2 | CThread::eAttributePinnedAff
                                : CThread::eAttributeNone,
              priority, 0x10000)
        {
        }

        void Run(std::function<void()> entry)
        {
            m_entry = entry;
            resumeThread();
        }

        void executeThread() override
        {
            m_entry();
        }

    private:
        std::function<void()> m_entry;
    };

    DecoderThread m_videoThread{1, 2};
    DecoderThread m_videoNV12Thread{2, 2};
    DecoderThread m_audioThread{1, 0};

    bool m_audio = false;
    u8 m_frameRateType = 0;
    u32 m_maxWidth = 0;
    u32 m_maxHeight = 0;

    std::unique_ptr<u8> m_rgbFrame;
    u32 m_rgbFrameSize = 0;

    void* m_savedImageData = nullptr;

    static constexpr u32 FrameBufferCount = 4;
    static constexpr u32 FBQueueCount = 8;

    std::unique_ptr<u8> m_fbData;
    std::unique_ptr<u8> m_fbNv12Data;

    OSMessage m_fbInMsg[FBQueueCount] = {};
    OSMessageQueue m_fbInQueue = {};
    OSMessage m_fbNv12Msg[FBQueueCount] = {};
    OSMessageQueue m_fbNv12Queue = {};
    OSMessage m_fbOutMsg[FBQueueCount] = {};
    OSMessageQueue m_fbOutQueue = {};

    enum class CtrlCmd {
        ChangeMovie,
        Shutdown,
    };

    enum class AudioCtrlCmd {
        ChangeAudio,
        Shutdown,
    };

    OSMessage m_ctrlMsg[8] = {};
    OSMessageQueue m_ctrlQueue = {};
    OSMessage m_audioCtrlMsg[8] = {};
    OSMessageQueue m_audioCtrlQueue = {};

    /**
     * Left audio voice.
     */
    AudioVoiceStream m_voiceL[2];

    /**
     * Right audio voice.
     */
    AudioVoiceStream m_voiceR[2];

    int m_curVoice = 0;

    OSMessage m_audioNotifyMsg[4] = {};
    OSMessageQueue m_audioNotifyQueue = {};

    class Decoder
    {
        friend class Ctrl_Movie;

        /**
         * Parent class.
         */
        Ctrl_Movie* m_parent;

    public:
        Decoder(Ctrl_Movie* parent, u32 profile, u32 level, u32 maxWidth, u32 maxHeight);
        ~Decoder();

        /**
         * Open the movie file for parsing. Automatically closes any previous
         * context.
         */
        bool OpenMovie(FILE* movieFile);

        /**
         * Open the audio file for parsing. Automatically closes any previous
         * context.
         */
        bool OpenAudio(FILE* audioFile);

        /**
         * Close the current movie context.
         */
        void CloseMovie();

        /**
         * Read raw data from the file stream.
         */
        bool Read(u8* out, u32 offset, u32 len);

        /**
         * Decode a frame from the stream. Returns false if no frame is
         * available (end of stream).
         */
        bool DecodeFrame(u8* nv12Fb, u8* rgbaFb, OSMessageQueue* respQueue);

        /**
         * Decode audio from the stream.
         * @param end Notifies if the audio stream has ended.
         * @returns The number of samples decoded.
         */
        u32 DecodeAudio(u16* bufferL, u16* bufferR, u32 sampleCount, bool* end);

    protected:
        /**
         * Read function for minimp4.
         */
        static int MP4ReadCallback(s64 offset, void* buffer, size_t size, void* token);

        /**
         * H264DEC frame output callback.
         */
        static void H264FrameCallback(H264DecodeOutput* output);

        /**
         * OGG read callback.
         */
        static int OGGRead(void* object, u8* ptr, int count);

        /**
         * OGG seek callback.
         */
        static int OGGSeek(void* object, opus_int64 offset, int whence);

        /**
         * OGG close callback.
         */
        static int OGGClose(void* object);

        /**
         * OGG tell callback.
         */
        static s64 OGGTell(void* object);

        /**
         * OGG callbacks struct.
         */
        static const OpusFileCallbacks s_oggCallbacks;

        /**
         * The MP4 file to read from, assumed valid if not nullptr.
         */
        FILE* m_file = nullptr;
        u32 m_fileSize = 0;

        MP4D_demux_t m_mp4 = {};
        MP4D_track_t* m_tr = nullptr;
        u32 m_sample = 0;
        u32 m_frameNum = 0;
        double m_frameRate = 60.0;
        std::unique_ptr<u8[]> m_sampleData;

        static constexpr int MaxCacheSize = 4096 * 2;

        u32 m_cacheOffset = 0;
        u32 m_cacheSize = 0;
        alignas(32) u8 m_cache[MaxCacheSize] = {};
        u32 m_seekOffset = 0;

        std::unique_ptr<u8[]> m_memory;

        /**
         * Data for the H264 decoding callback.
         */
        OSMutex m_h264Mutex = {};
        void* m_inputNV12Data = nullptr;
        void* m_inputRGBAData = nullptr;
        OSMessageQueue* m_inputRespQueue = nullptr;

        OggOpusFile* m_oggFile = nullptr;
        FILE* m_audioFile = nullptr;

        std::unique_ptr<opus_int16> m_audioBuffer;
        u32 m_audioBufferSize = 0;
    };

    Decoder m_decoder;

    bool m_streamEnded = false;
};
