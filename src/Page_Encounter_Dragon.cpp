// Page_Encounter_Dragon.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_Encounter_Dragon.hpp"
#include "Config.hpp"
#include <cstdlib>

enum {
    IMG_NOTSELECTED = 0,
    IMG_SELECTED = 1,
    IMG_DISABLED = 2,
};

void Page_Encounter_Dragon::InitSpell(Spell spell, const char* const* images, int posX, int posY)
{
    u32 btn = u32(spell);
    assert(btn < SpellCount);

    m_buttons[btn].Init("SpellButton", RES_ROOT "/Image/Encounter/Spell/Common", images, 3);
    m_buttons[btn].SetImages(IMG_SELECTED, IMG_NOTSELECTED, IMG_DISABLED);

    m_buttons[btn].SetOnSelectHandler([&](Ctrl_Spell* spell) {
        DeselectAll();
        spell->Select();
    });
    m_buttons[btn].SetOnReleaseHandler([&](Ctrl_Spell* spell) {
        // Do nothing
    });

    m_buttons[btn].setPosition(posX, posY);
    float height = m_buttons[btn].GetImage(0).getHeight();
    m_buttons[btn].setScaleX(280 / height);
    m_buttons[btn].setScaleY(280 / height);

    m_buttons[btn].SetSelectable(false);

    append(&m_buttons[btn]);
}

void Page_Encounter_Dragon::Init()
{
    using StringArray = const char* const[];

    InitSpell(Spell::Freeze,
      StringArray{
        [IMG_NOTSELECTED] = "Freeze",
        [IMG_SELECTED] = "FreezeHover",
        [IMG_DISABLED] = "FreezeDull",
      },
      -480, 250);

    InitSpell(Spell::Protection,
      StringArray{
        [IMG_NOTSELECTED] = "Protection",
        [IMG_SELECTED] = "ProtectionHover",
        [IMG_DISABLED] = "ProtectionDull",
      },
      0, 0);

    InitSpell(Spell::IceArrow,
      StringArray{
        [IMG_NOTSELECTED] = "IceArrow",
        [IMG_SELECTED] = "IceArrowHover",
        [IMG_DISABLED] = "IceArrowDull",
      },
      480, 250);

    InitSpell(Spell::Reveal,
      StringArray{
        [IMG_NOTSELECTED] = "Reveal",
        [IMG_SELECTED] = "RevealHover",
        [IMG_DISABLED] = "RevealDull",
      },
      480, -250);

    m_imgMq.Init(
      "MQSpell", RES_ROOT "/Image/Encounter/Spell/Common", StringArray{"MQ", "MQDull"}, 2);
    m_imgMq.SetImages(0, 1, 1);
    m_imgMq.setPosition(-480, -250);
    float height = m_imgMq.GetImage(0).getHeight();
    m_imgMq.setScaleX(250 / height);
    m_imgMq.setScaleY(250 / height);
    m_imgMq.SetSelectable(false);
    m_imgMq.Select();
    append(&m_imgMq);
}

void Page_Encounter_Dragon::process()
{
    if (!m_initialized) {
        Init();
        m_initialized = true;
    }

    GuiFrame::process();
}

void Page_Encounter_Dragon::Transition()
{
    m_nextPhase = Phase::End;
    ForceNextMovie();
}

const char* Page_Encounter_Dragon::NextPhase()
{
    int damage;

    switch (m_currentPhase) {
    case Phase::End:
        m_nextPhase = Phase::Idle;
        SetMana(0, 0);
        SetMana(1, 0);
        m_imgMq.Select();
        for (u32 i = 0; i < SpellCount; i++) {
            m_buttons[i].SetSelectable(false);
        }
        // Fall through

    case Phase::Idle:
        return "Dragon0001";

    case Phase::Start:
        m_nextPhase = Phase::Fireball;
        m_magiHitCount = 0;
        m_dragonHitCount = 0;
        return "Dragon0002";

    case Phase::Fireball:
    case Phase::ArrowFireball:
        damage =
          GetDamageTable(m_castProtection, Config::UseDragonWand)[std::min(m_magiHitCount++, 9u)];
        SetMana(0, std::max(int(GetMana(0)) - damage, 0));

        if (GetMana(0) == 0) {
            // Lose
            m_nextPhase = Phase::End;
            m_buttons[u32(Spell::Protection)].SetSelectable(false);
            m_buttons[u32(Spell::Freeze)].SetSelectable(false);
            m_buttons[u32(Spell::IceArrow)].SetSelectable(false);
            return "Dragon0010A";
        }

        // Reset every spell
        m_buttons[u32(Spell::Protection)].SetSelectable(true);
        m_buttons[u32(Spell::Freeze)].SetSelectable(true);
        m_buttons[u32(Spell::IceArrow)].SetSelectable(true);
        m_allowProtection = true;
        m_allowFreeze = false;
        m_allowIceArrow = false;
        m_castProtection = false;
        m_castFreeze = false;
        m_castIceArrow = false;
        DeselectAll();

        m_nextPhase = Phase::TailSweep;

        if (m_dragonHitCount == 0) {
            return "Dragon0003";
        } else if (m_dragonHitCount == 1) {
            return "Dragon0012A";
        } else {
            return "Dragon0015A";
        }

    case Phase::TailSweep:
        if (m_castFreeze) {
            m_nextPhase = Phase::Freeze;
            m_castFreeze = false;
            m_allowFreeze = false;
            m_allowIceArrow = true;

            if (m_dragonHitCount == 0) {
                return "Dragon0011";
            } else if (m_dragonHitCount == 1) {
                return "Dragon0014";
            } else {
                return "Dragon0017";
            }
        } else {
            // Fail
            m_nextPhase = Phase::Fireball;
            m_allowFreeze = false;
            m_allowIceArrow = false;
            m_allowProtection = false;
            m_buttons[u32(Spell::Protection)].SetSelectable(false);
            m_buttons[u32(Spell::Freeze)].SetSelectable(false);
            m_buttons[u32(Spell::IceArrow)].SetSelectable(false);
            DeselectAll();

            if (m_dragonHitCount == 0) {
                return "Dragon0004";
            } else if (m_dragonHitCount == 1) {
                return "Dragon0013";
            } else {
                return "Dragon0016";
            }
        }

    case Phase::Freeze:
        m_allowIceArrow = false;

        if (m_castIceArrow) {
            m_castIceArrow = false;
            m_dragonHitCount++;
            SetMana(1, std::max(int(GetMana(1)) - 6, 0));

            if (GetMana(1) == 0) {
                // Win
                m_nextPhase = Phase::End;
                m_buttons[u32(Spell::Protection)].SetSelectable(false);
                m_buttons[u32(Spell::Freeze)].SetSelectable(false);
                m_buttons[u32(Spell::IceArrow)].SetSelectable(false);
                DeselectAll();

                return "Dragon0020";
            }

            m_nextPhase = Phase::ArrowFireball;
            m_allowProtection = true;

            if (m_dragonHitCount == 1) {
                return "Dragon0012";
            } else {
                return "Dragon0015";
            }
        } else {
            // Fail
            m_nextPhase = Phase::Fireball;
            m_allowFreeze = false;
            m_allowIceArrow = false;
            m_allowProtection = false;
            m_buttons[u32(Spell::Protection)].SetSelectable(false);
            m_buttons[u32(Spell::Freeze)].SetSelectable(false);
            m_buttons[u32(Spell::IceArrow)].SetSelectable(false);
            DeselectAll();

            if (m_dragonHitCount == 0) {
                return "Dragon0004";
            } else if (m_dragonHitCount == 1) {
                return "Dragon0013";
            } else {
                return "Dragon0016";
            }
        }

    default:
        PANIC("Unknown phase");
    }
}

const char* Page_Encounter_Dragon::NextMovie()
{
    m_currentPhase = m_nextPhase;

    auto name = NextPhase();

    if (name == nullptr) {
        // Default to the Dragon idle screen
        strcpy(m_phaseMoviePath, RES_ROOT "/Movie/Dragon/Dragon0001.mp4");
    } else {
        snprintf(m_phaseMoviePath, sizeof(m_phaseMoviePath), RES_ROOT "/Movie/Dragon/%s.mp4", name);
    }

    return m_phaseMoviePath;
}

void Page_Encounter_Dragon::NextFrame(u32 frame)
{
    // The original encounter makes you wait to cast certain spells for some reason

    if (m_nextPhase == Phase::TailSweep) {
        // Tailsweep only allows casting freeze after 4:15 seconds
        if (frame == 4 * 30 + 15) {
            m_allowFreeze = true;
        }
    }

    if (m_nextPhase == Phase::ArrowFireball) {
        // Attack only allows casting protection in the first 8 seconds, but only for clips 12 and
        // 15 seemingly
        if (frame == 8 * 30 && m_dragonHitCount >= 1) {
            m_allowProtection = false;
            m_buttons[u32(Spell::Protection)].SetSelectable(false);
        }
    }
}

void Page_Encounter_Dragon::Cast(
  Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ)
{
    LOG(LogSystem, "Cast");

    if (m_nextPhase == Phase::Idle) {
        SetMana(0, 16);
        SetMana(1, 16);
        DeselectAll();
        m_imgMq.Deselect();
        m_nextPhase = Phase::Start;

        // Prepare input for the next phase immediately
        m_allowFreeze = false;
        m_allowIceArrow = false;
        m_allowProtection = true;
        m_allowReveal = false;
        m_buttons[u32(Spell::Protection)].SetSelectable(true);
        m_buttons[u32(Spell::Freeze)].SetSelectable(true);
        m_buttons[u32(Spell::IceArrow)].SetSelectable(true);
        m_buttons[u32(Spell::Reveal)].SetSelectable(GetEncounterType() != Encounter::Type::Dragon);
        m_castProtection = false;
        m_castFreeze = false;
        m_castIceArrow = false;
        m_castReveal = false;
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
    } else if (m_buttons[u32(Spell::Reveal)].IsSelected()) {
        if (!m_allowReveal) {
            return;
        }
        m_castReveal = true;
        m_buttons[u32(Spell::Reveal)].SetSelectable(false);
    } else {
        // No spell selected
        return;
    }

    DeselectAll();
}

void Page_Encounter_Dragon::DeselectAll()
{
    for (u32 i = 0; i < SpellCount; i++) {
        if (m_buttons[i].IsSelected()) {
            m_buttons[i].Deselect();
        }
    }
}
