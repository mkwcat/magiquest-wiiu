// Page_ModeSelect.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Ctrl_Image.hpp"
#include "System.hpp"
#include <gui/GuiButton.h>
#include <gui/GuiFrame.h>

class Page_ModeSelect : public GuiFrame, public sigslot::has_slots<>
{
public:
    Page_ModeSelect();

    /**
     * GuiElement process.
     */
    void process() override;

private:
    void OnSelectTouchDuel(GuiButton* button, const GuiController* controller, GuiTrigger* trigger);

    void OnSelectCastDuel(GuiButton* button, const GuiController* controller, GuiTrigger* trigger);

    void OnSelectRealWand(GuiButton* button, const GuiController* controller, GuiTrigger* trigger);

private:
    Ctrl_Image m_title;

    Ctrl_Image m_bannerTouchDuel;
    GuiButton m_btnTouchDuel;

    Ctrl_Image m_bannerCastDuel;
    GuiButton m_btnCastDuel;

    Ctrl_Image m_bannerRealWand;
    GuiButton m_btnRealWand;

    GuiTrigger m_touchTrigger{GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH};
};
