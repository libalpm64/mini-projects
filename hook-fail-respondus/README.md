# LockDown Browser Analysis

## Introduction
LockDown Browser, developed by Respondus, is a custom browser designed to create a secure testing environment during online exams. It prevents users from accessing external resources or performing unauthorized actions, such as opening additional applications or using keyboard shortcuts. While similar to other secure browsers like the ETS Secure Browser, LockDown Browser distinguishes itself by providing warnings before closing programs and restoring system settings upon exit.
Like all of these 'secure' browser they can be easily intercepted they don't invoke a kernel level driver like usual. 
## Main Module (LockdownBrowser.exe)
The main module is responsible for most of the browser's functionality, including:

- Rendering webpages using the Chromium Embedded Framework (CEF).
- Applying security protections.
- Reporting detections to the server.

When analyzing the binary, it becomes evident that the executable is packed and obfuscated. Static analysis tools like IDA Pro initially yield limited results due to the obfuscation. However, runtime analysis reveals the use of `GetSystemInfo` and other system calls.

Despite the obfuscation, the imports are relatively intact, allowing for easier reverse engineering. Key functions related to detection reporting and system modifications can be identified through dynamic analysis.

## External Module (LockdownBrowser.dll)
The external module handles low-level hooks for the keyboard, mouse, and shell. Unlike the main module, this DLL is neither obfuscated nor virtualized, making it straightforward to analyze.

One notable function, `CLDBDoSomeStuff`, sets up three types of Windows hooks:

- **Keyboard Hook (`WH_KEYBOARD_LL`)**: Blocks key combinations like `Alt+Tab`, `Ctrl+Esc`, and others.
- **Mouse Hook (`WH_MOUSE`)**: Restricts mouse actions based on configuration flags.
- **Shell Hook (`WH_SHELL`)**: Monitors system events, such as window creation.

The keyboard hook callback performs extensive checks to block specific keycodes. For example:

- `Alt+Tab` triggers a detection flag and reports it to the main application.
- Other blocked keys include system keys (e.g., `Win`, `Esc`) and function keys (e.g., `F2–F24`).

## Registry Modifications
LockDown Browser modifies Windows registry keys to disable certain system features, such as Task Manager (`DisableTaskMgr`). These changes are applied once during initialization and are not rechecked later. As a result, resetting the registry values can restore access to restricted features.

## Blacklist & Whitelist
Upon startup, LockDown Browser scans for blacklisted processes and prompts the user to close them. If the user declines, the browser terminates the process using `TerminateProcess`.

The blacklist is stored in a custom array structure containing process names and window titles. By analyzing memory pointers, it is possible to dump the list of blacklisted processes.

### Bypassing Blacklist Checks
Two approaches can be used to bypass blacklist restrictions:

1. **Hook `TerminateProcess`**: Intercept calls to `TerminateProcess` and spoof a successful return or pass a NULL handle.
2. **Clear the Blacklist Array**: Modify the array's length to zero, preventing the browser from detecting blacklisted processes.

## VM Detection
LockDown Browser includes mechanisms to detect virtual machines (VMs). It scans for specific driver names, system names, and hardware information using the `CPUID` instruction. If a VM is detected, the browser displays a warning or shuts down.

### Bypassing VM Detection
The VM detection logic can be bypassed by patching the relevant function. Since the function does not return a value, replacing its instructions with a simple `retn` instruction effectively disables the check.

## Miscellaneous Detections
### Integrity Checks
The browser verifies the integrity of its executable using `WinVerifyTrust`. If the binary has been modified, the browser displays an error message and shuts down.

### Secondary Monitor Detection
LockDown Browser detects the presence of secondary monitors and requires users to disconnect them before proceeding. This detection can be bypassed by hooking functions like `EnumDisplayMonitors` and `EnumDisplayDevicesA` to spoof the return values.

### Miscellaneous Modifications
#### Clipboard Clearing
When the browser gains focus, it clears the clipboard using `OpenClipboard`, `EmptyClipboard`, and `CloseClipboard`. This behavior can be prevented by hooking `EmptyClipboard` and forcing it to return `true`.

#### Shell Window Hiding
The browser hides the taskbar (shell window) during initialization and restores it upon exit. This is joke to bypass because you can call usermode32.dll to reshow your window and bam you're back in business. 

### Conclusion
This was heavily inspiried from the article: https://systemfailu.re/2020/11/14/lockdownbrowser-analysis/
However they failed to provide like a full code sample, I have provided all of these in python and an IDA plugin to automatically dump the offsets. Enjoy if you found it useful. 
