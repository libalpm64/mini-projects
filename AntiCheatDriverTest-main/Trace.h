/*++

Module Name:

    Trace.h

Abstract:

    Header file for the debug tracing related function defintions and macros.

Environment:

    Kernel mode

--*/

#pragma once

// Define the tracing flags
#define TRACE_QUEUE       0x00000001
#define TRACE_DRIVER      0x00000002

// Define trace levels if not already defined
#ifndef TRACE_LEVEL_NONE
#define TRACE_LEVEL_NONE        0
#define TRACE_LEVEL_CRITICAL    1
#define TRACE_LEVEL_FATAL       1
#define TRACE_LEVEL_ERROR       2
#define TRACE_LEVEL_WARNING     3
#define TRACE_LEVEL_INFORMATION 4
#define TRACE_LEVEL_VERBOSE     5
#define TRACE_LEVEL_RESERVED6   6
#define TRACE_LEVEL_RESERVED7   7
#define TRACE_LEVEL_RESERVED8   8
#define TRACE_LEVEL_RESERVED9   9
#endif

// WPP tracing configuration
//
// begin_wpp config
// FUNC Trace{FLAG=MYDRIVER_ALL_INFO}(LEVEL, MSG, ...);
// FUNC TraceEvents(LEVEL, FLAGS, MSG, ...);
// end_wpp
//

#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags) \
           WPP_LEVEL_LOGGER(flags)
               
#define WPP_LEVEL_FLAGS_ENABLED(lvl, flags) \
           (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)

// Custom macro to reorder arguments
#define WPP_RECORDER_FLAGS_LEVEL_ARGS(flags, lvl) \
    WPP_RECORDER_LEVEL_FLAGS_ARGS(lvl, flags)
#define WPP_RECORDER_FLAGS_LEVEL_FILTER(flags, lvl) \
    WPP_RECORDER_LEVEL_FLAGS_FILTER(lvl, flags)
