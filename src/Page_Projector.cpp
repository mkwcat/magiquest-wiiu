// Page_Projector.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_Projector.hpp"
#include "AXManager.hpp"
#include "System.hpp"
#include "Util.hpp"
#include <cstdio>

constexpr const char IdleMovie[] = RES_ROOT "/Movie/Dragon/Dragon0001.mp4";

Page_Projector::Page_Projector()
  : m_movie(true)
  , m_movieImg(nullptr)
{
    OSInitMutexEx(&m_mutex, "Page_Projector::m_mutex");

    m_movie.SetOnEndHandler([&](Ctrl_Movie* movie) {
        Lock lock(m_mutex);
        if (m_encounter == nullptr) {
            m_movie.ChangeMovie(IdleMovie);
            return;
        }

        auto nextMovie = m_encounter->NextMovie();
        if (nextMovie == nullptr) {
            m_movie.ChangeMovie(IdleMovie);
            return;
        }

        m_movie.ChangeMovie(nextMovie);
    });

    m_movie.SetOnFrameHandler([&](Ctrl_Movie* movie, u32 frame) {
        Lock lock(m_mutex);
        if (m_encounter == nullptr) {
            return;
        }

        m_encounter->NextFrame(frame);
    });

    append(&m_movieImg);

    m_movie.ChangeMovie(RES_ROOT "/Movie/Dragon/Dragon0001.mp4");
}

Page_Projector::~Page_Projector()
{
}

void Page_Projector::EndMovie()
{
    Lock lock(m_mutex);
    m_movie.ForceEndStream();
}

void Page_Projector::SetEncounter(Encounter* encounter)
{
    Lock lock(m_mutex);
    m_encounter = encounter;
}

void Page_Projector::ManaUpdate(u8 side, u8 value)
{
    if (value == 0) {
        m_manaSound = ManaSound::ManaReset;
    }

    if (m_manaSound == ManaSound::ManaReset) {
        return;
    }

    m_manaSound = ManaSound::ManaDown;
}

void Page_Projector::process()
{
    static int voiceReset = -1;
    static int voiceDown = -1;
    if (!m_initialized) {
        m_initialized = true;

        auto ax = AXManager::s_instance;

        voiceReset = ax->AcquireVoice(0x30000000, true, true, true, true, 48000, true, false);
        assert(voiceReset != -1);

        voiceDown = ax->AcquireVoice(0x30000000, true, true, true, true, 48000, true, false);
        assert(voiceDown != -1);

        u32 dataLen;
        void* data = System::s_instance->RipFile(RES_ROOT "/Sound/ManaReset.pcm16", &dataLen);
        assert(data != nullptr);

        ax->PushBuffer(voiceReset, (u16*) data, dataLen / 2, false);

        data = System::s_instance->RipFile(RES_ROOT "/Sound/ManaDown.pcm16", &dataLen);
        assert(data != nullptr);

        ax->PushBuffer(voiceDown, (u16*) data, dataLen / 2, false);
    }

    m_movie.process();

    m_movieImg.setImageData(&m_movie);
    m_movieImg.setScaleX(1440.0 / m_movieImg.getWidth());
    m_movieImg.setScaleY(1080.0 / m_movieImg.getHeight());

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
