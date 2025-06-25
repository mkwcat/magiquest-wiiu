// Page_DuelGolem.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Ctrl_Spell.hpp"
#include "Encounter.hpp"
#include "System.hpp"
#include <gui/GuiButton.h>
#include <gui/GuiFrame.h>
#include <gui/GuiImage.h>

class Page_DuelGolem : public GuiFrame, public Encounter
{
    enum class Spell {
        Magma,
        Chisel,
        CloudHammer,
        Thunder,

        None,
    };
    static constexpr u32 SpellCount = u32(Spell::None);

    enum class Phase {
        End,
        Idle,
        Start,
        Intro,
        Phase1Trigger,
        Phase1Start,
        Phase2Trigger,
        Phase2Start,
        Phase3Trigger,
        Phase3Start,
        Phase4Start,
    };

public:
    Page_DuelGolem();
    ~Page_DuelGolem();

protected:
    Ctrl_Spell m_buttons[SpellCount];
    Spell m_castSpell;

    void InitSpell(Spell spell, const char** images, int posX, int posY);

    void Init();

public:
    /**
     * GuiElement process.
     */
    void process() override;

    /**
     * Transition from Encounter.
     */
    void Transition() override;

protected:
    const char* NextPhase(Spell castSpell);

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
        return m_currentPhase != Phase::Idle && m_currentPhase != Phase::End;
    }

protected:
    /**
     * Deselect all spells.
     */
    void DeselectAll();

    bool m_initialized;

    Phase m_currentPhase;
    Phase m_nextPhase;
    char m_phaseMoviePath[128];
    bool m_isInputPhase;

    bool m_started;
};
