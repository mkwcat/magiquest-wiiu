// Page_CastTutorial.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_CastTutorial.hpp"
#include "Ctrl_CommonButton.hpp"
#include "Page_TouchDuel.hpp"
#include "Util.hpp"

void Page_CastTutorial::Init()
{
    m_btnBack.InitDefault(Ctrl_CommonButton::Type::LeftBack);
    m_btnBack.SetOnSelectGotoPage(System::GetPageID(this), System::PageID::ModeSelect);

    append(&m_btnBack);

    append(&m_title);

    m_instruction[0].Load(RES_ROOT "/Image/Menu/Title/CastDuelInstructionLine1.png");
    m_instruction[0].setPosition(0, 150);
    m_instruction[0].setScaleX(DRC_PIXEL_SCALE);
    m_instruction[0].setScaleY(DRC_PIXEL_SCALE);

    append(&m_instruction[0]);

    m_instruction[1].Load(RES_ROOT "/Image/Menu/Title/CastDuelInstructionLine2.png");
    m_instruction[1].setPosition(0, 50);
    m_instruction[1].setScaleX(DRC_PIXEL_SCALE);
    m_instruction[1].setScaleY(DRC_PIXEL_SCALE);

    append(&m_instruction[1]);

    m_instruction[2].Load(RES_ROOT "/Image/Menu/Title/CastDuelInstructionLine3.png");
    m_instruction[2].setPosition(0, -50);
    m_instruction[2].setScaleX(DRC_PIXEL_SCALE);
    m_instruction[2].setScaleY(DRC_PIXEL_SCALE);

    append(&m_instruction[2]);

    m_instruction[3].Load(RES_ROOT "/Image/Menu/Title/CastDuelInstructionLine4.png");
    m_instruction[3].setPosition(0, -150);
    m_instruction[3].setScaleX(DRC_PIXEL_SCALE);
    m_instruction[3].setScaleY(DRC_PIXEL_SCALE);

    append(&m_instruction[3]);

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
