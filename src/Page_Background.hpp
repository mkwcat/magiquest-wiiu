// Page_Background.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "System.hpp"
#include <gui/GuiFrame.h>
#include <gui/GuiImage.h>

class Page_Background : public GuiFrame
{
public:
    Page_Background()
      : m_bgImg(nullptr)
    {
    }

    void process()
    {
        if (!m_initialized) {
            {
                Lock l(sys()->FileMutex());
                m_bgImgData.loadImageFromFile(
                  RES_ROOT "/Image/sgStone-Tasks.jpg", GX2_TEX_CLAMP_MODE_CLAMP,
                  GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8);
            }
            m_bgImg.setImageData(&m_bgImgData);

            m_bgImg.setScaleX(1920 / m_bgImg.getWidth());
            m_bgImg.setScaleY(1080 / m_bgImg.getHeight());

            append(&m_bgImg);

            m_initialized = true;
        }
    }

private:
    bool m_initialized = false;

    GuiImage m_bgImg;
    GuiImageData m_bgImgData;
};
