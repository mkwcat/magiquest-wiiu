// Page_DuelXavier.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_DuelXavier.hpp"
#include "Page_Background.hpp"
#include "Page_Movie.hpp"
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

void Page_DuelXavier::InitSpell(
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

void Page_DuelXavier::Init()
{
    auto page = System::GetPageStatic<Page_Movie>();
    assert(page != nullptr);

    page->SetEncounter(this);

    m_imgManaLeft.Update(Ctrl_Mana::Left, 0);
    m_imgManaRight.Update(Ctrl_Mana::Right, 0);

    m_imgManaLeft.setPosition(-(1920 / 2) + 40, -(1080 / 2) + 35);
    m_imgManaLeft.setAlignment(ALIGN_LEFT | ALIGN_BOTTOM);
    m_imgManaLeft.setScaleX((1080 / m_imgManaLeft.getHeight()) * 0.85);
    m_imgManaLeft.setScaleY((1080 / m_imgManaLeft.getHeight()) * 0.85);

    m_imgManaRight.setPosition((1920 / 2) - 40, -(1080 / 2) + 35);
    m_imgManaRight.setAlignment(ALIGN_RIGHT | ALIGN_BOTTOM);
    m_imgManaRight.setScaleX((1080 / m_imgManaRight.getHeight()) * 0.85);
    m_imgManaRight.setScaleY((1080 / m_imgManaRight.getHeight()) * 0.85);

    append(&m_imgManaLeft);
    append(&m_imgManaRight);

    static const char* KreigerImages[] = {
      [IMG_NOTSELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/Kreiger.png",
      [IMG_SELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/KreigerSelected.png",
      [IMG_CASTED] = RES_ROOT //
      "/Image/Encounter/Spell/KreigerCasted.png",
      [IMG_HALF_NOTSELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/KreigerHalf.png",
      [IMG_HALF_SELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/KreigerHalfSelected.png",
      [IMG_HALF_CASTED] = RES_ROOT //
      "/Image/KreigerHalfCasted.png",
      [IMG_NOTSELECTABLE] = RES_ROOT //
      "/Image/KreigerDisabled.png",
    };

    InitSpell(Spell::Warrior, KreigerImages, 0, 0);

    static const char* KoniImages[] = {
      [IMG_NOTSELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/Koni.png",
      [IMG_SELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/KoniSelected.png",
      [IMG_CASTED] = RES_ROOT //
      "/Image/Encounter/Spell/KoniCasted.png",
      [IMG_HALF_NOTSELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/KoniHalf.png",
      [IMG_HALF_SELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/KoniHalfSelected.png",
      [IMG_HALF_CASTED] = RES_ROOT //
      "/Image/Encounter/Spell/KoniHalfCasted.png",
      [IMG_NOTSELECTABLE] = RES_ROOT //
      "/Image/Encounter/Spell/KoniDisabled.png",
    };

    InitSpell(Spell::Majestic, KoniImages, -500, 0);

    static const char* WaldImages[] = {
      [IMG_NOTSELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/Wald.png",
      [IMG_SELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/WaldSelected.png",
      [IMG_CASTED] = RES_ROOT //
      "/Image/Encounter/Spell/WaldCasted.png",
      [IMG_HALF_NOTSELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/WaldHalf.png",
      [IMG_HALF_SELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/WaldHalfSelected.png",
      [IMG_HALF_CASTED] = RES_ROOT //
      "/Image/Encounter/Spell/WaldHalfCasted.png",
      [IMG_NOTSELECTABLE] = RES_ROOT //
      "/Image/Encounter/Spell/WaldDisabled.png",
    };

    InitSpell(Spell::Woodsy, WaldImages, 250, -350);

    static const char* WitzImages[] = {
      [IMG_NOTSELECTED] = RES_ROOT //
      //
      "/Image/Encounter/Spell/Witz.png",
      [IMG_SELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/WitzSelected.png",
      [IMG_CASTED] = RES_ROOT //
      "/Image/Encounter/Spell/WitzCasted.png",
      [IMG_HALF_NOTSELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/WitzHalf.png",
      [IMG_HALF_SELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/WitzHalfSelected.png",
      [IMG_HALF_CASTED] = RES_ROOT //
      "/Image/Encounter/Spell/WitzHalfCasted.png",
      [IMG_NOTSELECTABLE] = RES_ROOT //
      "/Image/Encounter/Spell/WitzDisabled.png",
    };

    InitSpell(Spell::Trixter, WitzImages, 500, 0);

    static const char* SchattenImages[] = {
      [IMG_NOTSELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/Schatten.png",
      [IMG_SELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/SchattenSelected.png",
      [IMG_CASTED] = RES_ROOT //
      "/Image/Encounter/Spell/SchattenCasted.png",
      [IMG_HALF_NOTSELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/SchattenHalf.png",
      [IMG_HALF_SELECTED] = RES_ROOT //
      "/Image/Encounter/Spell/SchattenHalfSelected.png",
      [IMG_HALF_CASTED] = RES_ROOT //
      "/Image/Encounter/Spell/SchattenHalfCasted.png",
      [IMG_NOTSELECTABLE] = RES_ROOT //
      "/Image/Encounter/Spell/SchattenDisabled.png",
    };

    InitSpell(Spell::Shadow, SchattenImages, -250, 350);
}

void Page_DuelXavier::process()
{
    if (!m_initialized) {
        Init();
        m_initialized = true;
    }

    m_imgManaLeft.Update(Ctrl_Mana::Left, GetMana(Ctrl_Mana::Left));
    m_imgManaRight.Update(Ctrl_Mana::Right, GetMana(Ctrl_Mana::Right));

    GuiFrame::process();
}

void Page_DuelXavier::Transition()
{
    System::GetPageStatic<Page_Background>()->SetImage(
      Page_Background::ImageType::TouchDuelGolem);
}

const char* Page_DuelXavier::NextPhase(Spell castSpell)
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

const char* Page_DuelXavier::NextMovie()
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

void Page_DuelXavier::NextFrame(u32 frame)
{
    if (m_nextPhase != Phase::Retreat)
        return;

    if (m_castSpell != Spell::None && m_castSpell != Spell::Warrior) {
        // Hardcoded ending frames for when Xavier is not on screen
        for (u32 f : {96, 225, 339}) {
            if (frame == f)
                ForceNextMovie();
        }
    }
}

void Page_DuelXavier::Cast(
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

void Page_DuelXavier::DeselectAll()
{
    for (u32 i = 0; i < SpellCount; i++) {
        m_buttons[i].Deselect();
    }
}
