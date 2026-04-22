/*++

Module Name:

    driver.h

Abstract:

    This file contains the driver definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#pragma once

#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>
#include <ntimage.h>
#include <ntstrsafe.h>
#include "Device.h"
#include "Queue.h"

EXTERN_C_START

// Only define these if not already defined
#ifndef _MEMORY_INFORMATION_CLASS_DEFINED
#define _MEMORY_INFORMATION_CLASS_DEFINED
typedef enum _MEMORY_INFORMATION_CLASS {
    MemoryBasicInformation
} MEMORY_INFORMATION_CLASS;
#endif

#ifndef _KAPC_STATE_DEFINED
#define _KAPC_STATE_DEFINED
typedef struct _KAPC_STATE {
    LIST_ENTRY ApcListHead[MaximumMode];
    struct _KPROCESS* Process;
    BOOLEAN KernelApcInProgress;
    BOOLEAN KernelApcPending;
    BOOLEAN UserApcPending;
} KAPC_STATE, *PKAPC_STATE;
#endif

#ifndef _SYSTEM_INFORMATION_CLASS_DEFINED
#define _SYSTEM_INFORMATION_CLASS_DEFINED
typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation = 0,
    SystemModuleInformation = 11
} SYSTEM_INFORMATION_CLASS;
#endif

// Add ZwQuerySystemInformation declaration
NTKERNELAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation(
    _In_ SYSTEM_INFORMATION_CLASS SystemInformationClass,
    _Out_writes_bytes_opt_(SystemInformationLength) PVOID SystemInformation,
    _In_ ULONG SystemInformationLength,
    _Out_opt_ PULONG ReturnLength
);

NTKERNELAPI VOID KeStackAttachProcess(_In_ PEPROCESS Process, _Out_ PKAPC_STATE ApcState);
NTKERNELAPI VOID KeUnstackDetachProcess(_In_ PKAPC_STATE ApcState);
NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(_In_ HANDLE ProcessId, _Deref_out_ PEPROCESS* Process);
NTKERNELAPI PPEB PsGetProcessPeb(_In_ PEPROCESS Process);

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD KMDFDriver1EvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP KMDFDriver1EvtDriverContextCleanup;

EXTERN_C_END
