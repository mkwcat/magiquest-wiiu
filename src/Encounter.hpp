// Encounter.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Wand.hpp"
#include "WandHandler.hpp"
#include <coreinit/mutex.h>
#include <gctypes.h>

class Encounter : public WandHandler
{
public:
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

private:
    u8 m_manaLeft = 0;
    u8 m_manaRight = 0;

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
