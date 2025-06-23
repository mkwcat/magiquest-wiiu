#pragma once

#include "ResourceManager.hpp"
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
        ResourceManager::GetImage(&m_data, path);
        setImageData(&m_data);
        assert(m_data.getTexture() != nullptr);
    }

private:
    GuiImageData m_data = {};
};
