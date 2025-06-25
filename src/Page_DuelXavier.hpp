// Page_DuelXavier.hpp
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

class Page_DuelXavier : public GuiFrame, public Encounter
{
    enum class Spell {
        Warrior,
        Majestic,
        Woodsy,
        Trixter,
        Shadow,
        None,
    };
    static constexpr u32 SpellCount = u32(Spell::None);

    enum class Phase {
        Idle,
        Start,
        Restart,
        Intro,
        Attack,
        AttackEnd,
        Retreat,
        Stunned,
        MagiAttack,
        XavierNoMana,
        MagiWin,
        MagiLose,
        End,
    };

    enum class AttackPhase {
        Majestic,
        Shadow,
        Trixter,
        Warrior,
        Woodsy,
        Default,
    };
    static constexpr u32 AttackPhaseCount = u32(AttackPhase::Default);

protected:
    Ctrl_Spell m_buttons[SpellCount];
    int m_buttonUses[SpellCount];
    Spell m_castSpell;
    bool m_doneAttackPhase[AttackPhaseCount];

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
     * NextFrame from Encounter.
     */
    virtual void NextFrame(u32 frame) override;

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
        return m_currentPhase != Phase::Idle && m_currentPhase != Phase::MagiWin &&
               m_currentPhase != Phase::MagiLose && m_currentPhase != Phase::End;
    }

    /**
     * GetBackgroundType from Encounter.
     */
    Page_Background::ImageType GetBackgroundType() const override
    {
        return Page_Background::ImageType::TouchDuelXavier;
    }

protected:
    /**
     * Deselect all spells.
     */
    void DeselectAll();

    bool m_initialized = false;

    Phase m_currentPhase = Phase::Idle;
    Phase m_nextPhase = Phase::Idle;
    AttackPhase m_attackPhase;
    char m_phaseMoviePath[128];
    bool m_isInputPhase = false;

    bool m_started = false;
};
