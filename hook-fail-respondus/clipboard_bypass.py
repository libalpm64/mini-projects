import ctypes
from ctypes import wintypes

user32 = ctypes.WinDLL("user32", use_last_error=True)

EmptyClipboard = user32.EmptyClipboard
EmptyClipboard.argtypes = []
EmptyClipboard.restype = wintypes.BOOL

def hooked_EmptyClipboard():
    print("EmptyClipboard called, but bypassed.")
    return True

def hook_EmptyClipboard():
    global EmptyClipboard
    EmptyClipboard = hooked_EmptyClipboard
    print("EmptyClipboard hooked successfully.")

if __name__ == "__main__":
    print("Hooking EmptyClipboard...")
    hook_EmptyClipboard()
    print("EmptyClipboard bypassed successfully!")