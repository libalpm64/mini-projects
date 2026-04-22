/*++

Module Name:

    device.h

Abstract:

    This file contains the device definitions.

Environment:

    Kernel-mode Driver Framework

--*/

#pragma once

#include <ntddk.h>
#include <wdf.h>

// Device Interface GUID
// {YOUR-GUID-HERE}
DEFINE_GUID(GUID_DEVINTERFACE_KMDFDriver1,
    0x12345678, 0x1234, 0x1234, 0x12, 0x34, 0x12, 0x34, 0x56, 0x78, 0x90, 0x12);

// Device context structure
typedef struct _DEVICE_CONTEXT
{
    ULONG PrivateDeviceData;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

// This macro will generate an inline function called DeviceGetContext
// to retrieve a pointer to the device context memory
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

NTSTATUS
KMDFDriver1CreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
);
