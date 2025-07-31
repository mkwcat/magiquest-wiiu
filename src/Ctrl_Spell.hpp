// Ctrl_Spell.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Ctrl_Image.hpp"
#include <cstring>
#include <gctypes.h>
#include <gui/GuiButton.h>
#include <vector>

class Ctrl_Spell : public GuiButton, public sigslot::has_slots<>
{
public:
    Ctrl_Spell()
      : GuiButton(180, 180)
    {
        clickable = true;
        selectable = true;
        setTrigger(&m_touchTrigger);
        clicked.connect<Ctrl_Spell>(this, &Ctrl_Spell::OnHover);
        pointedOff.connect<Ctrl_Spell>(this, &Ctrl_Spell::OnReleased);
    }

    ~Ctrl_Spell()
    {
    }

    void Init(const char* name, const char* root, const char* const* images, u32 imageCount)
    {
        assert(strlen(name) < 32);
        strcpy(m_name, name);

        // Construct loop
        for (u32 i = 0; i < imageCount; i++) {
            m_images.push_back(Ctrl_Image());
        }

        // Init loop
        for (u32 i = 0; i < imageCount; i++) {
            char path[256];
            snprintf(path, sizeof(path), "%s/%s.png", root, images[i]);
            m_images[i].Load(path);
        }
    }

    void SetImages(u32 hoveredImg, u32 notHoveredImg, u32 disabledImg)
    {
        assert(hoveredImg < m_images.size());
        assert(notHoveredImg < m_images.size());
        assert(disabledImg < m_images.size());

        m_hoveredImg = hoveredImg;
        m_notHoveredImg = notHoveredImg;
        m_disabledImg = disabledImg;

        SetImage(m_notHoveredImg);
    }

    void SetOnHoverHandler(std::function<void(Ctrl_Spell*)> handler)
    {
        m_onHoverHandler = handler;
    }

    void SetOnReleaseHandler(std::function<void(Ctrl_Spell*)> handler)
    {
        m_onReleaseHandler = handler;
    }

    void Hover()
    {
        SetImage(m_hoveredImg);
    }

    void Unhover()
    {
        SetImage(m_notHoveredImg);
    }

    bool IsHovered() const
    {
        return m_currentImg == m_hoveredImg;
    }

    void SetHoverable(bool value)
    {
        bool oldClickable = clickable;
        clickable = value;
        selectable = value;

        if (!clickable) {
            SetImage(m_disabledImg);
            clearState(STATE_OVER);
        } else if (!oldClickable) {
            SetImage(m_notHoveredImg);
        }
    }

    bool IsHoverable() const
    {
        return clickable;
    }

    const char* GetName() const
    {
        return m_name;
    }

    /**
     * Called when the user touches the icon.
     */
    void OnHover(GuiButton* button, const GuiController* controller, GuiTrigger* trigger)
    {
        if (!clickable) {
            return;
        }

        m_onHoverHandler(this);
    }

    /**
     * Called when the user stops touching the icon.
     */
    void OnReleased(GuiButton* button, const GuiController* controller)
    {
        if (!clickable) {
            return;
        }

        m_onReleaseHandler(this);
    }

    Ctrl_Image& GetImage(u32 index)
    {
        assert(index < m_images.size());
        return m_images[index];
    }

    void SetImage(u32 index)
    {
        assert(index < m_images.size());
        m_currentImg = index;
        setImage(&m_images[index]);
    }

private:
    // Spell name
    char m_name[32];

    std::vector<Ctrl_Image> m_images;

    GuiTrigger m_touchTrigger{GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH};

    u32 m_hoveredImg;
    u32 m_notHoveredImg;
    u32 m_disabledImg;
    u32 m_currentImg = 0;

    std::function<void(Ctrl_Spell*)> m_onHoverHandler;
    std::function<void(Ctrl_Spell*)> m_onReleaseHandler;
};
