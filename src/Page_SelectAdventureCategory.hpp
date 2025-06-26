// Page_SelectAdventureCategory.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Ctrl_Image.hpp"
#include "Page_SelectAdventure.hpp"
#include <gui/GuiButton.h>
#include <gui/GuiFrame.h>

class Page_SelectAdventureCategory : public GuiFrame, public sigslot::has_slots<>
{
public:
    Page_SelectAdventureCategory();

    /**
     * GuiElement process.
     */
    void process() override;

private:
    void InitButton(
      Ctrl_Image& banner, GuiButton& button, const char* imagePath, float posX, float posY);

    void OnSelect(GuiButton* button, const GuiController* controller, GuiTrigger* trigger);

private:
    Ctrl_Image m_title;

    Ctrl_Image m_bannerMagiAdventure;
    GuiButton m_btnMagiAdventure{520, 200};

    Ctrl_Image m_bannerMasterMagiAdventure;
    GuiButton m_btnMasterMagiAdventure{520, 200};

    Ctrl_Image m_bannerPortalAdventure;
    GuiButton m_btnPortalAdventure{520, 200};

    Ctrl_Image m_bannerHeroicAdventure;
    GuiButton m_btnHeroicAdventure{520, 200};

    GuiTrigger m_touchTrigger{GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH};
};
