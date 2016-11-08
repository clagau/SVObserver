$MTXsrvName = "Matrox GigE Vision Assistant Service"
$RRSsrvName = "Run Reject Server"
$waitLoop=0
$maxLoop=30
$WaitTimeMs=500

write-eventlog -logname Application -source SVException -eventID 13 -entrytype Information -message "Starting SVObserver powershell script" -Category 0

imdisk -d -m V:
if ($LastExitCode -ne 0) {
  echo "Could not create empty V-Drive via imdisk"
  write-eventlog -logname Application -source SVException -eventID 13  -entrytype Warning -message "Could not create empty V-Drive via imdisk"  -Category 0
}

c:\SVObserver\bin\SVRemoveKeyboards.exe

dskcache.exe -w -p


#stop-service $MTXsrvName
restart-service $MTXsrvName



# format the V:\ drive
imdisk -a -s 6G -m V: -p "/fs:ntfs /q /y"
if ($LastExitCode -ne 0) {
  echo "Could not format imdisk for V-drive"
  write-eventlog -logname Application -source SVException -eventID 13  -entrytype Warning -message "Could not format imdisk for V-drive"  -Category 0
}

# On a good run through InitializeIOSubsystem takes 13 seconds on both 
C:\SVObserver\bin\InitializeIOSubsystem.exe
# disable write cache and prove protect


# Check the screen bit depth and set it to 16bit
C:\SVObserver\bin\qres.exe /S | find "16 bits"
if ($LastExitCode -ne 0) {
  echo "Display changed to 16bit"
  write-eventlog -logname Application -source SVException -eventID 13 -entrytype Information -message "The V-drive could not be initialized in time"  -Category 0
  & C:\SVObserver\bin\qres.exe /c:16
} else {
  echo "No display chnage"
}

#stop-service $RRSsrvName
restart-service $RRSsrvName

$MTXservicePrior = Get-Service $MTXsrvName 
$RRSservicePrior = Get-Service $RRSsrvName 
# make a loop until V-drive is ready and Matrox service is started
do{
  $waitLoop++
  Start-Sleep -m $WaitTimeMs
  $booleanVDrive = (New-Object System.IO.DriveInfo("V:")).DriveType -ne 'NoRootDirectory'
  $booleanMatroxService = $MTXservicePrior.status -eq "Running"
  $booleanRRSService = $RRSservicePrior.status -eq "Running"
  Write-Progress -activity "Initialize SVObserver" -status "Progress: ($waitLoop / $maxLoop)" -PercentComplete (($waitLoop / $maxLoop)  * 100)
  echo "Waiting Loop $waitLoop for Matrox $booleanMatroxService, RRS $booleanRRSService and V-Drive $booleanVDrive"
}
# Test until timout or (V-Drive ready and Martox service ready)
while($waitLoop -ne $maxLoop -and ($booleanVDrive -eq $false -or $booleanMatroxService -eq $false -or $booleanRRSService -eq $false))
if ($booleanVDrive -eq $false){
  write-eventlog -logname Application -source SVException -eventID 13 -entrytype Warning -message "The V-drive could not be initialized in time"  -Category 0
  echo "Could not initialize V-Drive"
} else {
  if ($booleanMatroxService -eq $false) {
    write-eventlog -logname Application -source SVException -eventID 25 -entrytype Warning -message "The Matrox service could not be started in time"  -Category 0
    echo "Could not start Matrox Service"
  } else {
    if ($booleanRRSService -eq $false) {
        write-eventlog -logname Application -source SVException -eventID 1 -entrytype Warning -message "The RRS service could not be started in time"  -Category 0
        echo "Could not start RRS Service"
    } else {
        echo "Everything is fine"
    }
  }
}
C:\SVObserver\bin\SVObserver.exe
start /MIN C:\SVObserver\bin\SyncSystemTime.bat