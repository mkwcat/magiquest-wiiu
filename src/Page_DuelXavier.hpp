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

public:
    Page_DuelXavier();
    ~Page_DuelXavier();

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
    virtual void Cast(Wand::CastMode castMode, bool curValid, float curX,
      float curY, float curZ) override;

protected:
    /**
     * Deselect all spells.
     */
    void DeselectAll();

    bool m_initialized;

    Phase m_currentPhase;
    Phase m_nextPhase;
    AttackPhase m_attackPhase;
    char m_phaseMoviePath[128];
    bool m_isInputPhase;

    GuiImage m_imgManaLeft;
    GuiImage m_imgManaRight;

    bool m_started;
};
