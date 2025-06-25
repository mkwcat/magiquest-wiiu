// Page_Background.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Ctrl_Image.hpp"
#include "System.hpp"
#include <gui/GuiFrame.h>

class Page_Background : public GuiFrame
{
private:
    void LoadImage(Ctrl_Image& image, const char* path)
    {
        image.Load(path);
        image.setScaleX(1920 / image.getWidth());
        image.setScaleY(1080 / image.getHeight());
        append(&image);
    }

public:
    void process()
    {
        GuiFrame::process();

        if (m_initialized) {
            return;
        }

        LoadImage(m_statusStationImage, RES_ROOT "/Image/Background/GameStation.jpg");
        LoadImage(m_touchDuelXavierImage, RES_ROOT "/Image/Background/TouchduelXavier.jpg");
        LoadImage(
          m_touchDuelBlizzardDawnImage, RES_ROOT "/Image/Background/TouchduelBlizzardDawn.jpg");

        SetImage(ImageType::StatusStation);

        m_initialized = true;
    }

    enum class ImageType {
        StatusStation,
        TouchDuelXavier,
        TouchDuelBlizzardDawn,
    };

    void SetImage(ImageType type)
    {
        m_statusStationImage.setVisible(type == ImageType::StatusStation);
        m_touchDuelXavierImage.setVisible(type == ImageType::TouchDuelXavier);
        m_touchDuelBlizzardDawnImage.setVisible(type == ImageType::TouchDuelBlizzardDawn);

        GuiFrame::process();
    }

private:
    bool m_initialized = false;

    Ctrl_Image m_statusStationImage;
    Ctrl_Image m_touchDuelXavierImage;
    Ctrl_Image m_touchDuelBlizzardDawnImage;
};
