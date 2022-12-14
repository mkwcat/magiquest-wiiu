// System.cpp
//   Written by Palapeli
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "System.hpp"
#include "AudioMgr.hpp"
#include "Exception.hpp"
#include <cassert>
#include <coreinit/core.h>
#include <coreinit/dynload.h>
#include <cstdlib>
#include <gui/memory.h>
#include <gui/sounds/SoundHandler.hpp>
#include <gui/video/CVideo.h>
#include <padscore/kpad.h>
#include <proc_ui/procui.h>
#include <whb/crash.h>
#include <whb/proc.h>

#include <whb/log.h>
#include <whb/log_cafe.h>
#if CONF_LOG_MASK != 0
#  include <whb/log_udp.h>
#endif

#include "Page_Background.hpp"
#include "Page_CastModeConfirm.hpp"
#include "Page_DuelXavier.hpp"
#include "Page_ModeSelect.hpp"
#include "Page_Movie.hpp"

System* System::s_instance = nullptr;

System::System()
  : CThread(
      CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff, 0, 0x80000)
  , m_video(new CVideo(GX2_TV_SCAN_MODE_1080P))
  , m_gamepad(GuiTrigger::CHANNEL_1)
  , m_imgCursor(nullptr)
  , m_pages{
      {
        .id = PageID::Movie,
        .tv = false,
        .drc = false,
        .element = new Page_Movie(),
      },
      {
        .id = PageID::Background,
        .tv = false,
        .drc = false,
        .element = new Page_Background(),
      },
      {
        .id = PageID::ModeSelect,
        .tv = false,
        .drc = false,
        .element = new Page_ModeSelect(),
      },
      {
        .id = PageID::CastModeConfirm,
        .tv = false,
        .drc = false,
        .element = new Page_CastModeConfirm(),
      },
      {
        .id = PageID::TouchDuel,
        .tv = false,
        .drc = false,
        .element = new Page_DuelXavier(),
      },
    }
{
    if (s_instance == nullptr) {
        s_instance = this;
    }

    LOG(LogSystem, "Starting application");

    m_imgCursorTimer = 0;
    m_frameId = 0;

    // Disable display
    m_video->tvEnable(false);
    m_video->drcEnable(false);

    // Enable Wii remotes
    KPADInit();
}

System::~System()
{
    AudioMgr::s_instance->Shutdown();

    for (auto set : m_pages) {
        delete set.element;
        set.element = nullptr;
    }

    if (s_instance == this) {
        s_instance = nullptr;
    }

    delete AudioMgr::s_instance;

    LOG(LogSystem, "Leaving application");
}

void System::Start()
{
    resumeThread();
    // This will wait until the thread exits.
    shutdownThread();
}

void System::executeThread()
{
    ShowPage(System::PageID::Movie, System::Display::TV);
    ShowPage(System::PageID::Background, System::Display::DRC);
    ShowPage(System::PageID::ModeSelect, System::Display::DRC);

    WaitVSync();

    m_imgCursorData.loadImageFromFile(RES_ROOT "/Image/cursor.png",
      GX2_TEX_CLAMP_MODE_CLAMP, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8);
    m_imgCursor.setImageData(&m_imgCursorData);

    ProcUIRegisterCallback(
      PROCUI_CALLBACK_ACQUIRE, CallbackAcquire, nullptr, 100);
    ProcUIRegisterCallback(
      PROCUI_CALLBACK_RELEASE, CallbackRelease, nullptr, 100);

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
    auto file = fopen(path, "rb");

    if (file == nullptr)
        return nullptr;

    fseek(file, 0, SEEK_END);

    u32 filesize = ftell(file);

    fseek(file, 0, SEEK_SET);

    u8* data = new u8[filesize];

    auto ret = fread(data, filesize, 1, file);

    fclose(file);

    if (ret != 1) {
        delete[] data;
        data = nullptr;
    }

    if (length != nullptr)
        *length = filesize;

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
    auto set = &m_pages[u32(page)];

    LOG(LogSystem, "Showing page %d on display %d", page, display);

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
    }
}

void System::HidePage(PageID page, Display display)
{
    assert(u32(page) < u32(PageID::PageCount));
    auto set = &m_pages[u32(page)];

    LOG(LogSystem, "Hiding page %d from display %d", page, display);

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
    m_wand.Update(&curX, &curY, &curZ, &curValid);

    if (m_wand.IsCast()) {
        if (m_wand.GetCastMode() == Wand::CastMode::WiiRemoteCastRune &&
            curValid) {
            m_imgCursorTimer = 60;
            m_imgCursor.setPosition(curX, curY);
            LOG(LogSystem, "Cast at %f, %f", curX, curY);
        }

        for (u32 i = 0; i < PageCount; i++) {
            auto handler = dynamic_cast<WandHandler*>(m_pages[i].element);
            if (handler != nullptr)
                handler->Cast(m_wand.GetCastMode(), curValid, curX, curY, curZ);
        }
    }

    bool update =
      m_gamepad.update(m_video->getTvWidth(), m_video->getTvHeight());

    for (auto set : m_pages) {
        set.element->process();

        if (set.drc && update) {
            set.element->update(&m_gamepad);
        }
    }

    m_imgCursor.process();

    m_video->prepareDrcRendering();
    for (auto set : m_pages) {
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

    m_video->prepareTvRendering();
    for (auto set : m_pages) {
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
    return &m_wand;
}

#include <coreinit/debug.h>

static void getStackTrace(uint32_t* stackPtr)
{
    int i;
    char name[256];

    WHBLogPrintf("Address:      Back Chain    LR Save");

    for (i = 0; i < 16; ++i) {
        uint32_t addr;

        if (!stackPtr || (uintptr_t) stackPtr == 0x1 ||
            (uintptr_t) stackPtr == 0xFFFFFFFF) {
            break;
        }

        addr = OSGetSymbolName(stackPtr[1], name, sizeof(name));
        if (addr) {
            WHBLogPrintf("0x%08x:   0x%08x    0x%08x %s+0x%x",
              (uintptr_t) stackPtr, (uintptr_t) stackPtr[0],
              (uintptr_t) stackPtr[1], name, (uintptr_t) (stackPtr[1] - addr));
        } else {
            WHBLogPrintf("0x%08x:   0x%08x    0x%08x", (uintptr_t) stackPtr,
              (uintptr_t) stackPtr[0], (uintptr_t) stackPtr[1]);
        }

        stackPtr = (uint32_t*) *stackPtr;
    }
}

extern "C" void __assert_func(
  const char* file, int line, const char* function, const char* condition)
{
    WHBLogPrintf("%s:%d   ASSERTION FAILED (%s)", file, line, condition);
    WHBProcShutdown();
    exit(EXIT_FAILURE);
}

extern "C" void abort()
{
    u32 lr = 0;
    u32 sp = 0;
    asm volatile("mflr %0" : "=r"(lr));
    asm volatile("mr %0, 1" : "=r"(sp));

    WHBLogPrintf("Abort was called! LR = %08X", lr);

    getStackTrace(&sp);

    WHBProcShutdown();
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
    WHBProcInit();
    WHBLogCafeInit();
#if CONF_LOG_MASK != 0
    WHBLogUdpInit();
#endif

    Exception::Init();

    libgui_memoryInitialize();

    // Create the audio manager
    new AudioMgr();

    // Initialize random state
    srand(OSGetSystemTime());
    rand();
    rand();

    // Scoped System
    {
        System sys;

        LOG(LogMP4, "Sleeping for 1...");
        sleep(1);
        LOG(LogMP4, "Done!");

        sys.Start();
    }

    WHBProcShutdown();
    return EXIT_SUCCESS;
}
