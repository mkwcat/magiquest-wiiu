// Page_Encounter_Xavier.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_Encounter_Xavier.hpp"
#include "Ctrl_Mana.hpp"
#include "Page_Background.hpp"
#include <cstdlib>

enum {
    IMG_NOTSELECTED = 0,
    IMG_SELECTED = 1,
    IMG_CASTED = 2,
    IMG_HALF_NOTSELECTED = 3,
    IMG_HALF_SELECTED = 4,
    IMG_HALF_CASTED = 5,
    IMG_NOTSELECTABLE = 6,
};

void Page_Encounter_Xavier::InitSpell(Spell spell, const char** images, int posX, int posY)
{
    u32 btn = u32(spell);
    assert(btn < SpellCount);

    m_buttonUses[btn] = 0;

    m_buttons[btn].Init("SpellButton", RES_ROOT "/Image/Encounter/Spell/Xavier", images, 7);
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

    m_buttons[btn].SetHoverable(false);

    append(&m_buttons[btn]);
}

Page_Encounter_Xavier::Page_Encounter_Xavier()
{
    static const char* KreigerImages[] = {
      [IMG_NOTSELECTED] = "Kreiger",
      [IMG_SELECTED] = "KreigerSelected",
      [IMG_CASTED] = "KreigerCasted",
      [IMG_HALF_NOTSELECTED] = "KreigerHalf",
      [IMG_HALF_SELECTED] = "KreigerHalfSelected",
      [IMG_HALF_CASTED] = "KreigerHalfCasted",
      [IMG_NOTSELECTABLE] = "KreigerDisabled",
    };

    InitSpell(Spell::Warrior, KreigerImages, 0, 0);

    static const char* KoniImages[] = {
      [IMG_NOTSELECTED] = "Koni",
      [IMG_SELECTED] = "KoniSelected",
      [IMG_CASTED] = "KoniCasted",
      [IMG_HALF_NOTSELECTED] = "KoniHalf",
      [IMG_HALF_SELECTED] = "KoniHalfSelected",
      [IMG_HALF_CASTED] = "KoniHalfCasted",
      [IMG_NOTSELECTABLE] = "KoniDisabled",
    };

    InitSpell(Spell::Majestic, KoniImages, -500, 0);

    static const char* WaldImages[] = {
      [IMG_NOTSELECTED] = "Wald",
      [IMG_SELECTED] = "WaldSelected",
      [IMG_CASTED] = "WaldCasted",
      [IMG_HALF_NOTSELECTED] = "WaldHalf",
      [IMG_HALF_SELECTED] = "WaldHalfSelected",
      [IMG_HALF_CASTED] = "WaldHalfCasted",
      [IMG_NOTSELECTABLE] = "WaldDisabled",
    };

    InitSpell(Spell::Woodsy, WaldImages, 250, -350);

    static const char* WitzImages[] = {
      [IMG_NOTSELECTED] = "Witz",
      [IMG_SELECTED] = "WitzSelected",
      [IMG_CASTED] = "WitzCasted",
      [IMG_HALF_NOTSELECTED] = "WitzHalf",
      [IMG_HALF_SELECTED] = "WitzHalfSelected",
      [IMG_HALF_CASTED] = "WitzHalfCasted",
      [IMG_NOTSELECTABLE] = "WitzDisabled",
    };

    InitSpell(Spell::Trixter, WitzImages, 500, 0);

    static const char* SchattenImages[] = {
      [IMG_NOTSELECTED] = "Schatten",
      [IMG_SELECTED] = "SchattenSelected",
      [IMG_CASTED] = "SchattenCasted",
      [IMG_HALF_NOTSELECTED] = "SchattenHalf",
      [IMG_HALF_SELECTED] = "SchattenHalfSelected",
      [IMG_HALF_CASTED] = "SchattenHalfCasted",
      [IMG_NOTSELECTABLE] = "SchattenDisabled",
    };

    InitSpell(Spell::Shadow, SchattenImages, -250, 350);
}

void Page_Encounter_Xavier::TransitionFirst()
{
    m_currentPhase = Phase::Idle;
    m_nextPhase = Phase::MagiWin;
    m_isInputPhase = false;
    ForceNextMovie();
}

const char* Page_Encounter_Xavier::NextPhase(Spell castSpell)
{
    switch (m_currentPhase) {
    case Phase::MagiWin:
    case Phase::MagiLose:
        m_nextPhase = Phase::Idle;

        SetMana(Ctrl_Mana::Left, 0);
        SetMana(Ctrl_Mana::Right, 0);
        // Fall through

    case Phase::Idle:
        m_isInputPhase = false;
        return "XavierIdle";

    case Phase::Start:
        m_nextPhase = Phase::Intro;
        m_isInputPhase = false;
        SetMana(Ctrl_Mana::Left, 0);
        SetMana(Ctrl_Mana::Right, 0);
        return "XavierDuelIntro";

    case Phase::Restart:
        m_nextPhase = Phase::Intro;
        m_isInputPhase = false;
        SetMana(Ctrl_Mana::Left, 0);
        SetMana(Ctrl_Mana::Right, 0);
        return "XavierDuelIntroRepeatTry";

    case Phase::Intro:
        m_started = true;
        m_attackPhase = AttackPhase::Default;

        for (u32 i = 0; i < SpellCount; i++) {
            m_buttonUses[i] = 0;
        }

        for (u32 i = 0; i < AttackPhaseCount; i++) {
            m_doneAttackPhase[i] = false;
        }

        SetMana(Ctrl_Mana::Left, 16);
        SetMana(Ctrl_Mana::Right, 16);
        // Fall through

    case Phase::MagiAttack:
        if (GetMana(1) == 0) {
            m_nextPhase = Phase::XavierNoMana;
            m_isInputPhase = true;
            return "XavierDuelXavierNoMana";
        }

        m_nextPhase = Phase::Attack;
        m_isInputPhase = true;

        // Random sort of attack phases
        {
            u32 numPhaseLeft = 0;
            for (u32 i = 0; i < AttackPhaseCount; i++) {
                numPhaseLeft += m_doneAttackPhase[i] ? 0 : 1;
            }

            u32 random;

            // Warrior has no retreat phase after it, so if it happens to be the
            // last attack phase then it won't even show up due to Xavier
            // already being out of mana. This check ensures that never happens.
            if (numPhaseLeft <= 2 && !m_doneAttackPhase[u32(AttackPhase::Warrior)]) {
                random = u32(AttackPhase::Warrior);
            } else {
                random = Random(numPhaseLeft);
                for (u32 i = 0; i < AttackPhaseCount; i++) {
                    random += m_doneAttackPhase[i] ? 1 : 0;
                    if (random == i)
                        break;
                }
            }

            m_attackPhase = AttackPhase(random);
            m_doneAttackPhase[random] = true;
        }

        switch (m_attackPhase) {
        case AttackPhase::Majestic:
            return "XavierDuelMajesticAttack";

        case AttackPhase::Shadow:
            return "XavierDuelShadowAttack";

        case AttackPhase::Trixter:
            return "XavierDuelTrixterAttack";

        case AttackPhase::Warrior:
            return "XavierDuelWarriorAttack";

        case AttackPhase::Woodsy:
            return "XavierDuelWoodsyAttack";

        default:
            assert(!"Invalid attack phase");
        }

    case Phase::Attack:
        m_nextPhase = Phase::AttackEnd;
        m_isInputPhase = false;

        switch (m_attackPhase) {
        case AttackPhase::Majestic:
            if (castSpell != Spell::Trixter) {
                SetMana(Ctrl_Mana::Left, 0);
                return "XavierDuelMajesticAttackMagiLose";
            }

            return "XavierDuelMajesticAttackMagiWin";

        case AttackPhase::Shadow:
            if (castSpell != Spell::Shadow) {
                SetMana(Ctrl_Mana::Left, 0);
                return "XavierDuelShadowAttackMagiLose";
            }

            return "XavierDuelShadowAttackMagiWin";

        case AttackPhase::Trixter:
            if (castSpell != Spell::Majestic) {
                SetMana(Ctrl_Mana::Left, 0);
                return "XavierDuelTrixterAttackMagiLose";
            }

            return "XavierDuelTrixterAttackMagiWin";

        case AttackPhase::Warrior:
            if (castSpell != Spell::Warrior) {
                SetMana(Ctrl_Mana::Left, 0);
                return "XavierDuelWarriorAttackMagiLose";
            }

            // Skip the retreat phase
            m_nextPhase = Phase::MagiAttack;

            return "XavierDuelWarriorAttackMagiWin";

        case AttackPhase::Woodsy:
            if (castSpell != Spell::Woodsy) {
                SetMana(Ctrl_Mana::Left, 0);
                return "XavierDuelWoodsyAttackMagiLose";
            }

            return "XavierDuelWoodsyAttackMagiWin";

        default:
            assert(!"Invalid attack phase");
        }

    case Phase::AttackEnd:
        if (GetMana(0) == 0) {
            m_nextPhase = Phase::MagiLose;
            m_isInputPhase = false;
            return "XavierDuelMagiNoManaLose";
        }

        m_nextPhase = Phase::Retreat;
        m_isInputPhase = true;
        return "XavierDuelRetreat";

    case Phase::Retreat:
        m_nextPhase = Phase::Stunned;
        m_isInputPhase = true;

        switch (castSpell) {
        case Spell::Majestic:
            return "XavierDuelMagiKoni";

        case Spell::Woodsy:
            return "XavierDuelMagiWald";

        case Spell::Trixter:
            return "XavierDuelMagiWitz";

        case Spell::Shadow:
            return "XavierDuelMagiSchatten";

        default:
            m_nextPhase = Phase::MagiLose;
            m_isInputPhase = false;
            SetMana(Ctrl_Mana::Left, 0);
            return "XavierDuelXavierInstantKill";
        }

    case Phase::Stunned:
        if (castSpell != Spell::Warrior) {
            m_nextPhase = Phase::Retreat;
            m_isInputPhase = true;
            return "XavierDuelRetreat";
        }

        m_nextPhase = Phase::MagiAttack;
        m_isInputPhase = false;
        SetMana(Ctrl_Mana::Right, GetMana(1) - 4);
        return "XavierDuelMagiKreiger";

    case Phase::XavierNoMana:
        if (castSpell != Spell::Warrior) {
            m_nextPhase = Phase::MagiLose;
            m_isInputPhase = true;
            SetMana(Ctrl_Mana::Left, 0);
            return "XavierDuelMagiLose";
        }

        m_nextPhase = Phase::MagiWin;
        m_isInputPhase = false;
        return "XavierDuelMagiWinWithRune";

    default:
        return nullptr;
    }
}

const char* Page_Encounter_Xavier::NextMovie()
{
    Spell castSpell = m_castSpell;
    m_castSpell = Spell::None;

    m_currentPhase = m_nextPhase;

    if (m_isInputPhase && castSpell != Spell::None && castSpell != Spell::Warrior) {
        m_buttonUses[int(castSpell)]++;
    }

    auto name = NextPhase(castSpell);
    assert(!!name);
    snprintf(m_phaseMoviePath, sizeof(m_phaseMoviePath), RES_ROOT "/Movie/Xavier/%s.mp4", name);

    // Disable all buttons
    for (u32 i = 0; i < SpellCount; i++) {
        switch (m_buttonUses[i]) {
        case 0:
            m_buttons[i].SetImages(IMG_SELECTED, IMG_NOTSELECTED, IMG_NOTSELECTED);
            break;
        case 1:
            m_buttons[i].SetImages(IMG_HALF_SELECTED, IMG_HALF_NOTSELECTED, IMG_HALF_NOTSELECTED);
            break;
        case 2:
        default:
            m_buttons[i].SetImages(IMG_SELECTED, IMG_NOTSELECTABLE, IMG_NOTSELECTABLE);
            break;
        }

        m_buttons[i].Unhover();
        m_buttons[i].SetHoverable(m_isInputPhase && m_buttonUses[i] < 2);
    }

    return m_phaseMoviePath;
}

void Page_Encounter_Xavier::NextFrame(u32 frame)
{
    if (m_nextPhase != Phase::Retreat)
        return;

    if (m_castSpell != Spell::None && m_castSpell != Spell::Warrior) {
        // Hardcoded ending frames for when Xavier is not on screen
        for (u32 f : {96, 225, 339}) {
            if (frame == f) {
                ForceNextMovie();
            }
        }
    }
}

void Page_Encounter_Xavier::Cast(
  Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ)
{
    LOG(LogSystem, "Cast");

    if (m_currentPhase == Phase::Idle || m_currentPhase == Phase::MagiWin) {
        m_nextPhase = Phase::Start;
        ForceNextMovie();
        return;
    }

    if (m_currentPhase == Phase::MagiLose) {
        m_nextPhase = Phase::Restart;
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

    for (u32 i = 0; i < SpellCount; i++) {
        if (!m_buttons[i].IsHoverable()) {
            continue;
        }

        DeselectAll();
        for (u32 j = 0; j < SpellCount; j++) {
            switch (m_buttonUses[j]) {
            case 0:
                m_buttons[j].SetImages(IMG_SELECTED, IMG_NOTSELECTED, IMG_NOTSELECTED);
                break;
            case 1:
                m_buttons[j].SetImages(
                  IMG_HALF_SELECTED, IMG_HALF_NOTSELECTED, IMG_HALF_NOTSELECTED);
                break;
            case 2:
            default:
                m_buttons[j].SetImages(IMG_SELECTED, IMG_NOTSELECTABLE, IMG_NOTSELECTABLE);
                break;
            }
        }

        switch (m_buttonUses[i]) {
        case 0:
            m_buttons[i].SetImages(IMG_SELECTED, IMG_CASTED, IMG_NOTSELECTED);
            break;
        case 1:
            m_buttons[i].SetImages(IMG_HALF_SELECTED, IMG_HALF_CASTED, IMG_HALF_NOTSELECTED);
            break;
        }
        m_castSpell = Spell(i);
    }
}

void Page_Encounter_Xavier::DeselectAll()
{
    for (u32 i = 0; i < SpellCount; i++) {
        m_buttons[i].Unhover();
    }
}
