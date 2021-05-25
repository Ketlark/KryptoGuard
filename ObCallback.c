#pragma once

#include "ObCallback.h"
#include "AntiAuto.h"
#include <string.h>

PVOID ObHandle = NULL;
HANDLE ProtectedProcessPid[24] = { 0 };
SHORT OffsetPid = 0;

LPSTR GetProcessNameFromPid(HANDLE pid) {
	PEPROCESS Process;
	if (PsLookupProcessByProcessId(pid, &Process) == STATUS_INVALID_PARAMETER) {
		return "pid???";
	}

	return (LPSTR)PsGetProcessImageFileName(Process);
}

VOID SetProtectedProcess(HANDLE pid) {
	ProtectedProcessPid[OffsetPid] = pid;
	OffsetPid++;

	if (!monitorStatus) EnableMouseMonitor();
}

OB_PREOP_CALLBACK_STATUS ProcessPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation) {
	UNREFERENCED_PARAMETER(RegistrationContext);

	PEPROCESS OpenedProcess = (PEPROCESS)OperationInformation->Object;

	if (OperationInformation->KernelHandle)
		return OB_PREOP_SUCCESS;

	for (size_t i = 0; i < OffsetPid; i++) {

		if(ProtectedProcessPid[i] == 0)
			return OB_PREOP_SUCCESS;

		if (PsGetCurrentProcessId() == ProtectedProcessPid[i]) {
			return OB_PREOP_SUCCESS;
		}

		if (ProtectedProcessPid[i] != 0 && PsGetProcessId(OpenedProcess) == ProtectedProcessPid[i]) {
			if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE) {
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = (SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION);
			}
			else {
				OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = (SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION);
			}
		}
	}

	return OB_PREOP_SUCCESS;
}

OB_PREOP_CALLBACK_STATUS ThreadPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation) {
	UNREFERENCED_PARAMETER(RegistrationContext);

	LPSTR ProcName;
	PEPROCESS OpenedProcess = (PEPROCESS)OperationInformation->Object;

	if (OperationInformation->KernelHandle)
		return OB_PREOP_SUCCESS;

	for (size_t i = 0; i < OffsetPid; i++) {
		if (ProtectedProcessPid[i] == 0)
			return OB_PREOP_SUCCESS;

		if (PsGetCurrentProcessId() == ProtectedProcessPid[i]) {
			return OB_PREOP_SUCCESS;
		}

		if (ProtectedProcessPid[i] != 0 && PsGetThreadProcessId(OpenedProcess) == ProtectedProcessPid[i]) {
			ProcName = GetProcessNameFromPid(ProtectedProcessPid[i]);
			//LogDebug("(Thread) Protected name target : [%s]:%d by [%s]:%d", ProcName, ProtectedProcessPid[i], GetProcessNameFromPid(PsGetCurrentProcessId()), PsGetCurrentProcessId());

			if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE) {
				OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = (SYNCHRONIZE | THREAD_QUERY_LIMITED_INFORMATION);
			}
			else {
				OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = (SYNCHRONIZE | THREAD_QUERY_LIMITED_INFORMATION);
			}
		}
	}

	return OB_PREOP_SUCCESS;
}

NTSTATUS InitObCallback() {
	NTSTATUS NtHandleCallback = STATUS_UNSUCCESSFUL;

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


	if (!NT_SUCCESS(NtHandleCallback)) {
		DestroyObCallback();
		LogError("Deinitialization is completed");
		return NtHandleCallback;
	}

	LogInfo("Initialization is completed");
	return NtHandleCallback;
}

VOID InitNotifyRoutine() {
	// Thread notify creation -- no need atm
	//PsSetCreateThreadNotifyRoutine(CreateThreadNotifyRoutine);

	PsSetCreateProcessNotifyRoutineEx(ProcessNotifyRoutine, FALSE);
}

VOID CreateThreadNotifyRoutine(IN HANDLE ProcessId, IN HANDLE ThreadId, IN BOOLEAN Create) {
	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(ThreadId);

	//LogDebug("Thread notify routine : operation, pid:%d, theadId:%d, create:%d", (ULONG)ProcessId, (ULONG)ThreadId, Create);
}

VOID ProcessNotifyRoutine(PEPROCESS Process, HANDLE ProcessId, PPS_CREATE_NOTIFY_INFO CreateInfo) {
	UNREFERENCED_PARAMETER(Process);

	//Decrease offset when protected PID was killed/exited
	if (CreateInfo == NULL) {
		for (size_t i = 0; i < OffsetPid; i++) {
			if (ProtectedProcessPid[i] == ProcessId) {
				ProtectedProcessPid[i] = 0;
				OffsetPid--;
				if (OffsetPid <= 0 && monitorStatus) DisableMouseMonitor();
			}
		}
	}
}

VOID DestroyObCallback() {
	if (ObHandle) {
		ObUnRegisterCallbacks(ObHandle);
		ObHandle = NULL;
	}
}