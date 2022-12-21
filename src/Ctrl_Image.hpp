#pragma once

#include "System.hpp"
#include <gui/GuiImage.h>
#include <gui/GuiImageData.h>

class Ctrl_Image : public GuiImage
{
public:
    Ctrl_Image()
      : GuiImage(nullptr)
    {
    }

    void Load(const char* path)
    {
        Lock l(sys()->FileMutex());

        m_data.loadImageFromFile(
          path, GX2_TEX_CLAMP_MODE_CLAMP, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8);
        setImageData(&m_data);
    }

private:
    GuiImageData m_data;
};
