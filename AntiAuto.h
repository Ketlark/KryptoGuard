#pragma once

#include <ntifs.h>
#include <ntdef.h>
#include <ntddmou.h>

USHORT clickStatus;
BOOLEAN monitorStatus;

EXTERN_C
NTSTATUS HandleClickMouse(PMOUSE_INPUT_DATA pInputData);

NTSTATUS EnableMouseMonitor();
NTSTATUS DisableMouseMonitor();