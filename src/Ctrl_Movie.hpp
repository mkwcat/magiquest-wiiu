// Ctrl_Movie.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

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

class Ctrl_Movie : public GuiElement
{
public:
    Ctrl_Movie();
    Ctrl_Movie(const char* path);
    ~Ctrl_Movie();

    static constexpr u32 MaxWidth = 768;
    static constexpr u32 MaxHeight = 480;

    /**
     * GuiElement process.
     */
    void process();

    /**
     * GuiElement draw.
     */
    void draw(CVideo* video);

    /**
     * Video decoding procedure.
     */
    void VideoDecodeProc();

    /**
     * Video decoding procedure.
     */
    void AudioDecodeProc();

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
    /**
     * Left audio voice.
     */
    int m_voiceL;

    /**
     * Right audio voice.
     */
    int m_voiceR;

    /**
     * Left voice buffer return message data.
     */
    OSMessage m_voiceLReturnMsg[8];

    /**
     * Left voice buffer return.
     */
    OSMessageQueue m_voiceLReturnQueue;

    /**
     * Right voice buffer return message data.
     */
    OSMessage m_voiceRReturnMsg[8];

    /**
     * Right voice buffer return.
     */
    OSMessageQueue m_voiceRReturnQueue;

    GuiImage m_image;
    GuiImageData m_imageData;
    std::unique_ptr<u8> m_rgbFrame;
    u32 m_rgbFrameSize;

    void* m_savedImageData;

    static constexpr u32 FrameBufferCount = 4;

    std::unique_ptr<u8> m_fbData;

    OSMessage m_fbInMsg[FrameBufferCount];
    OSMessageQueue m_fbInQueue;
    OSMessage m_fbOutMsg[FrameBufferCount];
    OSMessageQueue m_fbOutQueue;

    enum class CtrlCmd {
        ChangeMovie,
    };

    enum class AudioCtrlCmd {
        ChangeAudio,
    };

    OSMessage m_ctrlMsg[8];
    OSMessageQueue m_ctrlQueue;
    OSMessage m_audioCtrlMsg[8];
    OSMessageQueue m_audioCtrlQueue;

    /**
     * Audio buffers for use in the voice queues.
     */
    std::unique_ptr<u16> m_audioData;

    /**
     * The sample count for a single audio buffer.
     */
    u32 m_audioBufferSize;

    class Decoder
    {
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
        bool DecodeFrame(void* outFb, u32* outWidth, u32* outHeight);

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
        std::unique_ptr<u8[]> m_sampleData;

        static constexpr int MaxCacheSize = 4096 * 2;

        u32 m_cacheOffset;
        u32 m_cacheSize;
        u8 m_cache[MaxCacheSize];
        u32 m_seekOffset;

        std::unique_ptr<u8[]> m_memory;
        std::unique_ptr<u8> m_nv12Fb;

        /**
         * The H264 response queue
         */
        OSMessage m_respMsg[4];
        OSMessageQueue m_respQueue;

        OggVorbis_File m_oggFile;
        FILE* m_audioFile;

        std::unique_ptr<u16> m_audioBuffer;
        u32 m_audioBufferSize;
    };

    class DecoderThread : public CThread
    {
    public:
        DecoderThread()
          : CThread(CThread::eAttributeNone, 16, 0x10000)
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

    Decoder m_decoder;
    DecoderThread m_videoThread;
    DecoderThread m_audioThread;

    OSMutex m_fileMutex;

    bool m_streamEnded;
};
