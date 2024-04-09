// Page_DuelGolem.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_DuelGolem.hpp"
#include "Page_Movie.hpp"
#include <cstdlib>

Page_DuelGolem::Page_DuelGolem()
{
    m_currentPhase = Phase::Idle;
    m_nextPhase = Phase::Idle;
    m_isInputPhase = false;
    m_started = false;

    m_initialized = false;
}

Page_DuelGolem::~Page_DuelGolem()
{
}

enum {
    IMG_NOTSELECTED = 0,
    IMG_SELECTED = 1,
};

void Page_DuelGolem::InitSpell(
  Spell spell, const char** images, int posX, int posY)
{
    u32 btn = u32(spell);
    assert(btn < SpellCount);

    m_buttons[btn].Init("SpellButton", images, 2);
    m_buttons[btn].SetImages(IMG_SELECTED, IMG_NOTSELECTED, IMG_NOTSELECTED);

    m_buttons[btn].SetOnSelectHandler([&](Ctrl_Spell* spell) {
        DeselectAll();
        spell->Select();
    });
    m_buttons[btn].SetOnReleaseHandler([&](Ctrl_Spell* spell) {
        // Do nothing
    });

    m_buttons[btn].setPosition(posX, posY);
    m_buttons[btn].setScaleX(1.5);
    m_buttons[btn].setScaleY(1.5);

    m_buttons[btn].SetSelectable(true);

    append(&m_buttons[btn]);
}

void Page_DuelGolem::Init()
{
    auto page = System::GetPageStatic<Page_Movie>();
    assert(page != nullptr);

    page->SetEncounter(this);

    m_manaLeft.Update(Ctrl_Mana::Left, 0);
    m_manaRight.Update(Ctrl_Mana::Right, 0);

    m_manaLeft.setPosition(-(1920 / 2) + 40, -(1080 / 2) + 35);
    m_manaLeft.setAlignment(ALIGN_LEFT | ALIGN_BOTTOM);
    m_manaLeft.setScaleX((1080 / m_manaLeft.getHeight()) * 0.85);
    m_manaLeft.setScaleY((1080 / m_manaLeft.getHeight()) * 0.85);

    m_manaRight.setPosition((1920 / 2) - 40, -(1080 / 2) + 35);
    m_manaRight.setAlignment(ALIGN_RIGHT | ALIGN_BOTTOM);
    m_manaRight.setScaleX((1080 / m_manaRight.getHeight()) * 0.85);
    m_manaRight.setScaleY((1080 / m_manaRight.getHeight()) * 0.85);

    append(&m_manaLeft);
    append(&m_manaRight);

    static const char* MagmaImages[] = {
      [IMG_NOTSELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/Magma.png",
      [IMG_SELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/MagmaSelected.png",
    };

    InitSpell(Spell::Magma, MagmaImages, -600, 0);

    static const char* ChiselImages[] = {
      [IMG_NOTSELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/Chisel.png",
      [IMG_SELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/ChiselSelected.png",
    };

    InitSpell(Spell::Chisel, ChiselImages, -200, 0);

    static const char* CloudHammerImages[] = {
      [IMG_NOTSELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/CloudHammer.png",
      [IMG_SELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/CloudHammerSelected.png",
    };

    InitSpell(Spell::CloudHammer, CloudHammerImages, 200, 0);

    static const char* ThunderImages[] = {
      [IMG_NOTSELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/Thunder.png",
      [IMG_SELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/ThunderSelected.png",
    };

    InitSpell(Spell::Thunder, ThunderImages, 600, 0);
}

void Page_DuelGolem::process()
{
    if (!m_initialized) {
        Init();
        m_initialized = true;
    }

    auto page = System::GetPageStatic<Page_Movie>();
    assert(page != nullptr);

    m_manaLeft.Update(Ctrl_Mana::Left, GetMana(Ctrl_Mana::Left));
    m_manaRight.Update(Ctrl_Mana::Right, GetMana(Ctrl_Mana::Right));

    GuiFrame::process();
}

void Page_DuelGolem::Transition()
{
    System::GetPageStatic<Page_Background>()->SetImage(
      Page_Background::ImageType::TouchDuelGolem);
}

const char* Page_DuelGolem::NextPhase(Spell castSpell)
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

const char* Page_DuelGolem::NextMovie()
{
    Spell castSpell = m_castSpell;
    m_castSpell = Spell::None;

    m_currentPhase = m_nextPhase;

    auto name = NextPhase(castSpell);

    if (name == nullptr) {
        // Default to the Xavier idle screen
        strcpy(m_phaseMoviePath, RES_ROOT "/Movie/Xavier/XavierIdle.mp4");
    } else {
        snprintf(m_phaseMoviePath, sizeof(m_phaseMoviePath),
          RES_ROOT "/Movie/Golem/%s.mp4", name);
    }

    // Disable all buttons
    for (u32 i = 0; i < SpellCount; i++) {
        m_buttons[i].Deselect();
        m_buttons[i].SetSelectable(m_isInputPhase);
    }

    return m_phaseMoviePath;
}

void Page_DuelGolem::Cast(
  Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ)
{
    LOG(LogSystem, "Cast");

    // Ignore if this page is not visible.
    if (!System::s_instance->GetSetting(System::GetPageID(this))->drc) {
        return;
    }

    if (castMode == Wand::CastMode::WiiRemoteCastRune && curValid &&
        m_isInputPhase &&
        (curX < 640 && curX > -640 && curY < 450 && curY > -450)) {
        for (u32 i = 0; i < SpellCount; i++) {
            if (!m_buttons[i].IsSelectable())
                continue;

            auto x = m_buttons[i].getCenterX();
            auto y = m_buttons[i].getCenterY();
            if (curX > x - 240 && curX < x + 240 && curY > y - 240 &&
                curY < y + 240) {
                DeselectAll();
                m_buttons[i].Select();
            }
        }
        return;
    }

    if (m_currentPhase == Phase::Idle || m_currentPhase == Phase::End) {
        m_nextPhase = Phase::Start;
        ForceNextMovie();
        return;
    }

    if (!m_isInputPhase) {
        return;
    }

    for (u32 i = 0; i < SpellCount; i++) {
        if (!m_buttons[i].IsSelected())
            continue;

        DeselectAll();
        m_castSpell = Spell(i);
        ForceNextMovie();
        break;
    }
}

void Page_DuelGolem::DeselectAll()
{
    for (u32 i = 0; i < SpellCount; i++) {
        m_buttons[i].Deselect();
    }
}
