[string]$userName = 'SVIMRun'
[string]$userPassword = 'SVIMRun'
[securestring]$secStringPassword = ConvertTo-SecureString $userPassword -AsPlainText -Force
[pscredential]$credObject = New-Object System.Management.Automation.PSCredential ($userName, $secStringPassword)

$ipArray = @('192.168.15.9', '192.168.10.9')
for ($i=0; $i -lt $ipArray.Length; $i++)
{
    If(Test-Connection $ipArray[$i] -Count 2 -Delay 1)
    {
        #Start service on SVIM
        Start-Service -Name "WinRM"

        Echo ("Could reach " + $ipArray[$i])
        Set-Item WSMan:\localhost\Client\TrustedHosts -Value $ipArray[$i] -Force

        #Start service on HMI Panel
        Get-Service -Name "WinRM" -ComputerName $ipArray[$i] | Set-Service -Status Running 

        $timezoneID = Invoke-Command -ComputerName $ipArray[$i] -Credential $credObject -ScriptBlock {(Get-TimeZone).Id}

        Echo ("TimezoneID: " + $timezoneID)
        Set-TimeZone -Id $timezoneID


        Get-Service -Name "WinRM" -ComputerName $ipArray[$i]

        #Stop service on HMI Panel
        (get-service -ComputerName $ipArray[$i] -Name "WinRM").Stop()

        Start-Sleep -Seconds 1
        Get-Service -Name "WinRM" -ComputerName $ipArray[$i]

        #Stop service on SVIM
        Stop-Service -Name "WinRM"

        break

    }
    else
    {
        Echo ('Couldnt connect to ' + $ipArray[$i])
    }
}