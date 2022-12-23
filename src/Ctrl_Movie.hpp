// Ctrl_Movie.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "AXVoiceStream.hpp"
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
#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>

class Ctrl_Movie : public GuiImageData
{
public:
    Ctrl_Movie();
    Ctrl_Movie(const char* path);
    ~Ctrl_Movie();

    static constexpr u32 MaxWidth = 768;
    static constexpr u32 MaxHeight = 768;

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
        DecoderThread()
          : CThread(CThread::eAttributeNone, 16, 0x10000)
        {
        }

        DecoderThread(s32 priority)
          : CThread(CThread::eAttributeNone, priority, 0x10000)
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

    DecoderThread m_videoThread{20};
    DecoderThread m_videoNV12Thread{10};
    DecoderThread m_audioThread{1};

    std::unique_ptr<u8> m_rgbFrame;
    u32 m_rgbFrameSize;

    void* m_savedImageData;

    static constexpr u32 FrameBufferCount = 4;
    static constexpr u32 FBQueueCount = 8;

    std::unique_ptr<u8> m_fbData;
    std::unique_ptr<u8> m_fbNv12Data;

    OSMessage m_fbInMsg[FBQueueCount];
    OSMessageQueue m_fbInQueue;
    OSMessage m_fbNv12Msg[FBQueueCount];
    OSMessageQueue m_fbNv12Queue;
    OSMessage m_fbOutMsg[FBQueueCount];
    OSMessageQueue m_fbOutQueue;

    enum class CtrlCmd {
        ChangeMovie,
        Shutdown,
    };

    enum class AudioCtrlCmd {
        ChangeAudio,
        Shutdown,
    };

    OSMessage m_ctrlMsg[8];
    OSMessageQueue m_ctrlQueue;
    OSMessage m_audioCtrlMsg[8];
    OSMessageQueue m_audioCtrlQueue;

    /**
     * Left audio voice.
     */
    AXVoiceStream m_voiceL[2];

    /**
     * Right audio voice.
     */
    AXVoiceStream m_voiceR[2];

    int m_curVoice = 0;

    OSMessage m_audioNotifyMsg[4];
    OSMessageQueue m_audioNotifyQueue;

    class Decoder
    {
        friend class Ctrl_Movie;

        /**
         * Parent class.
         */
        Ctrl_Movie* m_parent;

    public:
        Decoder(Ctrl_Movie* parent, u32 profile, u32 level, u32 maxWidth,
          u32 maxHeight);
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
        vorbis_info* OpenAudio(FILE* audioFile);

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
        static int MP4ReadCallback(
          s64 offset, void* buffer, size_t size, void* token);

        /**
         * H264DEC frame output callback.
         */
        static void H264FrameCallback(H264DecodeOutput* output);

        /**
         * OGG read callback.
         */
        static size_t OGGRead(void* punt, size_t bytes, size_t blocks, void* f);

        /**
         * OGG seek callback.
         */
        static int OGGSeek(void* f, ogg_int64_t offset, int mode);

        /**
         * OGG close callback.
         */
        static int OGGClose(void* f);

        /**
         * OGG tell callback.
         */
        static long OGGTell(void* f);

        /**
         * OGG callbacks struct.
         */
        static const ov_callbacks s_oggCallbacks;

        /**
         * The MP4 file to read from, assumed valid if not nullptr.
         */
        FILE* m_file;
        u32 m_fileSize;

        MP4D_demux_t m_mp4;
        MP4D_track_t* m_tr;
        u32 m_sample;
        u32 m_frameNum;
        std::unique_ptr<u8[]> m_sampleData;

        static constexpr int MaxCacheSize = 4096 * 2;

        u32 m_cacheOffset;
        u32 m_cacheSize;
        u8 m_cache[MaxCacheSize];
        u32 m_seekOffset;

        std::unique_ptr<u8[]> m_memory;

        /**
         * Data for the H264 decoding callback.
         */
        OSMutex m_h264Mutex;
        void* m_inputNV12Data;
        void* m_inputRGBAData;
        OSMessageQueue* m_inputRespQueue;

        OggVorbis_File m_oggFile;
        FILE* m_audioFile;

        std::unique_ptr<u16> m_audioBuffer;
        u32 m_audioBufferSize;
    };

    Decoder m_decoder;

    bool m_streamEnded;
};
