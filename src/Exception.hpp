#pragma once

#include <gctypes.h>
#include <wut_types.h>

struct OSContext;

class Exception
{
public:
    static void Init();

private:
    static BOOL HandleAlignment(OSContext* context);
    static BOOL HandleDSI(OSContext* context);
    static BOOL HandleISI(OSContext* context);
    static BOOL HandleProgram(OSContext* context);

    static BOOL HandleException(const char* type, OSContext* context);
    static void AnalyzeSymbol(u32 addr);
    static s32 CrashReportThread(int argc, const char** argv);
};
