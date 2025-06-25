// Page_DuelDragon.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

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
        Tailsweep,
        FreezeFail,
        Freeze,
        AttackFail,
        FailTailsweep,
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
    const char* NextPhase();
    void NextInput();

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

    static std::array<int, 10> GetDamageTable(bool protection, bool dragonWand)
    {
        // Based on DragonEncounter2.SetMagMaPole (and corroborated with testing). If the index
        // overflows then the value should be 0.
        if (!protection) {
            return {8, 8, 8, 8, 8, 8, 8, 8, 8, 0};
        }

        if (dragonWand) {
            return {0, 2, 2, 2, 2, 2, 2, 2, 2, 0};
        }

        return {2, 2, 4, 6, 2, 2, 2, 2, 2, 0};
    }

    /**
     * UIVisible from Encounter.
     */
    bool UIVisible() const override
    {
        return m_nextPhase != Phase::Idle &&
               (m_currentPhase != Phase::Idle || m_nextPhase != Phase::End);
    }

private:
    /**
     * Deselect all spells.
     */
    void DeselectAll();

    Ctrl_Spell m_buttons[SpellCount];
    int m_dragonHitCount = 0;

    bool m_castProtection = false;
    bool m_castFreeze = false;
    bool m_castIceArrow = false;
    bool m_allowProtection = false;
    bool m_allowFreeze = false;
    bool m_allowIceArrow = false;

    // i.e iFireballThrown, value will overflow
    int m_magiHitCount = 0;

    bool m_initialized = false;

    Phase m_currentPhase = Phase::Idle;
    Phase m_nextPhase = Phase::Idle;
    char m_phaseMoviePath[128];
};
