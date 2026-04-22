# AntiCheatDriverTest
<img src="https://tokei.rs/b1/github/libalpm64/AntiCheatDriverTest?category=code&style=flat" alt="Lines of Code"/>

Based on the KMDFDriver Framework Expands on implementing how you would make a kernel level driver anti-cheat do certain actions.

Things covered:
- Memory address reading
- Preventing executables / drivers from being able to interact with the Game process (elf.exe) if they're not by the windows operating system.
- Checks for debuggers (This was a kindfold test, it doesn't really matter)
- Checks for suspicious processes on the computer.

You need the following to get this to work:
WDK (Window Driver Development kit): https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk
Windows SDK: https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/
MSVC v143 - VS 2022 C++ x64/x86 Spectre-mitigated libs

This project was made for some giggles use it on a Virtual Machine as when you add onto it you will experience blue screens a lot.
This is a "basic" implementation, anti-cheats are very complex and it's demostrated by just writing a kernel level driver here this took a lot of time and effort so make sure you star it if you like it.
I don't really care what you do with it.

Resources:
https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/_kernel/
