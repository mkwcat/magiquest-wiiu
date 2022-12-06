// Wand.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Wand.hpp"
#include "Util.hpp"
#include <padscore/kpad.h>
#include <vpad/input.h>

Wand::Wand()
{
    m_casted = false;
    m_timer = 20;
    m_castPhase = -1;
}

Wand::~Wand()
{
}

bool Wand::IsStill()
{
    return true;
}

WPADChan GetChanByInt(int chan)
{
    if (chan == 0) {
        return WPAD_CHAN_0;
    }
    if (chan == 1) {
        return WPAD_CHAN_1;
    }
    if (chan == 2) {
        return WPAD_CHAN_3;
    }
    if (chan == 3) {
        return WPAD_CHAN_3;
    }
    return WPAD_CHAN_0;
}

void Wand::UpdateAcc(float x, float y, float z)
{
    static float x0 = 0.0, x1 = 0.0, x2 = 0.0, x3 = 0.0;
    static float y0 = 0.0, y1 = 0.0, y2 = 0.0, y3 = 0.0;
    static float z0 = 0.0, z1 = 0.0, z2 = 0.0, z3 = 0.0;

    x *= 1.5;
    y = y * 1.5 + 0.5;

    m_curPosValid = false;

    // Average calculation
    if (((z0 + z1 + z2 + z3 + z) / 5) > 0) {
        m_curPosValid = true;
        m_curPosX = ((x0 + x1 + x2 + x3 + x) / 5) * 1000;
        m_curPosY = -((y0 + y1 + y2 + y3 + y) / 5) * 1000;
    }

    x0 = x1;
    x1 = x2;
    x2 = x3;
    y0 = y1;
    y1 = y2;
    y2 = y3;
    z0 = z1;
    z1 = z2;
    z2 = z3;

    x3 = x;
    y3 = y;
    z3 = z;
}

void Wand::Update(float* curX, float* curY, float* curZ, bool* curValid)
{
    if (m_timer > 0) {
        m_timer--;
    }

    KPADStatus data;

    if (m_castMode == CastMode::RealWand) {
        if (m_casted) {
            return;
        }

        // Search for any IR signal from the Wii Remotes. This will be messed
        // with by _any_ IR signal, including the sensor bar.

        for (int i = 0; i < 4; i++) {
            if (KPADRead(GetChanByInt(i), &data, 1) != 1) {
                return;
            }

            if (data.posValid != 0) {
                m_timer = 8;
                m_casted = true;
            }
        }

        return;
    }

    if (KPADRead(GetChanByInt(0), &data, 1) != 1) {
        return;
    }

    if (data.accMagnitude < 1.2 && data.accMagnitude > 0.8)
        UpdateAcc(data.acc.x, data.acc.y, data.acc.z);
    *curValid = m_curPosValid;
    *curX = m_curPosX;
    *curY = m_curPosY;
    *curZ = m_curPosZ;

    if (m_casted) {
        return;
    }

    bool isStill = data.accVariation < 0.7;

    // The value to start the timer at when the player shakes the Wii Remote.
    static constexpr u32 TimerStart = 30;

    switch (m_castPhase) {
    case -1:
        // Cast phase -1 is startup. This will wait for the Wii Remote data to
        // stabilize.
        if (m_timer == 0) {
            m_castPhase++;
        }

    case 0:
        // Cast phase 0 is when the Wii Remote is still. If the player shakes
        // the Wii Remote then the timer will start and move onto phase 1.
        if (!isStill && m_timer == 0) {
            m_timer = TimerStart;
            m_castPhase++;
        }
        break;

    case 1:
        // Cast phase 1 just waits 5 ticks.
        if (m_timer <= TimerStart - 5) {
            m_pointedAtTV = false;
            m_stillTicks = 0;
            m_castPhase++;
        }
        break;

    case 2:
        // Now check if the wand is still for 10 consecutive ticks until the
        // timer is up. TODO: We could also check the IR position to see if it's
        // actually pointing at the TV.
        if (isStill) {
            m_stillTicks++;
            m_pointedAtTV = true;
        } else {
            m_pointedAtTV = false;
            m_stillTicks = 0;
        }

        if (m_stillTicks >= 10 && m_pointedAtTV) {
            m_casted = true;
            m_castPhase = 0;
            break;
        }

        if (m_timer == 0) {
            m_castPhase = 0;
        }
        break;
    }
}

bool Wand::IsCast()
{
    if (m_casted && m_timer <= 8) {
        m_casted = false;
        return true;
    }

    return false;
}

Wand::CastMode Wand::GetCastMode() const
{
    return m_castMode;
}

void Wand::SetCastMode(CastMode mode)
{
    m_castMode = mode;
}
