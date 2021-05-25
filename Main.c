#pragma warning (disable : 4100)
#include "Main.h"
#include "Helper.h"
#include "IoController.h"
#include "ObCallback.h"
#include "mousehook.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath) {
	NTSTATUS status = STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(pRegistryPath);

	UNICODE_STRING DriverName, SymbolName;
	PDEVICE_OBJECT pDeviceObj;

	RtlInitUnicodeString(&DriverName, L"\\Device\\KryptoGuard"); // Giving the driver a name
	RtlInitUnicodeString(&SymbolName, L"\\DosDevices\\KryptoGuard"); // Giving the driver a symbol

	status = IoCreateDevice(pDriverObject, 0, &DriverName, FILE_DEVICE_MOUHID_INPUT_HOOK, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObj);
	if (!NT_SUCCESS(status)) {
		LogDebug("Failed to create IoDevice");
	}

	status = IoCreateSymbolicLink(&SymbolName, &DriverName);
	if (!NT_SUCCESS(status)) {
		LogDebug("Failed to create Symlink");
	}

	pDriverObject->DriverUnload = UnloadDriver;

	pDriverObject->Flags |= DO_DIRECT_IO;
	pDriverObject->Flags &= ~DO_DEVICE_INITIALIZING;
	
	status = InitObCallback();
	InitNotifyRoutine();

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

	InitMouseHook(pDriverObject);

	LogDebug("Driver Loaded");
	return STATUS_SUCCESS;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject) {
	UNICODE_STRING SymbolName;
	RtlInitUnicodeString(&SymbolName, L"\\DosDevices\\KryptoGuard"); // Giving the driver a symbol

	PsSetCreateProcessNotifyRoutineEx(ProcessNotifyRoutine, TRUE);
	//PsRemoveCreateThreadNotifyRoutine(CreateThreadNotifyRoutine);
	DestroyObCallback();

	UnloadMouseHook(pDriverObject);

	NTSTATUS result = IoDeleteSymbolicLink(&SymbolName);
	IoDeleteDevice(pDriverObject->DeviceObject);

	if (NT_SUCCESS(result)) LogDebug("Driver unloaded");
	return result;
}