#pragma once

#include <cassert>
#include <gctypes.h>
#include <gui/GuiImage.h>

class Ctrl_Mana : public GuiImage
{
public:
    Ctrl_Mana()
      : GuiImage(nullptr)
    {
    }

    enum {
        Left = 0, // Player
        Right = 1, // Opponent
    };

    void Update(u8 side, u8 mana);
};
