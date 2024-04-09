// Based on
// https://github.com/devkitPro/wut/blob/master/libraries/libwhb/src/crash.c

#include "Exception.hpp"

#include <coreinit/debug.h>
#include <coreinit/exception.h>
#include <coreinit/thread.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdarg.h>
#include <whb/log.h>
#include <whb/proc.h>

#define THREAD_STACK_SIZE (4096)
static OSThread s_crashThread;
static u8 s_crashThreadStack[THREAD_STACK_SIZE];
static char s_printBuffer[4096];
static u32 s_printLength;

void Exception::Init()
{
    OSSetExceptionCallbackEx(
      OS_EXCEPTION_MODE_GLOBAL, OS_EXCEPTION_TYPE_ALIGNMENT, HandleAlignment);
    OSSetExceptionCallbackEx(
      OS_EXCEPTION_MODE_GLOBAL, OS_EXCEPTION_TYPE_DSI, HandleDSI);
    OSSetExceptionCallbackEx(
      OS_EXCEPTION_MODE_GLOBAL, OS_EXCEPTION_TYPE_ISI, HandleISI);
    OSSetExceptionCallbackEx(
      OS_EXCEPTION_MODE_GLOBAL, OS_EXCEPTION_TYPE_PROGRAM, HandleProgram);
}

BOOL Exception::HandleAlignment(OSContext* context)
{
    return HandleException("ALIGNMENT", context);
}

BOOL Exception::HandleDSI(OSContext* context)
{
    return HandleException("DSI", context);
}

BOOL Exception::HandleISI(OSContext* context)
{
    return HandleException("ISI", context);
}

BOOL Exception::HandleProgram(OSContext* context)
{
    return HandleException("PROGRAM", context);
}

static void WriteRegister(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    s_printLength += vsprintf(s_printBuffer + s_printLength, fmt, args);
    s_printBuffer[s_printLength] = 0;
    va_end(args);
}

BOOL Exception::HandleException(const char* type, OSContext* context)
{
    // If the exception is not ISI, then we should be able to read the
    // instruction it crashed on.
    u32 code = strcmp(type, "ISI") == 0 ? 0 : *(u32*) context->srr0;

    WriteRegister("************ EXCEPTION '%s' OCCURRED ************\n", type);

    WriteRegister("GPR00 %08X GPR08 %08X GPR16 %08X GPR24 %08X\n",
      context->gpr[0], context->gpr[8], context->gpr[16], context->gpr[24]);
    WriteRegister("GPR01 %08X GPR09 %08X GPR17 %08X GPR25 %08X\n",
      context->gpr[1], context->gpr[9], context->gpr[17], context->gpr[25]);
    WriteRegister("GPR02 %08X GPR10 %08X GPR18 %08X GPR26 %08X\n",
      context->gpr[2], context->gpr[10], context->gpr[18], context->gpr[26]);
    WriteRegister("GPR03 %08X GPR11 %08X GPR19 %08X GPR27 %08X\n",
      context->gpr[3], context->gpr[11], context->gpr[19], context->gpr[27]);
    WriteRegister("GPR04 %08X GPR12 %08X GPR20 %08X GPR28 %08X\n",
      context->gpr[4], context->gpr[12], context->gpr[20], context->gpr[28]);
    WriteRegister("GPR05 %08X GPR13 %08X GPR21 %08X GPR29 %08X\n",
      context->gpr[5], context->gpr[13], context->gpr[21], context->gpr[29]);
    WriteRegister("GPR06 %08X GPR14 %08X GPR22 %08X GPR30 %08X\n",
      context->gpr[6], context->gpr[14], context->gpr[22], context->gpr[30]);
    WriteRegister("GPR07 %08X GPR15 %08X GPR23 %08X GPR31 %08X\n",
      context->gpr[7], context->gpr[15], context->gpr[23], context->gpr[31]);
    WriteRegister("LR    %08X SRR0  %08X DAR   %08X CODE  %08X\n", context->lr,
      context->srr0, context->dar, code);

    AnalyzeSymbol(context->srr0);
    AnalyzeSymbol(context->lr);

    // Analyze stack trace
    u32* stackPtr = (u32*) context->gpr[1];
    for (int i = 0; i < 5; i++, stackPtr = (u32*) *stackPtr) {
        if (!stackPtr || (u32) stackPtr == 0x1 ||
            (u32) stackPtr == 0xFFFFFFFF) {
            break;
        }

        // Skip if it's already been analyzed
        if (stackPtr[1] == context->srr0 || stackPtr[1] == context->lr) {
            i--;
            continue;
        }

        AnalyzeSymbol(stackPtr[1]);
    }

    OSCreateThread(&s_crashThread, CrashReportThread, 0, NULL,
      s_crashThreadStack + THREAD_STACK_SIZE, THREAD_STACK_SIZE, 16, 0);
    OSResumeThread(&s_crashThread);
    OSSuspendThread((OSThread*) context);

    return TRUE;
}

void Exception::AnalyzeSymbol(u32 addr)
{
    char name[256];
    u32 symAddr = OSGetSymbolName(addr, name, sizeof(name));
    if (symAddr) {
        WriteRegister("%08X %s+0x%X\n", addr, name, addr - symAddr);
    } else {
        WriteRegister("%08X <unknown>\n", addr);
    }
}

s32 Exception::CrashReportThread(int argc, const char** argv)
{
    WHBLogPrint(s_printBuffer);

    OSFatal(s_printBuffer);

    while (true) {
    }
}

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
