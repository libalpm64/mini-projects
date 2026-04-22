#include "Driver.h"
#include "Trace.h"

#pragma warning(disable: 4100) 
#pragma warning(disable: 4189) 
#pragma warning(disable: 4996)

#define WPP_CONTROL_GUIDS \
    WPP_DEFINE_CONTROL_GUID(DriverGuid,(82881111,1222,3333,4444,555555555555), \
        WPP_DEFINE_BIT(TRACE_DRIVER))

#define WPP_FLAG_LEVEL_LOGGER(flag, level)                                  \
    WPP_LEVEL_LOGGER(flag)

#define WPP_FLAG_LEVEL_ENABLED(flag, level)                                \
    (WPP_LEVEL_ENABLED(flag) &&                                            \
     WPP_CONTROL(WPP_BIT_ ## flag).Level >= level)

#define TraceEvents(level, flags, msg, ...) \
    WPP_LEVEL_LOGGER(flags) ? \
        WPP_LOG_ALWAYS(flags,(msg),__VA_ARGS__) : 0

#include "Driver.tmh"

NTKERNELAPI
VOID
KeStackAttachProcess(
    __in PEPROCESS Process,
    __out PKAPC_STATE ApcState
);

NTKERNELAPI
VOID
KeUnstackDetachProcess(
    __in PKAPC_STATE ApcState
);

NTKERNELAPI
NTSTATUS
PsLookupProcessByProcessId(
    __in HANDLE ProcessId,
    __deref_out PEPROCESS* Process
);

NTKERNELAPI
NTSTATUS
ZwQueryVirtualMemory(
    __in HANDLE ProcessHandle,
    __in PVOID BaseAddress,
    __in MEMORY_INFORMATION_CLASS MemoryInformationClass,
    __out_bcount(MemoryInformationLength) PVOID MemoryInformation,
    __in SIZE_T MemoryInformationLength,
    __out_opt PSIZE_T ReturnLength
);


typedef struct _RTL_USER_PROCESS_PARAMETERS {
    BYTE Reserved1[16];
    PVOID Reserved2[10];
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB_LDR_DATA {
    BYTE Reserved1[8];
    PVOID Reserved2[3];
    LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _PEB {
    BYTE Reserved1[2];
    BYTE BeingDebugged;
    BYTE Reserved2[1];
    PVOID Reserved3[2];
    PPEB_LDR_DATA Ldr;
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
    BYTE Reserved4[104];
    PVOID Reserved5[52];
    PVOID PostProcessInitRoutine;
    BYTE Reserved6[128];
    PVOID Reserved7[1];
    ULONG SessionId;
} PEB, * PPEB;

NTKERNELAPI
PPEB
NTAPI
PsGetProcessPeb(
    IN PEPROCESS Process
);

NTSTATUS RegisterProcessCallback(PDRIVER_OBJECT DriverObject);

typedef struct _MEMORY_BASIC_INFORMATION {
    PVOID BaseAddress;
    PVOID AllocationBase;
    ULONG AllocationProtect;
    SIZE_T RegionSize;
    ULONG State;
    ULONG Protect;
    ULONG Type;
} MEMORY_BASIC_INFORMATION, * PMEMORY_BASIC_INFORMATION;

// Add these definitions
#define PROCESS_VM_READ           0x0010
#define PROCESS_VM_WRITE          0x0020
#define PROCESS_VM_OPERATION      0x0008
#define PROCESS_CREATE_THREAD     0x0002

#define MAX_PATH 260

#define DEVICE_NAME L"\\Device\\AntiCheatDriver"
#define SYMBOLIC_LINK_NAME L"\\??\\AntiCheatDriver"

// Anti-cheat specific IOCTL codes
#define IOCTL_SCAN_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_CHECK_MODULES CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VERIFY_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_CHECK_SIGNATURES CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Structures for communication
typedef struct _MEMORY_SCAN_REQUEST {
    HANDLE ProcessId;
    PVOID StartAddress;
    SIZE_T Size;
    ULONG ScanFlags;
} MEMORY_SCAN_REQUEST, * PMEMORY_SCAN_REQUEST;

typedef struct _MODULE_CHECK_REQUEST {
    HANDLE ProcessId;
    BOOLEAN VerifySignatures;
} MODULE_CHECK_REQUEST, * PMODULE_CHECK_REQUEST;

typedef struct _SUSPICIOUS_PROCESS {
    HANDLE ProcessId;
    WCHAR ProcessName[260];
    ULONG Flags;
} SUSPICIOUS_PROCESS, * PSUSPICIOUS_PROCESS;

#define SUSPICIOUS_FLAG_DEBUGGER     0x0001
#define SUSPICIOUS_FLAG_INJECTOR     0x0002
#define SUSPICIOUS_FLAG_MEMORY_TOOL  0x0004

const WCHAR* SuspiciousProcessList[] = {
    L"cheatengine.exe",
    L"x64dbg.exe",
    L"ollydbg.exe",
    L"ida64.exe",
    L"wireshark.exe",
    L"processhacker.exe",
};

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;
_Dispatch_type_(IRP_MJ_CREATE) DRIVER_DISPATCH CreateDispatch;
_Dispatch_type_(IRP_MJ_CLOSE) DRIVER_DISPATCH CloseDispatch;
_Dispatch_type_(IRP_MJ_DEVICE_CONTROL) DRIVER_DISPATCH DeviceControl;

PDEVICE_OBJECT g_DeviceObject = NULL;
PVOID g_CallbackHandle = NULL;

// Forward declarations so we don't get the redefintion errors.
BOOLEAN IsWindowsProcess(PEPROCESS Process);
OB_PREOP_CALLBACK_STATUS ProcessPreOperationCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation);

// Scans the memory of a target process for suspicious patterns. 
// We first ensure that the process can be accessed safely, attaching to its context to avoid race conditions. 
// The memory is scanned in chunks, checking each region for accessibility and appropriate permissions (e.g., executable regions). 
// The actual scan looks for specific patterns based on the provided flags. Any exceptions during scanning are handled to prevent crashes, ensuring robustness.
// learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-pslookupprocessbyprocessid
// learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-kestackattachprocess
// learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-zwqueryvirtualmemory
// learn.microsoft.com/en-us/windows-hardware/drivers/kernel/exception-handling
NTSTATUS ScanProcessMemory(HANDLE ProcessId, PVOID StartAddress, SIZE_T Size, ULONG ScanFlags) {
    PEPROCESS Process;
    NTSTATUS status = PsLookupProcessByProcessId(ProcessId, &Process);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    __try {
        KAPC_STATE ApcState;
        KeStackAttachProcess(Process, &ApcState);

        // Perform memory scanning within process context
        for (SIZE_T offset = 0; offset < Size; offset += PAGE_SIZE) {
            MEMORY_BASIC_INFORMATION memInfo;
            status = ZwQueryVirtualMemory(
                ZwCurrentProcess(),
                (PVOID)((ULONG_PTR)StartAddress + offset),
                MemoryBasicInformation,
                &memInfo,
                sizeof(memInfo),
                NULL
            );

            if (NT_SUCCESS(status)) {
                // Check memory permissions and scan if accessible
                if (memInfo.State == MEM_COMMIT &&
                    (memInfo.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE))) {
                    // We are scanning most regions of memory in the the windows operating system enviroment.
                    // Our scan flags would be dependent on what we put in here. 
                }
            }
        }

        KeUnstackDetachProcess(&ApcState);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        status = GetExceptionCode();
    }

    ObDereferenceObject(Process);
    return status;
}
// For safety we have to implemenet this function
// We are retriving the PEProcess in corespondence to the ProcessID, If it faults it returns the error status, the reason we do this is to prevent BSOD.
// We enumerate Loaded modules in this case, ZwQuerySystemInformation is called to query system info related to loaded modules in kernel. Then we get the required bugger size to store the list of modules.
NTSTATUS VerifyModules(HANDLE ProcessId, BOOLEAN CheckSignatures) {
    PEPROCESS Process;
    NTSTATUS status = PsLookupProcessByProcessId(ProcessId, &Process);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    // Enumerate loaded modules
    PVOID Modules = NULL;
    SIZE_T ModulesSize = 0;

    status = ZwQuerySystemInformation(
        SystemModuleInformation,
        Modules,
        0,
        (PULONG)&ModulesSize
    );

    if (ModulesSize) {
        #if (NTDDI_VERSION >= NTDDI_WIN10_VB)
            POOL_FLAGS poolFlags = POOL_FLAG_NON_PAGED;
            Modules = ExAllocatePool2(poolFlags, ModulesSize, 'chAC');
        #else
            #pragma warning(suppress: 4996)
            Modules = ExAllocatePoolWithTag(NonPagedPool, ModulesSize, 'chAC');
        #endif
        if (Modules) {
            // Verify each module's integrity
            // Check for unsigned modules if CheckSignatures is TRUE
            ExFreePoolWithTag(Modules, 'chAC');
        }
    }

    ObDereferenceObject(Process);
    return status;
}


// Our I/O control handler for our kernel-mode driver. This processes the IRP (I/O request packet) which contains params such as the control code and data buffer.
// This should return information wether it's intialized to 0 and where our stored number of bytes in returned (though it may not be used in this case)
// We still are using the IOCTL SCAN MEMORY to scan targeted processes however so it's safe to include this (Why not?)
// IOCTL Check modules checks enumerated proccesses if the input bffers length is the size of the MODULE_CHECK_REQUEST it returns the Status of the buffer whether it's too small or not.
// If the IOControlCode doesn't match it indicates tthe request is invaild for the device. 
NTSTATUS DeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);

    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;
    ULONG_PTR information = 0;

    switch (stack->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_SCAN_MEMORY: {
        if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(MEMORY_SCAN_REQUEST)) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        PMEMORY_SCAN_REQUEST request = (PMEMORY_SCAN_REQUEST)Irp->AssociatedIrp.SystemBuffer;
        status = ScanProcessMemory(request->ProcessId, request->StartAddress,
            request->Size, request->ScanFlags);
        break;
    }

    case IOCTL_CHECK_MODULES: {
        if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(MODULE_CHECK_REQUEST)) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        PMODULE_CHECK_REQUEST request = (PMODULE_CHECK_REQUEST)Irp->AssociatedIrp.SystemBuffer;
        status = VerifyModules(request->ProcessId, request->VerifySignatures);
        break;
    }

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = information;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

NTSTATUS CreateDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS CloseDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT DriverObject) {
    if (g_CallbackHandle) {
        ObUnRegisterCallbacks(g_CallbackHandle);
    }

    UNICODE_STRING symbolicLinkName;
    RtlInitUnicodeString(&symbolicLinkName, SYMBOLIC_LINK_NAME);
    IoDeleteSymbolicLink(&symbolicLinkName);
    IoDeleteDevice(DriverObject->DeviceObject);
    // I'm extremely lazy so we are just printing when it's completed.
    KdPrint(("Anti-cheat driver unloaded\n"));
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);

    // Enable debug output (We set this as debug because we don't want to deal with the windows signature check and windows driver check flagging this when loaded).
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "Debug mode enabled\n"));

    NTSTATUS status;
    UNICODE_STRING deviceName;
    UNICODE_STRING symbolicLinkName;

    RtlInitUnicodeString(&deviceName, DEVICE_NAME);
    RtlInitUnicodeString(&symbolicLinkName, SYMBOLIC_LINK_NAME);

    // Set DO_BUFFERED_IO flag for debug builds
    // learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/nf-wdm-iocreatedevice
    // You will have to modify these flags based on which release mode you're doing.
    status = IoCreateDevice(
        DriverObject,
        0,
        &deviceName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN | DO_BUFFERED_IO,
        FALSE,
        &g_DeviceObject
    );

    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = IoCreateSymbolicLink(&symbolicLinkName, &deviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(g_DeviceObject);
        return status;
    }

    DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateDispatch;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseDispatch;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControl;
    DriverObject->DriverUnload = DriverUnload;

    status = RegisterProcessCallback(DriverObject);
    if (!NT_SUCCESS(status)) {
        IoDeleteSymbolicLink(&symbolicLinkName);
        IoDeleteDevice(g_DeviceObject);
        return status;
    }

    KdPrint(("Anti-cheat driver loaded\n"));
    return STATUS_SUCCESS;
}

NTSTATUS RegisterProcessCallback(PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);

    OB_OPERATION_REGISTRATION operationRegistration = { 0 };
    OB_CALLBACK_REGISTRATION callbackRegistration = { 0 };
    UNICODE_STRING altitude = RTL_CONSTANT_STRING(L"321000");

    operationRegistration.ObjectType = PsProcessType;
    operationRegistration.Operations = OB_OPERATION_HANDLE_CREATE;
    operationRegistration.PreOperation = ProcessPreOperationCallback;

    callbackRegistration.Version = OB_FLT_REGISTRATION_VERSION;
    callbackRegistration.OperationRegistrationCount = 1;
    callbackRegistration.RegistrationContext = NULL;
    callbackRegistration.Altitude = altitude;
    callbackRegistration.OperationRegistration = &operationRegistration;

    return ObRegisterCallbacks(&callbackRegistration, &g_CallbackHandle);
}

OB_PREOP_CALLBACK_STATUS ProcessPreOperationCallback(
    PVOID RegistrationContext,
    POB_PRE_OPERATION_INFORMATION OperationInformation
) {
    UNREFERENCED_PARAMETER(RegistrationContext);

    if (OperationInformation->ObjectType != *PsProcessType) {
        return OB_PREOP_SUCCESS;
    }

    PEPROCESS sourceProcess = PsGetCurrentProcess();
    PEPROCESS targetProcess = (PEPROCESS)OperationInformation->Object;

    // Skip if the process is coming from Windows.
    if (IsWindowsProcess(sourceProcess)) {
        return OB_PREOP_SUCCESS;
    }

    // Check for suspicious access flags, Could indicate that a program is trying to write to process memory.
    ACCESS_MASK suspiciousAccess = PROCESS_VM_READ |
        PROCESS_VM_WRITE |
        PROCESS_VM_OPERATION |
        PROCESS_CREATE_THREAD;

    if (OperationInformation->Operation == OB_OPERATION_HANDLE_CREATE &&
        (OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &
            suspiciousAccess)) {

        // Block if it's suspicious.
        OperationInformation->Parameters->CreateHandleInformation.DesiredAccess &=
            ~suspiciousAccess;

        // Print (if it blocked the program)
        KdPrint(("Blocked suspicious access from PID: %d to PID: %d\n",
            PsGetProcessId(sourceProcess),
            PsGetProcessId(targetProcess)));
    }

    return OB_PREOP_SUCCESS;
}

/// Checks if the given process is a Windows system process by examining its image path.
/// This check can be bypassed it's not full proof an attacker can change the image path in the PEB structure
/// Another thing is spoofing the image path through Process hooks or Rootkits
/// Directly Memory Patching by modifying the ImagePathName (though you can easily detect this and issue a ban)
BOOLEAN IsWindowsProcess(PEPROCESS Process) {
    UNICODE_STRING systemRoot;
    KAPC_STATE apcState;
    BOOLEAN result = FALSE;

    RtlInitUnicodeString(&systemRoot, L"\\SystemRoot\\System32\\");

    KeStackAttachProcess(Process, &apcState);
    __try {
        PPEB pPeb = PsGetProcessPeb(Process);
        if (pPeb && pPeb->ProcessParameters) {
            if (RtlPrefixUnicodeString(&systemRoot,
                &pPeb->ProcessParameters->ImagePathName,
                TRUE)) {
                result = TRUE;
            }
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        result = FALSE;
    }
    KeUnstackDetachProcess(&apcState);

    return result;
}
