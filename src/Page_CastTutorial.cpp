// Page_CastTutorial.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_CastTutorial.hpp"
#include "Ctrl_CommonButton.hpp"
#include "Page_TouchDuel.hpp"

void Page_CastTutorial::Init()
{
    m_btnBack.InitDefault(Ctrl_CommonButton::Type::LeftBack);
    m_btnBack.SetOnSelectGotoPage(System::GetPageID(this), System::PageID::ModeSelect);

    append(&m_btnBack);

    m_title.Load(RES_ROOT "/Image/Menu/Title/CastDuel.png");
    m_title.setPosition(0, 370);
    m_title.setScaleX(1.8);
    m_title.setScaleY(1.8);

    append(&m_title);

    m_instruction.Load(RES_ROOT "/Image/Menu/Title/CastDuelInstruction.png");
    m_instruction.setPosition(0, 20);
    m_instruction.setScaleX(1.5);
    m_instruction.setScaleY(1.5);

    append(&m_instruction);

    m_imgCheck.Load(RES_ROOT "/Image/Menu/Button/WaxSealGreenCheck.png");
    m_imgCheck.setPosition(550, -280);
    m_imgCheck.setSize(220, 220);

    append(&m_imgCheck);
}

void Page_CastTutorial::process()
{
    if (!m_initialized) {
        Init();
        m_initialized = true;
    }
}

void Page_CastTutorial::Cast(
  Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ)
{
    if (curValid && curX >= 380.0 && curX <= 700.0 && curY <= -110.0 && curY >= -410.0) {
        System::s_instance->HidePage(System::GetPageID(this), System::Display::All);
        System::GetPageStatic<Page_TouchDuel>()->Transition();
        sys()->ShowPage(System::GetPageID<Page_TouchDuel>(), System::Display::DRC);
    }
}
