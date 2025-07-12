// Page_Encounter_Dragon.hpp
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

class Page_Encounter_Dragon : public GuiFrame, public Encounter
{
protected:
    enum class Spell {
        Protection,
        Freeze,
        IceArrow,
        Reveal,
        None,
    };
    static constexpr u32 SpellCount = u32(Spell::None);

    enum class Phase {
        Idle,
        Ready,
        Start,
        Fireball,
        ArrowFireball,
        TailSweep,
        Freeze,
        End,
        IceDragonReveal,
        HeroicFail,
        HeroicExpectProtection,
        HeroicExpectFreeze,
        HeroicExpectIceArrow,
        HeroicExpectReveal,
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
        return Encounter::Type::Dragon;
    }

protected:
    /**
     * Deselect all spells.
     */
    void DeselectAll();

    Ctrl_Spell m_buttons[SpellCount];
    Ctrl_Spell m_imgMq;

    bool m_castProtection = false;
    bool m_castFreeze = false;
    bool m_castIceArrow = false;
    bool m_castReveal = false;
    bool m_allowProtection = false;
    bool m_allowFreeze = false;
    bool m_allowIceArrow = false;
    bool m_allowReveal = false;

    u32 m_dragonHitCount = 0;
    // i.e iFireballThrown, value would overflow
    u32 m_magiHitCount = 0;

    bool m_initialized = false;

    Phase m_currentPhase = Phase::Idle;
    Phase m_nextPhase = Phase::Idle;
    char m_phaseMoviePath[128];
};
