#pragma once

#include <ntifs.h>
#include "driver.h"

extern "C" {
	NTKERNELAPI NTSTATUS ioCreateDriver(
		PUNICODE_STRING DriverName,
		PDRIVER_INITIALIZE InitializationFunction
	);

	NTKERNELAPI NTSTATUS MmCopyVirtualMemory(
		PEPROCESS SourceProcess, PVOID SourceAddress,
		PEPROCESS TargetProcess, PVOID TargetAddress,
		SIZE_T BufferSize,
		KPROCESSOR_MODE PreviousMode,
		PSIZE_T ReturnSize
	);

}