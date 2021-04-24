#pragma once
#include "Main.h"
#include "Helper.h"

#define PROCESS_QUERY_LIMITED_INFORMATION      0x1000

VOID SetProtectedProcess(HANDLE pid);

extern UCHAR* PsGetProcessImageFileName(IN PEPROCESS Process);

LPSTR GetProcessNameFromPid(HANDLE pid);

VOID CreateThreadNotifyRoutine(IN HANDLE ProcessId, IN HANDLE ThreadId, IN BOOLEAN Create);

OB_PREOP_CALLBACK_STATUS ProcessPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation);

OB_PREOP_CALLBACK_STATUS ThreadPreCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation);

NTSTATUS InitObCallback();

VOID DestroyObCallback();