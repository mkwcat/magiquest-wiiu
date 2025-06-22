// Wand.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <gctypes.h>

class Wand
{
public:
    Wand();
    ~Wand();

private:
    /**
     * Check if the Wii remote is currently being held still.
     */
    bool IsStill();

    void UpdateAcc(float x, float y, float z);

public:
    enum class CastMode {
        WiiRemoteTouchRune = 0,
        WiiRemoteCastRune = 1,
        RealWand = 2,
    };

    /**
     * Call every frame to update the cast state.
     */
    void Update(float* curX, float* curY, float* curZ, bool* curValid);

    /**
     * If the wand has casted.
     */
    bool IsCast();

    /**
     * Get the current cast mode.
     */
    CastMode GetCastMode() const;

    /**
     * Set the current cast mode.
     */
    void SetCastMode(CastMode mode);

private:
    bool m_casted;
    bool m_testCasted = false;

    bool m_curPosValid = false;
    float m_curPosX = 0;
    float m_curPosY = 0;
    float m_curPosZ = 0;

    /**
     * Countdown timer, stop decrementing at 0. Will not cast unless this value
     * is 0.
     */
    u32 m_timer;

    s32 m_rumbleTimer = -1;

    s32 m_castPhase = -1;
    u32 m_stillTicks = 0;
    bool m_pointedAtTV = false;

    s32 m_irCountDetected = -1;

    CastMode m_castMode = CastMode::WiiRemoteCastRune;
};
