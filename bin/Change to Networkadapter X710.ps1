function Get-IniFile {
    param(
        [parameter(Mandatory = $true)] [string] $filePath,
        [string] $anonymous = 'NoSection',
        [switch] $comments,
        [string] $commentsSectionsSuffix = '_',
        [string] $commentsKeyPrefix = 'Comment'
    )
    $ini = @{}
    switch -regex -file ($filePath) {
        "^\[(.+)\]$" {
            # Section
            $section = $matches[1]
            $ini[$section] = @{}
            $CommentCount = 0
            if ($comments) {
                $commentsSection = $section + $commentsSectionsSuffix
                $ini[$commentsSection] = @{}
            }
            continue
        }
        "^(;.*)$" {
            # Comment
            if ($comments) {
                if (!($section)) {
                    $section = $anonymous
                    $ini[$section] = @{}
                }
                $value = $matches[1]
                $CommentCount = $CommentCount + 1
                $name = $commentsKeyPrefix + $CommentCount
                $commentsSection = $section + $commentsSectionsSuffix
                $ini[$commentsSection][$name] = $value
            }
            continue
        }

        "^(.+?)\s*=\s*(.*)$" {
            # Key
            if (!($section)) {
                $section = $anonymous
                $ini[$section] = @{}
            }
            $name, $value = $matches[1..2]
            $ini[$section][$name] = $value
            continue
        }
    }
    return $ini
}
function New-IniContent {
    [cmdletbinding()]
    param(
        [parameter(ValueFromPipeline)] [hashtable] $data,
        [string] $anonymous = 'NoSection'
    )
    process {
        $iniData = $data
        
        if ($iniData.Contains($anonymous)) {
            $iniData[$anonymous].GetEnumerator() |  ForEach-Object {
                Write-Output "$($_.Name)=$($_.Value)"
            }
            Write-Output ''
        }
        $iniData.GetEnumerator() | ForEach-Object {
            $sectionData = $_
            if ($sectionData.Name -ne $anonymous) {
                Write-Output "[$($sectionData.Name)]"
                $iniData[$sectionData.Name].GetEnumerator() |  ForEach-Object {
                    Write-Output "$($_.Name)=$($_.Value)"
                }
            }
            Write-Output ''
        }
    }
}

$OemInfoIniPath = "C:\Windows\System32\OEMINFO.INI"
function Switch-ModelNo
{
    param (
        [string] $newModelNo
    )
    $OemInfoIni = Get-IniFile  $OemInfoIniPath
    $ModelNoSplit = $OemInfoIni["OEMSpecific"]["ModelNo"].Split(" ")
    $ModelNoSplit[1] = $newModelNo
    $OemInfoIni["OEMSpecific"]["ModelNo"] = [System.String]::Join(" ", $ModelNoSplit)

    $ModelSplit = $OemInfoIni["general"]["Model"].Split(" ")
    $ModelSplit[3] = $newModelNo
    $OemInfoIni["general"]["Model"] = [System.String]::Join(" ", $ModelSplit)

    return $OemInfoIni
}

$changeOEMInfo = Switch-ModelNo "61"
Remove-Item $OemInfoIniPath -Force
New-IniContent  $changeOEMInfo | Out-File -FilePath $OemInfoIniPath

Remove-NetIPAddress -InterfaceAlias "GigE10G_Port_001" -Confirm:$false
New-NetIPAddress -InterfaceAlias "GigE10G_Port_001" -AddressFamily IPv4 -IPAddress 192.168.101.1 -PrefixLength 24
Restart-NetAdapter -Name "GigE10G_Port_001"

Remove-NetIPAddress -InterfaceAlias "GigE10G_Port_002" -Confirm:$false
New-NetIPAddress -InterfaceAlias "GigE10G_Port_002" -AddressFamily IPv4 -IPAddress 192.168.102.1 -PrefixLength 24
Restart-NetAdapter -Name "GigE10G_Port_002"

Remove-NetIPAddress -InterfaceAlias "GigE10G_Port_003" -Confirm:$false
New-NetIPAddress -InterfaceAlias "GigE10G_Port_003" -AddressFamily IPv4 -IPAddress 192.168.103.1 -PrefixLength 24
Restart-NetAdapter -Name "GigE10G_Port_003"

Remove-NetIPAddress -InterfaceAlias "GigE10G_Port_004" -Confirm:$false
New-NetIPAddress -InterfaceAlias "GigE10G_Port_004" -AddressFamily IPv4 -IPAddress 192.168.104.1 -PrefixLength 24
Restart-NetAdapter -Name "GigE10G_Port_004"


