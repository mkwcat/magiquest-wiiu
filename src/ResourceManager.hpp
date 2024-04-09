#pragma once

#include "System.hpp"
#include "Util.hpp"
#include <gui/GuiImageData.h>

class ResourceManager
{
public:
    static void Init()
    {
        for (u32 i = 0; i < 17; i++) {
            Lock l(sys()->FileMutex());

            char path[64];
            snprintf(path, 64, RES_ROOT "/Image/Encounter/Mana/%02u.png", i);

            s_manaImage[i].loadImageFromFile(path, GX2_TEX_CLAMP_MODE_CLAMP,
              GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8);
        }
    }

    static GuiImageData* GetImage(GuiImageData* image, const char* path,
      GX2TexClampMode clampMode = GX2_TEX_CLAMP_MODE_CLAMP,
      GX2SurfaceFormat format = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8)
    {
        Lock l(sys()->FileMutex());
        image->loadImageFromFile(path, clampMode, format);
        return image;
    }

    static GuiImageData* GetManaImage(u8 value)
    {
        if (value > 16) {
            return nullptr;
        }

        return &s_manaImage[value];
    }

protected:
    static GuiImageData s_manaImage[17];
};