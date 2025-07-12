// Page_SelectAdventure.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Ctrl_CommonButton.hpp"
#include "Ctrl_Image.hpp"
#include "Page_TouchDuel.hpp"
#include <gui/GuiButton.h>
#include <gui/GuiFrame.h>

class Page_SelectAdventure : public GuiFrame, public sigslot::has_slots<>
{
public:
    enum class Category {
        Magi,
        MasterMagi,
        Portal,
        Heroic,
    };

    Page_SelectAdventure();

    /**
     * GuiElement process.
     */
    void process() override;

    void RegisterAdventure(u32 index, int categoryIndex, const char* name, Category category,
      Encounter::Type encounter, const char* imageName = nullptr);

    void SetCategory(Category category);

private:
    void OnSelect(GuiButton* button, const GuiController* controller, GuiTrigger* trigger);

private:
    bool m_initialized = false;

    struct Adventure {
        Ctrl_Image banner;
        Ctrl_Image title;
        GuiButton button{300, 375};
        Encounter::Type encounterType;
        Category category;
    };

    Ctrl_Image m_title;
    std::array<Adventure, 6> m_adventures;

    GuiTrigger m_touchTrigger{GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH};

    Ctrl_CommonButton m_btnBack;
};
