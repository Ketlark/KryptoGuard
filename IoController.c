#include "IoController.h"
#include "Helper.h"
#include "ObCallback.h"
#include <stdlib.h>

BOOLEAN ProtectionModeReceived = FALSE;

NTSTATUS Create(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	LogDebug("CreateCall was called, connection established !");

	return STATUS_SUCCESS;
}
NTSTATUS Close(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	LogDebug("Connection terminated");
	return STATUS_SUCCESS;
}

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	ULONG BytesIO = 0;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	if (ControlCode == IO_SEND_PROCESSID) {
		char* Input = (char*)Irp->AssociatedIrp.SystemBuffer;
		LogDebug("Received pid from userland, pid : %d", atoi(Input));
		SetProtectedProcess((HANDLE)atoi(Input));

		Status = STATUS_SUCCESS;
		BytesIO = strlen(Input);
	} else {
		BytesIO = 0;
	}

	Irp->IoStatus.Status = Status;
	Irp->IoStatus.Information = BytesIO;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Status;
}