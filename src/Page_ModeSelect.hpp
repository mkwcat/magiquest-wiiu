// Page_ModeSelect.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "System.hpp"
#include <gui/GuiButton.h>
#include <gui/GuiFrame.h>
#include <gui/GuiImage.h>

class Page_ModeSelect : public GuiFrame, public sigslot::has_slots<>
{
public:
    Page_ModeSelect();

private:
    void Init();

public:
    /**
     * GuiElement process.
     */
    void process() override;

private:
    void OnSelectTouchDuel(
      GuiButton* button, const GuiController* controller, GuiTrigger* trigger);

    void OnSelectCastDuel(
      GuiButton* button, const GuiController* controller, GuiTrigger* trigger);

    void OnSelectRealWand(
      GuiButton* button, const GuiController* controller, GuiTrigger* trigger);

private:
    bool m_initialized = false;

    GuiImage m_title;
    GuiImageData m_titleData;

    GuiImage m_bannerTouchDuel;
    GuiImageData m_bannerTouchDuelData;
    GuiButton m_btnTouchDuel;

    GuiImage m_bannerCastDuel;
    GuiImageData m_bannerCastDuelData;
    GuiButton m_btnCastDuel;

    GuiImage m_bannerRealWand;
    GuiImageData m_bannerRealWandData;
    GuiButton m_btnRealWand;

    GuiTrigger m_touchTrigger{GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH};
};
