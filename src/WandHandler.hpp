// WandHandler.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Wand.hpp"

class WandHandler
{
public:
    /**
     * Called when the player casts at the screen.
     */
    virtual void Cast(Wand::CastMode castMode, bool curValid, float curX,
      float curY, float curZ) = 0;
};
