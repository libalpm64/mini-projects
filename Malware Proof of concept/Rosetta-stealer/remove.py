import os
import sys
import winreg
import shutil

def remove_malware():
    try:
        # Windows update registry entry (for self-replication)
        try:
            key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, winreg.KEY_ALL_ACCESS)
            winreg.DeleteValue(key, 'Windows Update')
            winreg.CloseKey(key)
        except:
            pass

        # Removes the startup file
        startup_folder = os.path.join(os.getenv('APPDATA'), 'Microsoft', 'Windows', 'Start Menu', 'Programs', 'Startup')
        startup_file = os.path.join(startup_folder, 'winupdate.pyw')
        if os.path.exists(startup_file):
            os.remove(startup_file)

        # Remove AppData for self-replicaiton
        appdata_file = os.path.join(os.getenv('APPDATA'), 'winupdate.pyw')
        if os.path.exists(appdata_file):
            os.remove(appdata_file)
            
        bat_path = os.path.join(os.getenv('TEMP'), "remove.bat")
        with open(bat_path, 'w') as bat_file:
            bat_file.write(f'''@echo off
timeout 3 >nul
del "{os.path.abspath(__file__)}"
del "%~f0"''')
        os.system(f'start "" "{bat_path}"')
        
        print("Malware removal complete. This file will self-delete in 3 seconds.")
        
    except Exception as e:
        print(f"Error during removal: {e}")

if __name__ == "__main__":
    remove_malware()
