#pragma once
#include "ObCallback.h"
#include <string.h>

PVOID ObHandle = NULL;
HANDLE ProtectedProcessPid = 0;

LPSTR GetProcessNameFromPid(HANDLE pid) {
	PEPROCESS Process;
	if (PsLookupProcessByProcessId(pid, &Process) == STATUS_INVALID_PARAMETER) {
		return "pid???";
	}

	return (LPSTR)PsGetProcessImageFileName(Process);
}

VOID SetProtectedProcess(HANDLE pid) {
	ProtectedProcessPid = pid;
}

OB_PREOP_CALLBACK_STATUS ProcessPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation) {
	UNREFERENCED_PARAMETER(RegistrationContext);

	LPSTR ProcName;
	PEPROCESS OpenedProcess = (PEPROCESS)OperationInformation->Object;

	if (ProtectedProcessPid == 0)
		return OB_PREOP_SUCCESS;

	if (OperationInformation->KernelHandle)
		return OB_PREOP_SUCCESS;

	if (ProtectedProcessPid != 0) {
		LogDebug("Process object operation, name:[%s], destPid:%d, srcTid:%d",
			PsGetProcessId(OpenedProcess), PsGetCurrentThreadId(), GetProcessNameFromPid(PsGetProcessId(OpenedProcess))
		);
	}

	if (ProtectedProcessPid != 0 && PsGetProcessId(OpenedProcess) == ProtectedProcessPid) {
		ProcName = GetProcessNameFromPid(ProtectedProcessPid);
		LogDebug("Protected name target : [%s]", ProcName);

		if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE) {
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = (SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION);
		}
		else {
			OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = (SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION);
		}
	}

	return OB_PREOP_SUCCESS;
}

OB_PREOP_CALLBACK_STATUS ThreadPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation) {
	UNREFERENCED_PARAMETER(RegistrationContext);

	LPSTR ProcName;
	PEPROCESS OpenedProcess = (PEPROCESS)OperationInformation->Object;
	PEPROCESS ProtectedProcess;

	if (ProtectedProcessPid == 0)
		return OB_PREOP_SUCCESS;

	NTSTATUS lookup = PsLookupProcessByProcessId(ProtectedProcessPid, &ProtectedProcess);
	if (!NT_SUCCESS(lookup)) {
		LogDebug("Failed to lookup protected pid");
	}

	if (OperationInformation->KernelHandle)
		return OB_PREOP_SUCCESS;

	if (ProtectedProcess != 0) {
		LogInfo("(Thread) object operation, srcName:[%s], dstName:[%s], destPid:%d, destTid:%d, srcPid:%d",
			GetProcessNameFromPid(PsGetCurrentProcessId),
			GetProcessNameFromPid(PsGetThreadProcessId),
			(ULONG)PsGetThreadProcessId(OperationInformation->Object),
			(ULONG)PsGetThreadId(OperationInformation->Object),
			(ULONG)PsGetCurrentProcessId()
		);
	}

	if (ProtectedProcess != 0 && PsGetThreadProcessId(OpenedProcess) == ProtectedProcess) {
		ProcName = GetProcessNameFromPid(ProtectedProcessPid);
		LogDebug("(Thread) Protected name target : [%s]", ProcName);

		if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE) {
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = (SYNCHRONIZE | THREAD_QUERY_LIMITED_INFORMATION);
		}
		else {
			OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = (SYNCHRONIZE | THREAD_QUERY_LIMITED_INFORMATION);
		}
	}
	return OB_PREOP_SUCCESS;
}

NTSTATUS InitObCallback() {
	NTSTATUS NtHandleCallback = STATUS_UNSUCCESSFUL;
	NTSTATUS NtThreadCallback = STATUS_UNSUCCESSFUL;

	OB_OPERATION_REGISTRATION OBOperationRegistration[2];
	OB_CALLBACK_REGISTRATION OBOCallbackRegistration;

	memset(&OBOCallbackRegistration, 0, sizeof(OBOCallbackRegistration));
	memset(&OBOperationRegistration, 0, sizeof(OBOperationRegistration));

	OBOperationRegistration[1].ObjectType = PsProcessType;
	OBOperationRegistration[1].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	OBOperationRegistration[1].PreOperation = ProcessPreCallback;
	OBOperationRegistration[1].PostOperation = NULL;


	OBOperationRegistration[0].ObjectType = PsThreadType;
	OBOperationRegistration[0].Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	OBOperationRegistration[0].PreOperation = ThreadPreCallback;
	OBOperationRegistration[0].PostOperation = NULL;

	OBOCallbackRegistration.Version = OB_FLT_REGISTRATION_VERSION;
	OBOCallbackRegistration.OperationRegistrationCount = 2;
	OBOCallbackRegistration.RegistrationContext = NULL;
	OBOCallbackRegistration.OperationRegistration = OBOperationRegistration;
	RtlInitUnicodeString(&OBOCallbackRegistration.Altitude, L"1000");

	//Attempt to register callbacks for protection thread/process
	//and store the obRegCallback adress if registration is successful
	NtHandleCallback = ObRegisterCallbacks(&OBOCallbackRegistration, &ObHandle);

	// Thread notify creation -- no need atm
	//PsSetCreateThreadNotifyRoutine(CreateThreadNotifyRoutine);

	if (!NT_SUCCESS(NtHandleCallback)) {
		DestroyObCallback();
		LogError("Deinitialization is completed");
		return NtHandleCallback;
	}

	LogInfo("Initialization is completed");
	return NtHandleCallback;
}

VOID CreateThreadNotifyRoutine(IN HANDLE ProcessId, IN HANDLE ThreadId, IN BOOLEAN Create) {
	//LogDebug("Thread notify routine : operation, pid:%d, theadId:%d, create:%d", (ULONG)ProcessId, (ULONG)ThreadId, Create);
}

VOID DestroyObCallback() {
	if (ObHandle) {
		ObUnRegisterCallbacks(ObHandle);
		ObHandle = NULL;
	}
}