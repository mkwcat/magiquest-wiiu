// Page_Movie.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Ctrl_Movie.hpp"
#include "Encounter.hpp"
#include <gui/GuiFrame.h>
#include <gui/GuiImage.h>
#include <gui/GuiImageData.h>

class Page_Movie : public GuiFrame
{
public:
    Page_Movie();
    ~Page_Movie();

    /**
     * Gets the GuiImageData for the specified mana value.
     * side: 0 = left, 1 = right
     */
    GuiImageData* GetManaImage(u8 side, u8 value);

    /**
     * Force ends the movie.
     */
    void EndMovie();

    /**
     * Set the current encounter.
     */
    void SetEncounter(Encounter* encounter);

    /**
     * Notify that the mana has been updated.
     */
    void ManaUpdate(u8 side, u8 value);

    /**
     * GuiElement process.
     */
    void process() override;

protected:
    Ctrl_Movie m_movie;

    OSMutex m_mutex;

    GuiImage m_imgManaLeft;
    GuiImage m_imgManaRight;
    GuiImageData m_manaLeft[17];
    GuiImageData m_manaRight[17];

    enum class ManaSound {
        None,
        ManaDown,
        ManaReset,
    };
    ManaSound m_manaSound;

    Encounter* m_encounter;
};
