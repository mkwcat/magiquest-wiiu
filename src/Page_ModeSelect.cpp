// Page_ModeSelect.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Page_ModeSelect.hpp"
#include "Encounter.hpp"
#include "Page_Background.hpp"
#include "Page_CastModeConfirm.hpp"
#include "Page_Movie.hpp"

Page_ModeSelect::Page_ModeSelect()
  : m_title(nullptr)
  , m_bannerTouchDuel(nullptr)
  , m_btnTouchDuel(520, 200)
  , m_bannerCastDuel(nullptr)
  , m_btnCastDuel(520, 200)
  , m_bannerRealWand(nullptr)
  , m_btnRealWand(520, 200)
{
}

void Page_ModeSelect::Init()
{
    m_titleData.loadImageFromFile(RES_ROOT "/Image/mqTitleSelectCastMode.png",
      GX2_TEX_CLAMP_MODE_CLAMP, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8);
    m_title.setImageData(&m_titleData);

    m_title.setPosition(0, 370);
    m_title.setScaleX(1.8);
    m_title.setScaleY(1.8);

    append(&m_title);

    m_bannerTouchDuelData.loadImageFromFile(RES_ROOT
      "/Image/mqBannerTouchDuel.png",
      GX2_TEX_CLAMP_MODE_CLAMP, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8);
    m_bannerTouchDuel.setImageData(&m_bannerTouchDuelData);

    m_bannerTouchDuel.setScaleX(300 / m_bannerTouchDuel.getHeight());
    m_bannerTouchDuel.setScaleY(300 / m_bannerTouchDuel.getHeight());
    m_btnTouchDuel.setImage(&m_bannerTouchDuel);
    m_btnTouchDuel.setPosition(-375, 130);
    m_btnTouchDuel.setTrigger(&m_touchTrigger);
    m_btnTouchDuel.clicked.connect<Page_ModeSelect>(
      this, &Page_ModeSelect::OnSelectTouchDuel);

    append(&m_btnTouchDuel);

    m_bannerCastDuelData.loadImageFromFile(RES_ROOT
      "/Image/mqBannerCastDuel.png",
      GX2_TEX_CLAMP_MODE_CLAMP, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8);
    m_bannerCastDuel.setImageData(&m_bannerCastDuelData);

    m_bannerCastDuel.setScaleX(300 / m_bannerCastDuel.getHeight());
    m_bannerCastDuel.setScaleY(300 / m_bannerCastDuel.getHeight());
    m_btnCastDuel.setImage(&m_bannerCastDuel);
    m_btnCastDuel.setPosition(375, 130);
    m_btnCastDuel.setTrigger(&m_touchTrigger);
    m_btnCastDuel.clicked.connect<Page_ModeSelect>(
      this, &Page_ModeSelect::OnSelectCastDuel);

    append(&m_btnCastDuel);

    m_bannerRealWandData.loadImageFromFile(RES_ROOT
      "/Image/mqBannerTouchDuelRealWand.png",
      GX2_TEX_CLAMP_MODE_CLAMP, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8);
    m_bannerRealWand.setImageData(&m_bannerRealWandData);

    m_bannerRealWand.setScaleX(300 / m_bannerRealWand.getHeight());
    m_bannerRealWand.setScaleY(300 / m_bannerRealWand.getHeight());
    m_btnRealWand.setImage(&m_bannerRealWand);
    m_btnRealWand.setPosition(0, -220);
    m_btnRealWand.setTrigger(&m_touchTrigger);
    m_btnRealWand.clicked.connect<Page_ModeSelect>(
      this, &Page_ModeSelect::OnSelectRealWand);

    append(&m_btnRealWand);
}

void Page_ModeSelect::process()
{
    if (!m_initialized) {
        Init();
        m_initialized = true;
    }

    GuiFrame::process();
}

void Page_ModeSelect::OnSelectTouchDuel(
  GuiButton* button, const GuiController* controller, GuiTrigger* trigger)
{
    LOG(LogSystem, "Select Touch Duel");

    System::s_instance->GetWand()->SetCastMode(
      Wand::CastMode::WiiRemoteTouchRune);

    System::s_instance->HidePage(System::GetPageID(this), System::Display::All);
    System::s_instance->HidePage(
      System::GetPageID<Page_Background>(), System::Display::All);
    System::s_instance->ShowPage(
      System::GetPageID<Encounter>(), System::Display::DRC);
}

void Page_ModeSelect::OnSelectCastDuel(
  GuiButton* button, const GuiController* controller, GuiTrigger* trigger)
{
    LOG(LogSystem, "Select Cast Duel");

    System::s_instance->GetWand()->SetCastMode(
      Wand::CastMode::WiiRemoteCastRune);

    System::s_instance->HidePage(System::GetPageID(this), System::Display::All);
    System::s_instance->ShowPage(
      System::GetPageID<Page_CastModeConfirm>(), System::Display::DRC);
}

void Page_ModeSelect::OnSelectRealWand(
  GuiButton* button, const GuiController* controller, GuiTrigger* trigger)
{
    LOG(LogSystem, "Select Real Wand");

    System::s_instance->GetWand()->SetCastMode(Wand::CastMode::RealWand);

    System::s_instance->HidePage(System::GetPageID(this), System::Display::All);
    System::s_instance->HidePage(
      System::GetPageID<Page_Background>(), System::Display::All);
    System::s_instance->ShowPage(
      System::GetPageID<Encounter>(), System::Display::DRC);
}
