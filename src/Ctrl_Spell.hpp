// Ctrl_Spell.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Util.hpp"
#include <cstring>
#include <functional>
#include <gctypes.h>
#include <gui/GuiButton.h>
#include <utility>
#include <vector>

class Ctrl_Spell : public GuiButton, public sigslot::has_slots<>
{
public:
    Ctrl_Spell()
      : GuiButton(180, 180)
    {
        m_selectable = true;
        setTrigger(&m_touchTrigger);
        clicked.connect<Ctrl_Spell>(this, &Ctrl_Spell::OnClick);
        released.connect<Ctrl_Spell>(this, &Ctrl_Spell::OnReleased);
    }

    ~Ctrl_Spell()
    {
    }

    void Init(const char* name, const char** images, u32 imageCount)
    {
        assert(strlen(name) < 32);
        strcpy(m_name, name);

        // Construct loop
        for (u32 i = 0; i < imageCount; i++) {
            m_images.push_back(
              std::make_pair(GuiImageData(), GuiImage(nullptr)));
        }

        // Init loop
        for (u32 i = 0; i < imageCount; i++) {
            m_images[i].first.loadImageFromFile(images[i],
              GX2_TEX_CLAMP_MODE_CLAMP, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8);
            m_images[i].second.setImageData(&m_images[i].first);
        }
    }

    void SetImages(u32 selectedImg, u32 notSelectedImg, u32 notSelectableImg)
    {
        assert(selectedImg < m_images.size());
        assert(notSelectedImg < m_images.size());
        assert(notSelectableImg < m_images.size());

        m_selectedImg = selectedImg;
        m_notSelectedImg = notSelectedImg;
        m_notSelectableImg = notSelectableImg;

        setImage(&m_images[m_notSelectedImg].second);

        holdable = true;
        clickable = true;
    }

    void SetOnSelectHandler(std::function<void(Ctrl_Spell*)> handler)
    {
        m_onSelectHandler = handler;
    }

    void SetOnReleaseHandler(std::function<void(Ctrl_Spell*)> handler)
    {
        m_onReleaseHandler = handler;
    }

    void Select()
    {
        setImage(&m_images[m_selectedImg].second);
        m_selected = true;
    }

    void Deselect()
    {
        setImage(&m_images[m_notSelectedImg].second);
        m_selected = false;
    }

    bool IsSelected() const
    {
        return m_selected;
    }

    void SetSelectable(bool value)
    {
        m_selectable = value;

        if (!m_selectable) {
            setImage(&m_images[m_notSelectableImg].second);
            m_selected = false;
        } else {
            setImage(&m_images[m_notSelectedImg].second);
            m_selected = false;
        }
    }

    bool IsSelectable() const
    {
        return m_selectable;
    }

    const char* GetName() const
    {
        return m_name;
    }

    /**
     * Called when the user touches the icon.
     */
    void OnClick(
      GuiButton* button, const GuiController* controller, GuiTrigger* trigger)
    {
        if (!m_selectable)
            return;

        Select();
        m_onSelectHandler(this);
    }

    /**
     * Called when the user stops touching the icon.
     */
    void OnReleased(
      GuiButton* button, const GuiController* controller, GuiTrigger* trigger)
    {
        m_onReleaseHandler(this);
    }

private:
    // Spell name
    char m_name[32];

    std::vector<std::pair<GuiImageData, GuiImage>> m_images;

    GuiTrigger m_touchTrigger{GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH};

    u32 m_selectedImg;
    u32 m_notSelectedImg;
    u32 m_notSelectableImg;

    bool m_selected;
    bool m_selectable;

    std::function<void(Ctrl_Spell*)> m_onSelectHandler;
    std::function<void(Ctrl_Spell*)> m_onReleaseHandler;
};
