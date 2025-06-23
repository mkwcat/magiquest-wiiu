// Page_DuelDragon.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Ctrl_Mana.hpp"
#include "Ctrl_Spell.hpp"
#include "Encounter.hpp"
#include "System.hpp"
#include <array>
#include <gui/GuiButton.h>
#include <gui/GuiFrame.h>
#include <gui/GuiImage.h>

class Page_DuelDragon : public GuiFrame, public Encounter
{
    enum class Spell {
        Protection,
        Freeze,
        IceArrow,
        None,
    };
    static constexpr u32 SpellCount = u32(Spell::None);

    enum class Phase {
        Idle,
        Ready,
        Start,
        Attack,
        Retreat,
        FreezeFail,
        Freeze,
        AttackFail,
        FailRetreat,
        End,
    };

    void InitSpell(Spell spell, const char* const* images, int posX, int posY);

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

private:
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

    static std::array<int, 10> GetDamageTable(bool protection, bool dragonWand)
    {
        if (!protection) {
            return {8, 8, 8, 8, 8, 8};
        }

        if (dragonWand) {
            return {0, 2, 2, 2, 2, 2, 2, 2, 2};
        }

        return {2, 2, 4, 6, 8, 10};
    }

private:
    /**
     * Deselect all spells.
     */
    void DeselectAll();

    Ctrl_Spell m_buttons[SpellCount];
    int m_buttonUses[SpellCount] = {};
    Spell m_castSpell = Spell::None;
    int m_dragonHitCount = 0;
    int m_magiHitCount = 0;
    bool m_repeatCycle = false;

    bool m_initialized = false;

    Phase m_currentPhase = Phase::Idle;
    Phase m_nextPhase = Phase::Idle;
    char m_phaseMoviePath[128];
    bool m_isInputPhase = false;

    Ctrl_Mana m_manaLeft;
    Ctrl_Mana m_manaRight;

    bool m_started = false;
};
