#pragma once

#include "Ctrl_Image.hpp"
#include "System.hpp"
#include <functional>
#include <gui/GuiButton.h>

class Ctrl_CommonButton : public GuiButton, public sigslot::has_slots<>
{
public:
    Ctrl_CommonButton()
      : GuiButton(180, 180)
    {
    }

    virtual ~Ctrl_CommonButton() = default;

    enum class Type {
        LeftBack,
        LeftCancel,
        MiddleMenu,
    };

    void InitDefault(Type type)
    {
        switch (type) {
        case Type::LeftBack:
            m_image.Load(RES_ROOT "/Image/Menu/Button/Common/LeftBack.png");
            break;
        case Type::LeftCancel:
            m_image.Load(RES_ROOT "/Image/Menu/Button/Common/LeftCancel.png");
            break;
        case Type::MiddleMenu:
            m_image.Load(RES_ROOT "/Image/Menu/Button/Common/MiddleMenu.png");
            break;
        }
        setImage(&m_image);
        setTrigger(&m_touchTrigger);
        holdable = true;
        clickable = true;

        m_image.setSize(float(m_image.getWidth()) / float(m_image.getHeight()) * 180.0, 180.0);

        switch (type) {
        case Type::LeftBack:
        case Type::LeftCancel:
            setPosition(-(1920.0 / 2) + 128, -(1080.0 / 2) + 88);
            setAlignment(ALIGN_LEFT | ALIGN_BOTTOM);
            break;

        case Type::MiddleMenu:
            setPosition(0, -(1080.0 / 2) + 88);
            setAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
            break;
        }

        clicked.connect<Ctrl_CommonButton>(this, &Ctrl_CommonButton::OnClick);
        released.connect<Ctrl_CommonButton>(this, &Ctrl_CommonButton::OnReleased);
    }

    void SetOnSelectHandler(std::function<void(Ctrl_CommonButton*)> handler)
    {
        m_onSelectHandler = handler;
    }

    void SetOnReleaseHandler(std::function<void(Ctrl_CommonButton*)> handler)
    {
        m_onReleaseHandler = handler;
    }

    void SetOnSelectGotoPage(System::PageID hidePage, System::PageID showPage)
    {
        m_onSelectHandler = [hidePage, showPage](Ctrl_CommonButton* button) {
            System::Display display = sys()->GetDisplay(hidePage);
            sys()->HidePage(hidePage, System::Display::All);
            sys()->ShowPage(showPage, display);
        };
    }

private:
    void OnClick(GuiButton* button, const GuiController* controller, GuiTrigger* trigger)
    {
        if (m_onSelectHandler) {
            m_onSelectHandler(this);
        }
    }

    void OnReleased(GuiButton* button, const GuiController* controller, GuiTrigger* trigger)
    {
        if (m_onReleaseHandler) {
            m_onReleaseHandler(this);
        }
    }

    std::function<void(Ctrl_CommonButton*)> m_onSelectHandler = nullptr;
    std::function<void(Ctrl_CommonButton*)> m_onReleaseHandler = nullptr;

    Ctrl_Image m_image;
    GuiTrigger m_touchTrigger{GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH};
};