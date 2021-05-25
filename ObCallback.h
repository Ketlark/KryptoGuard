#pragma once
#include "Main.h"
#include "Helper.h"

#define PROCESS_QUERY_LIMITED_INFORMATION      0x1000

VOID SetProtectedProcess(HANDLE pid);

LPSTR GetProcessNameFromPid(HANDLE pid);

extern UCHAR* PsGetProcessImageFileName(IN PEPROCESS Process);

VOID CreateThreadNotifyRoutine(IN HANDLE ProcessId, IN HANDLE ThreadId, IN BOOLEAN Create);

VOID ProcessNotifyRoutine(PEPROCESS Process, HANDLE ProcessId, PPS_CREATE_NOTIFY_INFO CreateInfo);

OB_PREOP_CALLBACK_STATUS ProcessPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation);

OB_PREOP_CALLBACK_STATUS ThreadPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation);

NTSTATUS InitObCallback();

VOID InitNotifyRoutine();

VOID DestroyObCallback();