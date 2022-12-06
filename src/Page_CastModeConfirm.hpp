// Page_CastModeConfirm.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "System.hpp"
#include "WandHandler.hpp"
#include <gui/GuiButton.h>
#include <gui/GuiFrame.h>
#include <gui/GuiImage.h>

class Page_CastModeConfirm : public GuiFrame,
                             public WandHandler,
                             public sigslot::has_slots<>
{
public:
    Page_CastModeConfirm();

private:
    void Init();

public:
    /**
     * GuiElement process.
     */
    void process() override;

    /**
     * Cast from WandHandler.
     */
    virtual void Cast(Wand::CastMode castMode, bool curValid, float curX,
      float curY, float curZ) override;

private:
    bool m_initialized = false;

    GuiImage m_title;
    GuiImageData m_titleData;

    GuiImage m_instruction;
    GuiImageData m_instructionData;

    GuiImage m_imgCheck;
    GuiImageData m_imgCheckData;
};
