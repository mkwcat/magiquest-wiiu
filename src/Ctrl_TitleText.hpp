#pragma once

#include "Ctrl_Image.hpp"

class Ctrl_TitleText : public Ctrl_Image
{
public:
    Ctrl_TitleText(const char* name)
    {
        char path[256];
        std::snprintf(path, sizeof(path), RES_ROOT "/Image/Menu/Title/%s.png", name);
        Load(path);

        setPosition(0, 370);
        setScaleX(DRC_PIXEL_SCALE);
        setScaleY(DRC_PIXEL_SCALE);
    }
};