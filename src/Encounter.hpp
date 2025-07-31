// Encounter.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Page_Background.hpp"
#include "WandHandler.hpp"
#include <coreinit/mutex.h>
#include <gctypes.h>

class Encounter : public WandHandler
{
public:
    enum class Type {
        Dragon,
        GoblinKing,
        IceDragon,
        HeroicDragon,
        SilverDragon,
        Golem,
        Xavier,
    };

    Encounter();
    virtual ~Encounter();

    /**
     * Returns the next movie to play. Called when the current movie stream
     * ends. The result is assumed valid until called again.
     */
    virtual const char* NextMovie() = 0;

    /**
     * Called each time a frame is rendered in the movie stream.
     */
    virtual void NextFrame(u32 frame)
    {
    }

    /**
     * Called when the encounter is selected in the menu.
     */
    virtual void TransitionFirst()
    {
    }

    /**
     * Called when the UI transitions to this page.
     */
    virtual void TransitionSecond()
    {
    }

    virtual bool UIVisible() const
    {
        return true;
    }

    virtual bool BackButtonAllowed() const
    {
        return !UIVisible();
    }

    virtual Page_Background::ImageType GetBackgroundType() const
    {
        return Page_Background::ImageType::TouchDuelBlizzardDawn;
    }

    virtual Type GetEncounterType() const = 0;

private:
    u8 m_manaLeft;
    u8 m_manaRight;

public:
    /**
     * Get the mana value. 0 = left, 1 = right.
     */
    u8 GetMana(u8 side);

    /**
     * Set the mana value. 0 = left, 1 = right.
     */
    void SetMana(u8 side, u8 value);

protected:
    /**
     * Force change to the next movie.
     */
    void ForceNextMovie();
};
