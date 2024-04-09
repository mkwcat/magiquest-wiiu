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
        if (m_initialized) {
            return;
        }

        LoadImage(
          m_statusStationImage, RES_ROOT "/Image/Background/GameStation.jpg");
        LoadImage(
          m_touchDuelXavierImage, RES_ROOT "/Image/Background/TouchduelXavier.jpg");
        LoadImage(m_touchDuelGolemImage,
          RES_ROOT "/Image/Background/TouchduelXavier.jpg");

        SetImage(ImageType::StatusStation);

        m_initialized = true;
    }

    enum class ImageType {
        StatusStation,
        TouchDuelXavier,
        TouchDuelGolem,
    };

    void SetImage(ImageType type)
    {
        m_statusStationImage.setVisible(type == ImageType::StatusStation);
        m_touchDuelXavierImage.setVisible(type == ImageType::TouchDuelXavier);
        m_touchDuelGolemImage.setVisible(type == ImageType::TouchDuelGolem);
    }

private:
    bool m_initialized = false;

    Ctrl_Image m_statusStationImage;
    Ctrl_Image m_touchDuelXavierImage;
    Ctrl_Image m_touchDuelGolemImage;
};
