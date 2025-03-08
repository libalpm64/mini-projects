import ctypes
from ctypes import wintypes

user32 = ctypes.WinDLL("user32", use_last_error=True)

UnhookWindowsHookEx = user32.UnhookWindowsHookEx
UnhookWindowsHookEx.argtypes = [wintypes.HHOOK]
UnhookWindowsHookEx.restype = wintypes.BOOL

# Version 2.12.09 of lockdown browser
WH_KEYBOARD_LL_OFFSET = 0x10017000
WH_SHELL_OFFSET = 0x10017004
WH_MOUSE_OFFSET = 0x10017008


def unhook_anti_measures():
    WH_KEYBOARD_LL = ctypes.cast(WH_KEYBOARD_LL_OFFSET, ctypes.POINTER(ctypes.c_void_p)).contents.value
    WH_SHELL = ctypes.cast(WH_SHELL_OFFSET, ctypes.POINTER(ctypes.c_void_p)).contents.value
    WH_MOUSE = ctypes.cast(WH_MOUSE_OFFSET, ctypes.POINTER(ctypes.c_void_p)).contents.value

    if WH_KEYBOARD_LL:
        print("Unhooking WH_KEYBOARD_LL...")
        UnhookWindowsHookEx(WH_KEYBOARD_LL)
    else:
        print("WH_KEYBOARD_LL not found or already unhooked.")

    if WH_SHELL:
        print("Unhooking WH_SHELL...")
        UnhookWindowsHookEx(WH_SHELL)
    else:
        print("WH_SHELL not found or already unhooked.")

    if WH_MOUSE:
        print("Unhooking WH_MOUSE...")
        UnhookWindowsHookEx(WH_MOUSE)
    else:
        print("WH_MOUSE not found or already unhooked.")

if __name__ == "__main__":
    print("Attempting to unhook anti-measures...")
    unhook_anti_measures()
    print("Anti-measures unhooked successfully!")