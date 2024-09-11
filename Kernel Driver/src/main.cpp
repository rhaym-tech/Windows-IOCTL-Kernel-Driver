#include "includes/includes.h"

void debugPrint(PCSTR msg) {
	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, msg));
}

NTSTATUS driver_main(PDRIVER_OBJECT driver_object, PUNICODE_STRING registery_path) {
	UNREFERENCED_PARAMETER(registery_path);

	UNICODE_STRING deviceName = {};
	RtlInitUnicodeString(&deviceName, L"\\Device\\MyKernelDriver");

	PDEVICE_OBJECT deviceOject = nullptr;
	NTSTATUS status = IoCreateDevice(driver_object, 0, &deviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &deviceOject);

	if (status != STATUS_SUCCESS) {
		debugPrint("[-] Failed to create driver device.\n");
		return status;
	}

	debugPrint("[+] Driver device created successfully!\n");

	UNICODE_STRING symlink = {};
	RtlInitUnicodeString(&symlink, L"\\DosDevice\\MyKernelDriver");

	status = IoCreateSymbolicLink(&symlink, &deviceName);

	if (status != STATUS_SUCCESS) {
		debugPrint("[-] Failed to etabilish symbolic link.\n");
		return status;
	}

	debugPrint("[+] Driver symbolic link estabilished!\n");

	SetFlag(deviceOject->Flags, DO_BUFFERED_IO);

	// Things to be done here:

	// setup the handles
	driver_object->MajorFunction[IRP_MJ_CREATE] = driver::create;
	driver_object->MajorFunction[IRP_MJ_CLOSE] = driver::close;
	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = driver::deviceControl;

	// Device initializing done
	ClearFlag(deviceOject->Flags, DO_DEVICE_INITIALIZING);

	debugPrint("[+] Driver device has been initialized successfully!\n");


	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry() {
	debugPrint("[+] Hello World from the kernel!\n");

	UNICODE_STRING driverName = {};
	RtlInitUnicodeString(&driverName, L"\\Driver\\MyKernelDriver");


	return ioCreateDriver(&driverName, &driver_main);
}