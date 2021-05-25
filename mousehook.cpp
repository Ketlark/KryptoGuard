#include "mousehook.h"
#include "Helper.h"
#include "ObCallback.h"

//=============================================================================
// Meta Interface
//=============================================================================
NTSTATUS
InitMouseHook(PDRIVER_OBJECT pDriverObject) {
    UNICODE_STRING usDeviceName = {};
    BOOLEAN fMclLoaded = FALSE;
    BOOLEAN fMhkLoaded = FALSE;
    BOOLEAN fMhmLoaded = FALSE;
    NTSTATUS ntstatus = STATUS_SUCCESS;

    LogDebug("Loading Mouse hook");

    //
    // Load the driver modules.
    //
    ntstatus = MhdDriverEntry();
    if (!NT_SUCCESS(ntstatus))
    {
        LogDebug("MhdDriverEntry failed: 0x%X", ntstatus);
        goto exit;
    }

    ntstatus = MclDriverEntry(pDriverObject);
    if (!NT_SUCCESS(ntstatus))
    {
        LogDebug("MclDriverEntry failed: 0x%X", ntstatus);
        goto exit;
    }
    //
    fMclLoaded = TRUE;

    ntstatus = MhkDriverEntry();
    if (!NT_SUCCESS(ntstatus))
    {
        LogDebug("MhkDriverEntry failed: 0x%X", ntstatus);
        goto exit;
    }
    //
    fMhkLoaded = TRUE;

    ntstatus = MhmDriverEntry();
    if (!NT_SUCCESS(ntstatus))
    {
        LogDebug("MhmDriverEntry failed: 0x%X", ntstatus);
        goto exit;
    }
    //
    fMhmLoaded = TRUE;

    LogDebug("Mouse hook loaded");

exit:
    if (!NT_SUCCESS(ntstatus))
    {
        if (fMhmLoaded)
        {
            MhmDriverUnload();
        }

        if (fMhkLoaded)
        {
            MhkDriverUnload();
        }

        if (fMclLoaded)
        {
            MclDriverUnload();
        }
    }

    return ntstatus;
}

VOID
UnloadMouseHook(PDRIVER_OBJECT pDriverObject) {
    UNREFERENCED_PARAMETER(pDriverObject);
    DBG_PRINT("Unloading Mouse hook.");
    //
    // Unload the driver modules.
    //
    MhmDriverUnload();
    MhkDriverUnload();
    MclDriverUnload();


    DBG_PRINT("Mouse hook unloaded.");
}


//=============================================================================
// Private Interface
//=============================================================================
EXTERN_C
NTSTATUS
DispatchCreate(PDEVICE_OBJECT pDeviceObject, PIRP pIrp) {
    UNREFERENCED_PARAMETER(pDeviceObject);
    DBG_PRINT("Processing IRP_MJ_CREATE.");

    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

EXTERN_C
NTSTATUS
DispatchClose(PDEVICE_OBJECT pDeviceObject,PIRP pIrp) {
    UNREFERENCED_PARAMETER(pDeviceObject);
    DBG_PRINT("Processing IRP_MJ_CLOSE.");

    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}
