#pragma once

#include <Ntddk.h>

#define NORMALIZE_INCREAMENT (USHORT)0x200

#define _LogMsg(lvl, lvlname, frmt, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, lvl , "hidden!" __FUNCTION__ "[irql:%d,pid:%d][" lvlname "]: " frmt "\n", (ULONG)KeGetCurrentIrql(), (ULONG)PsGetCurrentProcessId(), __VA_ARGS__)

#define LogError(frmt,   ...) _LogMsg(DPFLTR_ERROR_LEVEL,   "error",   frmt, __VA_ARGS__)
#define LogWarning(frmt, ...) _LogMsg(DPFLTR_WARNING_LEVEL, "warning", frmt, __VA_ARGS__)
#define LogTrace(frmt,   ...) _LogMsg(DPFLTR_TRACE_LEVEL,   "trace",   frmt, __VA_ARGS__)
#define LogInfo(frmt,    ...) _LogMsg(DPFLTR_INFO_LEVEL,    "info",    frmt, __VA_ARGS__)
#define LogDebug(frmt,    ...) DbgPrintEx(0, 0, frmt "\n", __VA_ARGS__)