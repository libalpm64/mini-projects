﻿function Run-Command($command) {
    try {
        Invoke-Expression $command
    } catch {
        return $_.Exception.Message
    }
}

function Check-Status {
    $data = @{}
    
    $data['Report Generated By'] = "Snoop"
    $data['Report Generated On'] = (Get-Date -Format "yyyy-MM-dd HH:mm:ss")

    $data[''] = "=== Windows System Information ==="
    
    $osInfo = Get-CimInstance -ClassName Win32_OperatingSystem
    $productName = $osInfo.Caption
    $version = $osInfo.Version
    $buildNumber = [int]$osInfo.BuildNumber
    
    try {
        $releaseId = Get-ItemProperty -Path "HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion" -ErrorAction Stop
        $displayVersion = $releaseId.DisplayVersion
        $ubr = $releaseId.UBR
    } catch {
        $displayVersion = "Unknown"
        $ubr = ""
    }

    $featureUpdate = switch ($buildNumber) {
        {$_ -ge 26100} { "24H2" }
        {$_ -ge 25915} { "24H2" }
        {$_ -ge 25400} { "23H2" }
        {$_ -ge 22621} { "22H2" }
        {$_ -ge 22000} { "21H2" }
        default { $displayVersion }
    }

    if ($version -eq "10.0" -and $buildNumber -ge 22000) {
        $data['Windows Version'] = "Windows 11 $productName (Build $buildNumber.$ubr, Feature Update: $featureUpdate)"
    } elseif ($version -eq "10.0" -and $buildNumber -lt 22000) {
        $featureUpdate10 = switch ($buildNumber) {
            {$_ -ge 19045} { "22H2" }
            {$_ -ge 19044} { "21H2" }
            {$_ -ge 19043} { "21H1" }
            {$_ -ge 19042} { "20H2" }
            default { $displayVersion }
        }
        $data['Windows Version'] = "Windows 10 $productName (Build $buildNumber.$ubr, Feature Update: $featureUpdate10)"
    }

    $data['System Language'] = (Get-WinSystemLocale).DisplayName
    $data['Time Zone'] = (Get-TimeZone).DisplayName
    $data['System Uptime'] = [TimeSpan]::FromSeconds((Get-CimInstance Win32_OperatingSystem).LastBootUpTime.ToOADate()).ToString("dd\.hh\:mm\:ss")

    $computerSystem = Get-WmiObject Win32_ComputerSystem
    $data['Computer Name'] = $computerSystem.Name
    $data['Computer Model'] = $computerSystem.Model
    $data['Domain/Workgroup'] = $computerSystem.Domain
    $data['Username'] = [System.Security.Principal.WindowsIdentity]::GetCurrent().Name

    $data[''] = "`n=== Security Features ==="
    
    $data['IOMMU / VT-X'] = Run-Command "(Get-WmiObject -Class Win32_Processor).VirtualizationFirmwareEnabled"
    
    $hyperv = Get-WindowsOptionalFeature -Online -FeatureName Microsoft-Hyper-V-All -ErrorAction SilentlyContinue
    $data['Hyper-V'] = if ($hyperv.State -eq "Enabled") { "Enabled" } else { "Disabled" }
    
    $vmp = Get-WindowsOptionalFeature -Online -FeatureName VirtualMachinePlatform -ErrorAction SilentlyContinue
    $data['Virtual Machine Platform'] = if ($vmp.State -eq "Enabled") { "Enabled" } else { "Disabled" }
    
    $dseStatus = Run-Command "bcdedit | Select-String TESTSIGNING"
    $data['Driver Signature Enforcement'] = if ($dseStatus -match "Yes") { "Disabled" } else { "Enabled" }

    try {
        $hvciStatus = Get-ItemProperty -Path 'HKLM:\System\CurrentControlSet\Control\DeviceGuard' -Name EnableVirtualizationBasedSecurity -ErrorAction SilentlyContinue
        $data['HVCI'] = if ($hvciStatus.EnableVirtualizationBasedSecurity -eq 1) { "Enabled" } else { "Disabled" }
    } catch {
        $data['HVCI'] = "Registry key not found"
    }

    try {
        $coreIsolationStatus = Get-ItemProperty -Path 'HKLM:\System\CurrentControlSet\Control\DeviceGuard' -Name HVCIMode -ErrorAction SilentlyContinue
        $data['Core Isolation'] = if ($coreIsolationStatus.HVCIMode -eq 1) { "Enabled" } else { "Disabled" }
    } catch {
        $data['Core Isolation'] = "Registry key not found"
    }

    $data[''] = "`n=== TPM Information ==="
    
    $tpm = Get-Tpm
    if ($tpm) {
        $data['TPM Manufacturer'] = $tpm.ManufacturerIdTxt
        $data['TPM Manufacturer Version'] = $tpm.ManufacturerVersion
        $data['TPM Physical Presence Version'] = $tpm.PhysicalPresenceVersionInfo
        $data['TPM Specification Version'] = $tpm.SpecVersion
        $data['TPM Enabled'] = $tpm.TpmEnabled
        $data['TPM Activated'] = $tpm.TpmActivated
        $data['TPM Owner Auth'] = $tpm.AutoProvisioning
    } else {
        $data['TPM Status'] = "Not Available"
    }

    $data[''] = "`n=== Additional Security Features ==="
    
    $cfgStatus = Run-Command "Get-ProcessMitigation -System"
    $data['Control Flow Guard'] = if ($cfgStatus -and !$cfgStatus.SystemCall.DisableUserModeCallbackFilter) { "Enabled" } else { "Disabled" }
    
    $depStatus = Run-Command "wmic OS Get DataExecutionPrevention_Available"
    $data['DEP'] = if ($depStatus -match "TRUE") { "Enabled" } else { "Disabled" }
    
    try {
        $aslrStatus = Get-ItemProperty -Path 'HKLM:\System\CurrentControlSet\Control\Session Manager\Memory Management' -Name MoveImages -ErrorAction SilentlyContinue
        $data['Mandatory ASLR'] = if ($aslrStatus.MoveImages -eq 1) { "Enabled" } else { "Disabled" }
    } catch {
        $data['Mandatory ASLR'] = "Registry key not found"
    }

    try {
        $bottomUpAslr = Get-ItemProperty -Path 'HKLM:\System\CurrentControlSet\Control\Session Manager\Memory Management' -Name BottomUp -ErrorAction SilentlyContinue
        $data['Bottom-up ASLR'] = if ($bottomUpAslr.BottomUp -eq 1) { "Enabled" } else { "Disabled" }
    } catch {
        $data['Bottom-up ASLR'] = "Registry key not found"
    }

    try {
        $defenderStatus = Run-Command "Get-MpComputerStatus | Select-Object -ExpandProperty AMServiceEnabled"
        $data['Windows Defender'] = if ($defenderStatus -eq $true) { "Enabled" } else { "Disabled" }
    } catch {
        $data['Windows Defender'] = "Not Available"
    }

    $data[''] = "`n=== WSL Status ==="
    try {
        $wslStatus = Run-Command "wsl --status"
        $data['WSL Installed'] = if ($wslStatus) { "Yes" } else { "No" }
        $wslDistros = Run-Command "wsl --list --verbose"
        $data['WSL Distributions'] = if ($wslDistros) { $wslDistros } else { "None" }
    } catch {
        $data['WSL Status'] = "Not Available"
    }

    $data[''] = "`n=== USB Devices ==="
    $usbDevices = Get-WmiObject Win32_USBHub
    $i = 1
    foreach ($device in $usbDevices) {
        $data["USB Device ${i} Name"] = $device.Name
        $data["USB Device ${i} DeviceID"] = $device.DeviceID
        $data["USB Device ${i} Status"] = $device.Status
        $i++
    }

    $data[''] = "`n=== USB Storage Devices ==="
    $usbStorage = Get-WmiObject Win32_DiskDrive | Where-Object { $_.InterfaceType -eq "USB" }
    $i = 1
    foreach ($drive in $usbStorage) {
        $data["USB Storage ${i} Model"] = $drive.Model
        $data["USB Storage ${i} Size"] = "$([math]::Round($drive.Size / 1GB, 2)) GB"
        $data["USB Storage ${i} Status"] = $drive.Status
        $i++
    }

    $data[''] = "`n=== Power Status ==="
    $powerStatus = Get-WmiObject Win32_Battery
    if ($powerStatus) {
        $data['Battery Present'] = "Yes"
        $data['Battery Status'] = $powerStatus.Status
        $data['Battery Charge'] = "$($powerStatus.EstimatedChargeRemaining)%"
    } else {
        $data['Power Type'] = "AC Power"
    }
    
    $currentPlan = Run-Command "powercfg /getactivescheme"
    $data['Power Plan'] = if ($currentPlan) { $currentPlan } else { "Unknown" }

    $data[''] = "`n=== Critical Services Status ==="
    $criticalServices = @(
        "wuauserv",      
        "WinDefend",     
        "mpssvc",        
        "DiagTrack",     
        "sppsvc"         
    )
    foreach ($service in $criticalServices) {
        $svc = Get-Service -Name $service -ErrorAction SilentlyContinue
        $data["Service - $($svc.DisplayName)"] = if ($svc) { $svc.Status } else { "Not Found" }
    }

    $data[''] = "`n=== Network Configuration ==="
    $networkAdapters = Get-WmiObject Win32_NetworkAdapterConfiguration | Where-Object { $_.IPEnabled -eq $true }
    $i = 1
    foreach ($adapter in $networkAdapters) {
        $data["Network ${i} Description"] = $adapter.Description
        $data["Network ${i} IP Address"] = $adapter.IPAddress -join ", "
        $data["Network ${i} Subnet"] = $adapter.IPSubnet -join ", "
        $data["Network ${i} Gateway"] = $adapter.DefaultIPGateway -join ", "
        $data["Network ${i} DNS"] = $adapter.DNSServerSearchOrder -join ", "
        $data["Network ${i} DHCP Enabled"] = $adapter.DHCPEnabled
        $i++
    }

    $data[''] = "`n=== Windows Features Status ==="
    $features = @(
        "Microsoft-Windows-Subsystem-Linux",
        "VirtualMachinePlatform",
        "Microsoft-Hyper-V",
        "Containers-DisposableClientVM",
        "Microsoft-Windows-Sandbox"
    )
    foreach ($feature in $features) {
        $status = Get-WindowsOptionalFeature -Online -FeatureName $feature -ErrorAction SilentlyContinue
        $data["Feature - $feature"] = if ($status) { $status.State } else { "Not Available" }
    }

    $data[''] = "`n=== System Performance ==="
    $processor = Get-WmiObject Win32_Processor
    $data['CPU Load'] = "$($processor.LoadPercentage)%"
    $os = Get-WmiObject Win32_OperatingSystem
    $data['Memory Usage'] = "$([math]::Round(($os.TotalVisibleMemorySize - $os.FreePhysicalMemory) / $os.TotalVisibleMemorySize * 100, 2))%"
    $data['Available Memory'] = "$([math]::Round($os.FreePhysicalMemory / 1MB, 2)) GB"
    
    $pageFile = Get-WmiObject Win32_PageFileUsage
    if ($pageFile) {
        $data['Page File Location'] = $pageFile.Name
        $data['Page File Size'] = "$($pageFile.AllocatedBaseSize) MB"
        $data['Page File Usage'] = "$($pageFile.CurrentUsage) MB"
    }

    $data[''] = "`n=== System Protection ==="
    try {
        $restoreStatus = Run-Command "vssadmin list shadowstorage"
        $data['System Restore Status'] = if ($restoreStatus) { "Enabled" } else { "Disabled" }
    } catch {
        $data['System Restore Status'] = "Unknown"
    }

    $data[''] = "`n=== Anti-Cheat Information ==="
    
    $drivers = Get-WindowsDriver -Online -All
    $data['Valorant Anti-Cheat (vgk.sys)'] = if ($drivers | Where-Object {$_.OriginalFileName -like "*vgk.sys*"}) { "Present" } else { "Not Found" }
    $data['EasyAntiCheat Driver (EasyAntiCheat.sys)'] = if ($drivers | Where-Object {$_.OriginalFileName -like "*EasyAntiCheat.sys*"}) { "Present" } else { "Not Found" }
    $data['FaceIT Driver (faceit.sys)'] = if ($drivers | Where-Object {$_.OriginalFileName -like "*faceit.sys*"}) { "Present" } else { "Not Found" }
    $data['ACE Driver (ace64.sys)'] = if ($drivers | Where-Object {$_.OriginalFileName -like "*ace64.sys*"}) { "Present" } else { "Not Found" }

    $data[''] = "`n=== Hardware Information ==="
    
    $motherboard = Get-WmiObject Win32_BaseBoard
    $data['Motherboard Manufacturer'] = $motherboard.Manufacturer
    $data['Motherboard Model'] = $motherboard.Product
    $data['Motherboard Serial'] = $motherboard.SerialNumber
    
    $bios = Get-WmiObject Win32_BIOS
    $data['BIOS Manufacturer'] = $bios.Manufacturer
    $data['BIOS Version'] = $bios.Version
    $data['BIOS Serial'] = $bios.SerialNumber

    $cpu = Get-WmiObject Win32_Processor
    $data['CPU Model'] = $cpu.Name
    $data['CPU Vendor ID'] = $cpu.ProcessorId
    $data['CPU Serial'] = $cpu.SerialNumber
    $data['CPU Cores'] = $cpu.NumberOfCores
    $data['CPU Logical Processors'] = $cpu.NumberOfLogicalProcessors
    $data['CPU Max Clock Speed'] = "$($cpu.MaxClockSpeed) MHz"

    $ram = Get-WmiObject Win32_PhysicalMemory
    $totalRAM = 0
    $i = 1
    foreach ($module in $ram) {
        $data["RAM Module ${i} Manufacturer"] = $module.Manufacturer
        $data["RAM Module ${i} Serial"] = $module.SerialNumber
        $data["RAM Module ${i} Capacity"] = "$([math]::Round($module.Capacity / 1GB, 2)) GB"
        $data["RAM Module ${i} Speed"] = "$($module.Speed) MHz"
        $totalRAM += $module.Capacity
        $i++
    }
    $data['Total RAM'] = "$([math]::Round($totalRAM / 1GB, 2)) GB"

    $data[''] = "`n=== Graphics Information ==="
    
    $gpus = Get-WmiObject Win32_VideoController
    $i = 1
    foreach ($gpu in $gpus) {
        $data["GPU ${i} Model"] = $gpu.Name
        $data["GPU ${i} Driver Version"] = $gpu.DriverVersion
        $data["GPU ${i} Video Memory"] = "$([math]::Round($gpu.AdapterRAM / 1GB, 2)) GB"
        $data["GPU ${i} Resolution"] = "$($gpu.CurrentHorizontalResolution) x $($gpu.CurrentVerticalResolution)"
        $data["GPU ${i} Refresh Rate"] = "$($gpu.CurrentRefreshRate) Hz"
        $i++
    }

    $data[''] = "`n=== Monitor Information ==="
    
    $monitors = Get-WmiObject WmiMonitorID -Namespace root\wmi
    $i = 1
    foreach ($monitor in $monitors) {
        $manufacturerName = [System.Text.Encoding]::ASCII.GetString($monitor.ManufacturerName).Trim("`0")
        $productCode = [System.Text.Encoding]::ASCII.GetString($monitor.ProductCodeID).Trim("`0")
        $serialNumber = [System.Text.Encoding]::ASCII.GetString($monitor.SerialNumberID).Trim("`0")
        $data["Monitor ${i} Manufacturer"] = $manufacturerName
        $data["Monitor ${i} Product Code"] = $productCode
        $data["Monitor ${i} Serial"] = $serialNumber
        $i++
    }

    $data[''] = "`n=== Storage Information ==="
    
    $disks = Get-WmiObject Win32_DiskDrive
    $i = 1
    foreach ($disk in $disks) {
        $data["Disk ${i} Model"] = $disk.Model
        $data["Disk ${i} Serial"] = $disk.SerialNumber
        $data["Disk ${i} Size"] = "$([math]::Round($disk.Size / 1GB, 2)) GB"
        $data["Disk ${i} Interface"] = $disk.InterfaceType
        $i++
    }

    $data[''] = "`n=== Network Adapters ==="
    
    $nics = Get-WmiObject Win32_NetworkAdapter | Where-Object { $_.PhysicalAdapter -eq $true }
    $i = 1
    foreach ($nic in $nics) {
        $data["Network Adapter ${i}"] = $nic.Name
        $data["Network Adapter ${i} MAC"] = $nic.MACAddress
        $i++
    }

    $data[''] = "`n=== System Identifiers ==="
    
    $data['System SID'] = ([System.Security.Principal.WindowsIdentity]::GetCurrent()).User.Value
    $data['Machine GUID'] = (Get-ItemProperty -Path "HKLM:\SOFTWARE\Microsoft\Cryptography" -Name MachineGuid).MachineGuid

    $data[''] = "`n=== PCI Devices ==="
    
    $pciDevices = Get-WmiObject Win32_PnPEntity | Where-Object { $_.PNPClass }
    foreach ($device in ($pciDevices | Sort-Object PNPClass)) {
        if (-not [string]::IsNullOrEmpty($device.Name)) {
            $data["PCI - $($device.PNPClass)"] = "$($device.Name) [$($device.DeviceID)]"
        }
    }

    $data[''] = "`n=== EFI System Details ==="
    $data['Boot Type'] = (Get-WmiObject Win32_ComputerSystem).SecureBootUEFI
    $data['EFI Variables'] = (Run-Command "bcdedit /enum firmware") -join "`n"
    $data['Boot Entries'] = (Run-Command "bcdedit /enum") -join "`n"
    
    $data[''] = "`n=== System Drivers ==="
    $drivers = Get-WmiObject Win32_SystemDriver | Select-Object Name, DisplayName, PathName, State, StartMode
    foreach ($driver in $drivers) {
        $data["Driver - $($driver.Name)"] = "Path: $($driver.PathName) | State: $($driver.State) | Start: $($driver.StartMode)"
    }

    $data[''] = "`n=== Loaded Kernel Modules ==="
    $data['Loaded Modules'] = (Run-Command "driverquery /v") -join "`n"

    $data[''] = "`n=== File System Layout ==="
    $data['Disk Partitions'] = (Get-Partition | Format-Table -AutoSize | Out-String).Trim()
    $data['Volume Info'] = (Get-Volume | Format-Table -AutoSize | Out-String).Trim()
    
    $data[''] = "`n=== Recent System Events ==="
    $data['Driver Events'] = (Get-WinEvent -FilterHashtable @{
        LogName = 'System'
        ID = 7045  
    } -MaxEvents 50 | Format-Table -AutoSize | Out-String).Trim()

    $data[''] = "`n=== Memory Regions ==="
    $data['Memory Map'] = (Get-WmiObject Win32_PhysicalMemory | Format-List | Out-String).Trim()

    $data[''] = "`n=== Boot Configuration ==="
    $data['BCD Store'] = (Run-Command "bcdedit /enum all") -join "`n"

    return $data
}

function Save-ToFile($data) {
    $output = @()
    
    $output += "Snoops Sysinformer"
    $output += "=" * 50
    $output += ""
    
    $categories = [ordered]@{
        'System Information' = @(
            'Windows Version',
            'System Language',
            'Time Zone',
            'System Uptime',
            'Computer Name',
            'Computer Model',
            'Domain/Workgroup',
            'Username',
            'System SID',
            'Machine GUID'
        )
        'Hardware Information' = @(
            'CPU Model',
            'CPU Cores',
            'CPU Logical Processors',
            'CPU Max Clock Speed',
            'Total RAM',
            'Motherboard Manufacturer',
            'Motherboard Model',
            'BIOS Manufacturer',
            'BIOS Version'
        )
        'Graphics and Display' = @(
            {param($key) $key -like "GPU *"},
            {param($key) $key -like "Monitor *"}
        )
        'Memory Modules' = @(
            {param($key) $key -like "RAM Module *"}
        )
        'Storage Devices' = @(
            {param($key) $key -like "Disk *"}
        )
        'Security Features' = @(
            'HVCI',
            'Core Isolation',
            'Control Flow Guard',
            'DEP',
            'Mandatory ASLR',
            'Bottom-up ASLR',
            'Windows Defender',
            'Driver Signature Enforcement'
        )
        'TPM Status' = @(
            'TPM Manufacturer',
            'TPM Manufacturer Version',
            'TPM Enabled',
            'TPM Activated'
        )
        'Virtualization' = @(
            'IOMMU / VT-X',
            'Hyper-V',
            'Virtual Machine Platform'
        )
        'Anti-Cheat Status' = @(
            'Valorant Anti-Cheat (vgk.sys)',
            'EasyAntiCheat Driver',
            'FaceIT Driver',
            'ACE Driver'
        )
        'Network Configuration' = @(
            {param($key) $key -like "Network * Description"},
            {param($key) $key -like "Network * IP *"},
            {param($key) $key -like "Network * DNS"},
            {param($key) $key -like "Network * DHCP *"}
        )
        'Network Adapters' = @(
            {param($key) $key -like "Network Adapter *"}
        )
        'USB Devices' = @(
            {param($key) $key -like "USB Device *"}
        )
        'USB Storage' = @(
            {param($key) $key -like "USB Storage *"}
        )
        'Power Management' = @(
            'Battery Present',
            'Battery Status',
            'Battery Charge',
            'Power Type',
            'Power Plan'
        )
        'System Performance' = @(
            'CPU Load',
            'Memory Usage',
            'Available Memory',
            'Page File Location',
            'Page File Size',
            'Page File Usage'
        )
        'Critical Services' = @(
            {param($key) $key -like "Service - *"}
        )
        'Windows Features' = @(
            {param($key) $key -like "Feature - *"}
        )
 
    }

    foreach ($category in $categories.Keys) {
        $hasContent = $false
        $categoryOutput = @()
        
        $categoryOutput += ""
        $categoryOutput += "=== $category ==="
        $categoryOutput += "-" * ($category.Length + 6)
        
        foreach ($item in $categories[$category]) {
            if ($item -is [ScriptBlock]) {
                $matchingKeys = $data.Keys | Where-Object { & $item $_ } | Sort-Object
                foreach ($key in $matchingKeys) {
                    if ($data[$key]) {
                        $hasContent = $true
                        $categoryOutput += "{0,-35} : {1}" -f $key, $data[$key]
                    }
                }
            } else {
                if ($data[$item]) {
                    $hasContent = $true
                    $categoryOutput += "{0,-35} : {1}" -f $item, $data[$item]
                }
            }
        }

        if ($hasContent) {
            $output += $categoryOutput
        }
    }

    $output | Set-Content -Path "system_check_results.txt" -Encoding UTF8
}

$data = Check-Status
Save-ToFile -data $data
