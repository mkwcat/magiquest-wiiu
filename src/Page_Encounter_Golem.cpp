// Page_Encounter_Golem.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_Encounter_Golem.hpp"
#include <cstdlib>

enum {
    IMG_NOTSELECTED = 0,
    IMG_SELECTED = 1,
};

Page_Encounter_Golem::Page_Encounter_Golem()
{
    static const char* MagmaImages[] = {
      [IMG_NOTSELECTED] = "Magma",
      [IMG_SELECTED] = "MagmaSelected",
    };

    InitSpell(Spell::Magma, MagmaImages, -600, 0);

    static const char* ChiselImages[] = {
      [IMG_NOTSELECTED] = "Chisel",
      [IMG_SELECTED] = "ChiselSelected",
    };

    InitSpell(Spell::Chisel, ChiselImages, -200, 0);

    static const char* CloudHammerImages[] = {
      [IMG_NOTSELECTED] = "CloudHammer",
      [IMG_SELECTED] = "CloudHammerSelected",
    };

    InitSpell(Spell::CloudHammer, CloudHammerImages, 200, 0);

    static const char* ThunderImages[] = {
      [IMG_NOTSELECTED] = "Thunder",
      [IMG_SELECTED] = "ThunderSelected",
    };

    InitSpell(Spell::Thunder, ThunderImages, 600, 0);
}

Page_Encounter_Golem::~Page_Encounter_Golem()
{
}

void Page_Encounter_Golem::InitSpell(Spell spell, const char** images, int posX, int posY)
{
    u32 btn = u32(spell);
    assert(btn < SpellCount);

    m_buttons[btn].Init("SpellButton", RES_ROOT "/Image/Encounter/Spell/Golem", images, 2);
    m_buttons[btn].SetImages(IMG_SELECTED, IMG_NOTSELECTED, IMG_NOTSELECTED);

    m_buttons[btn].SetOnHoverHandler([&](Ctrl_Spell* spell) {
        DeselectAll();
        spell->Hover();
    });
    m_buttons[btn].SetOnReleaseHandler([&](Ctrl_Spell* spell) {
        // Do nothing
    });

    m_buttons[btn].setPosition(posX, posY);
    m_buttons[btn].setScaleX(1.5);
    m_buttons[btn].setScaleY(1.5);

    m_buttons[btn].SetHoverable(true);

    append(&m_buttons[btn]);
}

void Page_Encounter_Golem::TransitionSecond()
{
    m_nextPhase = Phase::Start;
    m_isInputPhase = false;
    ForceNextMovie();
}

const char* Page_Encounter_Golem::NextPhase(Spell castSpell)
{
    switch (m_currentPhase) {
    case Phase::End:
        m_nextPhase = Phase::Idle;
        SetMana(0, 0);
        SetMana(1, 0);
        // Fall through

    case Phase::Idle:
        m_isInputPhase = false;
        // No idle phase for Golem
        return nullptr;

    case Phase::Start:
        SetMana(0, 0);
        SetMana(1, 0);

        m_nextPhase = Phase::Intro;
        m_isInputPhase = false;
        return "GolemIntro";

    case Phase::Intro:
        SetMana(0, 16);
        SetMana(1, 16);
        // Fall through

    case Phase::Phase1Trigger:
        if (GetMana(0) == 0) {
            m_nextPhase = Phase::End;
            return "GolemFail";
        }

        m_nextPhase = Phase::Phase1Start;
        m_isInputPhase = true;
        return "GolemPhase1Start";

    case Phase::Phase1Start:
        m_isInputPhase = false;

        switch (castSpell) {
        case Spell::Chisel:
            SetMana(0, std::max(int(GetMana(0)) - 6, 0));
            m_nextPhase = Phase::Phase1Trigger;
            return "GolemPhase1Chisel";

        case Spell::CloudHammer:
            // The correct option
            m_nextPhase = Phase::Phase2Trigger;
            return "GolemPhase1CloudHammer";

        case Spell::Magma:
            SetMana(0, std::max(int(GetMana(0)) - 6, 0));
            m_nextPhase = Phase::Phase1Trigger;
            return "GolemPhase1Magma";

        case Spell::Thunder:
            SetMana(0, std::max(int(GetMana(0)) - 6, 0));
            m_nextPhase = Phase::Phase1Trigger;
            return "GolemPhase1Thunder";

        default:
            SetMana(0, std::max(int(GetMana(0)) - 6, 0));
            m_nextPhase = Phase::Phase1Trigger;
            return "GolemPhase1NoCast";
        }

    case Phase::Phase2Trigger:
        if (GetMana(0) == 0) {
            m_nextPhase = Phase::End;
            return "GolemFail";
        }

        m_nextPhase = Phase::Phase2Start;
        m_isInputPhase = true;
        return "GolemPhase2Start";

    case Phase::Phase2Start:
        m_isInputPhase = false;

        switch (castSpell) {
        case Spell::Chisel:
            SetMana(0, std::max(int(GetMana(0)) - 6, 0));
            m_nextPhase = Phase::Phase2Trigger;
            return "GolemPhase2Chisel";

        case Spell::CloudHammer:
            SetMana(0, std::max(int(GetMana(0)) - 6, 0));
            m_nextPhase = Phase::Phase2Trigger;
            return "GolemPhase2CloudHammer";

        case Spell::Magma:
            // The correct option
            m_nextPhase = Phase::Phase3Trigger;
            return "GolemPhase2Magma";

        case Spell::Thunder:
            SetMana(0, std::max(int(GetMana(0)) - 6, 0));
            m_nextPhase = Phase::Phase2Trigger;
            return "GolemPhase2Thunder";

        default:
            SetMana(0, std::max(int(GetMana(0)) - 6, 0));
            m_nextPhase = Phase::Phase2Trigger;
            return "GolemPhase2NoCast";
        }

    case Phase::Phase3Trigger:
        if (GetMana(0) == 0) {
            m_nextPhase = Phase::End;
            return "GolemFail";
        }

        m_nextPhase = Phase::Phase3Start;
        m_isInputPhase = true;
        return "GolemPhase3Start";

    case Phase::Phase3Start:
        m_isInputPhase = false;

        switch (castSpell) {
        case Spell::Chisel:
            SetMana(0, std::max(int(GetMana(0)) - 6, 0));
            m_nextPhase = Phase::Phase3Trigger;
            return "GolemPhase3Chisel";

        case Spell::CloudHammer:
            SetMana(0, std::max(int(GetMana(0)) - 6, 0));
            m_nextPhase = Phase::Phase3Trigger;
            return "GolemPhase3CloudHammer";

        case Spell::Magma:
            SetMana(0, std::max(int(GetMana(0)) - 6, 0));
            m_nextPhase = Phase::Phase3Trigger;
            return "GolemPhase3Magma";

        case Spell::Thunder:
            // The correct option
            m_isInputPhase = true;
            m_nextPhase = Phase::Phase4Start;
            return "GolemPhase3ThunderStart";

        default:
            SetMana(0, std::max(int(GetMana(0)) - 6, 0));
            m_nextPhase = Phase::Phase3Trigger;
            return "GolemPhase3NoCast";
        }

    case Phase::Phase4Start:
        m_isInputPhase = false;

        switch (castSpell) {
        case Spell::Chisel:
            // The correct option
            SetMana(1, std::max(int(GetMana(1)) - 6, 0));
            m_isInputPhase = true;
            m_nextPhase = Phase::Phase4Start;
            switch (GetMana(1)) {
            case 10 ... 16:
                return "GolemPhase4Chisel1";

            case 1 ... 9:
                return "GolemPhase4Chisel2";

            case 0:
            default:
                m_isInputPhase = false;
                m_nextPhase = Phase::End;
                return "GolemBattleSuccess";
            }

        case Spell::CloudHammer:
            SetMana(0, 0);
            m_nextPhase = Phase::End;
            return "GolemPhase4CloudHammer";

        case Spell::Magma:
            SetMana(0, 0);
            m_nextPhase = Phase::End;
            return "GolemPhase4Magma";

        case Spell::Thunder:
            SetMana(0, 0);
            m_nextPhase = Phase::End;
            return "GolemPhase4Thunder";

        default:
            SetMana(0, 0);
            m_nextPhase = Phase::End;
            return "GolemPhase3ThunderNoCast";
        }

    default:
        PANIC("Unknown phase");
    }
}

const char* Page_Encounter_Golem::NextMovie()
{
    Spell castSpell = m_castSpell;
    m_castSpell = Spell::None;

    m_currentPhase = m_nextPhase;

    auto name = NextPhase(castSpell);

    if (name == nullptr) {
        // Default to the web portal idle screen
        strcpy(m_phaseMoviePath, RES_ROOT "/Movie/SilverDragon/InternetPortalwTextandGameplay.mp4");
    } else {
        snprintf(m_phaseMoviePath, sizeof(m_phaseMoviePath), RES_ROOT "/Movie/Golem/%s.mp4", name);
    }

    // Disable all buttons
    for (u32 i = 0; i < SpellCount; i++) {
        m_buttons[i].Unhover();
        m_buttons[i].SetHoverable(m_isInputPhase);
    }

    return m_phaseMoviePath;
}

void Page_Encounter_Golem::Cast(
  Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ)
{
    LOG(LogSystem, "Cast");

    if (m_currentPhase == Phase::Idle || m_currentPhase == Phase::End) {
        m_nextPhase = Phase::Start;
        ForceNextMovie();
        return;
    }

    if (castMode == Wand::CastMode::WiiRemoteCastRune && curValid && m_isInputPhase &&
        (curX < 640 && curX > -640 && curY < 450 && curY > -450)) {
        for (u32 i = 0; i < SpellCount; i++) {
            if (!m_buttons[i].IsHoverable()) {
                continue;
            }

            auto x = m_buttons[i].getCenterX();
            auto y = m_buttons[i].getCenterY();
            if (curX > x - 240 && curX < x + 240 && curY > y - 240 && curY < y + 240) {
                DeselectAll();
                m_buttons[i].Hover();
            }
        }
        return;
    }

    if (!m_isInputPhase) {
        return;
    }

    for (u32 i = 0; i < SpellCount; i++) {
        if (!m_buttons[i].IsHoverable())
            continue;

        DeselectAll();
        m_castSpell = Spell(i);
        ForceNextMovie();
        break;
    }
}

void Page_Encounter_Golem::DeselectAll()
{
    for (u32 i = 0; i < SpellCount; i++) {
        m_buttons[i].Unhover();
    }
}
