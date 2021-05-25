#include "IoController.h"
#include "Helper.h"
#include "AntiAuto.h"
#include "ObCallback.h"
#include <stdlib.h>

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

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
	}
	else if(ControlCode == IO_SEND_PING) {
		char* Input = (char*)Irp->AssociatedIrp.SystemBuffer;
		CHAR* pong = "pong";

		if (!strcmp(Input, "ping")) {
			RtlCopyMemory(Irp->AssociatedIrp.SystemBuffer, pong, strlen(pong));

			Status = STATUS_SUCCESS;
			BytesIO = strlen(pong);
		}
	} else if (ControlCode == IO_SEND_CLICK) {
		char* Input = (char*)Irp->AssociatedIrp.SystemBuffer;
		USHORT defaultClick = 99;

		if (!strcmp(Input, "click")) {
			RtlCopyMemory(Irp->AssociatedIrp.SystemBuffer, &clickStatus != NULL ? &clickStatus : &defaultClick, sizeof(clickStatus));
			clickStatus = 0;

			Status = STATUS_SUCCESS;
			BytesIO = sizeof(clickStatus);
		}
	} else {
		BytesIO = 0;
	}

	Irp->IoStatus.Status = Status;
	Irp->IoStatus.Information = BytesIO;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Status;
}