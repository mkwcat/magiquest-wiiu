// Page_CastModeConfirm.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_CastModeConfirm.hpp"
#include "Encounter.hpp"
#include "Page_Background.hpp"

void Page_CastModeConfirm::Init()
{
    m_title.Load(RES_ROOT "/Image/mqTitleCastDuel.png");
    m_title.setPosition(0, 370);
    m_title.setScaleX(1.8);
    m_title.setScaleY(1.8);

    append(&m_title);

    m_instruction.Load(RES_ROOT "/Image/mqInstructionCastDuel.png");
    m_instruction.setPosition(0, 20);
    m_instruction.setScaleX(1.5);
    m_instruction.setScaleY(1.5);

    append(&m_instruction);

    m_imgCheck.Load(RES_ROOT "/Image/icWaxSeal-Green-Check.png");
    m_imgCheck.setPosition(550, -280);
    m_imgCheck.setScaleX(2.0);
    m_imgCheck.setScaleY(2.0);

    append(&m_imgCheck);
}

void Page_CastModeConfirm::process()
{
    if (!m_initialized) {
        Init();
        m_initialized = true;
    }
}

void Page_CastModeConfirm::Cast(
  Wand::CastMode castMode, bool curValid, float curX, float curY, float curZ)
{
    if (curValid && curX >= 380.0 && curX <= 700.0 && curY <= -110.0 &&
        curY >= -410.0) {
        System::s_instance->HidePage(
          System::GetPageID(this), System::Display::All);
        System::s_instance->HidePage(
          System::GetPageID<Page_Background>(), System::Display::All);
        System::s_instance->ShowPage(
          System::GetPageID<Encounter>(), System::Display::DRC);
    }
}
