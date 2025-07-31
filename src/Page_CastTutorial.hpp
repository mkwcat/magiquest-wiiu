// Page_CastTutorial.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Ctrl_CommonButton.hpp"
#include "Ctrl_Image.hpp"
#include "Ctrl_TitleText.hpp"
#include "System.hpp"
#include "WandHandler.hpp"
#include <gui/GuiButton.h>
#include <gui/GuiFrame.h>

class Page_CastTutorial : public GuiFrame, public WandHandler, public sigslot::has_slots<>
{
private:
    void Init();

public:
    /**
     * GuiElement process.
     */
    void process() override;

    /**
     * Cast from WandHandler.
     */
    virtual void Cast(
      Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ) override;

private:
    bool m_initialized = false;

    Ctrl_TitleText m_title{"CastDuel"};
    Ctrl_Image m_instruction[4];
    Ctrl_Image m_imgCheck;
    Ctrl_CommonButton m_btnBack;
};
