#pragma once

#include <ntifs.h>
#include <ntdef.h>

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegristryPath);
NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject);