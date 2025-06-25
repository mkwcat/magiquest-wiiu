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
  : m_encounterButton{
      {300, 375},
      {300, 375},
      {300, 375},
      {300, 375},
      {300, 375},
      {300, 375},
    }
{
    m_title.Load(RES_ROOT "/Image/Menu/Title/ModeSelect.png");
    m_title.setPosition(0, 370);
    m_title.setScaleX(1.8);
    m_title.setScaleY(1.8);

    append(&m_title);

    for (int i = 0; i < 6; i++) {
        m_encounterBanner[i].Load(RES_ROOT "/Image/Menu/Button/Encounter/Frame.png");
        m_encounterButton[i].setImage(&m_encounterBanner[i]);
        m_encounterButton[i].setPosition((i % 3 - 1) * 400, ((i / 3) * -400));
        m_encounterButton[i].setTrigger(&m_touchTrigger);
        m_encounterButton[i].clicked.connect<Page_SelectAdventure>(
          this, &Page_SelectAdventure::OnSelect);
    }

    RegisterEncounter(0, "Dragon", Page_TouchDuel::EncounterType::Dragon);
    RegisterEncounter(1, "Silver Dragon", Page_TouchDuel::EncounterType::SilverDragon);
    RegisterEncounter(2, "Xavier", Page_TouchDuel::EncounterType::Xavier);
    RegisterEncounter(3, "Golem", Page_TouchDuel::EncounterType::Golem);
}

void Page_SelectAdventure::process()
{
    if (sys()->GetDisplay(sys()->GetPageID(this)) != System::Display::None) {
        Page_Background* background = sys()->GetPageStatic<Page_Background>();
        assert(background != nullptr);

        background->SetImage(Page_Background::ImageType::StatusStation);
    }
}

void Page_SelectAdventure::RegisterEncounter(
  int index, const char* name, Page_TouchDuel::EncounterType encounter)
{
    assert(index >= 0 && index < 6);
    m_encounterType[index] = encounter;
    append(&m_encounterButton[index]);
}

void Page_SelectAdventure::OnSelect(
  GuiButton* button, const GuiController* controller, GuiTrigger* trigger)
{
    Page_TouchDuel* setting = sys()->GetPageStatic<Page_TouchDuel>();

    for (int i = 0; i < 6; i++) {
        if (button == &m_encounterButton[i]) {
            setting->LoadEncounter(m_encounterType[i]);
            break;
        }
    }

    sys()->HidePage(System::GetPageID(this), System::Display::All);
    sys()->ShowPage(System::GetPageID<Page_ModeSelect>(), System::Display::DRC);
}