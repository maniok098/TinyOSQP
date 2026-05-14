## How to setup TwinCAT 4024 on win11 24H2 


1. Disable core isolation (permenantly)
2. Open powershell as Admin and set execution policy to bypass `Set-ExecutionPolicy Bypass`
3. Turn off `Tamper Protection` / `Manipulationsschutz` (DE) temporarily
4. Execute this script `disable_vbs_persistent.ps1`
    ```powershell
    #requires -RunAsAdministrator
    <#
    .SYNOPSIS
    Persistently disable VBS (HVCI/Memory Integrity) and Credential Guard on Windows 11.

    .DESCRIPTION
    Sets *policy* keys under HKLM:\SOFTWARE\Policies\Microsoft\Windows\DeviceGuard
    and *runtime* keys under HKLM:\SYSTEM\CurrentControlSet\Control\DeviceGuard and Lsa.
    Also disables hvservice, turns HypervisorLaunchType off, and (optionally) disables DISM features.

    NOTE: Temporarily turnc OFF "Tamper Protection" in Windows-Sicherheit -> Viren- & Bedrohungsschutz,
            otherwise Defender can revert some settings.
    #>

    Write-Host "== Persistently disable VBS/HVCI/Credential Guard ==" -ForegroundColor Cyan

    # 0) Admin check
    $principal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
    if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    throw "Bitte PowerShell *als Administrator* starten."
    }

    # 1) Turn off memory integrity via policy (Device Guard policy keys)
    $pol = 'HKLM:\SOFTWARE\Policies\Microsoft\Windows\DeviceGuard'
    New-Item -Path $pol -Force | Out-Null
    New-ItemProperty -Path $pol -Name EnableVirtualizationBasedSecurity -PropertyType DWord -Value 0 -Force | Out-Null
    New-ItemProperty -Path $pol -Name RequirePlatformSecurityFeatures -PropertyType DWord -Value 0 -Force | Out-Null
    New-ItemProperty -Path $pol -Name HypervisorEnforcedCodeIntegrity -PropertyType DWord -Value 0 -Force | Out-Null
    New-ItemProperty -Path $pol -Name HVCIMATRequired -PropertyType DWord -Value 0 -Force | Out-Null
    New-ItemProperty -Path $pol -Name LsaCfgFlags -PropertyType DWord -Value 0 -Force | Out-Null

    # 2) Turn off runtime keys
    $dg = 'HKLM:\SYSTEM\CurrentControlSet\Control\DeviceGuard'
    New-Item -Path $dg -Force | Out-Null
    New-ItemProperty -Path $dg -Name EnableVirtualizationBasedSecurity -PropertyType DWord -Value 0 -Force | Out-Null
    New-ItemProperty -Path $dg -Name RequirePlatformSecurityFeatures -PropertyType DWord -Value 0 -Force | Out-Null

    # Scenarios
    $hvcikey = Join-Path $dg 'Scenarios\HypervisorEnforcedCodeIntegrity'
    New-Item -Path $hvcikey -Force | Out-Null
    New-ItemProperty -Path $hvcikey -Name Enabled -PropertyType DWord -Value 0 -Force | Out-Null

    $cgkey = Join-Path $dg 'Scenarios\CredentialGuard'
    New-Item -Path $cgkey -Force | Out-Null
    New-ItemProperty -Path $cgkey -Name Enabled -PropertyType DWord -Value 0 -Force | Out-Null

    # LSA isolation
    $lsa = 'HKLM:\SYSTEM\CurrentControlSet\Control\Lsa'
    New-ItemProperty -Path $lsa -Name LsaCfgFlags -PropertyType DWord -Value 0 -Force | Out-Null

    # 3) Stop/disable hypervisor related service
    Try {
    Stop-Service hvservice -ErrorAction SilentlyContinue
    Set-Service hvservice -StartupType Disabled -ErrorAction SilentlyContinue
    } Catch {}

    # 4) Ensure hypervisor does not launch at boot
    & bcdedit /set {current} hypervisorlaunchtype off | Out-Null

    # 5) Optional: make sure DISM features are disabled (won't error if already off)
    $features = @(
    'Microsoft-Hyper-V-All',
    'VirtualMachinePlatform',
    'Windows-Hypervisor-Platform',
    'IsolatedUserMode',
    'Windows-Defender-ApplicationGuard',
    'Containers-DisposableClientVM'  # Windows Sandbox
    )
    foreach ($f in $features) {
    Start-Process -FilePath dism.exe -ArgumentList "/Online","/Disable-Feature:$f","/NoRestart" -Wait -WindowStyle Hidden
    }

    Write-Host "Done. Please reboot, then check msinfo32. If it still shows a hypervisor, disable VT-d/IOMMU in BIOS." -ForegroundColor Green

    ```
5. Reboot the PC
6. Run the script `C:\TwinCAT\3.1\System\win8settick`
7. Reboot the PC
8. Turn on `Tamper Protection` 