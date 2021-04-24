#pragma warning (disable : 4100)
#include "Main.h"
#include "Helper.h"
#include "IoController.h"
#include "ObCallback.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath) {
	//NTSTATUS status = RegisterCallbackFunction();
	/*if (!NT_SUCCESS(status))
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Failed to RegisterCallbackFunction .status : 0x%X \n", status);
	}*/

	NTSTATUS status = STATUS_SUCCESS;

	UNICODE_STRING DriverName, SymbolName;
	PDEVICE_OBJECT pDeviceObj;

	RtlInitUnicodeString(&DriverName, L"\\Device\\KryptoLoader"); // Giving the driver a name
	RtlInitUnicodeString(&SymbolName, L"\\DosDevices\\KryptoLoader"); // Giving the driver a symbol

	status = IoCreateDevice(pDriverObject, 0, &DriverName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObj);
	if (!NT_SUCCESS(status)) {
		LogError("Failed to create IoDevice");
	}

	status = IoCreateSymbolicLink(&SymbolName, &DriverName);
	if (!NT_SUCCESS(status)) {
		LogError("Failed to create Symlink");
	}

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = Create;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = Close;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

	pDeviceObj->Flags |= DO_DIRECT_IO;
	pDeviceObj->Flags &= ~DO_DEVICE_INITIALIZING;
	
	pDriverObject->DriverUnload = UnloadDriver;

	status = InitObCallback();
	if (!NT_SUCCESS(status)) {
		LogError("Error, callback monitor haven't started = code:%08x", status);
	}

	LogDebug("Driver Loaded");
	return STATUS_SUCCESS;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject) {

	UNICODE_STRING SymbolName;
	RtlInitUnicodeString(&SymbolName, L"\\DosDevices\\KryptoLoader"); // Giving the driver a symbol

	//PsRemoveCreateThreadNotifyRoutine(CreateThreadNotifyRoutine);
	DestroyObCallback();

	NTSTATUS result = IoDeleteSymbolicLink(&SymbolName);
	IoDeleteDevice(pDriverObject->DeviceObject);

	if (NT_SUCCESS(result)) LogDebug("Driver unloaded");
	return result;
}

//Callback part

/*OB_PREOP_CALLBACK_STATUS PreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation) {
	LPSTR ProcName;

	UNREFERENCED_PARAMETER(RegistrationContext);

	PEPROCESS OpenedProcess = (PEPROCESS)OperationInformation->Object,
		CurrentProcess = PsGetCurrentProcess();

	PsLookupProcessByProcessId(ProtectedProcess, &ProtectedProcessProcess); // Getting the PEPROCESS using the PID 
	PsLookupProcessByProcessId(Lsass, &LsassProcess); // Getting the PEPROCESS using the PID 
	PsLookupProcessByProcessId(Csrss1, &Csrss1Process); // Getting the PEPROCESS using the PID 
	PsLookupProcessByProcessId(Csrss2, &Csrss2Process); // Getting the PEPROCESS using the PID 

	//Get ProcessName when process is created
	ProcName = GetProcessNameFromPid(PsGetProcessId((PEPROCESS)OperationInformation->Object));
}

VOID ObjectPostCallback(IN PVOID RegistrationContext, IN POB_POST_OPERATION_INFORMATION OperationInformation) {
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "PostProcCreateRoutine. \n");
}

NTSTATUS RegisterCallbackFunction() {
	NTSTATUS ntStatus = STATUS_SUCCESS;
	return ntStatus;
}*/