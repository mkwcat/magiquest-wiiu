// Page_Movie.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_Movie.hpp"
#include "AXManager.hpp"
#include "System.hpp"
#include "Util.hpp"
#include <cstdio>

Page_Movie::Page_Movie()
  : m_imgManaLeft(nullptr)
  , m_imgManaRight(nullptr)
{
    OSInitMutexEx(&m_mutex, "Page_Movie::m_mutex");

    m_movie.SetOnEndHandler([&](Ctrl_Movie* movie) {
        Lock lock(m_mutex);
        if (m_encounter == nullptr)
            return;

        auto nextMovie = m_encounter->NextMovie();
        if (nextMovie == nullptr)
            return;

        m_movie.ChangeMovie(nextMovie);
    });

    m_movie.SetOnFrameHandler([&](Ctrl_Movie* movie, u32 frame) {
        Lock lock(m_mutex);
        if (m_encounter == nullptr)
            return;

        m_encounter->NextFrame(frame);
    });

    append(&m_movie);

    for (u32 i = 0; i < 17; i++) {
        char path[64];
        snprintf(path, 64, RES_ROOT "/Image/mana_l%u.png", i);

        m_manaLeft[i].loadImageFromFile(
          path, GX2_TEX_CLAMP_MODE_CLAMP, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8);

        snprintf(path, 64, RES_ROOT "/Image/mana_r%u.png", i);

        m_manaRight[i].loadImageFromFile(
          path, GX2_TEX_CLAMP_MODE_CLAMP, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8);
    }

    m_imgManaLeft.setImageData(GetManaImage(0, 0));
    m_imgManaRight.setImageData(GetManaImage(1, 0));

    m_imgManaLeft.setPosition(-(1920 / 2), -((1080 - 880) / 2));
    m_imgManaLeft.setScaleX(880 / m_imgManaLeft.getHeight());
    m_imgManaLeft.setScaleY(880 / m_imgManaLeft.getHeight());
    m_imgManaLeft.setAlignment(ALIGN_LEFT);

    m_imgManaRight.setPosition((1920 / 2), -((1080 - 880) / 2));
    m_imgManaRight.setScaleX(880 / m_imgManaRight.getHeight());
    m_imgManaRight.setScaleY(880 / m_imgManaRight.getHeight());
    m_imgManaRight.setAlignment(ALIGN_RIGHT);

    append(&m_imgManaLeft);
    append(&m_imgManaRight);

    m_movie.ChangeMovie(RES_ROOT "/Movie/Xavier/XavierIdle.mp4");
}

Page_Movie::~Page_Movie()
{
}

GuiImageData* Page_Movie::GetManaImage(u8 side, u8 value)
{
    if (value > 16) {
        return nullptr;
    }

    if (side == 0) {
        return &m_manaLeft[value];
    }

    if (side == 1) {
        return &m_manaRight[value];
    }

    return nullptr;
}

void Page_Movie::EndMovie()
{
    Lock lock(m_mutex);
    m_movie.ForceEndStream();
}

void Page_Movie::SetEncounter(Encounter* encounter)
{
    Lock lock(m_mutex);
    m_encounter = encounter;
}

void Page_Movie::ManaUpdate(u8 side, u8 value)
{
    if (side == 0)
        m_imgManaLeft.setImageData(GetManaImage(0, value));
    else if (side == 1)
        m_imgManaRight.setImageData(GetManaImage(1, value));

    if (value == 0)
        m_manaSound = ManaSound::ManaReset;

    if (m_manaSound == ManaSound::ManaReset)
        return;

    m_manaSound = ManaSound::ManaDown;
}

void Page_Movie::process()
{
    static bool started = false;
    static int voiceReset = -1;
    static int voiceDown = -1;
    if (!started) {
        started = true;
        LOG(LogAudio, "Making an audio player");

        auto ax = AXManager::s_instance;

        voiceReset = ax->AcquireVoice(
          0x30000000, true, true, true, true, 48000, true, false);
        assert(voiceReset != -1);

        voiceDown = ax->AcquireVoice(
          0x30000000, true, true, true, true, 48000, true, false);
        assert(voiceDown != -1);

        u32 dataLen;
        void* data = System::s_instance->RipFile(
          RES_ROOT "/Sound/mana_powerup.pcm16", &dataLen);
        assert(data != nullptr);

        ax->PushBuffer(voiceReset, (u16*) data, dataLen / 2, false);

        data = System::s_instance->RipFile(
          RES_ROOT "/Sound/manadown.pcm16", &dataLen);
        assert(data != nullptr);

        ax->PushBuffer(voiceDown, (u16*) data, dataLen / 2, false);
    }

    GuiFrame::process();

    Lock lock(m_mutex);

    if (m_manaSound != ManaSound::None) {
        AXManager::s_instance->Stop(voiceReset);
        AXManager::s_instance->Stop(voiceDown);

        if (m_manaSound == ManaSound::ManaReset) {
            AXManager::s_instance->Restart(voiceReset);
            AXManager::s_instance->Start(voiceReset);
        } else if (m_manaSound == ManaSound::ManaDown) {
            AXManager::s_instance->Restart(voiceDown);
            AXManager::s_instance->Start(voiceDown);
        }

        m_manaSound = ManaSound::None;
    }
}
