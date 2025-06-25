// Page_DuelDragon.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_DuelDragon.hpp"
#include "Config.hpp"
#include "Page_Background.hpp"
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

    m_buttons[btn].SetSelectable(false);

    append(&m_buttons[btn]);
}

void Page_DuelDragon::Init()
{
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

    GuiFrame::process();
}

void Page_DuelDragon::Transition()
{
    m_nextPhase = Phase::End;
    ForceNextMovie();
}

const char* Page_DuelDragon::NextPhase()
{
    int damage;

    switch (m_currentPhase) {
    case Phase::End:
        m_nextPhase = Phase::Idle;
        SetMana(0, 0);
        SetMana(1, 0);
        // Fall through

    case Phase::Idle:
        return "Dragon0001";

    case Phase::Start:
        m_nextPhase = Phase::Attack;
        m_magiHitCount = 0;
        m_dragonHitCount = 0;
        DeselectAll();
        return "Dragon0002";

    case Phase::Attack:
        damage = GetDamageTable(m_castProtection, false)[std::min(m_magiHitCount++, 9)];
        SetMana(0, std::max(int(GetMana(0)) - damage, 0));
        m_castProtection = false;

        DeselectAll();

        if (GetMana(0) == 0) {
            // Lose
            m_nextPhase = Phase::End;
            return "Dragon0010A";
        }

        m_nextPhase = Phase::Tailsweep;

        if (m_dragonHitCount == 0) {
            return "Dragon0003";
        } else if (m_dragonHitCount == 1) {
            return "Dragon0012A";
        } else {
            return "Dragon0015A";
        }

    case Phase::Tailsweep:
        if (m_castFreeze) {
            m_nextPhase = Phase::Freeze;
            m_castFreeze = false;

            if (m_dragonHitCount == 0) {
                return "Dragon0011";
            } else if (m_dragonHitCount == 1) {
                return "Dragon0014";
            } else {
                return "Dragon0017";
            }
        } else {
            m_nextPhase = Phase::FreezeFail;

            if (!Config::EnableUnusedClips) {
                if (m_dragonHitCount == 0) {
                    return "Dragon0004";
                } else if (m_dragonHitCount == 1) {
                    return "Dragon0013";
                } else {
                    return "Dragon0016";
                }
            }

            if (m_dragonHitCount == 0) {
                return "Dragon0004";
            } else if (m_dragonHitCount == 1) {
                return "Dragon0006";
            } else {
                return "Dragon0008";
            }
        }

    case Phase::FreezeFail:
        damage = GetDamageTable(m_castProtection, false)[std::min(m_magiHitCount++, 9)];
        SetMana(0, std::max(int(GetMana(0)) - damage, 0));
        m_castProtection = false;

        DeselectAll();

        if (GetMana(0) == 0) {
            // Lose
            m_nextPhase = Phase::End;
            return "Dragon0010A";
        }

        m_nextPhase = Phase::Tailsweep;

        if (!Config::EnableUnusedClips) {
            if (m_dragonHitCount == 0) {
                return "Dragon0003";
            } else if (m_dragonHitCount == 1) {
                return "Dragon0012A";
            } else {
                return "Dragon0015A";
            }
        }

        if (m_dragonHitCount == 0) {
            return "Dragon0005";
        } else if (m_dragonHitCount == 1) {
            return "Dragon0007";
        } else {
            return "Dragon0009";
        }

    case Phase::Freeze:
        if (m_castIceArrow) {
            m_castIceArrow = false;
            m_dragonHitCount++;
            SetMana(1, std::max(int(GetMana(1)) - 6, 0));
            if (GetMana(1) == 0) {
                // Win
                m_nextPhase = Phase::End;
                return "Dragon0020";
            }

            m_nextPhase = Phase::Attack;

            if (m_dragonHitCount == 1) {
                return "Dragon0012";
            } else {
                return "Dragon0015";
            }
        }
        // Fail
        m_nextPhase = Phase::AttackFail;

        if (!Config::EnableUnusedClips) {
            if (m_dragonHitCount == 0) {
                return "Dragon0004";
            } else if (m_dragonHitCount == 1) {
                return "Dragon0013";
            } else {
                return "Dragon0016";
            }
        }

        if (m_dragonHitCount == 0) {
            return "Dragon0013";
        } else if (m_dragonHitCount == 1) {
            return "Dragon0016";
        } else {
            return "Dragon0019";
        }

    case Phase::AttackFail:
        damage = GetDamageTable(m_castProtection, false)[std::min(m_magiHitCount++, 9)];
        SetMana(0, std::max(int(GetMana(0)) - damage, 0));
        m_castProtection = false;

        DeselectAll();

        if (GetMana(0) == 0) {
            // Lose
            m_nextPhase = Phase::End;
            return "Dragon0010A";
        }

        m_nextPhase = Phase::FailTailsweep;

        if (!Config::EnableUnusedClips) {
            if (m_dragonHitCount == 0) {
                return "Dragon0003";
            } else if (m_dragonHitCount == 1) {
                return "Dragon0012A";
            } else {
                return "Dragon0015A";
            }
        }

        if (m_dragonHitCount == 0) {
            return "Dragon0005";
        } else if (m_dragonHitCount == 1) {
            return "Dragon0007";
        } else {
            return "Dragon0009";
        }

    case Phase::FailTailsweep:
        if (m_castFreeze) {
            m_castFreeze = false;
            m_nextPhase = Phase::Freeze;

            if (m_dragonHitCount == 0) {
                return "Dragon0011";
            } else if (m_dragonHitCount == 1) {
                return "Dragon0014";
            } else {
                return "Dragon0017";
            }
        } else {
            m_nextPhase = Phase::FreezeFail;

            if (!Config::EnableUnusedClips) {
                if (m_dragonHitCount == 0) {
                    return "Dragon0004";
                } else if (m_dragonHitCount == 1) {
                    return "Dragon0013";
                } else {
                    return "Dragon0016";
                }
            }

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

void Page_DuelDragon::NextInput()
{
    m_allowProtection = false;
    m_allowFreeze = false;
    m_allowIceArrow = false;
    for (u32 i = 0; i < SpellCount; i++) {
        m_buttons[i].SetSelectable(true);
    }

    switch (m_nextPhase) {
    default:
        break;

    case Phase::Attack:
        m_allowProtection = true;
        if (m_currentPhase != Phase::Start) {
            // These are explicitly not allowed to be hovered after the first attack phase
            m_buttons[u32(Spell::Freeze)].SetSelectable(false);
            m_buttons[u32(Spell::IceArrow)].SetSelectable(false);
        }
        break;

    case Phase::Tailsweep:
    case Phase::FailTailsweep:
        // Need to wait until 4:15 seconds into the tailsweep phase to cast freeze
        m_allowProtection = true;
        m_allowFreeze = false;
        break;

    case Phase::Freeze:
        m_buttons[u32(Spell::Protection)].SetSelectable(!m_castProtection);
        m_allowIceArrow = true;
        break;

    case Phase::AttackFail:
    case Phase::FreezeFail:
    case Phase::End:
        for (u32 i = 0; i < SpellCount; i++) {
            m_buttons[i].SetSelectable(false);
        }
    }
}

const char* Page_DuelDragon::NextMovie()
{
    m_currentPhase = m_nextPhase;

    auto name = NextPhase();
    NextInput();

    if (name == nullptr) {
        // Default to the Dragon idle screen
        strcpy(m_phaseMoviePath, RES_ROOT "/Movie/Dragon/DragonIdle.mp4");
    } else {
        snprintf(m_phaseMoviePath, sizeof(m_phaseMoviePath), RES_ROOT "/Movie/Dragon/%s.mp4", name);
    }

    return m_phaseMoviePath;
}

void Page_DuelDragon::NextFrame(u32 frame)
{
    // The original encounter makes you wait to cast certain spells for some reason

    if (m_nextPhase == Phase::Tailsweep || m_nextPhase == Phase::FailTailsweep) {
        // Tailsweep only allows casting freeze after 4:15 seconds
        if (frame == 4 * 30 + 15) {
            m_allowFreeze = true;
        }
    }

    if (m_nextPhase == Phase::Attack) {
        // Attack only allows casting protection in the first 8 seconds, but only for clips 12 and
        // 15 seemingly
        if (frame == 8 * 30 && m_dragonHitCount >= 1) {
            m_allowProtection = false;
            m_buttons[u32(Spell::Protection)].SetSelectable(false);
        }
    }
}

void Page_DuelDragon::Cast(
  Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ)
{
    LOG(LogSystem, "Cast");

    if (m_nextPhase == Phase::Idle) {
        SetMana(0, 16);
        SetMana(1, 16);
        m_nextPhase = Phase::Start;
        return;
    }

    if (castMode == Wand::CastMode::WiiRemoteCastRune && curValid &&
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
            }
        }
        return;
    }

    if (m_buttons[u32(Spell::Protection)].IsSelected()) {
        if (!m_allowProtection) {
            return;
        }
        m_castProtection = true;
        m_buttons[u32(Spell::Protection)].SetSelectable(false);
    } else if (m_buttons[u32(Spell::Freeze)].IsSelected()) {
        if (!m_allowFreeze) {
            return;
        }
        m_castFreeze = true;
        m_buttons[u32(Spell::Freeze)].SetSelectable(false);
    } else if (m_buttons[u32(Spell::IceArrow)].IsSelected()) {
        if (!m_allowIceArrow) {
            return;
        }
        m_castIceArrow = true;
        m_buttons[u32(Spell::IceArrow)].SetSelectable(false);
    } else {
        // No spell selected
        return;
    }

    DeselectAll();
}

void Page_DuelDragon::DeselectAll()
{
    for (u32 i = 0; i < SpellCount; i++) {
        m_buttons[i].Deselect();
    }
}
