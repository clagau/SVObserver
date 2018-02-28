$MTXsrvName = "Matrox GigE Vision Assistant Service"
$waitLoop=0
$maxLoop=30
$WaitTimeMs=500

$cmdout = get-WmiObject win32_networkadapterconfiguration -Filter "Description=""Intel(R) PRO/1000 MT Desktop Adapter""" | select-object -expand IPAddress
write-eventlog -logname Application -source SVException -eventID 13 -entrytype Information -message "... Starting SVObserver powershell script. $env:computername - $cmdout" -Category 0
echo $cmdout.IPAddress

$cmdout = imdisk -d -m V:
if ($LastExitCode -ne 0) {
  echo "Could not create empty V-Drive via imdisk"
  write-eventlog -logname Application -source SVException -eventID 13  -entrytype Information -message "Could not create empty V-Drive via imdisk. $cmdout"  -Category 0
}

c:\SVObserver\bin\SVRemoveKeyboards.exe

#Endable disk caching for harddrive
dskcache.exe +w +p

#stop-service $MTXsrvName
restart-service $MTXsrvName


$var = Get-WmiObject -class Win32_processor | ft NumberOfCores

# Get the WMI class Win32_PhysicalMemory and total the capacity of all installed memory modules
[long]$memory = 0
Get-WmiObject -Class Win32_PhysicalMemory | ForEach-Object -Process { $memory += $_.Capacity }
$memory = $memory / 1GB
if ($memory -gt 12){
  # format the V:\ drive 6GB
  $cmdout = imdisk -a -s 3G -m V: -p "/fs:ntfs /q /y"
  if ($LastExitCode -ne 0) {
    echo "Could not format imdisk for V-drive"
    write-eventlog -logname Application -source SVException -eventID 13  -entrytype Warning -message "Could not format imdisk for V-drive 6GB. $cmdout"  -Category 0
  }
  # Wait 5 Seconds
  Start-Sleep -s 20
  # On a good run through InitializeIOSubsystem takes 13 seconds on both 
  C:\SVObserver\bin\InitializeIOSubsystem.exe
  # Wait 5 Seconds
  Start-Sleep -s 20
} else {
  # format the V:\ drive 100MB
  write-eventlog -logname Application -source SVException -eventID 13 -entrytype Information -message "Local RAM is low ($memory). V-Drive will be initialized with 100MB only"  -Category 0
  $cmdout = imdisk -a -s 100M -m V: -p "/fs:ntfs /q /y"
  if ($LastExitCode -ne 0) {
    echo "Could not format imdisk for V-drive"
    write-eventlog -logname Application -source SVException -eventID 13  -entrytype Warning -message "Could not format imdisk for V-drive 100MB. $cmdout"  -Category 0
  }
}




# Check the screen bit depth and set it to 16bit
$cmdout = C:\SVObserver\bin\qres.exe /S | find "16 bits"
if ($LastExitCode -ne 0) {
  echo "Display changed to 16bit"
  write-eventlog -logname Application -source SVException -eventID 13 -entrytype Information -message "The display has been changed to 16bit. $cmdout"  -Category 0
  & C:\SVObserver\bin\qres.exe /c:16
} else {
  echo "No display chnage"
}




$MTXservicePrior = Get-Service $MTXsrvName 


# make a loop until V-drive is ready and Matrox service is started
do{
  $waitLoop++
  Start-Sleep -m $WaitTimeMs
  $booleanVDrive = (New-Object System.IO.DriveInfo("V:")).DriveType -ne 'NoRootDirectory'
  $booleanMatroxService = $MTXservicePrior.status -eq "Running"
 Write-Progress -activity "Initialize SVObserver" -status "Progress: ($waitLoop / $maxLoop)" -PercentComplete (($waitLoop / $maxLoop)  * 100)
  echo "Waiting Loop $waitLoop for Matrox $booleanMatroxService,  V-Drive $booleanVDrive"
}
# Test until timout or (V-Drive ready and Martox service ready)
while($waitLoop -ne $maxLoop -and ($booleanVDrive -eq $false -or $booleanMatroxService -eq $false ))
if ($booleanVDrive -eq $false){
  write-eventlog -logname Application -source SVException -eventID 13 -entrytype Warning -message "The V-drive could not be initialized in time"  -Category 0
  echo "Could not initialize V-Drive"
} else {
  if ($booleanMatroxService -eq $false) {
    write-eventlog -logname Application -source SVException -eventID 25 -entrytype Warning -message "The Matrox service could not be started in time"  -Category 0
    echo "Could not start Matrox Service"
  } 
  else 
  {
    echo "Everything is fine"
	}
}

C:\SVObserver\bin\SVObserver.exe
start-process "C:\SVObserver\bin\RunRejectServer.exe" 
start-process "C:\SVObserver\bin\SyncSystemTime.bat" -WindowStyle Minimized
