Set-PSDebug -Trace 1

$MTXsrvName = "Matrox GigE Vision Assistant Service"
$SVOWebSrvName = "SVO Gateway"
$waitLoop=0
$maxLoop=30
$WaitTimeMs=500

$IpAddress=""
$cmdout = get-WmiObject win32_networkadapterconfiguration -Filter "Description=""Intel(R) I210 Gigabit Network Connection""" | select-object -expand IPAddress
if( $cmdout -ne $null)
{
  $IpAddress = $cmdout[0]
}
write-eventlog -logname Application -source SVException -eventID 13 -entrytype Information -message "... Starting SVObserver powershell script. $env:computername - $IpAddress" -Category 0
echo $IpAddress

$cmdout = imdisk -d -m V:
if ($LastExitCode -ne 0)
{
  echo "Could not create empty V-Drive via imdisk"
  write-eventlog -logname Application -source SVException -eventID 13  -entrytype Information -message "Could not create empty V-Drive via imdisk. $cmdout"  -Category 0
}


C:\Windows\System32\DisplaySwitch.exe /clone

c:\SVObserver\bin\SVRemoveKeyboards.exe

restart-service $MTXsrvName


$var = Get-WmiObject -class Win32_processor | ft NumberOfCores

# Get the WMI class Win32_PhysicalMemory and total the capacity of all installed memory modules
[long]$memory = 0
Get-WmiObject -Class Win32_PhysicalMemory | ForEach-Object -Process { $memory += $_.Capacity }
$memory = $memory / 1GB
if ($memory -gt 48)
{
  # format the V:\ drive 10GB
  $cmdout = imdisk -a -s 10G -m V: -p "/fs:ntfs /q /y"
  if ($LastExitCode -ne 0)
  {
    echo "Could not format imdisk for V-drive"
    write-eventlog -logname Application -source SVException -eventID 13  -entrytype Warning -message "Creation of V-drive (10 GB) with imdisk failed. $cmdout"  -Category 0
  }
}
elseif ($memory -gt 24)
{
  # format the V:\ drive 8GB
  $cmdout = imdisk -a -s 8G -m V: -p "/fs:ntfs /q /y"
  if ($LastExitCode -ne 0)
  {
    echo "Could not format imdisk for V-drive" 
    write-eventlog -logname Application -source SVException -eventID 13  -entrytype Warning -message "Creation of V-drive (8 GB) with imdisk failed. $cmdout"  -Category 0
  }
}
elseif ($memory -gt 12 -and $memory -le 24)
{
  # format the V:\ drive 3GB
  $cmdout = imdisk -a -s 3G -m V: -p "/fs:ntfs /q /y"
  if ($LastExitCode -ne 0)
  {
    echo "Could not format imdisk for V-drive"
    write-eventlog -logname Application -source SVException -eventID 13  -entrytype Warning -message "Creation of V-drive (3 GB) with imdisk failed. $cmdout"  -Category 0
  }
}
else
{
  # format the V:\ drive 100MB
  write-eventlog -logname Application -source SVException -eventID 13 -entrytype Information -message "Local RAM is low ($memory). V-Drive will be initialized with 100MB only"  -Category 0
  $cmdout = imdisk -a -s 100M -m V: -p "/fs:ntfs /q /y"
  if ($LastExitCode -ne 0)
  {
    echo "Could not format imdisk for V-drive"
    write-eventlog -logname Application -source SVException -eventID 13  -entrytype Warning -message "Creation of V-drive (100 MB) with imdisk failed. $cmdout"  -Category 0
  }
}

#Check whether the [IOBoard] code in the model number (taken from the registry) indicates that a RabbitBoard is to be used
$ioboard_code = ((Get-ItemProperty Registry::HKEY_LOCAL_MACHINE\HARDWARE\DESCRIPTION\System\BIOS -name SystemProductName).SystemProductName )[6..7] -join ''
if (($ioboard_code -as [int] -le 22) -and ($ioboard_code -as [int] -gt 0))
{ #if so: initialize RabbitBoard
  # Wait 20 Seconds
  Start-Sleep -s 20
  # On a good run through InitializeIOSubsystem takes 13 seconds on both 
  C:\SVObserver\bin\InitializeIOSubsystem.exe
}
# Wait 20 Seconds
Start-Sleep -s 20

restart-service $SVOWebSrvName

$MTXservicePrior = Get-Service $MTXsrvName 
$SVOWebservicePrior = Get-Service $SVOWebSrvName 


# make a loop until V-drive is ready and Matrox service is started
do
{
  $waitLoop++
  Start-Sleep -m $WaitTimeMs
  $booleanVDrive = (New-Object System.IO.DriveInfo("V:")).DriveType -ne 'NoRootDirectory'
  $booleanMatroxService = $MTXservicePrior.status -eq "Running"
  $booleanSVOWebService = $SVOWebservicePrior.status -eq "Running"
  Write-Progress -activity "Initialize SVObserver" -status "Progress: ($waitLoop / $maxLoop)" -PercentComplete (($waitLoop / $maxLoop)  * 100)
  echo "Waiting Loop $waitLoop for Matrox $booleanMatroxService, SVOWebSrv $booleanSVOWebService and V-Drive $booleanVDrive"
# Test until timout or (V-Drive ready, Martox service and SVO Web Server ready)
}while($waitLoop -ne $maxLoop -and ($booleanVDrive -eq $false -or $booleanMatroxService -eq $false ))

if ($booleanVDrive -eq $false)
{
  write-eventlog -logname Application -source SVException -eventID 13 -entrytype Warning -message "The V-drive could not be initialized in time"  -Category 0
  echo "Could not initialize V-Drive"
}
else
{
  #Create V:\Images Folder + set attribute to "compressed"
  $cmd = New-Item -ItemType Directory -Force -Path V:\Images
  C:\Windows\System32\compact.exe /C /S:V:\Images

  if ($booleanMatroxService -eq $false)
  {
    write-eventlog -logname Application -source SVException -eventID 25 -entrytype Warning -message "The Matrox service could not be started in time"  -Category 0
    echo "Could not start Matrox Service"
  }
  else 
  {
    if ($booleanSVOWebService -eq $false)
    {
        write-eventlog -logname Application -source SVException -eventID 1 -entrytype Warning -message "The SVO Web Server service could not be started in time"  -Category 0
        echo "Could not start SVO Web Server Service"
    }
    else 
    {
       echo "Everything is fine"
    }
  }
}

start-process "C:\SVObserver\bin\SyncSystemTime.bat" -WindowStyle Minimized
invoke-expression -Command "C:\SVObserver\bin\GetTimezoneFromHMI.ps1"
start-process "C:\SVObserver\bin\SVObserver.exe"