#include "AntiAuto.h"
#include "Helper.h"
#include "mousehook.h"

NTSTATUS HandleClickMouse(PMOUSE_INPUT_DATA pInputData) {
	if (pInputData->ButtonFlags != 0) {
		if (pInputData->ButtonFlags == MOUSE_LEFT_BUTTON_DOWN || pInputData->ButtonFlags == MOUSE_RIGHT_BUTTON_DOWN || pInputData->ButtonFlags == MOUSE_MIDDLE_BUTTON_DOWN || pInputData->ButtonFlags == MOUSE_BUTTON_4_DOWN || pInputData->ButtonFlags == MOUSE_BUTTON_5_DOWN) {
			clickStatus = 1;
		}
		//LogDebug("Mouse Packet Button - Flags : %d, Value : %d", pInputData->ButtonFlags, pInputData->ButtonData);
		//LogDebug("clickStatus : %d", clickStatus);
	}

	return STATUS_SUCCESS;
}

NTSTATUS EnableMouseMonitor() {
	NTSTATUS ntstatus = MhmEnableMouHidMonitor();
	monitorStatus = TRUE;

	if (!NT_SUCCESS(ntstatus)) {
		LogDebug("Failed to enable MouHid Monitor : 0x%X", ntstatus);
	}

	return ntstatus;
}

NTSTATUS DisableMouseMonitor() {
	NTSTATUS ntstatus = MhmDisableMouHidMonitor();
	monitorStatus = FALSE;

	if (!NT_SUCCESS(ntstatus)) {
		LogDebug("Failed to disable MouHid Monitor : 0x%X", ntstatus);
	}

	return ntstatus;
}