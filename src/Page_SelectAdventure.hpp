// Page_SelectAdventure.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Ctrl_Image.hpp"
#include "Page_TouchDuel.hpp"
#include <gui/GuiButton.h>
#include <gui/GuiFrame.h>

class Page_SelectAdventure : public GuiFrame, public sigslot::has_slots<>
{
public:
    Page_SelectAdventure();

    /**
     * GuiElement process.
     */
    void process() override;

    void RegisterEncounter(int index, const char* name, Page_TouchDuel::EncounterType encounter);

private:
    void OnSelect(GuiButton* button, const GuiController* controller, GuiTrigger* trigger);

private:
    bool m_initialized = false;

    Ctrl_Image m_title;
    Ctrl_Image m_encounterBanner[6];
    Ctrl_Image m_encounterTitle[6];
    GuiButton m_encounterButton[6];
    Page_TouchDuel::EncounterType m_encounterType[6];

    GuiTrigger m_touchTrigger{GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH};
};
