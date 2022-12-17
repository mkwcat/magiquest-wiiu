// System.hpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Util.hpp"
#include "Wand.hpp"
#include <array>
#include <gctypes.h>
#include <gui/GuiElement.h>
#include <gui/GuiFrame.h>
#include <gui/GuiImage.h>
#include <gui/GuiImageData.h>
#include <gui/VPadController.h>
#include <gui/system/CThread.h>
#include <vector>

class CVideo;

class System : public CThread
{
public:
    static System* s_instance;

    System();
    ~System();

    enum class PageID {
        Movie,
        Background,
        ModeSelect,
        CastModeConfirm,
        TouchDuel,

        PageCount,
    };
    static constexpr u32 PageCount = u32(PageID::PageCount);

    enum class Display {
        DRC = 0,
        TV = 1,
        All = 2,
    };

    struct PageSetting {
        PageID id;
        bool tv;
        bool drc;
        GuiElement* element;
    };

    void Start();

    static u32 CallbackAcquire(void* context);
    static u32 CallbackRelease(void* context);

    /**
     * Load an entire file into memory.
     */
    static void* RipFile(const char* path, u32* length = nullptr);

    /**
     * Returns true if the current environment is aroma.
     */
    static bool IsAroma();

    /**
     * Enable a page on the specified display.
     */
    void ShowPage(PageID page, Display display);

    /**
     * Disable a page from the specified display.
     */
    void HidePage(PageID page, Display display);

    /**
     * Get a page GuiElement.
     */
    GuiElement* GetPage(PageID page);

    /**
     * Simplified GetPage.
     */
    template <class T>
    static T* GetPageStatic()
    {
        assert(s_instance != nullptr);
        T* page;

        // Find the page with the specified type
        for (u32 i = 0; i < u32(PageID::PageCount); i++) {
            if ((page = dynamic_cast<T*>(s_instance->m_pages[i].element)) !=
                nullptr) {
                return page;
            }
        }

        return nullptr;
    }

    /**
     * Get the page ID from the object.
     */
    template <class T>
    static PageID GetPageID()
    {
        assert(s_instance != nullptr);
        T* page;

        // Find the page with the specified type
        for (u32 i = 0; i < u32(PageID::PageCount); i++) {
            if ((page = dynamic_cast<T*>(s_instance->m_pages[i].element)) !=
                nullptr) {
                return PageID(i);
            }
        }

        return PageID::PageCount;
    }

    /**
     * Get the page ID from the object.
     */
    template <class T>
    static PageID GetPageID(T* obj)
    {
        return GetPageID<T>();
    }

    /**
     * Get the page setting struct.
     */
    PageSetting* GetSetting(PageID page)
    {
        return &m_pages[u32(page)];
    }

    /**
     * Get the file access mutex.
     */
    OSMutex& FileMutex()
    {
        return m_fileMutex;
    }

    /**
     * Call this every frame.
     */
    bool Tick();

    /**
     * Wait for vertical sync.
     */
    void WaitVSync();

    /**
     * Get the wand object.
     */
    Wand* GetWand();

    /**
     * Get the current frame ID.
     */
    u32 GetFrameID() const
    {
        return m_frameId;
    }

    /**
     * Thread entry point.
     */
    void executeThread() override;

protected:
    CVideo* m_video;

    VPadController m_gamepad;

    GuiImageData m_imgCursorData;
    GuiImage m_imgCursor;
    s32 m_imgCursorTimer;
    Wand m_wand;

    OSMutex m_fileMutex;

    PageSetting m_pages[PageCount];

    u32 m_frameId;
};

static inline System* sys()
{
    return System::s_instance;
}
