// Page_EncounterSelect.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Ctrl_Image.hpp"
#include "Encounter.hpp"
#include "System.hpp"
#include <gui/GuiButton.h>
#include <gui/GuiFrame.h>

class Page_EncounterSelect : public GuiFrame, public sigslot::has_slots<>
{
public:
    Page_EncounterSelect();

    /**
     * GuiElement process.
     */
    void process() override;

    void RegisterEncounter(int index, const char* name, GuiElement* encounter);

private:
    void OnSelect(GuiButton* button, const GuiController* controller, GuiTrigger* trigger);

private:
    bool m_initialized = false;

    Ctrl_Image m_title;

    GuiElement* m_encounter[6] = {};
    Ctrl_Image m_encounterBanner[6];
    Ctrl_Image m_encounterTitle[6];
    GuiButton m_encounterButton[6];

    GuiTrigger m_touchTrigger{GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH};
};
