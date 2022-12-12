// Page_TouchDuel.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_TouchDuel.hpp"
#include "Page_Movie.hpp"
#include <cstdlib>

Page_TouchDuel::Page_TouchDuel()
  : m_imgManaLeft(nullptr)
  , m_imgManaRight(nullptr)
{
    m_currentPhase = Phase::Idle;
    m_nextPhase = Phase::Idle;
    m_isInputPhase = false;
    m_started = false;
}

Page_TouchDuel::~Page_TouchDuel()
{
}

enum {
    IMG_NOTSELECTED = 0,
    IMG_SELECTED = 1,
    IMG_CASTED = 2,
    IMG_HALF_NOTSELECTED = 3,
    IMG_HALF_SELECTED = 4,
    IMG_HALF_CASTED = 5,
    IMG_NOTSELECTABLE = 6,
};

void Page_TouchDuel::InitSpell(
  Spell spell, const char** images, int posX, int posY)
{
    u32 btn = u32(spell);
    assert(btn < SpellCount);

    m_buttonUses[btn] = 0;

    m_buttons[btn].Init("SpellButton", images, 7);
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

void Page_TouchDuel::Init()
{
    auto page = System::GetPageStatic<Page_Movie>();
    assert(page != nullptr);

    page->SetEncounter(this);

    m_imgManaLeft.setImageData(page->GetManaImage(0, 0));
    m_imgManaRight.setImageData(page->GetManaImage(1, 0));

    append(&m_imgManaLeft);
    append(&m_imgManaRight);

    m_imgManaLeft.setPosition(-(1920 / 2), 0);
    m_imgManaLeft.setAlignment(ALIGN_LEFT);
    m_imgManaLeft.setScaleX(1080 / m_imgManaLeft.getHeight());
    m_imgManaLeft.setScaleY(1080 / m_imgManaLeft.getHeight());

    m_imgManaRight.setPosition((1920 / 2), 0);
    m_imgManaRight.setAlignment(ALIGN_RIGHT);
    m_imgManaRight.setScaleX(1080 / m_imgManaRight.getHeight());
    m_imgManaRight.setScaleY(1080 / m_imgManaRight.getHeight());

    static const char* KreigerImages[] = {
      [IMG_NOTSELECTED] = RES_ROOT "/Image/kreiger1.png",
      [IMG_SELECTED] = RES_ROOT "/Image/kreiger2.png",
      [IMG_CASTED] = RES_ROOT "/Image/kreiger3.png",
      [IMG_HALF_NOTSELECTED] = RES_ROOT "/Image/kreiger4.png",
      [IMG_HALF_SELECTED] = RES_ROOT "/Image/kreiger5.png",
      [IMG_HALF_CASTED] = RES_ROOT "/Image/kreiger6.png",
      [IMG_NOTSELECTABLE] = RES_ROOT "/Image/kreiger7.png",
    };

    InitSpell(Spell::Warrior, KreigerImages, 0, 0);

    static const char* KoniImages[] = {
      [IMG_NOTSELECTED] = RES_ROOT "/Image/koni1.png",
      [IMG_SELECTED] = RES_ROOT "/Image/koni2.png",
      [IMG_CASTED] = RES_ROOT "/Image/koni3.png",
      [IMG_HALF_NOTSELECTED] = RES_ROOT "/Image/koni4.png",
      [IMG_HALF_SELECTED] = RES_ROOT "/Image/koni5.png",
      [IMG_HALF_CASTED] = RES_ROOT "/Image/koni6.png",
      [IMG_NOTSELECTABLE] = RES_ROOT "/Image/koni7.png",
    };

    InitSpell(Spell::Majestic, KoniImages, -500, 0);

    static const char* WaldImages[] = {
      [IMG_NOTSELECTED] = RES_ROOT "/Image/wald1.png",
      [IMG_SELECTED] = RES_ROOT "/Image/wald2.png",
      [IMG_CASTED] = RES_ROOT "/Image/wald3.png",
      [IMG_HALF_NOTSELECTED] = RES_ROOT "/Image/wald4.png",
      [IMG_HALF_SELECTED] = RES_ROOT "/Image/wald5.png",
      [IMG_HALF_CASTED] = RES_ROOT "/Image/wald6.png",
      [IMG_NOTSELECTABLE] = RES_ROOT "/Image/wald7.png",
    };

    InitSpell(Spell::Woodsy, WaldImages, 250, -350);

    static const char* WitzImages[] = {
      [IMG_NOTSELECTED] = RES_ROOT "/Image/witz1.png",
      [IMG_SELECTED] = RES_ROOT "/Image/witz2.png",
      [IMG_CASTED] = RES_ROOT "/Image/witz3.png",
      [IMG_HALF_NOTSELECTED] = RES_ROOT "/Image/witz4.png",
      [IMG_HALF_SELECTED] = RES_ROOT "/Image/witz5.png",
      [IMG_HALF_CASTED] = RES_ROOT "/Image/witz6.png",
      [IMG_NOTSELECTABLE] = RES_ROOT "/Image/witz7.png",
    };

    InitSpell(Spell::Trixter, WitzImages, 500, 0);

    static const char* SchattenImages[] = {
      [IMG_NOTSELECTED] = RES_ROOT "/Image/schatten1.png",
      [IMG_SELECTED] = RES_ROOT "/Image/schatten2.png",
      [IMG_CASTED] = RES_ROOT "/Image/schatten3.png",
      [IMG_HALF_NOTSELECTED] = RES_ROOT "/Image/schatten4.png",
      [IMG_HALF_SELECTED] = RES_ROOT "/Image/schatten5.png",
      [IMG_HALF_CASTED] = RES_ROOT "/Image/schatten6.png",
      [IMG_NOTSELECTABLE] = RES_ROOT "/Image/schatten7.png",
    };

    InitSpell(Spell::Shadow, SchattenImages, -250, 350);
}

void Page_TouchDuel::process()
{
    if (!m_initialized) {
        Init();
        m_initialized = true;
    }

    auto page = System::GetPageStatic<Page_Movie>();
    assert(page != nullptr);

    m_imgManaLeft.setImageData(page->GetManaImage(0, GetMana(0)));
    m_imgManaRight.setImageData(page->GetManaImage(1, GetMana(1)));

    GuiFrame::process();
}

const char* Page_TouchDuel::NextPhase(Spell castSpell)
{
    switch (m_currentPhase) {
    case Phase::MagiWin:
    case Phase::MagiLose:
        m_nextPhase = Phase::Idle;

        SetMana(0, 0);
        SetMana(1, 0);
        // Fall through

    case Phase::Idle:
        m_isInputPhase = false;
        return "XavierIdle";

    case Phase::Start:
        m_nextPhase = Phase::Intro;
        m_isInputPhase = false;
        SetMana(0, 0);
        SetMana(1, 0);
        return "XavierDuelIntro";

    case Phase::Restart:
        m_nextPhase = Phase::Intro;
        m_isInputPhase = false;
        SetMana(0, 0);
        SetMana(1, 0);
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

        SetMana(0, 16);
        SetMana(1, 16);
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
            if (numPhaseLeft <= 2 &&
                !m_doneAttackPhase[u32(AttackPhase::Warrior)]) {
                random = u32(AttackPhase::Warrior);
            } else {
                random = rand() % numPhaseLeft;
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
                SetMana(0, 0);
                return "XavierDuelMajesticAttackMagiLose";
            }

            return "XavierDuelMajesticAttackMagiWin";

        case AttackPhase::Shadow:
            if (castSpell != Spell::Shadow) {
                SetMana(0, 0);
                return "XavierDuelShadowAttackMagiLose";
            }

            return "XavierDuelShadowAttackMagiWin";

        case AttackPhase::Trixter:
            if (castSpell != Spell::Majestic) {
                SetMana(0, 0);
                return "XavierDuelTrixterAttackMagiLose";
            }

            return "XavierDuelTrixterAttackMagiWin";

        case AttackPhase::Warrior:
            if (castSpell != Spell::Warrior) {
                SetMana(0, 0);
                return "XavierDuelWarriorAttackMagiLose";
            }

            // Skip the retreat phase
            m_nextPhase = Phase::MagiAttack;

            return "XavierDuelWarriorAttackMagiWin";

        case AttackPhase::Woodsy:
            if (castSpell != Spell::Woodsy) {
                SetMana(0, 0);
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
            SetMana(0, 0);
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
        SetMana(1, GetMana(1) - 4);
        return "XavierDuelMagiKreiger";

    case Phase::XavierNoMana:
        if (castSpell != Spell::Warrior) {
            m_nextPhase = Phase::MagiLose;
            m_isInputPhase = true;
            SetMana(0, 0);
            return "XavierDuelMagiLose";
        }

        m_nextPhase = Phase::MagiWin;
        m_isInputPhase = false;
        return "XavierDuelMagiWinWithRune";

    default:
        return nullptr;
    }
}

const char* Page_TouchDuel::NextMovie()
{
    Spell castSpell = m_castSpell;
    m_castSpell = Spell::None;

    m_currentPhase = m_nextPhase;

    if (m_isInputPhase && castSpell != Spell::None &&
        castSpell != Spell::Warrior) {
        m_buttonUses[int(castSpell)]++;
    }

    auto name = NextPhase(castSpell);
    assert(!!name);
    snprintf(m_phaseMoviePath, sizeof(m_phaseMoviePath),
      RES_ROOT "/Movie/Xavier/%s.mp4", name);

    // Disable all buttons
    for (u32 i = 0; i < SpellCount; i++) {
        switch (m_buttonUses[i]) {
        case 0:
            m_buttons[i].SetImages(
              IMG_SELECTED, IMG_NOTSELECTED, IMG_NOTSELECTED);
            break;
        case 1:
            m_buttons[i].SetImages(
              IMG_HALF_SELECTED, IMG_HALF_NOTSELECTED, IMG_HALF_NOTSELECTED);
            break;
        case 2:
        default:
            m_buttons[i].SetImages(
              IMG_SELECTED, IMG_NOTSELECTABLE, IMG_NOTSELECTABLE);
            break;
        }

        m_buttons[i].Deselect();
        m_buttons[i].SetSelectable(m_isInputPhase && m_buttonUses[i] < 2);
    }

    return m_phaseMoviePath;
}

void Page_TouchDuel::NextFrame(u32 frame)
{
    if (m_nextPhase != Phase::Retreat)
        return;

    if (m_castSpell != Spell::None && m_castSpell != Spell::Warrior) {
        ForceNextMovie();
    }
}

void Page_TouchDuel::Cast(
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

    for (u32 i = 0; i < SpellCount; i++) {
        if (!m_buttons[i].IsSelected())
            continue;

        DeselectAll();
        for (u32 j = 0; j < SpellCount; j++) {
            switch (m_buttonUses[j]) {
            case 0:
                m_buttons[j].SetImages(
                  IMG_SELECTED, IMG_NOTSELECTED, IMG_NOTSELECTED);
                break;
            case 1:
                m_buttons[j].SetImages(IMG_HALF_SELECTED, IMG_HALF_NOTSELECTED,
                  IMG_HALF_NOTSELECTED);
                break;
            case 2:
            default:
                m_buttons[j].SetImages(
                  IMG_SELECTED, IMG_NOTSELECTABLE, IMG_NOTSELECTABLE);
                break;
            }
        }

        switch (m_buttonUses[i]) {
        case 0:
            m_buttons[i].SetImages(IMG_SELECTED, IMG_CASTED, IMG_NOTSELECTED);
            break;
        case 1:
            m_buttons[i].SetImages(
              IMG_HALF_SELECTED, IMG_HALF_CASTED, IMG_HALF_NOTSELECTED);
            break;
        }
        m_castSpell = Spell(i);
    }
}

void Page_TouchDuel::DeselectAll()
{
    for (u32 i = 0; i < SpellCount; i++) {
        m_buttons[i].Deselect();
    }
}
