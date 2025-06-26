// System.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "System.hpp"
#include "AXManager.hpp"
#include "Exception.hpp"
#include "ResourceManager.hpp"
#include "Wand.hpp"
#include <cassert>
#include <coreinit/core.h>
#include <coreinit/dynload.h>
#include <cstdlib>
#include <gui/memory.h>
#include <gui/video/CVideo.h>
#include <proc_ui/procui.h>
#include <whb/crash.h>
#include <whb/proc.h>

#include <whb/log.h>
#include <whb/log_cafe.h>
#if CONF_LOG_MASK != 0
#  include <whb/log_udp.h>
#endif

#include "Page_Background.hpp"
#include "Page_CastTutorial.hpp"
#include "Page_ModeSelect.hpp"
#include "Page_Projector.hpp"
#include "Page_SelectAdventure.hpp"
#include "Page_SelectAdventureCategory.hpp"
#include "Page_TouchDuel.hpp"

System* System::s_instance = nullptr;

System::System()
  : CThread(CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff, 0, 0x80000)
  , m_video(new CVideo(GX2_TV_SCAN_MODE_1080P))
  , m_gamepad(GuiTrigger::CHANNEL_1)
  , m_imgCursor(nullptr)
  , m_pages{// aggregate initialization of std::array
      PageSetting{PageID::Movie}, {PageID::Background}, {PageID::SelectAdventureCategory},
      {PageID::SelectAdventure}, {PageID::ModeSelect}, {PageID::CastTutorial}, {PageID::TouchDuel}}
{
    if (s_instance == nullptr) {
        s_instance = this;
    }

    LOG(LogSystem, "Starting application");

    OSInitMutexEx(&m_fileMutex, "System::m_fileMutex");

    m_wand = new Wand();

    // Initialize the resource manager
    ResourceManager::Init();

    // Construct page objects
    m_pages[u32(PageID::Movie)].element = new Page_Projector();
    m_pages[u32(PageID::Background)].element = new Page_Background();
    m_pages[u32(PageID::SelectAdventureCategory)].element = new Page_SelectAdventureCategory();
    m_pages[u32(PageID::SelectAdventure)].element = new Page_SelectAdventure();
    m_pages[u32(PageID::ModeSelect)].element = new Page_ModeSelect();
    m_pages[u32(PageID::CastTutorial)].element = new Page_CastTutorial();
    m_pages[u32(PageID::TouchDuel)].element = new Page_TouchDuel();

    m_imgCursorTimer = 0;
    m_frameId = 0;

    // Disable display
    m_video->tvEnable(false);
    m_video->drcEnable(false);
}

System::~System()
{
    LOG(LogSystem, "Leaving application");

    AXManager::s_instance->Shutdown();

    for (auto set : m_pages) {
        LOG(LogSystem, "Deleting page %d", static_cast<int>(set.id));
        delete set.element;
        LOG(LogSystem, "Deleted page %d", static_cast<int>(set.id));
        set.element = nullptr;
    }

    LOG(LogSystem, "Deleted all pages");

    if (s_instance == this) {
        s_instance = nullptr;
    }

    delete AXManager::s_instance;
    LOG(LogSystem, "Deleted AXManager");
    delete m_wand;

    LOG(LogSystem, "Successfully shutdown");
}

void System::Start()
{
    resumeThread();
    // This will wait until the thread exits.
    shutdownThread();
}

void System::executeThread()
{
    for (u32 i = 0; i < PageCount; i++) {
        m_nextSetting[i] = m_pages[i];
    }

    ShowPage(System::PageID::Movie, System::Display::TV);
    ShowPage(System::PageID::Background, System::Display::DRC);
    ShowPage(System::PageID::SelectAdventureCategory, System::Display::DRC);

    WaitVSync();

    {
        Lock l(sys()->FileMutex());
        m_imgCursorData.loadImageFromFile(RES_ROOT "/Image/System/Cursor.png",
          GX2_TEX_CLAMP_MODE_CLAMP, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8);
    }
    m_imgCursor.setImageData(&m_imgCursorData);

    ProcUIRegisterCallback(PROCUI_CALLBACK_ACQUIRE, CallbackAcquire, nullptr, 100);
    ProcUIRegisterCallback(PROCUI_CALLBACK_RELEASE, CallbackRelease, nullptr, 100);

    while (Tick()) {
        WaitVSync();

        m_frameId++;

        if (!WHBProcIsRunning()) {
            if (m_video != nullptr) {
                m_video->tvEnable(false);
                m_video->drcEnable(false);
            }
            break;
        }
    }
}

u32 System::CallbackAcquire(void* context)
{
    if (s_instance->m_video == nullptr) {
        libgui_memoryInitialize();
        s_instance->m_video = new CVideo(GX2_TV_SCAN_MODE_1080P);
    }

    return 0;
}

u32 System::CallbackRelease(void* context)
{
    delete s_instance->m_video;
    s_instance->m_video = nullptr;
    libgui_memoryRelease();

    return 0;
}

void* System::RipFile(const char* path, u32* length)
{
    Lock l(sys()->FileMutex());

    auto file = fopen(path, "rb");

    if (file == nullptr) {
        LOG(LogSystem, "Failed to open %s", path);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    u32 filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    u8* data = new (std::align_val_t(256)) u8[filesize];
    auto ret = fread(data, filesize, 1, file);
    fclose(file);

    if (ret != 1) {
        delete[] data;
        data = nullptr;
    }

    if (length != nullptr) {
        *length = filesize;
    }

    return data;
}

bool System::IsAroma()
{
    static bool checked = false;
    static bool aroma = false;

    if (!checked) {
        checked = true;
        OSDynLoad_Module mod;
        aroma = OSDynLoad_Acquire("homebrew_kernel", &mod) == OS_DYNLOAD_OK;
        if (aroma)
            OSDynLoad_Release(mod);
    }

    return aroma;
}

void System::ShowPage(PageID page, Display display)
{
    assert(u32(page) < u32(PageID::PageCount));
    auto set = &m_nextSetting[u32(page)];

    LOG(LogSystem, "Showing page %d on display %d", static_cast<int>(page),
      static_cast<int>(display));

    switch (display) {
    case Display::TV:
        set->tv = true;
        break;

    case Display::DRC:
        set->drc = true;
        break;

    case Display::All:
        set->tv = true;
        set->drc = true;
        break;

    default:
        break;
    }
}

void System::HidePage(PageID page, Display display)
{
    assert(u32(page) < u32(PageID::PageCount));
    auto set = &m_nextSetting[u32(page)];

    LOG(LogSystem, "Hiding page %d from display %d", static_cast<int>(page),
      static_cast<int>(display));

    switch (display) {
    case Display::TV:
        set->tv = false;
        break;

    case Display::DRC:
        set->drc = false;
        break;

    case Display::All:
        set->tv = false;
        set->drc = false;
        break;

    default:
        break;
    }
}

GuiElement* System::GetPage(PageID page)
{
    assert(u32(page) < u32(PageID::PageCount));
    return m_pages[u32(page)].element;
}

bool System::Tick()
{
    assert(m_video != nullptr);

    float curX, curY, curZ;

    bool curValid;
    m_wand->Update(&curX, &curY, &curZ, &curValid);

    if (m_wand->IsCast()) {
        if (m_wand->GetCastMode() == Wand::CastMode::WiiRemoteCastRune && curValid) {
            m_imgCursorTimer = 60;
            m_imgCursor.setPosition(curX, curY);
            LOG(LogSystem, "Cast at %f, %f", curX, curY);
        }

        for (u32 i = 0; i < PageCount; i++) {
            auto handler = dynamic_cast<WandHandler*>(m_pages[i].element);
            if (handler != nullptr) {
                handler->Cast(m_wand->GetCastMode(), curValid, curX, curY, curZ);
            }
        }
    }

    for (u32 i = 0; i < PageCount; i++) {
        m_pages[i] = m_nextSetting[i];
    }

    bool update = m_gamepad.update(m_video->getTvWidth(), m_video->getTvHeight());

    std::array<PageSetting, PageCount> sets = m_pages;

    for (auto set : sets) {
        if (set.element == nullptr) {
            continue;
        }

        set.element->process();

        if (set.drc && update) {
            set.element->update(&m_gamepad);
        }
    }

    m_imgCursor.process();

    m_video->prepareDrcRendering();
    for (auto set : sets) {
        if (set.element == nullptr) {
            continue;
        }

        if (set.drc) {
            set.element->draw(m_video);
        }
    }

    if (m_imgCursorTimer-- <= 0) {
        m_imgCursorTimer = 0;
    } else {
        m_imgCursor.draw(m_video);
    }
    m_video->drcDrawDone();

    sets = m_pages;

    m_video->prepareTvRendering();
    for (auto set : sets) {
        if (set.element == nullptr) {
            continue;
        }

        if (set.tv) {
            set.element->draw(m_video);
        }
    }
    m_video->tvDrawDone();

    m_video->tvEnable(true);
    m_video->drcEnable(true);

    return true;
}

void System::WaitVSync()
{
    m_video->waitForVSync();
}

Wand* System::GetWand()
{
    return m_wand;
}

int main(int argc, char** argv)
{
    WHBProcInit();
    WHBLogCafeInit();
#if CONF_LOG_MASK != 0
    WHBLogUdpInit();
#endif

    Exception::Init();

    // Automatically detect content directory
    FILE* f = nullptr;
    if ((f = fopen("MQ-Version.txt", "r")) != nullptr) {
        fclose(f);
        // chdir(".");
    } else if ((f = fopen("/vol/content/MQ-Version.txt", "r")) != nullptr) {
        fclose(f);
        chdir("/vol/content");
    } else if ((f = fopen("wiiu/apps/magiquest-wiiu/content/MQ-Version.txt", "r")) != nullptr) {
        fclose(f);
        chdir("wiiu/apps/magiquest-wiiu/content");
    } else {
        LOG(LogSystem, "Failed to find content directory");
        return EXIT_FAILURE;
    }

    // Initialize GUI memory
    libgui_memoryInitialize();

    // Create the audio manager
    new AXManager();

    // Scoped System
    {
        System sys;
        sys.Start();
    }

    WHBProcShutdown();
    return EXIT_SUCCESS;
}
