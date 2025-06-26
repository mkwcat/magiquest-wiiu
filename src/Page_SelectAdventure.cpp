// Page_SelectAdventure.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_SelectAdventure.hpp"
#include "Page_Background.hpp"
#include "Page_ModeSelect.hpp"
#include "Page_TouchDuel.hpp"
#include "System.hpp"

Page_SelectAdventure::Page_SelectAdventure()
{
    m_btnBack.InitDefault(Ctrl_CommonButton::Type::LeftBack);
    m_btnBack.SetOnSelectGotoPage(
      System::PageID::SelectAdventure, System::PageID::SelectAdventureCategory);
    append(&m_btnBack);

    m_title.Load(RES_ROOT "/Image/Menu/Title/ModeSelect.png");
    m_title.setPosition(0, 370);
    m_title.setScaleX(1.8);
    m_title.setScaleY(1.8);

    append(&m_title);

    // Magi Adventure
    RegisterAdventure( //
      0, 0, "Dragon", Category::Magi, Page_TouchDuel::EncounterType::Dragon, "Dragon.jpg");

    // Portal Adventure
    RegisterAdventure( //
      1, 0, "Silver Dragon", Category::Portal, Page_TouchDuel::EncounterType::SilverDragon,
      "SilverDragon.png");
    RegisterAdventure( //
      2, 1, "Xavier", Category::Portal, Page_TouchDuel::EncounterType::Xavier, "BaseFrame.png");
    RegisterAdventure( //
      3, 2, "Golem", Category::Portal, Page_TouchDuel::EncounterType::Golem, "BaseFrame.png");
}

void Page_SelectAdventure::process()
{
    if (sys()->GetDisplay(sys()->GetPageID(this)) != System::Display::None) {
        Page_Background* background = sys()->GetPageStatic<Page_Background>();
        assert(background != nullptr);

        background->SetImage(Page_Background::ImageType::StatusStation);
    }
}

void Page_SelectAdventure::RegisterAdventure(u32 index, int categoryIndex, const char* name,
  Category category, Page_TouchDuel::EncounterType encounter, const char* imageName)
{
    assert(index < std::size(m_adventures));
    m_adventures[index].encounterType = encounter;
    m_adventures[index].category = category;

    if (imageName == nullptr) {
        imageName = "BaseFrame.png";
    }

    char path[256];
    snprintf(path, sizeof(path), RES_ROOT "/Image/Menu/Button/Adventure/%s", imageName);

    m_adventures[index].banner.Load(path);
    m_adventures[index].banner.setSize(300, 375);
    m_adventures[index].button.setImage(&m_adventures[index].banner);
    m_adventures[index].button.setPosition(
      (categoryIndex % 3 - 1) * 400, (categoryIndex / 3) * -400);
    m_adventures[index].button.setTrigger(&m_touchTrigger);
    m_adventures[index].button.clicked.connect<Page_SelectAdventure>(
      this, &Page_SelectAdventure::OnSelect);

    append(&m_adventures[index].button);
}

void Page_SelectAdventure::SetCategory(Category category)
{
    for (u32 i = 0; i < std::size(m_adventures); i++) {
        bool enable = m_adventures[i].category == category;
        m_adventures[i].button.setVisible(enable);
        m_adventures[i].button.setClickable(enable);
    }
}

void Page_SelectAdventure::OnSelect(
  GuiButton* button, const GuiController* controller, GuiTrigger* trigger)
{
    Page_TouchDuel* setting = sys()->GetPageStatic<Page_TouchDuel>();

    for (u32 i = 0; i < std::size(m_adventures); i++) {
        if (button == &m_adventures[i].button) {
            setting->LoadEncounter(m_adventures[i].encounterType);
            break;
        }
    }

    sys()->HidePage(System::GetPageID(this), System::Display::All);
    sys()->ShowPage(System::GetPageID<Page_ModeSelect>(), System::Display::DRC);
}