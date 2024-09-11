#pragma once

namespace driver {
	namespace codes {
		constexpr ULONG ATTACH = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x696, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
		constexpr ULONG READ = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x697, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
		constexpr ULONG WRITE = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x698, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
	}

	struct Request {
		HANDLE PID;

		PVOID buffer;
		PVOID target;

		SIZE_T size;
		SIZE_T return_len;
	};

	NTSTATUS create(PDEVICE_OBJECT device_obj, PIRP irp) {
		UNREFERENCED_PARAMETER(device_obj);

		IoCompleteRequest(irp, IO_NO_INCREMENT);
		return irp->IoStatus.Status;
	}

	NTSTATUS close(PDEVICE_OBJECT device_obj, PIRP irp) {
		UNREFERENCED_PARAMETER(device_obj);

		IoCompleteRequest(irp, IO_NO_INCREMENT);
		return irp->IoStatus.Status;
	}

	//TODO: more things to be done
	NTSTATUS deviceControl(PDEVICE_OBJECT device_obj, PIRP irp) {
		UNREFERENCED_PARAMETER(device_obj);

		debugPrint("Device controll called!\n");

		NTSTATUS status = STATUS_UNSUCCESSFUL;

		// Obtaining which code has been sent from the User-Mode
		PIO_STACK_LOCATION stackIrp = IoGetCurrentIrpStackLocation(irp);
		
		// Access to the request object sent from the User_Mode:
		auto req = reinterpret_cast<Request*>(irp->AssociatedIrp.SystemBuffer);

		if (stackIrp == nullptr || req == nullptr) {
			IoCompleteRequest(irp, IO_NO_INCREMENT);
			return status;
		}

		static PEPROCESS targetProcess = nullptr;
		
		const ULONG CC = stackIrp->Parameters.DeviceIoControl.IoControlCode; // Control Code

		switch (CC) {
			case codes::ATTACH : {
				status = PsLookupProcessByProcessId(req->PID, &targetProcess);
				break;
			}
			case codes::READ : {
				if (targetProcess != nullptr)
					status = MmCopyVirtualMemory(targetProcess, req->target, PsGetCurrentProcess(), req->buffer, req->size, KernelMode, &req->return_len);
				break;
			}
			case codes::WRITE : {
				if (targetProcess != nullptr)
					status = MmCopyVirtualMemory(PsGetCurrentProcess(), req->buffer, targetProcess, req->target, req->size, KernelMode, &req->return_len);
				break;
			}

			default:
				break;
		}

		irp->IoStatus.Status = status;
		irp->IoStatus.Information = sizeof(Request);

		IoCompleteRequest(irp, IO_NO_INCREMENT);

		return status;
	}
}