#pragma once
#include <fltKernel.h>

#include "debug.h"
#include "log.h"
#include "mouhid.h"
#include "mouhid_hook_manager.h"
#include "mouhid_monitor.h"
#include "ioctl.h"

//=============================================================================
// Private Prototypes
//=============================================================================
EXTERN_C
NTSTATUS
InitMouseHook(
    PDRIVER_OBJECT pDriverObject
);

EXTERN_C
VOID
UnloadMouseHook(
    PDRIVER_OBJECT pDriverObject
);

_Dispatch_type_(IRP_MJ_CREATE)
EXTERN_C
DRIVER_DISPATCH
DispatchCreate;

_Dispatch_type_(IRP_MJ_CLOSE)
EXTERN_C
DRIVER_DISPATCH
DispatchClose;

_Dispatch_type_(IRP_MJ_DEVICE_CONTROL)
EXTERN_C
DRIVER_DISPATCH
DispatchDeviceControl;