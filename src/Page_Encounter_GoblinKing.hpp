// Page_Encounter_GoblinKing.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "AudioFileOpus.hpp"
#include "Ctrl_Spell.hpp"
#include "Encounter.hpp"
#include "System.hpp"
#include <gui/GuiButton.h>
#include <gui/GuiFrame.h>
#include <gui/GuiImage.h>

class Page_Encounter_GoblinKing : public GuiFrame, public Encounter
{
public:
    Page_Encounter_GoblinKing();

protected:
    enum class Phase {
        Idle,
        Start,
        Intro,
        GoblinKingAttackFirst,
        Dodge,
        GoblinKingAttack,
        MagiAttack,
        End,
    };

public:
    /**
     * TransitionFirst from Encounter.
     */
    void TransitionFirst() override;

    /**
     * TransitionSecond from Encounter.
     */
    void TransitionSecond() override;

private:
    const char* NextPhase();

public:
    /**
     * NextMovie from Encounter.
     */
    virtual const char* NextMovie() override;

    /**
     * NextFrame from Encounter.
     */
    virtual void NextFrame(u32 frame) override;

    /**
     * Cast from WandHandler.
     */
    virtual void Cast(
      Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ) override;

    /**
     * BackButtonAllowed from Encounter.
     */
    bool BackButtonAllowed() const override
    {
        return m_nextPhase == Phase::Idle ||
               (m_currentPhase == Phase::Idle && m_nextPhase == Phase::End);
    }

    /**
     * GetEncounterType from Encounter.
     */
    Encounter::Type GetEncounterType() const override
    {
        return Encounter::Type::GoblinKing;
    }

protected:
    /**
     * Deselect all spells.
     */
    void DeselectAll();

    Ctrl_Spell m_spell;

    bool m_cast = false;
    bool m_castLate = false;
    bool m_castAllowed = false;

    u32 m_goblinKingHitCount = 0;
    // i.e iFireballThrown, value would overflow
    u32 m_magiHitCount = 0;

    u32 m_dodgePhase = 0;

    Phase m_currentPhase = Phase::Idle;
    Phase m_nextPhase = Phase::Idle;
    char m_phaseMoviePath[128];

    u32 m_frameNum = 0;

    AudioFileOpus m_enterSound{"EnterKingChamber", true, true, true, true};
};
