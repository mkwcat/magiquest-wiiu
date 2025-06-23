// Page_DuelGolem.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_DuelDragon.hpp"
#include "Page_DuelGolem.hpp"
#include "Page_Movie.hpp"
#include <cstdlib>

enum {
    IMG_NOTSELECTED = 0,
    IMG_SELECTED = 1,
};

void Page_DuelDragon::InitSpell(Spell spell, const char* const* images, int posX, int posY)
{
    u32 btn = u32(spell);
    assert(btn < SpellCount);

    m_buttons[btn].Init("SpellButton", RES_ROOT "/Image/Encounter/Spell/Common", images, 2);
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

void Page_DuelDragon::Init()
{
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

    using StringArray = const char* const[];

    InitSpell(Spell::Protection,
      StringArray{
        [IMG_NOTSELECTED] = "Protection",
        [IMG_SELECTED] = "ProtectionSelected",
      },
      -400, 0);

    InitSpell(Spell::Freeze,
      StringArray{
        [IMG_NOTSELECTED] = "Freeze",
        [IMG_SELECTED] = "FreezeSelected",
      },
      0, 0);

    InitSpell(Spell::IceArrow,
      StringArray{
        [IMG_NOTSELECTED] = "IceArrow",
        [IMG_SELECTED] = "IceArrowSelected",
      },
      400, 0);
}

void Page_DuelDragon::process()
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

void Page_DuelDragon::Transition()
{
    auto movie = System::GetPageStatic<Page_Movie>();
    assert(movie != nullptr);

    movie->SetEncounter(this);
    m_nextPhase = Phase::End;
    m_isInputPhase = false;
    ForceNextMovie();

    System::GetPageStatic<Page_Background>()->SetImage(
      Page_Background::ImageType::TouchDuelBlizzardDawn);
}

const char* Page_DuelDragon::NextPhase(Spell castSpell)
{
    int damage;

    switch (m_currentPhase) {
    case Phase::End:
        m_nextPhase = Phase::Idle;
        SetMana(0, 0);
        SetMana(1, 0);
        // Fall through

    case Phase::Idle:
        m_isInputPhase = false;
        return "Dragon0001";

    case Phase::Start:
        m_nextPhase = Phase::Attack;
        m_isInputPhase = true;
        m_magiHitCount = 0;
        m_dragonHitCount = 0;
        m_started = true;
        return "Dragon0002";

    case Phase::Attack:
        damage = GetDamageTable(castSpell == Spell::Protection, false)[m_magiHitCount];
        if (m_magiHitCount < 9) {
            m_magiHitCount++;
        }
        SetMana(0, std::max(int(GetMana(0)) - damage, 0));

        if (GetMana(0) == 0) {
            // Lose
            m_nextPhase = Phase::End;
            m_isInputPhase = false;
            return "Dragon0010A";
        }

        m_nextPhase = Phase::Retreat;
        m_isInputPhase = true;
        if (m_dragonHitCount == 0) {
            return "Dragon0003";
        } else if (m_dragonHitCount == 1) {
            return "Dragon0012A";
        } else {
            return "Dragon0015A";
        }

    case Phase::Retreat:
        if (castSpell == Spell::Freeze) {
            m_nextPhase = Phase::Freeze;
            m_isInputPhase = true;

            if (m_dragonHitCount == 0) {
                return "Dragon0011";
            } else if (m_dragonHitCount == 1) {
                return "Dragon0014";
            } else {
                return "Dragon0017";
            }
        } else {
            m_nextPhase = Phase::FreezeFail;
            m_isInputPhase = false;

            if (m_dragonHitCount == 0) {
                return "Dragon0004";
            } else if (m_dragonHitCount == 1) {
                return "Dragon0006";
            } else {
                return "Dragon0008";
            }
        }

    case Phase::FreezeFail:
        damage = GetDamageTable(false, false)[m_magiHitCount];
        if (m_magiHitCount < 9) {
            m_magiHitCount++;
        }
        SetMana(0, std::max(int(GetMana(0)) - damage, 0));

        if (GetMana(0) == 0) {
            // Lose
            m_nextPhase = Phase::End;
            m_isInputPhase = false;
            return "Dragon0010A";
        }

        m_nextPhase = Phase::Retreat;
        m_isInputPhase = true;

        if (m_dragonHitCount == 0) {
            return "Dragon0005";
        } else if (m_dragonHitCount == 1) {
            return "Dragon0007";
        } else {
            return "Dragon0009";
        }

    case Phase::Freeze:
        if (castSpell == Spell::IceArrow) {
            m_dragonHitCount++;
            SetMana(1, std::max(int(GetMana(1)) - 6, 0));
            if (GetMana(1) == 0) {
                // Win
                m_nextPhase = Phase::End;
                m_isInputPhase = false;
                return "Dragon0020";
            }
            m_nextPhase = Phase::Attack;
            m_isInputPhase = true;

            if (m_dragonHitCount == 1) {
                return "Dragon0012";
            } else {
                return "Dragon0015";
            }
        } else {
            // Fail
            m_nextPhase = Phase::AttackFail;
            m_isInputPhase = false;
            if (m_dragonHitCount == 0) {
                return "Dragon0013";
            } else if (m_dragonHitCount == 1) {
                return "Dragon0016";
            } else {
                return "Dragon0019";
            }
        }

    case Phase::AttackFail:
        damage = GetDamageTable(false, false)[m_magiHitCount];
        if (m_magiHitCount < 9) {
            m_magiHitCount++;
        }
        SetMana(0, std::max(int(GetMana(0)) - damage, 0));

        if (GetMana(0) == 0) {
            // Lose
            m_nextPhase = Phase::End;
            m_isInputPhase = false;
            return "Dragon0010A";
        }

        m_nextPhase = Phase::FailRetreat;
        m_isInputPhase = true;

        if (m_dragonHitCount == 0) {
            return "Dragon0005";
        } else if (m_dragonHitCount == 1) {
            return "Dragon0007";
        } else {
            return "Dragon0009";
        }

    case Phase::FailRetreat:
        if (castSpell == Spell::Freeze) {
            m_nextPhase = Phase::Freeze;
            m_isInputPhase = true;

            if (m_dragonHitCount == 0) {
                return "Dragon0011";
            } else if (m_dragonHitCount == 1) {
                return "Dragon0014";
            } else {
                return "Dragon0017";
            }
        } else {
            m_nextPhase = Phase::FreezeFail;
            m_isInputPhase = false;

            if (m_dragonHitCount == 0) {
                return "Dragon0006";
            } else if (m_dragonHitCount == 1) {
                return "Dragon0008";
            } else {
                return "Dragon0010";
            }
        }

    default:
        PANIC("Unknown phase");
    }
}

const char* Page_DuelDragon::NextMovie()
{
    Spell castSpell = m_castSpell;
    m_castSpell = Spell::None;

    m_currentPhase = m_nextPhase;

    auto name = NextPhase(castSpell);

    if (name == nullptr) {
        // Default to the Dragon idle screen
        strcpy(m_phaseMoviePath, RES_ROOT "/Movie/Dragon/DragonIdle.mp4");
    } else {
        snprintf(m_phaseMoviePath, sizeof(m_phaseMoviePath), RES_ROOT "/Movie/Dragon/%s.mp4", name);
    }

    // Disable all buttons
    for (u32 i = 0; i < SpellCount; i++) {
        m_buttons[i].Deselect();
        m_buttons[i].SetSelectable(m_isInputPhase);
    }

    return m_phaseMoviePath;
}

void Page_DuelDragon::Cast(
  Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ)
{
    LOG(LogSystem, "Cast");

    // Ignore if this page is not visible.
    if (!System::s_instance->GetSetting(System::GetPageID(this))->drc) {
        return;
    }

    if (castMode == Wand::CastMode::WiiRemoteCastRune && curValid && m_isInputPhase &&
        (curX < 640 && curX > -640 && curY < 450 && curY > -450)) {
        for (u32 i = 0; i < SpellCount; i++) {
            if (!m_buttons[i].IsSelectable())
                continue;

            auto x = m_buttons[i].getCenterX();
            auto y = m_buttons[i].getCenterY();
            if (curX > x - 240 && curX < x + 240 && curY > y - 240 && curY < y + 240) {
                DeselectAll();
                m_buttons[i].Select();
            }
        }
        return;
    }

    if (m_currentPhase == Phase::Idle || m_currentPhase == Phase::End) {
        SetMana(0, 16);
        SetMana(1, 16);
        m_nextPhase = Phase::Start;
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
        break;
    }
}

void Page_DuelDragon::DeselectAll()
{
    for (u32 i = 0; i < SpellCount; i++) {
        m_buttons[i].Deselect();
    }
}
