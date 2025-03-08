import winreg

def reset_registry():
    key_path = r"Software\Microsoft\Windows\CurrentVersion\Policies\System"
    try:
        with winreg.OpenKey(winreg.HKEY_CURRENT_USER, key_path, 0, winreg.KEY_SET_VALUE) as key:
            winreg.SetValueEx(key, "DisableTaskMgr", 0, winreg.REG_DWORD, 0)
            print("Registry value 'DisableTaskMgr' reset successfully.")
    except FileNotFoundError:
        print("Registry key not found. It may not have been modified.")
    except PermissionError:
        print("Permission denied. Run the script as Administrator.")

if __name__ == "__main__":
    print("Resetting registry modifications...")
    reset_registry()
    print("Registry modifications reset successfully!")