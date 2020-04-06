#define METHOD_ENABLE_HOOK  0x31337
#define METHOD_DISABLE_HOOK 0x31333
#define METHOD_STATUS_HOOK  0x31339

typedef NTSTATUS(*NtTraceEvent_t)(
	_In_ PHANDLE TraceHandle,
	_In_ ULONG Flags,
	_In_ ULONG FieldSize,
	_In_ PVOID Fields);

typedef NTSTATUS(*IoCreateDriver_t)(
	_In_ PUNICODE_STRING DriverName, OPTIONAL
	_In_ PDRIVER_INITIALIZE InitializationFunction);

DRIVER_INITIALIZE DriverEntry;

void __fastcall SyscallStub(
	_In_ unsigned int SystemCallIndex,
	_Inout_ void** SystemCallFunction);

NTSTATUS DetourNtTraceEvent(
	_In_ PHANDLE TraceHandle,
	_In_ ULONG Flags,
	_In_ ULONG FieldSize,
	_In_ PVOID Fields);