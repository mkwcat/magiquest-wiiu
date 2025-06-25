// Page_DuelSilverDragon.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_DuelSilverDragon.hpp"
#include "Page_Background.hpp"
#include <cstdlib>

enum {
    IMG_NOTSELECTED = 0,
    IMG_SELECTED = 1,
    IMG_DISABLED = 2,
};

void Page_DuelSilverDragon::InitSpell(
  Spell spell, const char* const* images, float posX, float posY, float size)
{
    u32 btn = u32(spell);
    assert(btn < SpellCount);

    m_buttons[btn].Init("SpellButton", RES_ROOT "/Image/Encounter/Spell/SilverDragon", images, 3);
    m_buttons[btn].SetImages(IMG_SELECTED, IMG_NOTSELECTED, IMG_DISABLED);

    m_buttons[btn].SetOnSelectHandler([&, spell](Ctrl_Spell* spellObj) {
        spellObj->Select();
        if (m_queueSpellCount < std::size(m_queueSpell)) {
            m_queueSpell[m_queueSpellCount++] = spell;
        }
    });
    m_buttons[btn].SetOnReleaseHandler([](Ctrl_Spell* spellObj) { spellObj->Deselect(); });

    float scale = 0.9;
    posX *= scale;
    posY *= scale;
    posX -= 100;
    posY += 50;

    m_buttons[btn].setPosition(posX, posY);
    scale *= 1080.0 / m_buttons[btn].getHeight() * 0.85;
    m_buttons[btn].setScaleX(scale * size);
    m_buttons[btn].setScaleY(scale * size);

    m_buttons[btn].SetSelectable(false);

    append(&m_buttons[btn]);
}

void Page_DuelSilverDragon::Init()
{
    using StringArray = const char* const[];

    InitSpell(Spell::Blue,
      StringArray{
        [IMG_NOTSELECTED] = "CrystalBlue",
        [IMG_SELECTED] = "CrystalBlueSelected",
        [IMG_DISABLED] = "CrystalBlueDisabled",
      },
      -250, -240, 0.3);

    InitSpell(Spell::Red,
      StringArray{
        [IMG_NOTSELECTED] = "CrystalRed",
        [IMG_SELECTED] = "CrystalRedSelected",
        [IMG_DISABLED] = "CrystalRedDisabled",
      },
      -480, 40, 0.3);

    InitSpell(Spell::White,
      StringArray{
        [IMG_NOTSELECTED] = "CrystalWhite",
        [IMG_SELECTED] = "CrystalWhiteSelected",
        [IMG_DISABLED] = "CrystalWhiteDisabled",
      },
      0, 300, 0.3);

    InitSpell(Spell::Purple,
      StringArray{
        [IMG_NOTSELECTED] = "CrystalPurple",
        [IMG_SELECTED] = "CrystalPurpleSelected",
        [IMG_DISABLED] = "CrystalPurpleDisabled",
      },
      480, 40, 0.3);

    InitSpell(Spell::Yellow,
      StringArray{
        [IMG_NOTSELECTED] = "CrystalYellow",
        [IMG_SELECTED] = "CrystalYellowSelected",
        [IMG_DISABLED] = "CrystalYellowDisabled",
      },
      250, -240, 0.3);

    InitSpell(Spell::Medal,
      StringArray{
        [IMG_NOTSELECTED] = "Medal",
        [IMG_SELECTED] = "MedalSelected",
        [IMG_DISABLED] = "MedalDisabled",
      },
      0, -30, 0.25);

    InitSpell(Spell::Shield,
      StringArray{
        [IMG_NOTSELECTED] = "Shield",
        [IMG_SELECTED] = "ShieldSelected",
        [IMG_DISABLED] = "ShieldDisabled",
      },
      740, 320, 0.26);
}

void Page_DuelSilverDragon::process()
{
    if (!m_initialized) {
        Init();
        m_initialized = true;
    }

    GuiFrame::process();
}

void Page_DuelSilverDragon::Transition()
{
    m_nextPhase = Phase::End;
    m_isInputPhase = false;
    ForceNextMovie();
}

const char* Page_DuelSilverDragon::NextPhase()
{
    switch (m_currentPhase) {
    case Phase::End:
        m_nextPhase = Phase::Idle;
        SetMana(0, 0);
        SetMana(1, 0);
        // Fall through

    case Phase::Idle:
        m_isInputPhase = false;
        return nullptr;

    case Phase::Start:
        m_nextPhase = Phase::Intro;
        m_isInputPhase = false;
        m_sequence = 0;
        m_started = true;
        SetMana(0, 0);
        SetMana(1, 0);
        return "SilverDragonDuelIntro";

    case Phase::Intro:
        SetMana(0, 16);
        SetMana(1, 16);
        m_patternCount = 0;
        // Fall through

    case Phase::Crystal: {
        int crystalCount = 7;
        if (m_sequence == 0) {
            crystalCount = 3;
        } else if (m_sequence == 1) {
            crystalCount = 5;
        }

        if (m_patternCount >= crystalCount) {
            m_nextPhase = Phase::Attack;
            m_queueSpell = {};
            m_queueSpellCount = 0;
            m_castSpell = {};
            m_castSpellCount = 0;
            m_isInputPhase = true;
            switch (m_sequence) {
            case 0:
                return "SilverDragonSequence1Attack";
            case 1:
                return "SilverDragonSequence2Attack";
            case 2:
                return "SilverDragonSequence3Attack";
            default:
                return "SilverDragonSequence4Attack";
            }
        }

        m_nextPhase = Phase::Crystal;
        m_isInputPhase = false;

        Spell crystal = static_cast<Spell>(Random(static_cast<u32>(Spell::Medal)));
        m_pattern[m_patternCount++] = crystal;

        switch (crystal) {
        case Spell::Blue:
            return "SilverDragonDuelBlueCrystal";
        case Spell::Red:
            return "SilverDragonDuelRedCrystal";
        case Spell::White:
            return "SilverDragonDuelWhiteCrystal";
        case Spell::Purple:
            return "SilverDragonDuelPurpleCrystal";
        case Spell::Yellow:
            return "SilverDragonDuelYellowCrystal";
        default:
            // Unknown crystal, should not happen
            LOG(LogSystem, "Unknown crystal type %d", static_cast<int>(crystal));
            return "SilverDragonDuelBlueCrystal";
        }
    }

    case Phase::Attack: {
        bool usedMedal = false;
        bool usedShield = false;
        int correct = 0;
        bool wrong = false;
        // Check if spells were cast in the correct order
        for (u32 i = 0; i < m_castSpellCount; i++) {
            LOG(LogSystem, "Cast spell %d", static_cast<int>(m_castSpell[i]));

            if (!usedMedal && m_castSpell[i] < Spell::Medal && !wrong) {
                if (m_castSpell[i] != m_pattern[correct]) {
                    wrong = true;
                } else {
                    correct++;
                }
            } else {
                if (m_castSpell[i] == Spell::Medal) {
                    usedMedal = true;
                } else if (m_castSpell[i] == Spell::Shield) {
                    usedShield = true;
                }
            }
        }

        if (wrong || correct != m_patternCount || !usedMedal) {
            // Fail
            if (usedShield && GetMana(0) == 16) {
                m_nextPhase = Phase::Crystal;
                m_patternCount = 0;
                m_isInputPhase = false;
                SetMana(0, 8);

                switch (m_sequence) {
                case 0:
                    return "SilverDragonSequence1Shield";
                case 1:
                    return "SilverDragonSequence2Shield";
                case 2:
                    return "SilverDragonSequence3Shield";
                default:
                    return "SilverDragonSequence4Shield";
                }
            } else {
                m_nextPhase = Phase::End;
                m_isInputPhase = false;
                SetMana(0, 0);

                switch (m_sequence) {
                case 0:
                    return "SilverDragonSequence1Fail";
                case 1:
                    return "SilverDragonSequence2Fail";
                case 2:
                    return "SilverDragonSequence3Fail";
                default:
                    return "SilverDragonSequence4Fail";
                }
            }
        }

        // Win
        m_nextPhase = Phase::Crystal;
        m_patternCount = 0;
        m_isInputPhase = false;
        SetMana(1, 16 - ++m_sequence * 4);
        switch (m_sequence) {
        case 1:
            return "SilverDragonSequence1Success";
        case 2:
            return "SilverDragonSequence2Success";
        case 3:
            return "SilverDragonSequence3Success";
        default:
            m_nextPhase = Phase::End;
            return "SilverDragonSequence4Success";
        }
    }

    default:
        PANIC("Unknown phase");
    }
}

const char* Page_DuelSilverDragon::NextMovie()
{
    m_currentPhase = m_nextPhase;

    auto name = NextPhase();

    if (name == nullptr) {
        // Default to the web portal idle screen
        strcpy(m_phaseMoviePath, RES_ROOT "/Movie/SilverDragon/InternetPortalwTextandGameplay.mp4");
    } else {
        snprintf(
          m_phaseMoviePath, sizeof(m_phaseMoviePath), RES_ROOT "/Movie/SilverDragon/%s.mp4", name);
    }

    // Disable all buttons
    for (Spell i :
      {Spell::Blue, Spell::Red, Spell::White, Spell::Purple, Spell::Yellow, Spell::Medal}) {
        m_buttons[u32(i)].SetSelectable(m_isInputPhase);
    }

    m_buttons[u32(Spell::Shield)].SetSelectable(m_isInputPhase && GetMana(0) == 16);

    return m_phaseMoviePath;
}

void Page_DuelSilverDragon::Cast(
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
            if (!m_buttons[i].IsSelectable()) {
                continue;
            }

            auto x = m_buttons[i].getCenterX();
            auto y = m_buttons[i].getCenterY();
            if (curX > x - 240 && curX < x + 240 && curY > y - 240 && curY < y + 240) {
                DeselectAll();
                m_buttons[i].Select();
                if (m_queueSpellCount < std::size(m_queueSpell)) {
                    m_queueSpell[m_queueSpellCount++] = static_cast<Spell>(i);
                }
            }
        }
        return;
    }

    if (!m_isInputPhase) {
        return;
    }

    m_castSpell = m_queueSpell;
    m_castSpellCount = m_queueSpellCount;
}

void Page_DuelSilverDragon::DeselectAll()
{
    for (u32 i = 0; i < SpellCount; i++) {
        m_buttons[i].Deselect();
    }
}
