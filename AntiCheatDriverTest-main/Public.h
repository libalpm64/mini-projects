/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_KMDFDriver1,
    0xd34b965d,0x0c86,0x47ae,0x85,0xbf,0xcc,0xe2,0x87,0xde,0xc7,0x22);
// {d34b965d-0c86-47ae-85bf-cce287dec722}
