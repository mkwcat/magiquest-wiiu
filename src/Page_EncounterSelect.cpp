// Page_EncounterSelect.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_EncounterSelect.hpp"
#include "Page_DuelDragon.hpp"
#include "Page_DuelGolem.hpp"
#include "Page_DuelXavier.hpp"
#include "Page_ModeSelect.hpp"
#include "System.hpp"

Page_EncounterSelect::Page_EncounterSelect()
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
        m_encounterButton[i].clicked.connect<Page_EncounterSelect>(
          this, &Page_EncounterSelect::OnSelect);
    }

    RegisterEncounter(0, "Dragon", new Page_DuelDragon());
    RegisterEncounter(1, "Xavier", new Page_DuelXavier());
    RegisterEncounter(2, "Golem", new Page_DuelGolem());
}

void Page_EncounterSelect::process()
{
}

void Page_EncounterSelect::RegisterEncounter(int index, const char* name, GuiElement* encounter)
{
    assert(index >= 0 && index < 6);
    assert(m_encounter[index] == nullptr);

    m_encounter[index] = encounter;
    encounter->process();
    append(&m_encounterButton[index]);
}

void Page_EncounterSelect::OnSelect(
  GuiButton* button, const GuiController* controller, GuiTrigger* trigger)
{
    System::PageSetting* setting = sys()->GetNextSetting(System::PageID::TouchDuel);

    for (int i = 0; i < 6; i++) {
        if (button == &m_encounterButton[i]) {
            setting->element = m_encounter[i];
            break;
        }
    }

    sys()->HidePage(System::GetPageID(this), System::Display::All);
    sys()->ShowPage(System::GetPageID<Page_ModeSelect>(), System::Display::DRC);
}