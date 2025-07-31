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

    append(&m_title);

    // Magi Adventure
    RegisterAdventure( //
      0, 0, "Dragon", Category::Magi, Encounter::Type::Dragon, "Dragon.jpg", "Dragon.png");
    RegisterAdventure( //
      1, 1, "Goblin", Category::Magi, Encounter::Type::GoblinKing, "Goblin.jpg", "Goblin.png");

    // Master Magi Adventure
    RegisterAdventure( //
      2, 0, "Ice Dragon", Category::MasterMagi, Encounter::Type::IceDragon, "IceDragon.jpg",
      "IceDragon.png");

    // Heroic Adventure
    RegisterAdventure( //
      3, 0, "Heroic Dragon", Category::Heroic, Encounter::Type::HeroicDragon, "HeroicDragon.png",
      "HeroicDragon.png");

    // Portal Adventure
    RegisterAdventure( //
      4, 0, "Silver Dragon", Category::Portal, Encounter::Type::SilverDragon, "SilverDragon.png",
      "SilverDragon.png");
    RegisterAdventure( //
      5, 1, "Xavier", Category::Portal, Encounter::Type::Xavier, "BaseFrame.png", "Xavier.png");
    RegisterAdventure( //
      6, 2, "Golem", Category::Portal, Encounter::Type::Golem, "BaseFrame.png", "Golem.png");
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
  Category category, Encounter::Type encounter, const char* imageName, const char* titleName)
{
    assert(index < std::size(m_adventures));
    m_adventures[index].encounterType = encounter;
    m_adventures[index].category = category;

    if (imageName == nullptr) {
        imageName = "BaseFrame.png";
    }

    char path[256];
    std::snprintf(path, sizeof(path), RES_ROOT "/Image/Menu/Button/Adventure/%s", imageName);

    m_adventures[index].banner.Load(path);
    m_adventures[index].banner.setSize(300, 375);
    m_adventures[index].button.setImage(&m_adventures[index].banner);
    m_adventures[index].button.setPosition(
      (categoryIndex % 3 - 1) * 400, (categoryIndex / 3) * -400 + 50);
    m_adventures[index].button.setTrigger(&m_touchTrigger);
    m_adventures[index].button.clicked.connect<Page_SelectAdventure>(
      this, &Page_SelectAdventure::OnSelect);

    std::snprintf(path, sizeof(path), RES_ROOT "/Image/Menu/Title/Adventure/%s", titleName);
    m_adventures[index].title.Load(path);
    m_adventures[index].title.setPosition(
      m_adventures[index].button.getCenterX(), m_adventures[index].button.getCenterY() - 210);
    m_adventures[index].title.setScaleX(DRC_PIXEL_SCALE);
    m_adventures[index].title.setScaleY(DRC_PIXEL_SCALE);

    append(&m_adventures[index].button);
    append(&m_adventures[index].title);
}

void Page_SelectAdventure::SetCategory(Category category)
{
    for (u32 i = 0; i < std::size(m_adventures); i++) {
        bool enable = m_adventures[i].category == category;
        m_adventures[i].button.setVisible(enable);
        m_adventures[i].button.setClickable(enable);
        m_adventures[i].title.setVisible(enable);
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

    button->resetState();
    sys()->HidePage(System::GetPageID(this), System::Display::All);
    sys()->ShowPage(System::GetPageID<Page_ModeSelect>(), System::Display::DRC);
}