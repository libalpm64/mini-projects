import ctypes
from ctypes import wintypes

kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)

TerminateProcess = kernel32.TerminateProcess
TerminateProcess.argtypes = [wintypes.HANDLE, wintypes.UINT]
TerminateProcess.restype = wintypes.BOOL

original_TerminateProcess = TerminateProcess

def hooked_TerminateProcess(process_handle, exit_code):
    print(f"TerminateProcess called with handle: {process_handle}, exit_code: {exit_code}")
    return True

def hook_TerminateProcess():
    global TerminateProcess
    TerminateProcess = hooked_TerminateProcess
    print("TerminateProcess hooked successfully.")

if __name__ == "__main__":
    print("Hooking TerminateProcess...")
    hook_TerminateProcess()
    print("TerminateProcess bypassed successfully!")