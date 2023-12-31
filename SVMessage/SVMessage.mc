MessageIdTypedef = DWORD

FacilityNames = (
                 None                = 0x000:FAC_NONE
                 System              = 0x0ff:FAC_SYSTEM
                 Appl                = 0xfff:FAC_APPLICATION
                 SVProtoBuf          = 0x101:FAC_SVProtoBuf
                 SVGateway           = 0x102:FAC_SVGATEWAY
                 TriggerRecordController            = 0x103:FAC_TriggerRecordController
                 SVRemoteCtrl            = 0x104:FAC_SVRemoteCtrl
                 SVWebSocket            = 0x105:FAC_SVWebSocket
                 SVIMCommand         = 0x106:FAC_SVIMCOMMAND
                 Unused06            = 0x107:FAC_UNUSED06
                 Unused07            = 0x108:FAC_UNUSED07
                 SVLibrary           = 0x109:FAC_SVLIBRARY
                 Unused08            = 0x10a:FAC_UNUSED08
                 SVSharedMemory      = 0x10b:FAC_SVSHAREDMEMORY
                 Unused10            = 0x10c:FAC_UNUSED10
                 SVObserver          = 0x10d:FAC_SVOBSERVER
                 SVSecurity          = 0x10e:FAC_SVSECURITY
                 Unused11            = 0x10f:FAC_UNUSED11
                 Unused12            = 0x110:FAC_UNUSED12
                 SVCmdComServer      = 0x111:FAC_SVCMDCOMSERVER
                 SVCmdComClient      = 0x112:FAC_SVCMDCOMCLIENT
                 SVDataManager	     = 0x113:FAC_SVDATAMANAGER
                 SVAccess            = 0x114:FAC_SVACCESS
                 Unused13            = 0x115:FAC_UNUSED13
                 Unused14            = 0x116:FAC_UNUSED14
                 SVEquation          = 0x117:FAC_SVEQUATION
                 SVFileAcquisition   = 0x118:FAC_SVFILEACQUISITION
                 SVMatroxGige        = 0x119:FAC_SVMATROXGIGE
                 SVLptIO             = 0x11a:FAC_SVLPTIO
                 SVOLibrary          = 0x11b:FAC_SVOLIBRARY
                 SVSystemLibrary     = 0x11c:FAC_SVSYSTEMLIBRARY
                 SVMatroxLibrary     = 0x11d:FAC_SVMATROXLIBRARY
                 Unused15            = 0x11e:FAC_UNUSED15
                 SVXMLLibrary        = 0x11f:FAC_SVXMLLIBRARY
	         )

SeverityNames = (
                 Success       = 0x00:SEV_SUCCESS
                 Informational = 0x01:SEV_INFORMATIONAL
                 Warning       = 0x02:SEV_WARNING
                 Error         = 0x03:SEV_ERROR
                )

LanguageNames = (USEnglish	= 0x0409:USEnglish)

OutputBase = 16

MessageId = 1
Facility = None
Language = USEnglish
SVProtoBuf
.

MessageId = 2
Facility = None
Language = USEnglish
SVGateway
.

MessageId = 3
Facility = None
Language = USEnglish
TriggerRecordController
.

MessageId = 4
Facility = None
Language = USEnglish
SVRemoteCtrl
.

MessageId = 5
Facility = None
Language = USEnglish
SVWebSocket
.

MessageId = 6
Facility = None
Language = USEnglish
SVIMCOMMAND
.

MessageId = 7
Facility = None
Language = USEnglish
Unused06
.

MessageId = 8
Facility = None
Language = USEnglish
Unused07
.

MessageId = 9
Facility = None
Language = USEnglish
SVLIBRARY
.

MessageId = 10
Facility = None
Language = USEnglish
Unused08
.

MessageId = 11
Facility = None
Language = USEnglish
SVSharedMemory
.

MessageId = 12
Facility = None
Language = USEnglish
Unused10
.

MessageId = 13
Facility = None
Language = USEnglish
SVOBSERVER
.

MessageId = 14
Facility = None
Language = USEnglish
SVSecurity
.

MessageId = 15
Facility = None
Language = USEnglish
Unused11
.

MessageId = 16
Facility = None
Language = USEnglish
Unused12
.

MessageId = 17
Facility = None
Language = USEnglish
SVCMDCOMSERVER
.

MessageId = 18
Facility = None
Language = USEnglish
SVCMDCOMCLIENT
.

MessageId = 19
Facility = None
Language = USEnglish
SVDataManager
.

MessageId = 20
Facility = None
Language = USEnglish
SVAccess
.

MessageId = 21
Facility = None
Language = USEnglish
Unused13
.

MessageId = 22
Facility = None
Language = USEnglish
Unused14
.

MessageId = 23
Facility = None
Language = USEnglish
SVEquation
.

MessageId = 24
Facility = None
Language = USEnglish
SVFileAcquisition
.

MessageId = 25
Facility = None
Language = USEnglish
SVMatroxGige
.

MessageId = 26
Facility = None
Language = USEnglish
SVLptIO
.

MessageId = 27
Facility = None
Language = USEnglish
SVOLibrary
.

MessageId = 28
Facility = None
Language = USEnglish
SVSystemLibrary
.

MessageId = 29
Facility = None
Language = USEnglish
SVMatroxLibrary
.

MessageId = 30
Facility = None
Language = USEnglish
Unused15
.

MessageId = 31
Facility = None
Language = USEnglish
SVXML
.

;//
;// =============== SV ProtoBuf Messages ===============
;//
;// ------------------------------------------------------------
MessageId = 0
Facility = SVProtoBuf
Severity = Error
SymbolicName = SVMSG_SVProtoBuf_GENERAL_ERROR
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

;//
;// =============== SV Gateway Messages ===============
;//
;// ------------------------------------------------------------
MessageId = 0
Facility = SVGateway
Severity = Error
SymbolicName = SVMSG_SVGateway_0_GENERAL_ERROR
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 1
Severity = Warning
SymbolicName = SVMSG_SVGateway_1_GENERAL_WARNING
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 2
Severity = Informational
SymbolicName = SVMSG_SVGateway_2_GENERAL_INFORMATIONAL
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

;//
;// =============== SV Remotecontrol Messages ===============
;//
;// ------------------------------------------------------------
MessageId = 0
Facility = SVRemoteCtrl
Severity = Error
SymbolicName = SVMSG_REMOTECTRL_0_GENERAL_ERROR
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 1
Severity = Warning
SymbolicName = SVMSG_REMOTECTRL_1_GENERAL_WARNING
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 2
Severity = Informational
SymbolicName = SVMSG_REMOTECTRL_2_GENERAL_INFORMATIONAL
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
;//
;// =============== SV Websocket Messages ===============
;//
;// ------------------------------------------------------------
MessageId = 0
Facility = SVWebsocket
Severity = Error
SymbolicName = SVMSG_SVO_0_GENERAL_ERROR
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 1
Severity = Warning
SymbolicName = SVMSG_SVO_1_GENERAL_WARNING
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 2
Severity = Informational
SymbolicName = SVMSG_SVO_2_GENERAL_INFORMATIONAL
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------



;//
;// =============== SVIMCommand Messages ===============
;//

MessageId = 2
Facility = SVIMCommand
Severity = Error
SymbolicName = SVMSG_SVIMCMD_GO_ONLINE_FAILED

Language = USEnglish
Run Mode command failed
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 5
Severity = Warning
SymbolicName = SVMSG_SVIMCMD_REQUEST_REJECTED

Language = USEnglish
The request was rejected by the SVIM
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 11
Severity = Success
SymbolicName = SVMSG_SVIMCMD_RUNNING_TEST

Language = USEnglish
Running in Test Mode
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 21
Severity = Error
SymbolicName = SVMSG_REQUESTED_OBJECTS_ON_DIFFERENT_PPQS
Language = USEnglish
Requesting Objects are not on the same PPQ.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------


MessageId = 22
Severity = Informational
SymbolicName = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST
Language = USEnglish
One or more of the requested object do not exist.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------


MessageId = 24
Severity = Error
SymbolicName = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST
Language = USEnglish
One or more inspections do not exist.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 25
Severity = Error
SymbolicName = SVMSG_TOO_MANY_REQUESTED_ITEMS
Language = USEnglish
Too many requested items.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 26
Severity = Informational
SymbolicName = SVMSG_REQUESTED_LIST_IS_EMPTY
Language = USEnglish
The list of requested items is empty.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------


MessageId = 27
Severity = Error
SymbolicName = SVMSG_PRODUCT_NO_LONGER_AVAILABLE
Language = USEnglish
Product no longer available.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 29
Severity = Error
SymbolicName = SVMSG_CONFIGURATION_NOT_LOADED
Language = USEnglish
Configuration not loaded.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 30
Severity = Warning
SymbolicName = SVMSG_OBJECT_NOT_PROCESSED
Language = USEnglish
Object not processed.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 34
Severity = Informational
SymbolicName = SVMSG_OBJECT_CANNOT_BE_SET_WHILE_ONLINE
Language = USEnglish
Item cannot be set while the SVIM is in a Running Mode (Run, Test, Regression, ...).
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------


MessageId = 35
Severity = Informational
SymbolicName = SVMSG_OBJECT_CANNOT_BE_SET_REMOTELY
Language = USEnglish
Item cannot be set remotely.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 36
Severity = Error
SymbolicName = SVMSG_INCORRECT_ARRAY_DATA_TYPE
Language = USEnglish
The data type passed in the array is the incorrect type.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 37
Severity = Error
SymbolicName = SVMSG_INVALID_IMAGE_SOURCE
Language = USEnglish
The Image Source is not Valid.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 38
Severity = Informational
SymbolicName = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED
Language = USEnglish
Not all of the items in the list could be processed.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 39
Severity = Informational
SymbolicName = SVMSG_REQUEST_IMAGE_NOT_SOURCE_IMAGE
Language = USEnglish
The requested image is not a source image.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 44
Severity = Error
SymbolicName = SVMSG_44_RUNONCE_ONLINE

Language = USEnglish
RunOnce cannot be executed while SVObserver is in Run Mode.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 50
Severity = Error
SymbolicName = SVMSG_50_MODE_CHANGING_ERROR

Language = USEnglish
Set Mode Failed - Svim Mode Changing
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 51
Severity = Error
SymbolicName = SVMSG_51_MODE_CONFIGURATION_LOADING_ERROR

Language = USEnglish
Set Mode Failed - Svim Loading Configuration
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 52
Severity = Error
SymbolicName = SVMSG_52_MODE_GUI_IN_USE_ERROR

Language = USEnglish
Set Mode Failed - GUI in use
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 53
Severity = Informational
SymbolicName = SVMSG_53_SVIM_NOT_IN_RUN_MODE;
Language = USEnglish
Command cannot be executed unless the SVIM is in Run Mode
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 60
Severity = Warning
SymbolicName = SVMSG_INVALID_VARIANT_PARAMETER

Language = USEnglish
SVIM Command Com Control - Variant Parameter Invalid
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 63
Severity = Error
SymbolicName = SVMSG_63_SVIM_IN_WRONG_MODE;
Language = USEnglish
Command cannot be executed due to the SVIM being in the wrong Mode
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 64
Severity = Error
SymbolicName = SVMSG_64_SVIM_MODE_NOT_REMOTELY_SETABLE;
Language = USEnglish
The Mode being Set is not Allowed to be Set Remotely
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------


MessageId = 65
Severity = Informational
SymbolicName = SVMSG_ONE_OR_MORE_OBJECTS_INVALID
Language = USEnglish
One or more objects are invalid.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 66
Severity = Informational
SymbolicName = SVMSG_OBJECT_NOT_FOUND
Language = USEnglish
Object not found.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 67
Severity = Informational
SymbolicName = SVMSG_OBJECT_IN_WRONG_PPQ
Language = USEnglish
Object is in wrong PPQ.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 68
Severity = Informational
SymbolicName = SVMSG_OBJECT_WRONG_TYPE
Language = USEnglish
Object is the wrong type.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 70
Severity = Informational
SymbolicName = SVMSG_OBJECT_CANNOT_BE_SET_INDIRECT_VALUE
Language = USEnglish
Item cannot be set it is a linked value
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------


;//
;// =============== End SVIMCommand Messages ===============
;//

;//
;// =============== SVSharedMemory Messages ===============
;//
;// ------------------------------------------------------------
;//
;// =============== End SVSharedMemory Messages ===============
;//

;//
;// =============== SVLibrary Messages ===============
;//

MessageId = 2
Facility = SVLibrary
Severity = Error
SymbolicName = SVMSG_LIB_REGISTRY_KEY_OPEN_FAILED

Language = USEnglish
Unable to open registry key
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 3
Severity = Error
SymbolicName = SVMSG_LIB_REGISTRY_KEY_CREATE_FAILED

Language = USEnglish
Unable to create registry key
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 4
Severity = Warning
SymbolicName = SVMSG_LIB_SET_REGISTRY_VALUE_FAILED

Language = USEnglish
Unable to set registry value
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 5
Severity = Warning
SymbolicName = SVMSG_LIB_REGISTRY_EXPORT_FAILED

Language = USEnglish
Unable to export all keys to file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 6
Severity = Warning
SymbolicName = SVMSG_LIB_REGISTRY_KEY_EXPORT_FAILED

Language = USEnglish
Unable to export key to file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 7
Severity = Warning
SymbolicName = SVMSG_LIB_REGISTRY_VALUE_EXPORT_FAILED

Language = USEnglish
Unable to export value to file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 8
Severity = Warning
SymbolicName = SVMSG_LIB_REGISTRY_IMPORT_FAILED

Language = USEnglish
Unable to import file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 9
Severity = Warning
SymbolicName = SVMSG_LIB_REGISTRY_INVALID_IMPORT_FILE

Language = USEnglish
Import file does not start with 'REGEDIT4'
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 10
Severity = Warning
SymbolicName = SVMSG_LIB_REGISTRY_IMPORT_EXPECTED_KEY

Language = USEnglish
Import error -- expected key name
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 11
Severity = Error
SymbolicName = SVMSG_LIB_REGISTRY_IMPORT_FILE_IO_ERROR

Language = USEnglish
I/O Error reading registry import file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 12
Severity = Error
SymbolicName = SVMSG_LIB_PACKFILE_IO_ERROR

Language = USEnglish
I/O Error on disk file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

;//
;// =============== End SVLibrary Messages ===============
;//

;//
;// =============== SVObserver Messages ===============
;//
MessageId = 0
Facility = SVObserver

Severity = Success
SymbolicName = SVMSG_SVO_IGNORE_EXCEPTION

Language = USEnglish
Used to throw exceptions that are ignored at the catch area
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 2
Severity = Error
SymbolicName = SVMSG_SVO_UNHANDLED_EXCEPTION

Language = USEnglish
Unhandled Exception %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 3
Severity = Error
SymbolicName = SVMSG_SVO_ACCESS_DENIED

Language = USEnglish
Access Denied
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 20
Severity = Error
SymbolicName = SVMSG_SVO_20_INCORRECT_CAMERA_FILE
Language = USEnglish
Camera file does not match camera.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 25
Severity = Informational
SymbolicName = SVMSG_SVO_25_SVOBSERVER_STARTED
Language = USEnglish
SVObserver - Program has started. %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 26
Severity = Informational
SymbolicName = SVMSG_SVO_26_SVOBSERVER_STOPPED
Language = USEnglish
SVObserver - Program has stopped.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 27
Severity = Informational
SymbolicName = SVMSG_SVO_27_SVOBSERVER_GO_ONLINE
Language = USEnglish
SVObserver - Program has entered Run Mode. %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 28
Severity = Informational
SymbolicName = SVMSG_SVO_28_SVOBSERVER_GO_OFFLINE
Language = USEnglish
SVObserver - Program has gone offline. %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 29
Severity = Informational
SymbolicName = SVMSG_SVO_29_SVOBSERVER_CONFIG_LOADED
Language = USEnglish
SVObserver - Configuration loaded = %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 30
Severity = Warning
SymbolicName = SVMSG_SVO_30_EXCEPTION_IN_MIL
Language = USEnglish
SVObserver - Exception in MIL function (%8)
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 31
Severity = Error
SymbolicName = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL
Language = USEnglish
SVObserver - Exception in External Tool DLL (%8)
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 32
Severity = Error
SymbolicName = SVMSG_SVO_32_ARCHIVE_TOOL_OUT_OF_MEMORY
Language = USEnglish
SVObserver - Archive tool was not able to allocate memory for buffered images (limit %8 MB)
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 33
Severity = Warning
SymbolicName = SVMSG_SVO_33_OBJECT_INDEX_INVALID
Language = USEnglish
SVObserver - Object index invalid (%8)
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 34
Severity = Warning
SymbolicName = SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE
Language = USEnglish
SVObserver - Object index out of result range (%8)
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 37
Severity = Error
SymbolicName = SVMSG_SVO_37_INVALID_RUN_STATUS
Language = USEnglish
SVObserver - Run Status was in an invalid state.  Make sure time between triggers is greater than acquisition time.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 38
Severity = Error
SymbolicName = SVMSG_SVO_38_INPUT_REQUEST_FAILED
Language = USEnglish
SVObserver - Input Request failed
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 39
Severity = Error
SymbolicName = SVMSG_SVO_39_IMAGE_REQUEST_FAILED
Language = USEnglish
SVObserver - Image Request failed
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 40
Severity = Warning
SymbolicName = SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED
Language = USEnglish
Update Main Image Failed: 
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 44
Severity = Warning
SymbolicName = SVMSG_SVO_44_SHARED_MEMORY
Language = USEnglish
SVObserver - Shared Memory = %8 
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 48
Severity = Error
SymbolicName = SVMSG_SVO_48_LOAD_CONFIGURATION_MONITOR_LIST
Language = USEnglish
SVObserver - Loading Configuration failed.  Could not load monitor list.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 49
Severity = Error
SymbolicName = SVMSG_SVO_49_LOAD_CONFIGURATION_TOOLGROUPING
Language = USEnglish
SVObserver - Loading Configuration failed.  Could not load Tool Grouping.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 51
Severity = Warning
SymbolicName = SVMSG_SVO_51_CLIPBOARD_WARNING
Language = USEnglish
SVObserver - Clipboard = %8 
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 52
Severity = Error
SymbolicName = SVMSG_SVO_52_NOMATROX_DONGLE
Language = USEnglish
SVObserver - No Matrox dongle found.
Only limited functionality is available.
Please exit SVObserver, insert a Matrox dongle and restart for full functionality.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.

;// ------------------------------------------------------------
MessageId = 53
Severity = Error
SymbolicName = SVMSG_SVO_53_RESOURCE_DLL_LOADING_FAILED
Language = USEnglish
Could not load the resource dll.
The program will be terminated.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 54
Severity = Informational
SymbolicName = SVMSG_SVO_54_EMPTY
Language = USEnglish
%8 
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 55
Severity = Error
SymbolicName = SVMSG_SVO_55_DEBUG_BREAK_ERROR
Language = USEnglish
SVObserver - Debug Break Error = %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 56
Severity = Error
SymbolicName = SVMSG_SVO_56_INSPECTION_IMPORT_ERROR
Language = USEnglish
Import of inspection failed.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 57
Severity = Error
SymbolicName = SVMSG_SVO_57_PARSERTREE_INSPECTIONCREATE_ERROR
Language = USEnglish
Parser Tree - Creating of Inspections failed.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 58
Severity = Error
SymbolicName = SVMSG_SVO_58_TOOLADJUST_RESET_ERROR
Language = USEnglish
Tool size adjustment failed.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 61
Severity = Error
SymbolicName = SVMSG_SVO_61_RINGBUFFER_ERROR
Language = USEnglish
RingBuffer Tool error: %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 62
Severity = Error
SymbolicName = SVMSG_SVO_62_RINGBUFFER_INVALID_VALUE
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 63
Severity = Error
SymbolicName = SVMSG_SVO_63_LOAD_GLOBAL_CONSTANTS
Language = USEnglish
SVObserver - Loading Configuration failed.  Could not load the Global Constants.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 64
Severity = Informational
SymbolicName = SVMSG_SVO_64_EMPTY_FORMULAS_ARE_NOT_ALLOWED
Language = USEnglish
"Empty formulas are not allowed for sizing or positioning"
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 65
Severity = Warning
SymbolicName = SVMSG_SVO_65_ENTERED_VALUE_INVALID
Language = USEnglish
The value entered is invalid.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 66
Severity = Warning
SymbolicName = SVMSG_SVO_66_GLOBAL_NAME_INVALID
Language = USEnglish
The Global Constant Name [%8] is already being used, edit the constant in the list or enter another name.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 67
Severity = Warning
SymbolicName = SVMSG_SVO_67_MAIN_BRANCH_NOT_CREATED
Language = USEnglish
The main branch [%8] could not be created.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
MessageId = 69
Severity = Error
SymbolicName = SVMSG_SVO_69_PPQ_INDEX_NOT_RELEASED
Language = USEnglish
SVPPQObject::IndexPPQ-cannot release Input Buffer Index
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
MessageId = 70
Severity = Error
SymbolicName = SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT
Language = USEnglish
Invalid Discrete Outputs: Remove all Discrete Outputs and re-add them.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
MessageId = 71
Severity = Error
SymbolicName = SVMSG_SVO_71_INVALID_SOURCE_IMAGE_INDEX
Language = USEnglish
Invalid source image index
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
MessageId = 72
Severity = Error
SymbolicName = SVMSG_SVO_72_UNHANDLED_EXCEPTION
Language = USEnglish
An unhandled SVObserver exception, with the following Message number [%8], has occured
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
MessageId = 73
Severity = Error
SymbolicName = SVMSG_SVO_73_ARCHIVE_MEMORY
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 74
Severity = Error
SymbolicName = SVMSG_SVO_74_LOAD_FILE
Language = USEnglish
SVObserver - The loading of the following file failed:
Should loading continue ?
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 76
Severity = Error
SymbolicName = SVMSG_SVO_76_CONFIGURATION_HAS_OBSOLETE_ITEMS
Language = USEnglish
The configuration contains obsolete items and cannot be loaded.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 77
Severity = Error
SymbolicName = SVMSG_SVO_77_LOAD_VERSION_NUMBER_FAILED
Language = USEnglish
The loading of the file failed: the Version Number could not be loaded.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 78
Severity = Error
SymbolicName = SVMSG_SVO_78_LOAD_IO_FAILED
Language = USEnglish
The loading of the file failed. Because of following error in the IO section: 
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 79
Severity = Error
SymbolicName = SVMSG_SVO_79_LOAD_PPQ_FAILED
Language = USEnglish
The loading of the file failed. Because of following error in the PPQ section: 
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 80
Severity = Error
SymbolicName = SVMSG_SVO_80_LOAD_INSPECTION_FAILED
Language = USEnglish
The loading of the file failed.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 82
Severity = Error
SymbolicName = SVMSG_SVO_82_NO_ENVIROMENT_TAG
Language = USEnglish
The file could not be loaded: The environment tag is missing.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 83
Severity = Error
SymbolicName = SVMSG_SVO_83_SAX_PARSER_ERROR
Language = USEnglish
The file could not be loaded: The SAX Parser found following error:
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 84
Severity = Error
SymbolicName = SVMSG_SVO_84_SAX_PARSER_UNEXPECTED_ERROR
Language = USEnglish
The file could not be loaded: There was an unexpected error in the SAXParser:
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 87
Severity = Error
SymbolicName = SVMSG_SVO_87_GOONLINE_CAMERA_ERROR
Language = USEnglish
Go online: Camera error: 
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 88
Severity = Error
SymbolicName = SVMSG_SVO_88_LOADING_SCINTILLA_DLL_ERROR
Language = USEnglish
Could not load the scintilla dll.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 89
Severity = Warning
SymbolicName = SVMSG_SVO_89_ARCHIVE_TOOL_SYNCHRONOUS_MODE
Language = USEnglish
Caution:  Using this tool to archive images in Synchronous mode may cause serious delays in inspection processing
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 90
Severity = Error
SymbolicName = SVMSG_SVO_90_MATROX_SERVICE_NOT_RUNNING
Language = USEnglish
The acquisition dll [%8] could not be loaded because the Matrox Gige service is not running.
The Matrox Gige service needs to be started in Windows Services
Camera acquisition will not be possible until this problem is solved.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 91
Severity = Error
SymbolicName = SVMSG_SVO_91_LOAD_INSPECTION_FAILED
Language = USEnglish
The loading of the file failed. The Inspection could not be loaded.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 92
Severity = Error
SymbolicName = SVMSG_SVO_92_GENERAL_ERROR
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 93
Severity = Warning
SymbolicName = SVMSG_SVO_93_GENERAL_WARNING
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 94
Severity = Informational
SymbolicName = SVMSG_SVO_94_GENERAL_Informational
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 95
Severity = Error
SymbolicName = SVMSG_SVO_95_NO_CONFIGURATION_OBJECT
Language = USEnglish
No configuration object available.
SVObserver has no configuration loaded
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 96
Severity = Error
SymbolicName = SVMSG_SVO_96_DOTTED_NAME_NOT_UNIQUE
Language = USEnglish
The dotted name is not unique
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 97
Severity = Error
SymbolicName = SVMSG_SVO_97_CONFIGURATION_TOO_OLD
Language = USEnglish
Configuration prior to 4.3 are not allowed.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 98
Severity = Error
SymbolicName = SVMSG_SVO_98_TREE_ITEM
Language = USEnglish
Could not insert the item into the tree.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 99
Severity = Error
SymbolicName = SVMSG_SVO_NULL_POINTER
Language = USEnglish
Null pointer access avoided!. This should not happen
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 100
Severity = Error
SymbolicName = SVMSG_SVO_NON_PAGED_MEMORY_FULL
Language = USEnglish
Matrox buffer allocation failed, Non-Paged memory is probably full.
To change this setting call MIL Config and change the allocated Non-Paged memory
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 101
Severity = Warning
SymbolicName = SVMSG_SVO_NON_PAGED_MEMORY_LOW
Language = USEnglish
Matrox Non-Paged memory is running low
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 102
Severity = Warning
SymbolicName = SVMSG_SVO_CONDITIONAL_HISTORY
Language = USEnglish
This configuration has Conditional History attributes set, these have been reset as they are now deprecated.
Use the Run Reject Server with Monitored List instead.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 103
Severity = Error
SymbolicName = SVMSG_SVO_103_REPLACE_ERROR_TRAP
Language = USEnglish
SVObserver - Legacy Error Trap (%8)
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 104
Severity = Warning
SymbolicName = SVMSG_SVO_104_TA_DIALOG_CLOSING_ERROR
Language = USEnglish
Tool error while leaving TA-dialog - %8
Do you want to leave TA-dialog anyway?
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 105
Severity = Error
SymbolicName = SVMSG_SVO_105_CIRCULAR_REFERENCE
Language = USEnglish
A circular reference has been generated
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// 
MessageId = 106
Severity = Warning
SymbolicName = SVMSG_SVO_106_MONITOR_LIST_OBJECT_MISSING
Language = USEnglish
A monitor list object: %8; is not available, should it be deleted from the monitor list?
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
MessageId = 107
Facility = SVObserver
Severity = Error
SymbolicName = SVMSG_INVALID_VALUE_POINTER
Language = USEnglish
TODO
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.

;// ------------------------------------------------------------
MessageId = 108
Facility = SVObserver
Severity = Error
SymbolicName = SVMSG_INVALID_SIZE
Language = USEnglish
TODO
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.

;// ------------------------------------------------------------
MessageId = 109
Facility = SVObserver
Severity = Error
SymbolicName = SVMSG_NEGATIVE_POSITION
Language = USEnglish
TODO
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.

;// ------------------------------------------------------------
MessageId = 110
Facility = SVObserver
Severity = Error
SymbolicName = SVMSG_SVO_TO_LARGE_POSITION
Language = USEnglish
TODO
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 114
Severity = Error
SymbolicName = SVMSG_SVO_114_ASSERT_CONDITION_FAILED
Language = USEnglish
An assert condition was not fulfilled: 
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------



;// ------------------------------------------------------------
;// ------------------------------------------------------------
;//
;// ------------------------------------------------------------
;// Jim's messages starting at 5001

MessageId = 5001
Severity = Error
SymbolicName = SVMSG_SVO_5001_NULLPARENT
Language = USEnglish
Parent dialog is NULL.  This should not be able to happen.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5008
Severity = Error
SymbolicName = SVMSG_SVO_5008_NULLTOOL
Language = USEnglish
The associated Tool is NULL.  This should not be able to happen.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5016
Severity = Error
SymbolicName = SVMSG_SVO_5016_COULDNOTADDTAB
Language = USEnglish
Could not add Source tab to Image Control.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5017
Severity = Error
SymbolicName = SVMSG_SVO_5017_COULDNOTADDTAB
Language = USEnglish
Could not add Result tab to Image Control.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5018
Severity = Error
SymbolicName = SVMSG_SVO_5018_NULLIMAGE
Language = USEnglish
Source image is NULL.  This should not be able to happen.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5023
Severity = Error
SymbolicName = SVMSG_SVO_5023_ROIGETIMAGEHANDLEFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to use the ROI image.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5026
Severity = Error
SymbolicName = SVMSG_SVO_5026_OUTPUTGETIMAGEHANDLEFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to use the Output image.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5032
Severity = Error
SymbolicName = SVMSG_SVO_5032_GETINTERPOLATIONMODEFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to retrieve the Interpolation Mode.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5033
Severity = Error
SymbolicName = SVMSG_SVO_5033_GETOVERSCANFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to retrieve the Overscan.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5042
Severity = Error
SymbolicName = SVMSG_SVO_5042_COULDNOTGETSOURCEIMAGE;
Language = USEnglish
Resize: Could not get Source Image.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5043
Severity = Error
SymbolicName = SVMSG_SVO_5043_COULDNOTGETDESTINATIONIMAGE;
Language = USEnglish
Resize: Could not get Destination Image.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5044
Severity = Error
SymbolicName = SVMSG_SVO_5044_INVALIDINTERPOLATIONMODE;
Language = USEnglish
Resize: Invalid Interpolation Mode.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5045
Severity = Error
SymbolicName = SVMSG_SVO_5045_INVALIDOVERSCAN;
Language = USEnglish
Resize: Invalid Overscan value.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5047
Severity = Error
SymbolicName = SVMSG_SVO_5047_GETINPUTIMAGEFAILED;
Language = USEnglish
Resize: Could not retrieve the source image within ResetObject.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5048
Severity = Error
SymbolicName = SVMSG_SVO_5048_INITIALIZEROIIMAGEFAILED;
Language = USEnglish
Resize: Could not initialize the ROI image within ResetObject.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5049
Severity = Error
SymbolicName = SVMSG_SVO_5049_INITIALIZEOUTPUTIMAGEFAILED;
Language = USEnglish
Resize: Could not initialize the Output Image within ResetObject.
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5051
Severity = Error
SymbolicName = SVMSG_SVO_5051_DRIVEDOESNOTEXIST
Language = USEnglish
Expected drive does not exist:  %8.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.

;// ------------------------------------------------------------
MessageId = 5052
Severity = Error
SymbolicName = SVMSG_SVO_5052_DRIVENOTNTFSFORMAT
Language = USEnglish
Drive is not the required NTFS format:  %8.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5067
Severity = Error
SymbolicName = SVMSG_SVO_5067_IMAGEALLOCATIONFAILED;
Language = USEnglish
Image Allocation Failed.
Image:  %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5079
Severity = Error
SymbolicName = SVMSG_SVO_5079_CREATEBUFFERFAILED;
Language = USEnglish
Create Buffer Failed
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5080
Severity = Error
SymbolicName = SVMSG_SVO_5080_CREATEFILEMAPPINGFAILED;
Language = USEnglish
Create Shared Memory failed with error
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 5081
Severity = Error
SymbolicName = SVMSG_SVO_5081_MAPVIEWOFFileFAILED
Language = USEnglish
Map sharedmemory failed with errornumber:
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

MessageId = 5082
Severity = Error
SymbolicName = SVMSG_SVO_5082_UNMAPVIEWOFFileFAILED
Language = USEnglish
Unmap sharedmemory failed with errornumber:
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// End of Jim's messages starting at 5001



;// ------------------------------------------------------------
;//
;// =============== End SVObserver Messages ===============
;//

;//
;// =============== SVSecurity Messages ===============
;//
MessageId = 2
Facility = SVSecurity
Severity = Informational
SymbolicName = SVMSG_SVS_ACCESS_GRANTED

Language = USEnglish
Login - Access Granted
%8
.
;// ------------------------------------------------------------

MessageId = 3
Severity = Warning
SymbolicName = SVMSG_SVS_ACCESS_DENIED

Language = USEnglish
Login - Access Denied
%8
.
;// ------------------------------------------------------------

MessageId = 4
Severity = Warning
SymbolicName = SVMSG_SVS_ACCESS_LOGGED_OUT

Language = USEnglish
%1User: %8 Has Logged out.
.
;//
;// =============== End SVSecurity Messages ===============
;//

;//
;// =============== SVAccess Messages ===============
;//
MessageId = 0
Facility = SVAccess
Severity = Informational
SymbolicName = SECURITY_POINT_ACCESS_POINTS

Language = USEnglish
Access Points
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 1
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU

Language = USEnglish
File Menu
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 2
Severity = Informational
SymbolicName = SECURITY_POINT_ROOT

Language = USEnglish
Root
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 3
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_NEW

Language = USEnglish
New
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 4
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION

Language = USEnglish
Select Configuration
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 5
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION

Language = USEnglish
Close Configuration
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 6
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION_AS

Language = USEnglish
Save Configuration As
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 7
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION    

Language = USEnglish
Save Configuration
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 8
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_PRINT                 

Language = USEnglish
Print
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 9
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_PRINT_SETUP

Language = USEnglish
Print Setup
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 10
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_SAVE_IMAGE            

Language = USEnglish
Save Image
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 11
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_RECENT_CONFIGURATIONS 

Language = USEnglish
Recent Configurations
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 12
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_EXIT                  

Language = USEnglish
Exit
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 13
Severity = Informational
SymbolicName = SECURITY_POINT_VIEW_MENU
           
Language = USEnglish
View Menu
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 14
Severity = Informational
SymbolicName = SECURITY_POINT_VIEW_MENU_PPQ_BAR       

Language = USEnglish
PPQ Bar
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 15
Severity = Informational
SymbolicName = SECURITY_POINT_VIEW_MENU_IMAGE_DISPLAY_UPDATE

Language = USEnglish
Image Display Update
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 16
Severity = Informational
SymbolicName = SECURITY_POINT_MODE_MENU

Language = USEnglish
Mode Menu
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 17
Severity = Informational
SymbolicName = SECURITY_POINT_MODE_MENU_RUN

Language = USEnglish
Run
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 18
Severity = Informational
SymbolicName = SECURITY_POINT_MODE_MENU_STOP

Language = USEnglish
Stop
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 19
Severity = Informational
SymbolicName = SECURITY_POINT_MODE_MENU_EDIT_TOOLSET

Language = USEnglish
Edit Toolset
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 21
Severity = Informational
SymbolicName = SECURITY_POINT_MODE_MENU_TEST

Language = USEnglish
Test
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 22
Severity = Informational
SymbolicName = SECURITY_POINT_MODE_MENU_REGRESSION_TEST

Language = USEnglish
Regression Test
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 23
Severity = Informational
SymbolicName = SECURITY_POINT_EXTRAS_MENU    

Language = USEnglish
Extras Menu
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 24
Severity = Informational
SymbolicName = SECURITY_POINT_EXTRAS_MENU_ADDITIONAL_ENVIRON

Language = USEnglish
Additional Environment Settings
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 25
Severity = Informational
SymbolicName = SECURITY_POINT_EXTRAS_MENU_SECURITY_MANAGER

Language = USEnglish
Security Manager
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 26
Severity = Informational
SymbolicName = SECURITY_POINT_EXTRAS_MENU_TEST_OUTPUTS

Language = USEnglish
Test Outputs
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 27
Severity = Informational
SymbolicName = SECURITY_POINT_EXTRAS_MENU_UTILITIES_SETUP

Language = USEnglish
Utilities Setup
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 28
Severity = Informational
SymbolicName = SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN

Language = USEnglish
Utilities Run
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 29
Severity = Informational
SymbolicName = SECURITY_POINT_UNRESTRICTED_FILE_ACCESS

Language = USEnglish
Unrestricted File Access
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 30
Severity = Informational
SymbolicName = SECURITY_POINT_VIEW_MENU_RESET_COUNTS_CURRENT       

Language = USEnglish
Reset Counts Current IP
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 31
Severity = Informational
SymbolicName = SECURITY_POINT_VIEW_MENU_RESET_COUNTS_ALL       

Language = USEnglish
Reset Counts All IPs
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 32
Severity = Informational
SymbolicName = SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE       

Language = USEnglish
Exit Run Mode
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 33
Severity = Informational
SymbolicName = SECURITY_POINT_VIEW_MENU_RESULT_DISPLAY_UPDATE

Language = USEnglish
Result Display Update
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 34
Severity = Informational
SymbolicName = SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION

Language = USEnglish
Automatically Save Configuration
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 35
Severity = Informational
SymbolicName = SECURITY_POINT_EDIT_MENU

Language = USEnglish
Edit Menu
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 36
Severity = Informational
SymbolicName = SECURITY_POINT_EDIT_MENU_RESULT_PICKER

Language = USEnglish
Results Picker
%1User: %8
.
;// ------------------------------------------------------------
MessageId = 37
Severity = Informational
SymbolicName = SECURITY_POINT_EXTRAS_MENU_FBWF_CONFIGURATION

Language = USEnglish
Enable File Based Write Filter
%1User: %8
.
;// ------------------------------------------------------------

;//
;// =============== End SVAccess Messages ===============
;//

;//
;// =============== SVCmdComServer Messages ===============
;//
MessageId = 1
Facility = SVCmdComServer
Severity = Warning
SymbolicName = SVMSG_CMDCOMSRV_ERROR

Language = USEnglish
SVIM Command Com Server - Error
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 3
Severity = Warning
SymbolicName = SVMSG_CMDCOMSRV_MEMORY_ERROR

Language = USEnglish
SVIM Command Com Server - Memory Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 5
Severity = Warning
SymbolicName = SVMSG_CMDCOMSRV_FILE_ERROR

Language = USEnglish
SVIM Command Com Server - File Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 7
Severity = Warning
SymbolicName = SVMSG_NO_PPQ_FOUND

Language = USEnglish
SVIM Command Com Control - No PPQ Found
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------

;//
;// =============== End SVCmdComServer Messages ===============
;//

;//
;// =============== SVCmdComClient Messages ===============
;//
MessageId =
Facility = SVCmdComClient
Severity = Warning
SymbolicName = SVMSG_CMDCOMCTRL_FILE_ERROR

Language = USEnglish
SVIM Command Com Control - File Error
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
;//
;// =============== End SVCmdComClient Messages ===============
;//


;//
;// =============== SVEQUATION Messages ===============
;//
MessageId = 100
Facility = SVEquation

Severity = Error
SymbolicName = SVMSG_TOO_MANY_VARIABLES

Language = USEnglish
Equation Parser Error : Too Many Variables - %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
;//
;// =============== End SVEQUATION Messages ===============
;//

;// ------------------------------------------------------------
;//
;// =============== SVFILEACQUISITION Messages ===============
;//
MessageId = 118
Facility = SVFileAcquisition

Severity = Error
SymbolicName = SVMSG_IMAGE_FORMAT_ERROR

Language = USEnglish
File Acquisition Error : Image Format Error - %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;//
MessageId = 119
Facility = SVFileAcquisition

Severity = Error
SymbolicName = SVMSG_IMAGE_LOAD_ERROR

Language = USEnglish
File Acquisition Error : Image Load Error - %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
;//
;// =============== End SVFILEACQUISITION Messages ===============
;//

;//
;// =============== SVMATROXGIGE Messages ===============
;//
MessageId = 0
Facility = SVMatroxGige

Severity = Success
SymbolicName = SVMSG_SVMATROXGIGE_NO_ERROR

Language = USEnglish
No Error - %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;//
MessageId = 1
Facility = SVMatroxGige

Severity = Error
SymbolicName = SVMSG_MATROX_GIGE_RECONNECT_ERROR

Language = USEnglish
Matrox Gige Error : Reconnect Error - %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;//
MessageId = 2
Facility = SVMatroxGige

Severity = Error
SymbolicName = SVMSG_MATROX_GIGE_DISCONNECT_ERROR

Language = USEnglish
Matrox Gige Error : Disconnect Error - %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
;//
;// =============== End SVMATROXGIGE Messages ===============
;//

;// ------------------------------------------------------------
;//
;// =============== SVSystemLibrary Messages ===============
;//
MessageId = 2
Facility = SVSystemLibrary

Severity = Error
SymbolicName = SVMSG_THREAD_CREATION_ERROR

Language = USEnglish
SVThread : Thread Creation Error - %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
;//
;// =============== End SVSystemLibrary Messages ===============
;//
;// ------------------------------------------------------------
;//
;// =============== TriggerRecordController Messages ===============
;//
MessageId = 1
Facility = TriggerRecordController
Severity = Error
SymbolicName = SVMSG_TRC_GENERAL_ERROR
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 2
Severity = Warning
SymbolicName = SVMSG_TRC_GENERAL_WARNING
Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
;//
;// =============== End TriggerRecordController Messages ===============
;// ------------------------------------------------------------
;//
;// =============== SVLptIO Messages ===============
;//
MessageId = 1
Facility = SVLptIO

Severity = Warning
SymbolicName = SVMSG_INVALID_LINE_STATE

Language = USEnglish
SVLptIO Invalid Line State - %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 2
Severity = Error
SymbolicName = SVMSG_LPTIO_INITIALIZATION_FAILED

Language = USEnglish
SVLptIO Initialization failed - %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 3
Severity = Error
SymbolicName = SVMSG_IO_BOARD_VERSION

Language = USEnglish
IO Board invalid version - %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
;//
;// =============== End SVLptIO Messages ===============
;//

;//
;// =============== SVMatroxLibrary Messages ===============
;//
MessageId = 0
Facility = SVMatroxLibrary

Severity = Success
SymbolicName = SVMSG_SVMATROXLIBRARY_NO_ERROR

Language = USEnglish
No Error - %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
;//
MessageId = 0
Facility = SVMatroxLibrary

Severity = Error
SymbolicName = SVMSG_SVMATROXLIBRARY_UNKNOWN_FATAL_ERROR

Language = USEnglish
Error - %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
;//
MessageId = 1
Facility = SVMatroxLibrary

Severity = Error
SymbolicName = SVMSG_SVMATROXLIBRARY_GERNEAL_ERROR

Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
;//
MessageId = 2
Facility = SVMatroxLibrary

Severity = Warning
SymbolicName = SVMSG_SVMATROXLIBRARY_GERNEAL_WARNING

Language = USEnglish
%8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
MessageId = 3
Facility = SVMatroxLibrary

Severity = Warning
SymbolicName = SVMSG_SVMATROXLIBRARY_GENERAL_ERROR_NOT_IN_RUNMODE

Language = USEnglish
Matroxerror in non RunMode - %8
%1#Details#
%1Source File: %2:[%3] (%4)
%1Compiled:    %5 %6
%1Object:    %7
.
;// ------------------------------------------------------------
;//
;// =============== End SVMATROXGIGE Messages ===============
;//
