// Page_SelectAdventureCategory.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_SelectAdventureCategory.hpp"
#include "Page_Background.hpp"
#include "Page_SelectAdventure.hpp"

Page_SelectAdventureCategory::Page_SelectAdventureCategory()
{
    append(&m_title);

    InitButton(m_bannerMagiAdventure, m_btnMagiAdventure,
      RES_ROOT "/Image/Menu/Button/Banner/MagiAdventure.png", -375, 130);

    InitButton(m_bannerMasterMagiAdventure, m_btnMasterMagiAdventure,
      RES_ROOT "/Image/Menu/Button/Banner/MasterMagiAdventure.png", 375, 130);

    InitButton(m_bannerPortalAdventure, m_btnPortalAdventure,
      RES_ROOT "/Image/Menu/Button/Banner/PortalAdventure.png", -375, -220);

    InitButton(m_bannerHeroicAdventure, m_btnHeroicAdventure,
      RES_ROOT "/Image/Menu/Button/Banner/HeroicAdventure.png", 375, -220);
}

void Page_SelectAdventureCategory::InitButton(
  Ctrl_Image& banner, GuiButton& button, const char* imagePath, float posX, float posY)
{
    banner.Load(imagePath);
    banner.setScaleX(300 / banner.getHeight());
    banner.setScaleY(300 / banner.getHeight());

    button.setImage(&banner);
    button.setPosition(posX, posY);
    button.setTrigger(&m_touchTrigger);
    button.clicked.connect<Page_SelectAdventureCategory>(
      this, &Page_SelectAdventureCategory::OnSelect);
    append(&button);
}

void Page_SelectAdventureCategory::process()
{
    if (sys()->GetDisplay(sys()->GetPageID(this)) != System::Display::None) {
        Page_Background* background = sys()->GetPageStatic<Page_Background>();
        assert(background != nullptr);

        background->SetImage(Page_Background::ImageType::StatusStation);
    }
}

void Page_SelectAdventureCategory::OnSelect(
  GuiButton* button, const GuiController* controller, GuiTrigger* trigger)
{
    auto* adventurePage = sys()->GetPageStatic<Page_SelectAdventure>();

    if (button == &m_btnMagiAdventure) {
        adventurePage->SetCategory(Page_SelectAdventure::Category::Magi);
    } else if (button == &m_btnMasterMagiAdventure) {
        adventurePage->SetCategory(Page_SelectAdventure::Category::MasterMagi);
    } else if (button == &m_btnPortalAdventure) {
        adventurePage->SetCategory(Page_SelectAdventure::Category::Portal);
    } else if (button == &m_btnHeroicAdventure) {
        adventurePage->SetCategory(Page_SelectAdventure::Category::Heroic);
    }

    button->resetState();
    sys()->HidePage(System::GetPageID(this), System::Display::All);
    sys()->ShowPage(System::GetPageID<Page_SelectAdventure>(), System::Display::DRC);
}
