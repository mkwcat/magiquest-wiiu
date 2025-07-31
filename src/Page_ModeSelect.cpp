// Page_ModeSelect.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_ModeSelect.hpp"
#include "Page_CastTutorial.hpp"
#include "Page_TouchDuel.hpp"

Page_ModeSelect::Page_ModeSelect()
  : m_btnTouchDuel(520, 200)
  , m_btnCastDuel(520, 200)
  , m_btnRealWand(520, 200)
{
    m_btnBack.InitDefault(Ctrl_CommonButton::Type::LeftBack);
    m_btnBack.SetOnSelectGotoPage(System::PageID::ModeSelect, System::PageID::SelectAdventure);
    append(&m_btnBack);

    append(&m_title);

    m_bannerTouchDuel.Load(RES_ROOT "/Image/Menu/Button/Banner/TouchDuel.png");
    m_bannerTouchDuel.setScaleX(300 / m_bannerTouchDuel.getHeight());
    m_bannerTouchDuel.setScaleY(300 / m_bannerTouchDuel.getHeight());
    m_btnTouchDuel.setImage(&m_bannerTouchDuel);
    m_btnTouchDuel.setPosition(-375, 130);
    m_btnTouchDuel.setTrigger(&m_touchTrigger);
    m_btnTouchDuel.clicked.connect<Page_ModeSelect>(this, &Page_ModeSelect::OnSelectTouchDuel);

    append(&m_btnTouchDuel);

    m_bannerCastDuel.Load(RES_ROOT "/Image/Menu/Button/Banner/CastDuel.png");
    m_bannerCastDuel.setScaleX(300 / m_bannerCastDuel.getHeight());
    m_bannerCastDuel.setScaleY(300 / m_bannerCastDuel.getHeight());
    m_btnCastDuel.setImage(&m_bannerCastDuel);
    m_btnCastDuel.setPosition(375, 130);
    m_btnCastDuel.setTrigger(&m_touchTrigger);
    m_btnCastDuel.clicked.connect<Page_ModeSelect>(this, &Page_ModeSelect::OnSelectCastDuel);

    append(&m_btnCastDuel);

    m_bannerRealWand.Load(RES_ROOT "/Image/Menu/Button/Banner/RealWand.png");
    m_bannerRealWand.setScaleX(300 / m_bannerRealWand.getHeight());
    m_bannerRealWand.setScaleY(300 / m_bannerRealWand.getHeight());
    m_btnRealWand.setImage(&m_bannerRealWand);
    m_btnRealWand.setPosition(0, -220);
    m_btnRealWand.setTrigger(&m_touchTrigger);
    m_btnRealWand.clicked.connect<Page_ModeSelect>(this, &Page_ModeSelect::OnSelectRealWand);

    append(&m_btnRealWand);
}

void Page_ModeSelect::process()
{
}

void Page_ModeSelect::OnSelectTouchDuel(
  GuiButton* button, const GuiController* controller, GuiTrigger* trigger)
{
    LOG(LogSystem, "Select Touch Duel");

    sys()->GetWand()->SetCastMode(Wand::CastMode::WiiRemoteTouchRune);

    button->resetState();
    sys()->HidePage(System::GetPageID(this), System::Display::All);
    System::GetPageStatic<Page_TouchDuel>()->Transition();
    sys()->ShowPage(System::GetPageID<Page_TouchDuel>(), System::Display::DRC);
}

void Page_ModeSelect::OnSelectCastDuel(
  GuiButton* button, const GuiController* controller, GuiTrigger* trigger)
{
    LOG(LogSystem, "Select Cast Duel");

    sys()->GetWand()->SetCastMode(Wand::CastMode::WiiRemoteCastRune);

    button->resetState();
    sys()->HidePage(System::GetPageID(this), System::Display::All);
    sys()->ShowPage(System::GetPageID<Page_CastTutorial>(), System::Display::DRC);
}

void Page_ModeSelect::OnSelectRealWand(
  GuiButton* button, const GuiController* controller, GuiTrigger* trigger)
{
    LOG(LogSystem, "Select Real Wand");

    sys()->GetWand()->SetCastMode(Wand::CastMode::RealWand);

    button->resetState();
    sys()->HidePage(System::GetPageID(this), System::Display::All);
    System::GetPageStatic<Page_TouchDuel>()->Transition();
    sys()->ShowPage(System::GetPageID<Page_TouchDuel>(), System::Display::DRC);
}
