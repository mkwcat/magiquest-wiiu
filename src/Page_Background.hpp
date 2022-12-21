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
public:
    void process()
    {
        if (!m_initialized) {
            m_bgImg.Load(RES_ROOT "/Image/sgStone-Tasks.jpg");
            m_bgImg.setScaleX(1920 / m_bgImg.getWidth());
            m_bgImg.setScaleY(1080 / m_bgImg.getHeight());

            append(&m_bgImg);

            m_initialized = true;
        }
    }

private:
    bool m_initialized = false;

    Ctrl_Image m_bgImg;
};
