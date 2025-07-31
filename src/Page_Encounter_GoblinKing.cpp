// Page_Encounter_GoblinKing.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_Encounter_GoblinKing.hpp"
#include <cstdlib>

enum {
    IMG_NOTSELECTED = 0,
    IMG_SELECTED = 1,
    IMG_DISABLED = 2,
};

Page_Encounter_GoblinKing::Page_Encounter_GoblinKing()
{
    using StringArray = const char* const[];

    m_spell.Init("LightningSpell", RES_ROOT "/Image/Encounter/Spell/Common",
      StringArray{"Lightning", "LightningHover", "LightningDull"}, 3);
    m_spell.SetImages(IMG_NOTSELECTED, IMG_SELECTED, IMG_DISABLED);
    m_spell.setPosition(0, 0);
    float height = m_spell.GetImage(0).getHeight();
    m_spell.setScaleX(500.0 / height);
    m_spell.setScaleY(500.0 / height);
    m_spell.SetHoverable(false);
    m_spell.SetImage(IMG_DISABLED);
    append(&m_spell);
}

void Page_Encounter_GoblinKing::TransitionFirst()
{
    m_nextPhase = Phase::End;
    ForceNextMovie();
}

void Page_Encounter_GoblinKing::TransitionSecond()
{
    m_nextPhase = Phase::Start;
    ForceNextMovie();
}

const char* Page_Encounter_GoblinKing::NextPhase()
{
    switch (m_currentPhase) {
    case Phase::End:
        m_nextPhase = Phase::Idle;
        SetMana(0, 0);
        SetMana(1, 0);
        // Fall through

    case Phase::Idle:
        return "GoblinKing0001";

    case Phase::Start:
        m_nextPhase = Phase::Intro;
        m_magiHitCount = 0;
        m_goblinKingHitCount = 0;
        m_cast = false;
        m_castAllowed = false;
        m_castLate = false;
        m_dodgePhase = 0;
        m_enterSound.Play();
        return "GoblinKing0002";

    case Phase::Intro:
        m_nextPhase = Phase::GoblinKingAttackFirst;
        m_goblinKingHitCount++;
        SetMana(0, 16);
        SetMana(1, 16);
        return "GoblinKing0003";

    case Phase::GoblinKingAttackFirst:
        // Damage is delayed by one phase for some reason
        SetMana(0, GetMana(0) - 4);

    case Phase::Dodge:
        m_nextPhase = Phase::Dodge;

        if (m_cast) {
            m_cast = false;
            m_castLate = false;
            m_castAllowed = false;
            m_spell.SetImage(IMG_DISABLED);
            switch (m_magiHitCount++) {
            case 0:
                SetMana(1, 14);
                return "GoblinKing0011";

            case 1:
                SetMana(1, 10);
                return "GoblinKing0012";

            case 2:
                SetMana(1, 6);
                return "GoblinKing0013";

            case 3:
                SetMana(1, 2);
                return "GoblinKing0014";

            case 4:
            default:
                SetMana(1, 0);
                m_nextPhase = Phase::End;
                m_spell.SetImage(IMG_DISABLED);
                return "GoblinKing0015";
            }
        }

        m_cast = m_castLate;
        m_castLate = false;

        switch (m_dodgePhase++) {
        case 0:
            m_castAllowed = true;
            m_spell.SetImage(IMG_SELECTED);
            return "GoblinKing0004";

        case 1:
            m_castAllowed = true;
            m_spell.SetImage(IMG_SELECTED);
            return "GoblinKing0005";

        case 2:
            m_castAllowed = true;
            m_spell.SetImage(IMG_SELECTED);
            return "GoblinKing0006";

        case 3:
            m_castAllowed = true;
            m_spell.SetImage(IMG_SELECTED);
            return "GoblinKing0007";

        case 4:
        default:
            m_castAllowed = false;
            m_cast = m_castLate = false;
            m_dodgePhase = 0;
            m_spell.SetImage(IMG_DISABLED);
            SetMana(0, GetMana(0) - 4);
            switch (m_goblinKingHitCount++) {
            case 1:
                return "GoblinKing0008";

            case 2:
                return "GoblinKing0009";

            case 3:
            default:
                m_nextPhase = Phase::End;
                return "GoblinKing0010";
            }
        }

        m_nextPhase = Phase::Dodge;
        m_castAllowed = true;
        m_spell.SetImage(IMG_SELECTED);
        return "GoblinKing0004";

    default:
        PANIC("Unknown phase");
    }
}

const char* Page_Encounter_GoblinKing::NextMovie()
{
    m_currentPhase = m_nextPhase;

    auto name = NextPhase();

    if (name == nullptr) {
        // Default to the Goblin King idle screen
        return RES_ROOT "/Movie/GoblinKing/GoblinKing0001.mp4";
    } else {
        std::snprintf(
          m_phaseMoviePath, sizeof(m_phaseMoviePath), RES_ROOT "/Movie/GoblinKing/%s.mp4", name);
    }

    return m_phaseMoviePath;
}

void Page_Encounter_GoblinKing::NextFrame(u32 frame)
{
    m_frameNum = frame;
}

void Page_Encounter_GoblinKing::Cast(
  Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ)
{
    LOG(LogSystem, "Cast");

    if (m_nextPhase == Phase::Idle) {
        m_nextPhase = Phase::Start;
        ForceNextMovie();
        return;
    }

    if (m_castAllowed && !m_cast && !m_castLate) {
        if (m_frameNum < 40) {
            m_cast = true;
        } else {
            m_castLate = true;
        }
    }
}