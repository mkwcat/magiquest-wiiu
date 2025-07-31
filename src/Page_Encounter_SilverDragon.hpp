// Page_Encounter_Dragon.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Ctrl_Spell.hpp"
#include "Encounter.hpp"
#include "Page_Background.hpp"
#include "System.hpp"
#include <gui/GuiButton.h>
#include <gui/GuiFrame.h>
#include <gui/GuiImage.h>

class Page_Encounter_SilverDragon : public GuiFrame, public Encounter
{
public:
    Page_Encounter_SilverDragon();

protected:
    enum class Spell {
        // Crystals must be first

        Blue,
        Red,
        White,
        Purple,
        Yellow,
        Medal, // Must be directly after the crystals
        Shield,
        None,
    };
    static constexpr u32 SpellCount = u32(Spell::None);

    enum class Phase {
        End,
        Idle,
        Start,
        Intro,
        Crystal,
        Attack,
    };

    void InitSpell(Spell spell, const char* const* images, float posX, float posY, float size);

public:
    /**
     * TransitionFirst from Encounter.
     */
    void TransitionFirst() override;

private:
    const char* NextPhase();

public:
    /**
     * NextMovie from Encounter.
     */
    virtual const char* NextMovie() override;

    /**
     * Cast from WandHandler.
     */
    virtual void Cast(
      Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ) override;

    /**
     * UIVisible from Encounter.
     */
    bool UIVisible() const override
    {
        return m_currentPhase != Phase::Idle && m_currentPhase != Phase::End &&
               m_currentPhase != Phase::Start;
    }

    /**
     * BackButtonAllowed from Encounter.
     */
    bool BackButtonAllowed() const override
    {
        return m_currentPhase == Phase::Idle || m_currentPhase == Phase::End;
    }

    /**
     * GetEncounterType from Encounter.
     */
    Encounter::Type GetEncounterType() const override
    {
        return Encounter::Type::SilverDragon;
    }

private:
    /**
     * Deselect all spells.
     */
    void DeselectAll();

    std::array<Ctrl_Spell, SpellCount> m_buttons;
    std::array<Spell, 32> m_castSpell = {};
    u32 m_castSpellCount = 0;
    std::array<Spell, 32> m_queueSpell = {};
    u32 m_queueSpellCount = 0;
    int m_sequence = 0;
    std::array<Spell, 7> m_pattern = {};
    int m_patternCount = 0;
    std::array<Ctrl_Image, 32> m_queueDisplay;

    Phase m_currentPhase = Phase::Idle;
    Phase m_nextPhase = Phase::Idle;
    char m_phaseMoviePath[128];
    bool m_isInputPhase = false;

    bool m_started = false;
};
