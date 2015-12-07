MessageIdTypedef = DWORD

FacilityNames = (
                 None                = 0x000:FAC_NONE
                 System              = 0x0ff:FAC_SYSTEM
                 Appl                = 0xfff:FAC_APPLICATION
                 Unused01            = 0x101:FAC_UNUSED01
                 SVBatchReport       = 0x102:FAC_SVBATCHREPORT
                 SVFocusNT           = 0x103:FAC_SVFOCUSNT
                 SVFocusDB           = 0x104:FAC_SVFOCUSDB
                 SVFocusDBManager    = 0x105:FAC_SVFOCUSDBMANAGER
                 SVIMCommand         = 0x106:FAC_SVIMCOMMAND
                 SVIPC               = 0x107:FAC_SVIPC
                 SVLanguageManager   = 0x108:FAC_SVLANGUAGEMANAGER
                 SVLibrary           = 0x109:FAC_SVLIBRARY
                 SVPipes             = 0x10a:FAC_SVPIPES
                 Unused02            = 0x10b:FAC_UNUSED02
                 SVTCPIP             = 0x10c:FAC_SVTCPIP
                 SVObserver          = 0x10d:FAC_SVOBSERVER
                 SVSecurity          = 0x10e:FAC_SVSECURITY
                 Unused03            = 0x10f:FAC_UNUSED03
                 SVMachineMessage    = 0x110:FAC_SVMACHINEMESSAGE
                 SVCmdComServer      = 0x111:FAC_SVCMDCOMSERVER
                 SVCmdComClient      = 0x112:FAC_SVCMDCOMCLIENT
                 SVDataManager	     = 0x113:FAC_SVDATAMANAGER
                 SVAccess            = 0x114:FAC_SVACCESS
                 SVIntek             = 0x115:FAC_SVINTEK
                 SVImageCompression  = 0x116:FAC_SVIMAGECOMPRESSION
                 SVEquation          = 0x117:FAC_SVEQUATION
                 SVFileAcquisition   = 0x118:FAC_SVFILEACQUISITION
                 SVMatroxGige        = 0x119:FAC_SVMATROXGIGE
                 SVTVicLpt           = 0x11a:FAC_SVTVICLPT
                 SVOLibrary          = 0x11b:FAC_SVOLIBRARY
                 SVSystemLibrary     = 0x11c:FAC_SVSYSTEMLIBRARY
                 SVMatroxLibrary     = 0x11d:FAC_SVMATROXLIBRARY
                 SVCI                = 0x11e:FAC_SVCI
                 SVXMLLibrary        = 0x11f:FAC_SVXMLLIBRARY
	         )

SeverityNames = (
                 Success       = 0x00:SEV_SUCCESS
                 Informational = 0x01:SEV_INFORMATIONAL
                 Warning       = 0x02:SEV_WARNING
                 Fatal         = 0x03:SEV_FATAL
                )

LanguageNames = (USEnglish	= 0x0409:USEnglish)

OutputBase = 16

MessageId = 1
Facility = None
Language = USEnglish
Unused01
.

MessageId = 2
Facility = None
Language = USEnglish
SVBATCHREPORT
.

MessageId = 3
Facility = None
Language = USEnglish
SVFOCUSNT
.

MessageId = 4
Facility = None
Language = USEnglish
SVFOCUSDB
.

MessageId = 5
Facility = None
Language = USEnglish
SVFOCUSDBMANAGER
.

MessageId = 6
Facility = None
Language = USEnglish
SVIMCOMMAND
.

MessageId = 7
Facility = None
Language = USEnglish
SVIPC
.

MessageId = 8
Facility = None
Language = USEnglish
SVLANGUAGEMANAGER
.

MessageId = 9
Facility = None
Language = USEnglish
SVLIBRARY
.

MessageId = 10
Facility = None
Language = USEnglish
SVPIPES
.

MessageId = 11
Facility = None
Language = USEnglish
Unused02
.

MessageId = 12
Facility = None
Language = USEnglish
SVTCPIP
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
Unused03
.

MessageId = 16
Facility = None
Language = USEnglish
SVMachineMessage
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
SVINTEK
.

MessageId = 22
Facility = None
Language = USEnglish
SVImageCompression
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
SVTVicLpt
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
SVCI
.

MessageId = 31
Facility = None
Language = USEnglish
SVXML
.

;//
;// =============== SVBatchReport Messages ===============
;//
MessageId = 0
Facility = SVBatchReport

Severity = Success
SymbolicName = SVMSG_BR_NO_ERROR

Language = USEnglish
No Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//
;// =============== End SVBatchReport Messages ===============
;//

;//
;// =============== SVFocusNT Messages ===============
;//
MessageId = 0
Facility = SVFocusNT

Severity = Success
SymbolicName = SVMSG_SVF_NO_ERROR

Language = USEnglish
No Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Informational
SymbolicName = SVMSG_SVF_APPLICATION_STARTED

Language = USEnglish
SVFocusNT Started 
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Informational
SymbolicName = SVMSG_SVF_APPLICATION_ENDED

Language = USEnglish
SVFocusNT Ended
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_END_OF_FILE

Language = USEnglish
End of File
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_NO_ITEM_SELECTED

Language = USEnglish
List item was not selected.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_CREATOR_DATA_MISSING

Language = USEnglish
Creator is required
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_DESCRIPTION_DATA_MISSING

Language = USEnglish
Description is required
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_NAME_DATA_MISSING

Language = USEnglish
Name is required
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_GROUP_ALREADY_EXISTS

Language = USEnglish
Product Group already exists
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_PRODUCT_ALREADY_EXISTS

Language = USEnglish
Product already exists
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_PRODUCT_NOT_FOUND

Language = USEnglish
Product not found
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_SVF_UNHANDLED_EXCEPTION

Language = USEnglish
Unhandled Exception (Data contains unhandled exception)
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_SVF_FAILED_TO_INITIALIZE_PLC

Language = USEnglish
Unable to initialize PLC with Run-Setup values
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_SVF_FAILED_TO_INITIALIZE_BATCH_HISTORY

Language = USEnglish
Unable to Intialize Batch History Tables
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_ACCESS_DENIED

Language = USEnglish
Access Denied
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_MUST_BE_UNIQUE

Language = USEnglish
%11 Must be unique
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_NO_RESET_ADDRESS

Language = USEnglish
Address for Reset Group not Found
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_REMOTE_MONITOR_FAILED

Language = USEnglish
Remote Command Monitor failed
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_MISSING_REMOTE_COMMAND_PARAMETERS

Language = USEnglish
One or more Remote Command Parameters are missing
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_INVALID_COMMAND_RECEIVED

Language = USEnglish
Invalid command received from PLC
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Success
SymbolicName = SVMSG_SVF_REMOTE_ACTIVE

Language = USEnglish
Remote command processing is active
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Success
SymbolicName = SVMSG_SVF_REMOTE_DISABLED

Language = USEnglish
Remote command processing is disabled
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_COULD_NOT_SET_SCREEN_NBR_IN_PLC

Language = USEnglish
Could not set the screen number in the PLC.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_COULD_NOT_GET_SCREEN_NBR_FROM_PLC

Language = USEnglish
Could not get the screen number from the PLC.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_RUNREFRESH_THREAD_ERROR

Language = USEnglish
An error occurred updating run buffers from the PLC.  PLCRunRefresh () thread.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_RUNREFRESH_THREAD_ERROR_PARSING_MACHINE_MESSAGE

Language = USEnglish
There was an error parsing a Machine Message address. Please use SVFocusConfig to check Machine Message addresses.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.

;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_RUNREFRESH_THREAD_ERROR_PARSING_GROUP1_TOTALS

Language = USEnglish
There was an error parsing a Group 1 Totals address. Please use SVFocusConfig to check Group 1 Total addresses (Type 2; Total Counter).
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_RUNREFRESH_THREAD_ERROR_PARSING_GROUP2_TOTALS

Language = USEnglish
There was an error parsing a Group 2 Totals address. Please use SVFocusConfig to check Group 2 Total addresses (Type 2; Total Counter).
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_RUNREFRESH_THREAD_ERROR_PARSING_GROUP1_DEFECTS

Language = USEnglish
There was an error parsing a Group 1 Defect address. Please use SVFocusConfig to check Group 1 Defect addresses (Type 1; Defect Counter).
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.

;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_RUNREFRESH_THREAD_ERROR_PARSING_GROUP2_DEFECTS

Language = USEnglish
There was an error parsing a Group 2 Defect address. Please use SVFocusConfig to check Group 2 Defect addresses (Type 1; Defect Counter).
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_RUNREFRESH_THREAD_ERROR_PARSING_VALUES

Language = USEnglish
There was an error parsing a Monitored Runtime PLC Value address. Please use SVFocusConfig to check PLC Value addresses (Type 5; PLC Non-Value; Read Only).
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.

;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_MISSING_EQUIPMENT_ENTRY

Language = USEnglish
An equipment entry is missing from the Equipment table (see additional data)
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.

;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_NO_PRODUCT_LOADED

Language = USEnglish
No product has been loaded
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.

;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_SVF_NO_PRODUCT_SELECTED

Language = USEnglish
No product has been selected.  Go to SVFocus Main Menu screen and select the Product Select button.  On the Product Select screen select a product from the product list.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.

;// ------------------------------------------------------------

;//
;// =============== End SVFocusNT Messages ===============
;//

;//
;// =============== SVFocusDB Messages ===============
;//
MessageId = 0
Facility = SVFocusDB

Severity = Success
SymbolicName = SVMSG_DB_NO_ERROR

Language = USEnglish
No Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.

;// ------------------------------------------------------------

;//#define SV_USED_ENGLISH 5
MessageId = 
Severity = Warning
SymbolicName = SVMSG_DB_USED_ENGLISH

Language = USEnglish
Requested language string not found. Used USEnglish.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//#define SV_USED_ANY 6
MessageId = 
Severity = Warning
SymbolicName = SVMSG_DB_USED_ANY

Language = USEnglish
Requested language string not found. Used first available.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_DB_OPEN_FAILED

Language = USEnglish
Database Open Failed
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_DB_FETCH_FAILED

Language = USEnglish
Fetch Failed
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//
MessageId = 
Severity = Fatal
SymbolicName = SVMSG_DB_INSERT_FAILED

Language = USEnglish
Insert Failed
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
;//

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_DB_NOT_FOUND

Language = USEnglish
Record not found
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_DB_MULTI_LINGUAL_STRING_NOT_FOUND

Language = USEnglish
Multi-lingual string not found
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
;//

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_DB_UPDATE_FAILED

Language = USEnglish
Update Failed
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_DB_COMMIT_FAILED

Language = USEnglish
Database commit failed
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_DB_EXECUTE_SQL_FAILED

Language = USEnglish
Execution of SQL statement failed (data contains statement)
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_DB_MULTI_LINGUAL_DELETE_FAILED

Language = USEnglish
Failed to delete Multi-lingual string
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_DB_FUNCTION_NOT_IMPLEMENTED

Language = USEnglish
Function Not Implemented
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.

;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_DB_ODBC_ERROR

Language = USEnglish
An ODBC error occurred. Check event viewer for details.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_DB_INVALID_PRODUCT_CODE  

Language = USEnglish
The Product Code supplied is invalid
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_DB_PRODUCT_GROUP_NOT_FOUND

Language = USEnglish
Product group not found
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_DB_PRODUCT_NOT_FOUND

Language = USEnglish
Product not found
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_DB_MUST_BE_UNIQUE

Language = USEnglish
Must be unique
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_DB_NO_OPEN_BATCH_EXISTS

Language = USEnglish
There is no open batch
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//
;// =============== End SVFocusDB Messages ===============
;//

;//
;// =============== SVFocusDBManager Messages ===============
;//
MessageId = 0
Facility = SVFocusDBManager

Severity = Success
SymbolicName = SVMSG_DBM_NO_ERROR

Language = USEnglish
No Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//
;// =============== End SVFocusDBManager Messages ===============
;//

;//
;// =============== SVIMCommand Messages ===============
;//
MessageId = 0
Facility = SVIMCommand

Severity = Success
SymbolicName = SVMSG_SVIMCMD_NO_ERROR

Language = USEnglish
No Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 1
Severity = Fatal
SymbolicName = SVMSG_SVIMCMD_DOWNLOAD_FAILED

Language = USEnglish
SVIM Download Failed
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 2
Severity = Fatal
SymbolicName = SVMSG_SVIMCMD_GO_ONLINE_FAILED

Language = USEnglish
Run Mode command failed
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 3
Severity = Fatal
SymbolicName = SVMSG_SVIMCMD_OUT_OF_MEMORY

Language = USEnglish
Out of memory
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 4
Severity = Fatal
SymbolicName = SVMSG_SVIMCMD_INVALID_SERVER_RESPONSE

Language = USEnglish
Invalid response from server
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 5
Severity = Warning
SymbolicName = SVMSG_SVIMCMD_REQUEST_REJECTED

Language = USEnglish
The request was rejected by the SVIM
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 6
Severity = Warning
SymbolicName = SVMSG_SVIMCMD_FILE_IO_ERROR

Language = USEnglish
I/O error on disk file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 7
Severity = Success
SymbolicName = SVMSG_SVIMCMD_ONLINE

Language = USEnglish
Run
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 8
Severity = Success
SymbolicName = SVMSG_SVIMCMD_LOADING

Language = USEnglish
Loading
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 9
Severity = Success
SymbolicName = SVMSG_SVIMCMD_LOADED

Language = USEnglish
Loaded
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 10
Severity = Success
SymbolicName = SVMSG_SVIMCMD_ONLINE_PENDING

Language = USEnglish
Run Pending
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 11
Severity = Success
SymbolicName = SVMSG_SVIMCMD_RUNNING_TEST

Language = USEnglish
Running in Test Mode
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 12
Severity = Success
SymbolicName = SVMSG_SVIMCMD_RUNNING

Language = USEnglish
Running
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 13
Severity = Success
SymbolicName = SVMSG_SVIMCMD_REGRESSION_TEST

Language = USEnglish
Running Regression Test
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 14
Severity = Success
SymbolicName = SVMSG_SVIMCMD_SETUP_MODE

Language = USEnglish
Setup Mode
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 15
Severity = Success
SymbolicName = SVMSG_SVIMCMD_SAVING_CONFIG

Language = USEnglish
Saving Configuration
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 16
Severity = Success
SymbolicName = SVMSG_SVIMCMD_STOPPING

Language = USEnglish
Stopping
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 17
Severity = Fatal
SymbolicName = SVMSG_STREAM_ALREADY_REGISTERED_BY_ANOTHER_CONTROL

Language = USEnglish
Streaming Data has already been registered by another control.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 18
Severity = Fatal
SymbolicName = SVMSG_STREAM_ALREADY_REGISTERED_BY_THIS_CONTROL

Language = USEnglish
Streaming Data has already been registered by same control.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------


MessageId = 19
Severity = Fatal
SymbolicName = SVMSG_STREAM_NOT_REGISTERED
Language = USEnglish
Streaming data has not been registered.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 20
Severity = Fatal
SymbolicName = SVMSG_SERVER_UNABLE_TO_CONNECT_TO_CONTROL
Language = USEnglish
Unable to connect with control.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------


MessageId = 21
Severity = Fatal
SymbolicName = SVMSG_REQUESTED_OBJECTS_ON_DIFFERENT_PPQS
Language = USEnglish
Requesting Objects are not on the same PPQ.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------


MessageId = 22
Severity = Informational
SymbolicName = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST
Language = USEnglish
One or more of the requested object do not exist.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------


MessageId = 23
Severity = Informational
SymbolicName = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_NOT_AN_INPUT
Language = USEnglish
One or more requested objects is not an Input.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------


MessageId = 24
Severity = Informational
SymbolicName = SVMSG_ONE_OR_MORE_INSPECTIONS_DO_NOT_EXIST
Language = USEnglish
One or more inspections do not exist.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 25
Severity = Fatal
SymbolicName = SVMSG_TOO_MANY_REQUESTED_ITEMS
Language = USEnglish
Too many requested items.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 26
Severity = Informational
SymbolicName = SVMSG_REQUESTED_LIST_IS_EMPTY
Language = USEnglish
The list of requested items is empty.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------


MessageId = 27
Severity = Fatal
SymbolicName = SVMSG_PRODUCT_NO_LONGER_AVAILABLE
Language = USEnglish
Product no longer available.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 28
Severity = Informational
SymbolicName = SVMSG_REQUESTED_OBJECTS_UNAVAILABLE
Language = USEnglish
Requested object is unavailable.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 29
Severity = Fatal
SymbolicName = SVMSG_CONFIGURATION_NOT_LOADED
Language = USEnglish
Configuration not loaded.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 30
Severity = Warning
SymbolicName = SVMSG_OBJECT_NOT_PROCESSED
Language = USEnglish
Object not processed.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 31
Severity = Fatal
SymbolicName = SVMSG_UNSUPPORTED_COMPRESSION_FORMAT
Language = USEnglish
Unsupported compression format.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 32
Severity = Fatal
SymbolicName = SVMSG_INDEX_NOT_LOCKED
Language = USEnglish
Index not locked.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 33
Severity = Fatal
SymbolicName = SVMSG_INDEX_NOT_FOUND
Language = USEnglish
Index not found.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------


MessageId = 34
Severity = Informational
SymbolicName = SVMSG_OBJECT_CANNOT_BE_SET_WHILE_ONLINE
Language = USEnglish
Item cannot be set while the SVIM is in a Running Mode (Run, Test, Regression, ...).
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------


MessageId = 35
Severity = Informational
SymbolicName = SVMSG_OBJECT_CANNOT_BE_SET_REMOTELY
Language = USEnglish
Item cannot be set remotely.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 36
Severity = Fatal
SymbolicName = SVMSG_INCORRECT_ARRAY_DATA_TYPE
Language = USEnglish
The data type passed in the array is the incorrect type.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 37
Severity = Fatal
SymbolicName = SVMSG_INVALID_IMAGE_SOURCE
Language = USEnglish
The Image Source is not Valid.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 38
Severity = Informational
SymbolicName = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED
Language = USEnglish
Not all of the items in the list could be processed.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 39
Severity = Informational
SymbolicName = SVMSG_REQUEST_IMAGE_NOT_SOURCE_IMAGE
Language = USEnglish
The requested image is not a source image.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 40
Severity = Warning
SymbolicName = SVMSG_INCORRECT_CHECKSUM
Language = USEnglish
The checksum is incorrect.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 41
Severity = Fatal
SymbolicName = SVMSG_MEMORY_ERROR_IN_STREAMING_DATA
Language = USEnglish
There was a memory error processing streaming data. It will be unregistered.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------


MessageId = 42
Severity = Informational
SymbolicName = SVMSG_OBJECT_ALREADY_SET_IN_THIS_LIST
Language = USEnglish
Item already set in this list.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 43
Severity = Fatal
SymbolicName = SVMSG_43_FONT_STR_LEN_GREATER_THAN_MAX

Language = USEnglish
Supplied String Length is larger than Maximum String Length.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 44
Severity = Fatal
SymbolicName = SVMSG_44_RUNONCE_ONLINE

Language = USEnglish
RunOnce cannot be executed while SVObserver is in Run Mode.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 45
Severity = Warning
SymbolicName = SVMSG_45_CALIBRATEFONT_ERROR
Language = USEnglish
SVCalibrateFont - An error has occured. Please see Event Viewer on SVIM for further information - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 46
Severity = Warning
SymbolicName = SVMSG_46_LOADFONT_ERROR

Language = USEnglish
SVLoadFont - An error has occured. Please see Event Viewer on SVIM for further information.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 47
Severity = Warning
SymbolicName = SVMSG_47_SAVEFONT_ERROR

Language = USEnglish
SVSaveFont - An error has occured. Please see Event Viewer on SVIM for further information.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 48
Severity = Warning
SymbolicName = SVMSG_48_READSTRING_ERROR

Language = USEnglish
SVReadString - An error has occured. Please see Event Viewer on SVIM for further information.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 49
Severity = Warning
SymbolicName = SVMSG_49_VERIFYSTRING_ERROR

Language = USEnglish
SVVerifyString - An error has occured. Please see Event Viewer on SVIM for further information. %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 50
Severity = Fatal
SymbolicName = SVMSG_50_MODE_CHANGING_ERROR

Language = USEnglish
Set Mode Failed - Svim Mode Changing
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 51
Severity = Fatal
SymbolicName = SVMSG_51_MODE_CONFIGURATION_LOADING_ERROR

Language = USEnglish
Set Mode Failed - Svim Loading Configuration
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 52
Severity = Fatal
SymbolicName = SVMSG_52_MODE_GUI_IN_USE_ERROR

Language = USEnglish
Set Mode Failed - GUI in use
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 53
Severity = Informational
SymbolicName = SVMSG_53_SVIM_NOT_IN_RUN_MODE;
Language = USEnglish
Command cannot be executed unless the SVIM is in Run Mode
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 54
Severity = Informational
SymbolicName = SVMSG_54_SVIM_BUSY;
Language = USEnglish
Command cannot be executed because the SVIM is busy.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 55
Severity = Warning
SymbolicName = SVMSG_NO_FONT_FOUND

Language = USEnglish
SVIM Command Com Control - No Font Found
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------


MessageId = 56
Severity = Warning
SymbolicName = SVMSG_FONT_IMAGE_FAIL

Language = USEnglish
SVIM Command Com Control - Font Image Failure
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------


MessageId = 57
Severity = Warning
SymbolicName = SVMSG_FONT_COPY_FAIL

Language = USEnglish
SVIM Command Com Control - Font Copy Failure
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 58
Severity = Fatal
SymbolicName = SVMSG_FONT_COPY_THREW_EXCEPTION

Language = USEnglish
SVIM Command Com Control - Font Copy Threw Exception
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 59
Severity = Warning
SymbolicName = SVMSG_FONT_INVALID

Language = USEnglish
SVIM Command Com Control - Font Invalid or Timed Out
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 60
Severity = Warning
SymbolicName = SVMSG_INVALID_VARIANT_PARAMETER

Language = USEnglish
SVIM Command Com Control - Variant Parameter Invalid
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 61
Severity = Warning
SymbolicName = SVMSG_FAILED_TO_REMOVE_FONT

Language = USEnglish
SVIM Command Com Control - Font Failed to Remove
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------


MessageId = 62
Severity = Warning
SymbolicName = SVMSG_FAILED_TO_ADD_FONT

Language = USEnglish
SVIM Command Com Control - Font Failed to Add
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 63
Severity = Fatal
SymbolicName = SVMSG_63_SVIM_IN_WRONG_MODE;
Language = USEnglish
Command cannot be executed due to the SVIM being in the wrong Mode
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 64
Severity = Fatal
SymbolicName = SVMSG_64_SVIM_MODE_NOT_REMOTELY_SETABLE;
Language = USEnglish
The Mode being Set is not Allowed to be Set Remotely
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------


MessageId = 65
Severity = Informational
SymbolicName = SVMSG_ONE_OR_MORE_OBJECTS_INVALID
Language = USEnglish
One or more objects are invalid.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 66
Severity = Informational
SymbolicName = SVMSG_OBJECT_NOT_FOUND
Language = USEnglish
Object not found.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 67
Severity = Informational
SymbolicName = SVMSG_OBJECT_IN_WRONG_PPQ
Language = USEnglish
Object is in wrong PPQ.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 68
Severity = Informational
SymbolicName = SVMSG_OBJECT_WRONG_TYPE
Language = USEnglish
Object is the wrong type.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 69
Severity = Informational
SymbolicName = SVMSG_OBJECT_CANNOT_BE_SET_INVALID_REFERENCE
Language = USEnglish
Item cannot be set the reference is not valid
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//
;// =============== End SVIMCommand Messages ===============
;//

;//
;// =============== SVIPC Messages ===============
;//
MessageId = 0
Facility = SVIPC

Severity = Success
SymbolicName = SVMSG_IPC_NO_ERROR

Language = USEnglish
No Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 107
Facility = SVIPC

Severity = Fatal
SymbolicName = SVMSG_SVIPC_QUEUE_USER_APC_ERROR

Language = USEnglish
SVAsyncProcedure : QueueUserAPC Error - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
MessageId = 108
Facility = SVIPC

Severity = Fatal
SymbolicName = SVMSG_SVIPC_THREAD_CREATION_ERROR

Language = USEnglish
SVThread : Thread Creation Error - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
;//
;// =============== End SVIPC Messages ===============
;//

;//
;// =============== SVLanguageManager Messages ===============
;//
MessageId = 0
Facility = SVLanguageManager

Severity = Success
SymbolicName = SVMSG_LM_NO_ERROR

Language = USEnglish
No Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//
;// =============== End SVLanguageManager Messages ===============
;//

;//
;// =============== SVLibrary Messages ===============
;//
MessageId = 0
Facility = SVLibrary

Severity = Success
SymbolicName = SVMSG_LIB_NO_ERROR

Language = USEnglish
No Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_LIB_LISTCTRL_ACTION_FAILED

Language = USEnglish
SVListCtrl Action Failed
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_LIB_REGISTRY_KEY_OPEN_FAILED

Language = USEnglish
Unable to open registry key
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_LIB_REGISTRY_KEY_CREATE_FAILED

Language = USEnglish
Unable to create registry key
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_LIB_SET_REGISTRY_VALUE_FAILED

Language = USEnglish
Unable to set registry value
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_LIB_REGISTRY_EXPORT_FAILED

Language = USEnglish
Unable to export all keys to file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_LIB_REGISTRY_KEY_EXPORT_FAILED

Language = USEnglish
Unable to export key to file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_LIB_REGISTRY_VALUE_EXPORT_FAILED

Language = USEnglish
Unable to export value to file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_LIB_REGISTRY_IMPORT_FAILED

Language = USEnglish
Unable to import file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_LIB_REGISTRY_INVALID_IMPORT_FILE

Language = USEnglish
Import file does not start with 'REGEDIT4'
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_LIB_REGISTRY_IMPORT_EXPECTED_KEY

Language = USEnglish
Import error -- expected key name
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_LIB_REGISTRY_IMPORT_FILE_IO_ERROR

Language = USEnglish
I/O Error reading registry import file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_LIB_PACKFILE_IO_ERROR

Language = USEnglish
I/O Error on disk file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//
;// =============== End SVLibrary Messages ===============
;//

;//
;// =============== SVPipes Messages ===============
;//
;//#define SV_NOERROR 0
MessageId = 0
Facility = SVPipes

Severity = Success
SymbolicName = SVMSG_PIPES_NO_ERROR

Language = USEnglish
No Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//#define SV_OPENFAILED 1
MessageId = 
Severity = Fatal
SymbolicName = SVMSG_PIPES_OPEN_FAILED

Language = USEnglish
Pipe Open Failed
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//#define SV_READFAILED 2

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_PIPES_READ_FAILED

Language = USEnglish
Pipe Read Failed
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//#define SV_WRITEFAILED 3

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_PIPES_WRITE_FAILED

Language = USEnglish
Pipe Write Failed
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Fatal
SymbolicName = SVMSG_PIPES_IO_ERROR

Language = USEnglish
Communication error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_PIPES_END_OF_FILE

Language = USEnglish
End of file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_PIPES_REQUIRED_DATA_MISSING

Language = USEnglish
'Pipe Name' and 'Server Name' must both be supplied
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//
;// =============== End SVPipes Messages ===============
;//

;//
;// =============== SVTCPIP Messages ===============
;//
MessageId = 0
Facility = SVTCPIP

Severity = Success
SymbolicName = SVMSG_TCPIP_NO_ERROR

Language = USEnglish
No Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//
;// =============== End SVTCPIP Messages ===============
;//

;//
;// =============== SVObserver Messages ===============
;//
MessageId = 0
Facility = SVObserver

Severity = Success
SymbolicName = SVMSG_SVO_NO_ERROR

Language = USEnglish
No Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1
Severity = Warning
SymbolicName = SVMSG_SVO_THREAD_ERROR

Language = USEnglish
Unable To Create New Thread
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 15
Severity = Warning
SymbolicName = SVMSG_SVO_15_LOAD_CONFIG_WARNING
Language = USEnglish
Problem loading configuration.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 16
Severity = Error
SymbolicName = SVMSG_SVO_16_LOAD_CONFIG_ERROR
Language = USEnglish
Unable to allocate memory.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 17
Severity = Error
SymbolicName = SVMSG_SVO_17_NO_MEMORY_ERROR
Language = USEnglish
Out of memory
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 18
Severity = Error
SymbolicName = SVMSG_SVO_18_OUT_OF_NP_MEMORY
Language = USEnglish
Not enough non-paged memory allocated.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 19
Severity = Error
SymbolicName = SVMSG_SVO_19_PROBLEM_ALLOCATING_DIGITIZER
Language = USEnglish
Problem allocating digitizer
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 20
Severity = Error
SymbolicName = SVMSG_SVO_20_INCORRECT_CAMERA_FILE
Language = USEnglish
Camera file does not match camera.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 25
Severity = Informational
SymbolicName = SVMSG_SVO_25_SVOBSERVER_STARTED
Language = USEnglish
SVObserver - Program has started. %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 26
Severity = Informational
SymbolicName = SVMSG_SVO_26_SVOBSERVER_STOPPED
Language = USEnglish
SVObserver - Program has stopped.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 27
Severity = Informational
SymbolicName = SVMSG_SVO_27_SVOBSERVER_GO_ONLINE
Language = USEnglish
SVObserver - Program has entered Run Mode. %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 28
Severity = Informational
SymbolicName = SVMSG_SVO_28_SVOBSERVER_GO_OFFLINE
Language = USEnglish
SVObserver - Program has gone offline. %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 29
Severity = Informational
SymbolicName = SVMSG_SVO_29_SVOBSERVER_CONFIG_LOADED
Language = USEnglish
SVObserver - Configuration loaded = %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 30
Severity = Warning
SymbolicName = SVMSG_SVO_30_EXCEPTION_IN_MIL
Language = USEnglish
SVObserver - Exception in MIL function (%11)
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 31
Severity = Error
SymbolicName = SVMSG_SVO_31_EXCEPTION_IN_EXTERNAL_DLL
Language = USEnglish
SVObserver - Exception in External Tool DLL (%11)
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 32
Severity = Error
SymbolicName = SVMSG_SVO_32_ARCHIVE_TOOL_OUT_OF_MEMORY
Language = USEnglish
SVObserver - Archive tool was not able to allocate memory for buffered images (limit %11 MB)
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 33
Severity = Warning
SymbolicName = SVMSG_SVO_33_OBJECT_INDEX_INVALID
Language = USEnglish
SVObserver - Object index invalid (%11)
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 34
Severity = Warning
SymbolicName = SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE
Language = USEnglish
SVObserver - Object index out of result range (%11)
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 35
Severity = Fatal
SymbolicName = SVMSG_SVO_35_LEGACY_ERROR_TRAP
Language = USEnglish
SVObserver - Legacy Error Trap (%11)
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 36
Severity = Error
SymbolicName = SVMSG_SVO_36_MIL_ERROR
Language = USEnglish
SVObserver - MIL Error
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 37
Severity = Error
SymbolicName = SVMSG_SVO_37_INVALID_RUN_STATUS
Language = USEnglish
SVObserver - Run Status was in an invalid state.  Make sure time between triggers is greater than acquisition time.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 38
Severity = Error
SymbolicName = SVMSG_SVO_38_INPUT_REQUEST_FAILED
Language = USEnglish
SVObserver - Input Request failed
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 39
Severity = Error
SymbolicName = SVMSG_SVO_39_IMAGE_REQUEST_FAILED
Language = USEnglish
SVObserver - Image Request failed
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 40
Severity = Informational
SymbolicName = SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED
Language = USEnglish
SVObserver - Update Main Image Failed
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 40
Severity = Error
SymbolicName = SVMSG_SVO_40_UPDATE_MAINIMAGE_FAILED
Language = USEnglish
SVObserver - Update Main Image Failed
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 41
Severity = Informational
SymbolicName = SVMSG_SVO_41_ERROR_MIMBINARIZE
Language = USEnglish
SVObserver - MimBinarize Error Toolname = %11 
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 42
Severity = Informational
SymbolicName = SVMSG_SVO_42_CURRENT_INDEX
Language = USEnglish
SVObserver - Last set index for image = %11 
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 43
Severity = Informational
SymbolicName = SVMSG_SVO_43_GENERAL
Language = USEnglish
SVObserver - Msg = %11 
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 44
Severity = Warning
SymbolicName = SVMSG_SVO_44_SHARED_MEMORY
Language = USEnglish
SVObserver - Shared Memory = %11 
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 45
Severity = Warning
SymbolicName = SVMSG_SVO_45_SHARED_MEMORY_SETUP_LISTS
Language = USEnglish
SVObserver - Shared Memory Setup = %11 
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 46
Severity = Warning
SymbolicName = SVMSG_SVO_46_SHARED_MEMORY_DISK_SPACE
Language = USEnglish
SVObserver - Shared Memory Disk Space = %11 
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 48
Severity = Error
SymbolicName = SVMSG_SVO_48_LOAD_CONFIGURATION_MONITOR_LIST
Language = USEnglish
SVObserver - Loading Configuration failed.  Could not load monitor list.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 49
Severity = Error
SymbolicName = SVMSG_SVO_49_LOAD_CONFIGURATION_TOOLGROUPING
Language = USEnglish
SVObserver - Loading Configuration failed.  Could not load Tool Grouping.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 50
Severity = Error
SymbolicName = SVMSG_SVO_50_SHARED_MEMORY_COPY_LASTINSPECTED_TO_REJECT
Language = USEnglish
SVObserver - Copy LastInspected to Reject failed.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 51
Severity = Warning
SymbolicName = SVMSG_SVO_51_CLIPBOARD_WARNING
Language = USEnglish
SVObserver - Clipboard = %11 
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 52
Severity = Error
SymbolicName = SVMSG_SVO_52_NOMATROX_DONGLE
Language = USEnglish
SVObserver - No Matrox dongle found.
Only limited functionality is available.
Please exit SVObserver, insert a Matrox dongle and restart for full functionality.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.

;// ------------------------------------------------------------
MessageId = 53
Severity = Error
SymbolicName = SVMSG_SVO_53_RESOURCE_DLL_LOADING_FAILED
Language = USEnglish
Could not load the resource dll.
The program will be terminated.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 54
Severity = Informational
SymbolicName = SVMSG_SVO_54_EMPTY
Language = USEnglish
%11 
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 55
Severity = Error
SymbolicName = SVMSG_SVO_55_DEBUG_BREAK_ERROR
Language = USEnglish
SVObserver - Debug Break Error = %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 56
Severity = Error
SymbolicName = SVMSG_SVO_56_INSPECTION_IMPORT_ERROR
Language = USEnglish
Import of inspection failed.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 57
Severity = Error
SymbolicName = SVMSG_SVO_57_PARSERTREE_INSPECTIONCREATE_ERROR
Language = USEnglish
Parser Tree - Creating of Inspections failed.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 58
Severity = Error
SymbolicName = SVMSG_SVO_58_TOOLADJUST_RESET_ERROR
Language = USEnglish
Tool size adjustment failed.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 59
Severity = Error
SymbolicName = SVMSG_SVO_59_TOOLADJUST_INPUT_ERROR
Language = USEnglish
Invalid Tool size adjustment setting.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 60
Severity = Informational
SymbolicName = SVMSG_SVO_60_SHIFT_TOOL_SOURCE_IMAGE_EXTENTS_DISABLED
Language = USEnglish
The Source Image Extents have been disabled for this Shift Tool.  The values for any Blob Analyzer's "Box X ToolSet Image" and "Box Y ToolSet Image" results that use this Shift Tool as its source will not be valid and should not be used.  The Shift Tool may run faster with the Source Image Extents disabled.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 61
Severity = Error
SymbolicName = SVMSG_SVO_61_RINGBUFFER_ONVALIDATE_ERROR
Language = USEnglish
RingBuffer Tool: OnValidate failed: %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 62
Severity = Error
SymbolicName = SVMSG_SVO_62_RINGBUFFER_INVALID_VALUE
Language = USEnglish
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 63
Severity = Error
SymbolicName = SVMSG_SVO_63_LOAD_GLOBAL_CONSTANTS
Language = USEnglish
SVObserver - Loading Configuration failed.  Could not load the Global Constants.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 64
Severity = Informational
SymbolicName = SVMSG_SVO_64_EMPTY_FORMULAS_ARE_NOT_ALLOWED
Language = USEnglish
"Empty formulas are not allowed for sizing or positioning"
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 65
Severity = Warning
SymbolicName = SVMSG_SVO_65_ENTERED_VALUE_INVALID
Language = USEnglish
The value entered is invalid.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 66
Severity = Warning
SymbolicName = SVMSG_SVO_66_GLOBAL_NAME_INVALID
Language = USEnglish
The Global Constant Name [%11] is already being used, edit the constant in the list or enter another name.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 67
Severity = Warning
SymbolicName = SVMSG_SVO_67_MAIN_BRANCH_NOT_CREATED
Language = USEnglish
The main branch [%11] could not be created.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
MessageId = 68
Severity = Error
SymbolicName = SVMSG_SVO_68_RANGE_VALUE_SET_FAILED
Language = USEnglish
Set of Rangevalue failed: %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
MessageId = 69
Severity = Error
SymbolicName = SVMSG_SVO_69_PPQ_INDEX_NOT_RELEASED
Language = USEnglish
SVPPQObject::IndexPPQ-cannot release Input Buffer Index
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
MessageId = 70
Severity = Error
SymbolicName = SVMSG_SVO_70_DUPLICATE_DISCRETE_OUTPUT
Language = USEnglish
Invalid Discrete Outputs: Remove all Discrete Outputs and re-add them.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
MessageId = 71
Severity = Error
SymbolicName = SVMSG_SVO_71_INVALID_SOURCE_IMAGE_INDEX
Language = USEnglish
Invalid source image index
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
MessageId = 72
Severity = Error
SymbolicName = SVMSG_SVO_72_UNHANDLED_EXCEPTION
Language = USEnglish
An unhandled SVObserver exception, with the following Message number [%11], has occured
The application will be terminated
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
MessageId = 73
Severity = Error
SymbolicName = SVMSG_SVO_73_ARCHIVE_MEMORY
Language = USEnglish
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 74
Severity = Error
SymbolicName = SVMSG_SVO_74_LOAD_FILE
Language = USEnglish
SVObserver - The loading of the following file failed:
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 75
Severity = Error
SymbolicName = SVMSG_SVO_75_DESTROY_CONFIGURATION_FAILED
Language = USEnglish
The loading of the file failed: the old configuration could not be destroyed.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 77
Severity = Error
SymbolicName = SVMSG_SVO_77_LOAD_VERSION_NUMBER_FAILED
Language = USEnglish
The loading of the file failed: the Version Number could not be loaded.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 78
Severity = Error
SymbolicName = SVMSG_SVO_78_LOAD_IO_FAILED
Language = USEnglish
The loading of the file failed. Because of following error in the IO section: 
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 79
Severity = Error
SymbolicName = SVMSG_SVO_79_LOAD_PPQ_FAILED
Language = USEnglish
The loading of the file failed. Because of following error in the PPQ section: 
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 80
Severity = Error
SymbolicName = SVMSG_SVO_80_LOAD_INSPECTION_FAILED
Language = USEnglish
The loading of the file failed.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 81
Severity = Error
SymbolicName = SVMSG_SVO_81_LOAD_CONFIGUURATION_FAILED
Language = USEnglish
The following Configuration File could not be loaded.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 82
Severity = Error
SymbolicName = SVMSG_SVO_82_NO_ENVIROMENT_TAG
Language = USEnglish
The file could not be loaded: The enviroment tag is missing.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// 
;// ------------------------------------------------------------
;//
;// ------------------------------------------------------------
;// Jim's messages starting at 5001

MessageId = 5001
Severity = Error
SymbolicName = SVMSG_SVO_5001_NULLPARENT
Language = USEnglish
Parent dialog is NULL.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5002
Severity = Error
SymbolicName = SVMSG_SVO_5002_NULLPROPERTYTREE_AREA
Language = USEnglish
Property tree area is NULL.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5003
Severity = Error
SymbolicName = SVMSG_SVO_5003_COULDNOTINSERTGROUPITEM
Language = USEnglish
Could not allocate and insert the Tree Control group item.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5004
Severity = Error
SymbolicName = SVMSG_SVO_5004_COULDNOTCREATE
Language = USEnglish
Not able to create SVPropTree control.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5005
Severity = Error
SymbolicName = SVMSG_SVO_5005_COULDNOTCREATE
Language = USEnglish
Not able to create the tree root.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5006
Severity = Error
SymbolicName = SVMSG_SVO_5006_COULDNOTINSERTHEIGHT
Language = USEnglish
Could not allocate and insert the Height property.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5007
Severity = Error
SymbolicName = SVMSG_SVO_5007_NULLPARENT
Language = USEnglish
Parent dialog is NULL.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5008
Severity = Error
SymbolicName = SVMSG_SVO_5008_NULLTOOL
Language = USEnglish
The associated Tool is NULL.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5009
Severity = Error
SymbolicName = SVMSG_SVO_5009_COULDNOTINSERTWIDTH
Language = USEnglish
Could not allocate and insert the Width property.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5010
Severity = Error
SymbolicName = SVMSG_SVO_5010_COULDNOTINSERTINPUTIMAGEGROUP
Language = USEnglish
Could not allocate and insert the Input Image Group.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5011
Severity = Error
SymbolicName = SVMSG_SVO_5011_COULDNOTINSERTHEIGHT
Language = USEnglish
Could not allocate and insert the Input Image Height property.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5012
Severity = Error
SymbolicName = SVMSG_SVO_5012_COULDNOTINSERTWIDTH
Language = USEnglish
Could not allocate and insert the Input Image Width property.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5013
Severity = Error
SymbolicName = SVMSG_SVO_5013_COULDNOTINSERTOUTPUTIMAGEGROUP
Language = USEnglish
Could not allocate and insert the Output Image Group.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5014
Severity = Error
SymbolicName = SVMSG_SVO_5014_COULDNOTINSERTHEIGHT
Language = USEnglish
Could not allocate and insert the Output Image Height property.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5015
Severity = Error
SymbolicName = SVMSG_SVO_5015_COULDNOTINSERTWIDTH
Language = USEnglish
Could not allocate and insert the Output Image Width property.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5016
Severity = Error
SymbolicName = SVMSG_SVO_5016_COULDNOTADDTAB
Language = USEnglish
Could not add Source tab to Image Control.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5017
Severity = Error
SymbolicName = SVMSG_SVO_5017_COULDNOTADDTAB
Language = USEnglish
Could not add Result tab to Image Control.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5018
Severity = Error
SymbolicName = SVMSG_SVO_5018_NULLIMAGE
Language = USEnglish
Source image is NULL.  This should not be able to happen.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5019
Severity = Error
SymbolicName = SVMSG_SVO_5019_NOTUSED;
Language = USEnglish
Not used.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5020
Severity = Error
SymbolicName = SVMSG_SVO_5020_BASECLASSONRUNFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred in base class onRun.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5021
Severity = Error
SymbolicName = SVMSG_SVO_5021_SETIMAGEHANDLEFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred attempting to set the output image pool index.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5022
Severity = Error
SymbolicName = SVMSG_SVO_5022_UPDATEIMAGEEXTENTSFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred attempting to update the Image Extents.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5023
Severity = Error
SymbolicName = SVMSG_SVO_5023_ROIGETIMAGEHANDLEFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to use the ROI image.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5024
Severity = Error
SymbolicName = SVMSG_SVO_5024_ROIGETDATAFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to use the ROI image.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5025
Severity = Error
SymbolicName = SVMSG_SVO_5025_ROIGETBUFFERFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to use the ROI image.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5026
Severity = Error
SymbolicName = SVMSG_SVO_5026_OUTPUTGETIMAGEHANDLEFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to use the Output image.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5027
Severity = Error
SymbolicName = SVMSG_SVO_5027_OUTPUTGETDATAFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to use the Output image.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5028
Severity = Error
SymbolicName = SVMSG_SVO_5028_OUTPUTGETBUFFERFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to use the Output image.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5029
Severity = Error
SymbolicName = SVMSG_SVO_5029_GETEXTENTSFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to retrieve the image extents.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5030
Severity = Error
SymbolicName = SVMSG_SVO_5030_GETHEIGHTSFFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to retrieve the Height Scale Factor extent.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5031
Severity = Error
SymbolicName = SVMSG_SVO_5031_GETWIDTHSFFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to retrieve the Width Scale Factor extent.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5032
Severity = Error
SymbolicName = SVMSG_SVO_5032_GETINTERPOLATIONMODEFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to retrieve the Interpolation Mode.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5033
Severity = Error
SymbolicName = SVMSG_SVO_5033_GETOVERSCANFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to retrieve the Overscan.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5034
Severity = Error
SymbolicName = SVMSG_SVO_5034_GETPERFORMANCEFAILED;
Language = USEnglish
Resize Tool: An onRun error occurred when attempting to retrieve the Performance.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5035
Severity = Error
SymbolicName = SVMSG_SVO_5035_COULDNOTINSERTINTERPOLATIONMODEITEM;
Language = USEnglish
Resize Tool Dialog: Could not insert Interpolation Mode Combo item into Property Tree.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5036
Severity = Error
SymbolicName = SVMSG_SVO_5036_COULDNOTINSERTOVERSCANITEM;
Language = USEnglish
Resize Tool Dialog: Could not insert Overscan Combo item into Property Tree.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5037
Severity = Error
SymbolicName = SVMSG_SVO_5037_COULDNOTINSERTPERFORMANCEITEM;
Language = USEnglish
Resize Tool Dialog: Could not insert Performance Combo item into Property Tree.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5038
Severity = Error
SymbolicName = SVMSG_SVO_5038_COULDNOTINSERTOTHERITEM;
Language = USEnglish
Resize Tool Dialog: Could not insert Other Group item into Property Tree.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5039
Severity = Error
SymbolicName = SVMSG_SVO_5039_COULDNOTGETINPUTIMAGE;
Language = USEnglish
Resize Tool Dialog: Could not get Input Image.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5040
Severity = Error
SymbolicName = SVMSG_SVO_5040_COULDNOTGETROIIMAGE;
Language = USEnglish
Resize Tool Dialog: Could not get logical ROI Image.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5041
Severity = Error
SymbolicName = SVMSG_SVO_5041_COULDNOTGETOUTPUTIMAGE;
Language = USEnglish
Resize Tool Dialog: Could not get Output Image.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5042
Severity = Error
SymbolicName = SVMSG_SVO_5042_COULDNOTGETSOURCEIMAGE;
Language = USEnglish
Resize: Could not get Source Image.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5043
Severity = Error
SymbolicName = SVMSG_SVO_5043_COULDNOTGETDESTINATIONIMAGE;
Language = USEnglish
Resize: Could not get Destination Image.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5044
Severity = Error
SymbolicName = SVMSG_SVO_5044_INVALIDINTERPOLATIONMODE;
Language = USEnglish
Resize: Invalid Interpolation Mode.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5045
Severity = Error
SymbolicName = SVMSG_SVO_5045_INVALIDOVERSCAN;
Language = USEnglish
Resize: Invalid Overscan value.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5046
Severity = Error
SymbolicName = SVMSG_SVO_5046_INVALIDPERFORMANCE;
Language = USEnglish
Resize: Invalid Performance value.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5047
Severity = Error
SymbolicName = SVMSG_SVO_5047_GETINPUTIMAGEFAILED;
Language = USEnglish
Resize: Could not retrieve the source image within ResetObject.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5048
Severity = Error
SymbolicName = SVMSG_SVO_5048_INITIALIZEROIIMAGEFAILED;
Language = USEnglish
Resize: Could not initialize the ROI image within ResetObject.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5049
Severity = Error
SymbolicName = SVMSG_SVO_5049_INITIALIZEOUTPUTIMAGEFAILED;
Language = USEnglish
Resize: Could not initialize the Output Image within ResetObject.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5050
Severity = Error
SymbolicName = SVMSG_SVO_5050_BASECLASSFAILED;
Language = USEnglish
Resize: Baseclass ResetObject failed.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.

;// ------------------------------------------------------------
MessageId = 5051
Severity = Error
SymbolicName = SVMSG_SVO_5051_DRIVEDOESNOTEXIST
Language = USEnglish
Expected drive does not exist:  %11.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.

;// ------------------------------------------------------------
MessageId = 5052
Severity = Error
SymbolicName = SVMSG_SVO_5052_DRIVENOTNTFSFORMAT
Language = USEnglish
Drive is not the required NTFS format:  %11.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.


;// ------------------------------------------------------------
MessageId = 5053
Severity = Error
SymbolicName = SVMSG_SVO_5053_CHILDIMAGEHANDLESFALSE;
Language = USEnglish
GetChildImageHandle failed.  Check for Child returned S_FALSE.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5054
Severity = Error
SymbolicName = SVMSG_SVO_5054_NULLPARENTIMAGE;
Language = USEnglish
GetChildImageHandle failed.  NULL or self referencing Parent Image.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5055
Severity = Error
SymbolicName = SVMSG_SVO_5055_NULLCHILDHANDLE;
Language = USEnglish
GetChildImageHandle failed.  Check for Child returned NULL.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5056
Severity = Error
SymbolicName = SVMSG_SVO_5056_CHILDIMAGEHANDLESFALSE;
Language = USEnglish
GetChildImageHandle failed.  Check for Child returned S_FALSE.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5057
Severity = Error
SymbolicName = SVMSG_SVO_5057_NULLPARENTIMAGE;
Language = USEnglish
GetChildImageHandle failed.  NULL or self referencing Parent Image.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5058
Severity = Error
SymbolicName = SVMSG_SVO_5058_NULLCHILDHANDLE;
Language = USEnglish
GetChildImageHandle failed.  Check for Child returned NULL.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5059
Severity = Error
SymbolicName = SVMSG_SVO_5059_GETCHILDERROR;
Language = USEnglish
GetParentImageHandle failed.  Cannot get default Child Image Handle.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5060
Severity = Error
SymbolicName = SVMSG_SVO_5060_GETCHILDERROR;
Language = USEnglish
GetParentImageHandle failed.  Cannot get indexed Child Image Handle.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5061
Severity = Error
SymbolicName = SVMSG_SVO_5061_SFOUTSIDERANGE;
Language = USEnglish
Resize Tool: The Image Scale Factor is outside the defined range.
Field: %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5062
Severity = Error
SymbolicName = SVMSG_SVO_5062_NOTUSED;
Language = USEnglish
Resize Tool: The Image Height Scale Factor is outside the defined range.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5063
Severity = Error
SymbolicName = SVMSG_SVO_5063_ERRORGETTINGIMAGEEXTENTS;
Language = USEnglish
Resize Tool: Could not retrieve image extents.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5064
Severity = Error
SymbolicName = SVMSG_SVO_5064_NOTUSED;
Language = USEnglish
Resize Tool: The Image Width Scale Factor is outside the defined range.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5065
Severity = Error
SymbolicName = SVMSG_SVO_5065_COULDNOTCREATEIMAGEBUFFER;
Language = USEnglish
Could not create an image buffer.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5066
Severity = Error
SymbolicName = SVMSG_SVO_5066_CATCHRUNONCESFALSE;
Language = USEnglish
SetInspectionData: There was an error during the RunOnce.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5067
Severity = Error
SymbolicName = SVMSG_SVO_5067_IMAGEALLOCATIONFAILED;
Language = USEnglish
Image Allocation Failed.
Image:  %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5068
Severity = Error
SymbolicName = SVMSG_SVO_5068_INCONSISTENTDATA;
Language = USEnglish
Return code and Tool error data conflict.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5069
Severity = Error
SymbolicName = SVMSG_SVO_5069_RESETOBJECTFAILED;
Language = USEnglish
UpdateBufferArrays:  ResetObject failed.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5070
Severity = Error
SymbolicName = SVMSG_SVO_5070_IMAGEALLOCATIONFAILED;
Language = USEnglish
Image Allocation Failed.  Try using a reduced Scale Factor.
Field:  %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5071
Severity = Error
SymbolicName = SVMSG_SVO_5071_CAPTUREDSFALSE;
Language = USEnglish
Resize Tool: Could not get image extents.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5072
Severity = Error
SymbolicName = SVMSG_SVO_5072_INCONSISTENTDATA;
Language = USEnglish
Return code and Tool error data conflict.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5073
Severity = Error
SymbolicName = SVMSG_SVO_5073_INCONSISTENTDATA;
Language = USEnglish
Return code and Tool error data conflict.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5074
Severity = Error
SymbolicName = SVMSG_SVO_5074_BASECLASSONVALIDATEFAILED;
Language = USEnglish
Resize Tool:  Base class OnValidate failed.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5075
Severity = Error
SymbolicName = SVMSG_SVO_5075_INCONSISTENTDATA;
Language = USEnglish
Return code and Tool error data conflict.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5076
Severity = Error
SymbolicName = SVMSG_SVO_5076_EXTENTSNOTCOPIED;
Language = USEnglish
Extents could not be copied.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5077
Severity = Error
SymbolicName = SVMSG_SVO_5077_COULDNOTVALIDATE;
Language = USEnglish
Resize Tool: Page base class could not successfuly validate.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 5078
Severity = Error
SymbolicName = SVMSG_SVO_5078_INCONSISTENTDATA;
Language = USEnglish
Return code and Tool validation data conflict.
%11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// End of Jim's messages starting at 5001
;// ------------------------------------------------------------
;//
;// =============== End SVObserver Messages ===============
;//

;//
;// =============== SVSecurity Messages ===============
;//
MessageId = 0
Facility = SVSecurity

Severity = Success
SymbolicName = SVMSG_SVS_ACCESS_GRANTED_1

Language = USEnglish
User: %12, Dialog: %11
.
;// ------------------------------------------------------------
MessageId =
Facility = SVSecurity

Severity = Informational
SymbolicName = SVMSG_SVS_ACCESS_GRANTED

Language = USEnglish
User: %12, Dialog: %11
.
;// ------------------------------------------------------------

MessageId = 2
Severity = Informational
SymbolicName = SVMSG_SVS_ACCESS_GRANTED1

Language = USEnglish
Login - Access Granted
%1User: %12
%1Gained Access to - %11
.
;// ------------------------------------------------------------

MessageId = 3
Severity = Warning
SymbolicName = SVMSG_SVS_ACCESS_DENIED

Language = USEnglish
Login - Access Denied
%1User: %12
%1Attempted to Access - %11
.
;// ------------------------------------------------------------

MessageId = 4
Severity = Warning
SymbolicName = SVMSG_SVS_ACCESS_LOGGED_OUT

Language = USEnglish
%1User: %12 Has Logged out.
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
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 1
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU

Language = USEnglish
File Menu
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 2
Severity = Informational
SymbolicName = SECURITY_POINT_ROOT

Language = USEnglish
Root
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 3
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_NEW

Language = USEnglish
New
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 4
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION

Language = USEnglish
Select Configuration
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 5
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION

Language = USEnglish
Close Configuration
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 6
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION_AS

Language = USEnglish
Save Configuration As
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 7
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION    

Language = USEnglish
Save Configuration
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 8
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_PRINT                 

Language = USEnglish
Print
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 9
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_PRINT_SETUP

Language = USEnglish
Print Setup
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 10
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_SAVE_IMAGE            

Language = USEnglish
Save Image
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 11
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_RECENT_CONFIGURATIONS 

Language = USEnglish
Recent Configurations
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 12
Severity = Informational
SymbolicName = SECURITY_POINT_FILE_MENU_EXIT                  

Language = USEnglish
Exit
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 13
Severity = Informational
SymbolicName = SECURITY_POINT_VIEW_MENU
           
Language = USEnglish
View Menu
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 14
Severity = Informational
SymbolicName = SECURITY_POINT_VIEW_MENU_PPQ_BAR       

Language = USEnglish
PPQ Bar
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 15
Severity = Informational
SymbolicName = SECURITY_POINT_VIEW_MENU_IMAGE_DISPLAY_UPDATE

Language = USEnglish
Image Display Update
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 16
Severity = Informational
SymbolicName = SECURITY_POINT_MODE_MENU

Language = USEnglish
Mode Menu
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 17
Severity = Informational
SymbolicName = SECURITY_POINT_MODE_MENU_RUN

Language = USEnglish
Run
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 18
Severity = Informational
SymbolicName = SECURITY_POINT_MODE_MENU_STOP

Language = USEnglish
Stop
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 19
Severity = Informational
SymbolicName = SECURITY_POINT_MODE_MENU_EDIT_TOOLSET

Language = USEnglish
Edit Toolset
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 21
Severity = Informational
SymbolicName = SECURITY_POINT_MODE_MENU_TEST

Language = USEnglish
Test
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 22
Severity = Informational
SymbolicName = SECURITY_POINT_MODE_MENU_REGRESSION_TEST

Language = USEnglish
Regression Test
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 23
Severity = Informational
SymbolicName = SECURITY_POINT_EXTRAS_MENU    

Language = USEnglish
Extras Menu
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 24
Severity = Informational
SymbolicName = SECURITY_POINT_EXTRAS_MENU_ADDITIONAL_ENVIRON

Language = USEnglish
Additional Environment Settings
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 25
Severity = Informational
SymbolicName = SECURITY_POINT_EXTRAS_MENU_SECURITY_MANAGER

Language = USEnglish
Security Manager
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 26
Severity = Informational
SymbolicName = SECURITY_POINT_EXTRAS_MENU_TEST_OUTPUTS

Language = USEnglish
Test Outputs
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 27
Severity = Informational
SymbolicName = SECURITY_POINT_EXTRAS_MENU_UTILITIES_SETUP

Language = USEnglish
Utilities Setup
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 28
Severity = Informational
SymbolicName = SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN

Language = USEnglish
Utilities Run
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 29
Severity = Informational
SymbolicName = SECURITY_POINT_UNRESTRICTED_FILE_ACCESS

Language = USEnglish
Unrestricted File Access
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 30
Severity = Informational
SymbolicName = SECURITY_POINT_VIEW_MENU_RESET_COUNTS_CURRENT       

Language = USEnglish
Reset Counts Current IP
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 31
Severity = Informational
SymbolicName = SECURITY_POINT_VIEW_MENU_RESET_COUNTS_ALL       

Language = USEnglish
Reset Counts All IPs
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 32
Severity = Informational
SymbolicName = SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE       

Language = USEnglish
Exit Run Mode
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 33
Severity = Informational
SymbolicName = SECURITY_POINT_VIEW_MENU_RESULT_DISPLAY_UPDATE

Language = USEnglish
Result Display Update
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 34
Severity = Informational
SymbolicName = SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION

Language = USEnglish
Automatically Save Configuration
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 35
Severity = Informational
SymbolicName = SECURITY_POINT_EDIT_MENU

Language = USEnglish
Edit Menu
%1User: %12 - %11
.
;// ------------------------------------------------------------
MessageId = 36
Severity = Informational
SymbolicName = SECURITY_POINT_EDIT_MENU_RESULT_PICKER

Language = USEnglish
Results Picker
%1User: %12 - %11
.
;// ------------------------------------------------------------

;//
;// =============== End SVAccess Messages ===============
;//

;//
;// =============== SVMachineMessage Messages ===============
;//
MessageId = 0
Facility = SVMachineMessage

Severity = Success
SymbolicName = SVMSG_SVMM_BASE_MASK

Language = USEnglish
.
;// ------------------------------------------------------------
;//
;// =============== End SVMachineMessage Messages ===============
;//

;//
;// =============== SVCmdComServer Messages ===============
;//
MessageId = 0
Facility = SVCmdComServer

Severity = Success
SymbolicName = SVMSG_CMDCOMSRV_NO_ERROR

Language = USEnglish
SVIM Command Com Server - No Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 
Severity = Warning
SymbolicName = SVMSG_CMDCOMSRV_ERROR

Language = USEnglish
SVIM Command Com Server - Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 
Severity = Warning
SymbolicName = SVMSG_CMDCOMSRV_XML_ERROR

Language = USEnglish
SVIM Command Com Server - XML Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 
Severity = Warning
SymbolicName = SVMSG_CMDCOMSRV_MEMORY_ERROR

Language = USEnglish
SVIM Command Com Server - Memory Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 
Severity = Warning
SymbolicName = SVMSG_CMDCOMSRV_PACKEDFILE_ERROR

Language = USEnglish
SVIM Command Com Server - Packed File Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 
Severity = Warning
SymbolicName = SVMSG_CMDCOMSRV_FILE_ERROR

Language = USEnglish
SVIM Command Com Server - File Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId =
Severity = Warning
SymbolicName = SVMSG_IMAGE_NOT_LOCKED

Language = USEnglish
SVIM Command Com Control - Image Not Locked
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId =
Severity = Warning
SymbolicName = SVMSG_NOT_ALL_IMAGES_UNLOCKED

Language = USEnglish
SVIM Command Com Control - Not All Images Unlocked
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId =
Severity = Warning
SymbolicName = SVMSG_NO_PPQ_FOUND

Language = USEnglish
SVIM Command Com Control - No PPQ Found
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId =
Severity = Fatal
SymbolicName = SVMSG_NO_CONFIGURATION_FOUND

Language = USEnglish
SVIM Command Com Control - No Configuration Found
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId =
Severity = Fatal
SymbolicName = SVMSG_ERROR_LOADING_CONFIGURATION

Language = USEnglish
SVIM Command Com Control - Error loading configuration
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId =
Severity = Fatal
SymbolicName = SVMSG_ERROR_CREATING_DIRECTORY

Language = USEnglish
SVIM Command Com Control - Error creating directory
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId =
Severity = Fatal
SymbolicName = SVMSG_ERROR_CLOSING_CLEANING_CONFIG

Language = USEnglish
SVIM Command Com Control - Error in closing and cleaning configuration
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId =
Severity = Fatal
SymbolicName = SVMSG_ERROR_UNPACKING_FILE

Language = USEnglish
SVIM Command Com Control - Error unpacking file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId =
Severity = Fatal
SymbolicName = SVMSG_ERROR_OPENING_PACKING_FILE

Language = USEnglish
SVIM Command Com Control - Error opening pack file
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//
;// =============== End SVCmdComServer Messages ===============
;//

;//
;// =============== SVCmdComClient Messages ===============
;//
MessageId = 0
Facility = SVCmdComClient

Severity = Success
SymbolicName = SVMSG_CMDCOMCTRL_NO_ERROR

Language = USEnglish
SVIM Command Com Control - No Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId =
Severity = Warning
SymbolicName = SVMSG_CMDCOMCTRL_ERROR

Language = USEnglish
SVIM Command Com Control - Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 
Severity = Warning
SymbolicName = SVMSG_CMDCOMCTRL_XML_ERROR

Language = USEnglish
SVIM Command Com Control - XML Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId =
Severity = Warning
SymbolicName = SVMSG_CMDCOMCTRL_MEMORY_ERROR

Language = USEnglish
SVIM Command Com Control - Memory Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId =
Severity = Warning
SymbolicName = SVMSG_CMDCOMCTRL_FILE_ERROR

Language = USEnglish
SVIM Command Com Control - File Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId =
Severity = Warning
SymbolicName = SVMSG_CMDCOMCTRL_PROFESSIONAL_LICENSE_ONLY

Language = USEnglish
SVIM Command Com Control - This function is only available with a Professional license.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId =
Severity = Warning
SymbolicName = SVMSG_CMDCOMCTRL_VARIANT_FORMAT_ERROR

Language = USEnglish
SVIM Command Com Control - Invalid VARIANT Format
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6) %11
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//
;// =============== End SVCmdComClient Messages ===============
;//


;//
;// =============== SVDataManager Messages ===============
;//
MessageId = 0
Facility = SVDataManager

Severity = Success
SymbolicName = SVDM_NO_ERROR

Language = USEnglish
 Data Manager - No Error
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1500
Severity = Warning
SymbolicName = SVDM_1500INVALIDINDEXARRAYHANDLE_ERROR

Language = USEnglish
Data Manager - Tried to use an invalid index array handle.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1501
Severity = Warning
SymbolicName = SVDM_1501INVALIDLOCKTYPE_ERROR

Language = USEnglish
Data Manager - Tried to use an invalid lock type.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1502
Severity = Warning
SymbolicName = SVDM_1502NO_INDEXESAVAILABLE_ERROR

Language = USEnglish
Data Manager - There are no indexes currently available.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1503
Severity = Warning
SymbolicName = SVDM_1503INVALIDBUFFERINDEX_ERROR

Language = USEnglish
Data Manager - Tried to use an invalid buffer index.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1504
Severity = Warning
SymbolicName = SVDM_1504BUFFERINDEXNOTINUSE_ERROR

Language = USEnglish
Data Manager - Tried to use a buffer index that was not allocated.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1505
Severity = Warning
SymbolicName = SVDM_1505NOLOCKSTORELEASE_ERROR

Language = USEnglish
Data Manager - Tried to release a lock type that was not locked.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1506
Severity = Informational
SymbolicName = SVDM_1506FINALLOCKTYPERELEASE_INFORMATION

Language = USEnglish
Data Manager - This was the final release for the requested lock type.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1507
Severity = Informational
SymbolicName = SVDM_1507FINALINDEXRELEASE_INFORMATION

Language = USEnglish
Data Manager - This was the final release for the requested index.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1508
Severity = Fatal
SymbolicName = SVDM_1508INDEXESNOTFREED_FATAL

Language = USEnglish
Data Manager - An attempt was made to destroy a managed array prior to all references being released.  The managed array was NOT destroyed.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1509
Severity = Fatal
SymbolicName = SVDM_1509NOALLOCATE_FATAL

Language = USEnglish
Data Manager - Could not allocate structure for pNewManagedIndexArray.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1510
Severity = Warning
SymbolicName = SVDM_1510MAXIMUMDEPTH_ERROR

Language = USEnglish
Data Manager - The array depth used to create a managed index array was too large.  Maximum array depth permitted is 500,000.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1511
Severity = Fatal
SymbolicName = SVDM_1511NOALLOCATE_FATAL

Language = USEnglish
Data Manager - Could not allocate structure for svmpIndexArray.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1512
Severity = Fatal
SymbolicName = SVDM_1512COULDNOTLOCKDATAMANAGER_FATAL

Language = USEnglish
Data Manager - Could not acquire lock on DataManager.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1513
Severity = Fatal
SymbolicName = SVDM_1513COULDNOTLOCKMANAGEDARRAY_FATAL

Language = USEnglish
Data Manager - Could not acquire lock on pManagedIndexArray.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1514
Severity = Fatal
SymbolicName = SVDM_1514NOTUSED_FATAL

Language = USEnglish
Data Manager - NOT USED.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1515
Severity = Fatal
SymbolicName = SVDM_1515COULDNOTLOCKMANAGEDARRAY_FATAL

Language = USEnglish
Data Manager - Could not acquire lock on pManagedIndexArray.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1516
Severity = Fatal
SymbolicName = SVDM_1516COULDNOTLOCKMANAGEDARRAY_FATAL

Language = USEnglish
Data Manager - Could not acquire lock on pManagedIndexArray.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1517
Severity = Fatal
SymbolicName = SVDM_1517COULDNOTLOCKMANAGEDARRAY_FATAL

Language = USEnglish
Data Manager - Could not acquire lock on pManagedIndexArray.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1518
Severity = Fatal
SymbolicName = SVDM_1518COULDNOTLOCKDATAMANAGER_FATAL

Language = USEnglish
Data Manager - Could not acquire lock on DataManager.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1519
Severity = Fatal
SymbolicName = SVDM_1519ACQUISITIONNOLOCKTORELEASE_FATAL

Language = USEnglish
Data Manager - An attempt was made to free an index as type SV_ACQUISITION when there were no locks under that type.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1520
Severity = Fatal
SymbolicName = SVDM_1520INSPECTIONNOLOCKTORELEASE_FATAL

Language = USEnglish
Data Manager - An attempt was made to free an index as type SV_INSPECTION when there were no locks under that type.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1521
Severity = Fatal
SymbolicName = SVDM_1521PPQNOLOCKTORELEASE_FATAL

Language = USEnglish
Data Manager - An attempt was made to free an index as type SV_PPQ when there were no locks under that type.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1522
Severity = Fatal
SymbolicName = SVDM_1522ARCHIVENOLOCKTORELEASE_FATAL

Language = USEnglish
Data Manager - An attempt was made to free an index as type SV_ARCHIVE when there were no locks under that type.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1523
Severity = Fatal
SymbolicName = SVDM_1523DCOMNOLOCKTORELEASE_FATAL

Language = USEnglish
Data Manager - An attempt was made to free an index as type SV_DCOM when there were no locks under that type.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1524
Severity = Fatal
SymbolicName = SVDM_1524OTHERNOLOCKTORELEASE_FATAL

Language = USEnglish
Data Manager - An attempt was made to free an index as type SV_OTHER when there were no locks under that type.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1525
Severity = Informational
SymbolicName = SVDM_1525FINALACQUISITIONRELEASE_INFORMATION

Language = USEnglish
Data Manager - Performed the final release of type SV_ACQUISITION for the requested index.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1526
Severity = Informational
SymbolicName = SVDM_1526FINALINSPECTIONRELEASE_INFORMATION

Language = USEnglish
Data Manager - Performed the final release of type SV_INSPECTION for the requested index.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1527
Severity = Informational
SymbolicName = SVDM_1527FINALPPQRELEASE_INFORMATION

Language = USEnglish
Data Manager - Performed the final release of type SV_PPQ for the requested index.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

MessageId = 1528
Severity = Informational
SymbolicName = SVDM_1528FINALARCHIVERELEASE_INFORMATION

Language = USEnglish
Data Manager - Performed the final release of type SV_ARCHIVE for the requested index.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1529
Severity = Informational
SymbolicName = SVDM_1529FINALDCOMRELEASE_INFORMATION

Language = USEnglish
Data Manager - Performed the final release of type SV_DCOM for the requested index.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1530
Severity = Informational
SymbolicName = SVDM_1530FINALOTHERRELEASE_INFORMATION

Language = USEnglish
Data Manager - Performed the final release of type SV_OTHER for the requested index.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1531
Severity = Fatal
SymbolicName = SVDM_1531MANAGEDINDEXARRAY_FATAL

Language = USEnglish
Data Manager - The managed index array being referenced appears to be corrupt.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1532
Severity = Informational
SymbolicName = SVDM_1532FINALDCOMRELEASE_INFORMATION

Language = USEnglish
Data Manager - Performed the final release of type SV_PPQ for the DCOM for the requested index.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1533
Severity = Fatal
SymbolicName = SVDM_1533LASTINSPECTIONNOLOCKTORELEASE_FATAL

Language = USEnglish
Data Manager - An attempt was made to free an index as type SV_LASTINSPECTION when there were no locks under that type.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1534
Severity = Informational
SymbolicName = SVDM_1534FINALLASTINSPECTIONRELEASE_INFORMATION

Language = USEnglish
Data Manager - Performed the final release of type SV_LASTINSPECTION for the requested index.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1535
Severity = Fatal
SymbolicName = SVDM_1535COULDNOTLOCKMANAGEDARRAY_FATAL

Language = USEnglish
Data Manager - Could not acquire lock on pManagedIndexArray.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1536
Severity = Fatal
SymbolicName = SVDM_1536DISPLAY_NOLOCKTORELEASE_FATAL

Language = USEnglish
Data Manager - An attempt was made to free an index as type SV_DISPLAY when there were no locks under that type.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1537
Severity = Informational
SymbolicName = SVDM_1537DISPLAY_FINALRELEASE_INFORMATION

Language = USEnglish
Data Manager - Performed the final release of type SV_DISPLAY for the requested index.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1538
Severity = Fatal
SymbolicName = SVDM_1538LASTINSPECTED_NOLOCKTORELEASE_FATAL

Language = USEnglish
Data Manager - An attempt was made to free an index as type SV_LAST_INSPECTED when there were no locks under that type.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1539
Severity = Informational
SymbolicName = SVDM_1539LASTINSPECTED_FINALRELEASE_INFORMATION

Language = USEnglish
Data Manager - Performed the final release of type SV_LAST_INSPECTED for the requested index.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1540
Severity = Fatal
SymbolicName = SVDM_1540LOCK_INVALIDTRANSACTIONID_FATAL

Language = USEnglish
Data Manager - Attempted to lock an index using an incorrect Transaction ID.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1541
Severity = Fatal
SymbolicName = SVDM_1541RELEASE_INVALIDTRANSACTIONID_FATAL

Language = USEnglish
Data Manager - Attempted to release an index using an incorrect Transaction ID.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
MessageId = 1542
Severity = Warning
SymbolicName = SVDM_1542_USED_LAST_INDEX_WARNING

Language = USEnglish
Data Manager - Last index was aquired.  Release required before next index is acquired.
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------

;//
;// =============== SVINTEK Messages ===============
;//
MessageId = 0
Facility = SVIntek

Severity = Success
SymbolicName = SVMSG_SVINTEK_NO_ERROR

Language = USEnglish
No Error - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
;//
;// =============== End SVINTEK Messages ===============
;//

;// ------------------------------------------------------------
;//
;// =============== SVIMAGECOMPRESSION Messages ===============
;//
MessageId = 100
Facility = SVImageCompression

Severity = Fatal
SymbolicName = SVMSG_SVIMAGECOMPRESSION_NO_STATIC_POOL

Language = USEnglish
Unable to allocate the Static Pool - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
;//
;// =============== End SVIMAGECOMPRESSION Messages ===============
;//

;// ------------------------------------------------------------
;//
;// =============== SVEQUATION Messages ===============
;//
MessageId = 100
Facility = SVEquation

Severity = Fatal
SymbolicName = SVMSG_TOO_MANY_VARIABLES

Language = USEnglish
Equation Parser Error : Too Many Variables - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
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

Severity = Fatal
SymbolicName = SVMSG_IMAGE_FORMAT_ERROR

Language = USEnglish
File Acquisition Error : Image Format Error - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;//
MessageId = 119
Facility = SVFileAcquisition

Severity = Fatal
SymbolicName = SVMSG_IMAGE_LOAD_ERROR

Language = USEnglish
File Acquisition Error : Image Load Error - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
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
No Error - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;//
MessageId = 1
Facility = SVMatroxGige

Severity = Fatal
SymbolicName = SVMSG_MATROX_GIGE_RECONNECT_ERROR

Language = USEnglish
Matrox Gige Error : Reconnect Error - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;//
MessageId = 2
Facility = SVMatroxGige

Severity = Fatal
SymbolicName = SVMSG_MATROX_GIGE_DISCONNECT_ERROR

Language = USEnglish
Matrox Gige Error : Disconnect Error - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
;//
;// =============== End SVMATROXGIGE Messages ===============
;//

;// ------------------------------------------------------------
;//
;// =============== SVSystemLibrary Messages ===============
;//
MessageId = 1
Facility = SVSystemLibrary

Severity = Fatal
SymbolicName = SVMSG_QUEUE_USER_APC_ERROR

Language = USEnglish
SVAsyncProcedure : QueueUserAPC Error - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
MessageId = 2
Facility = SVSystemLibrary

Severity = Fatal
SymbolicName = SVMSG_THREAD_CREATION_ERROR

Language = USEnglish
SVThread : Thread Creation Error - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
MessageId = 3
Facility = SVSystemLibrary

Severity = Fatal
SymbolicName = SVMSG_THREAD_EXIT_ERROR

Language = USEnglish
SVAsyncProcedure : Thread Exit Error - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
;//
;// =============== End SVSystemLibrary Messages ===============
;//
;// ------------------------------------------------------------
;//
;// =============== SVTVicLpt Messages ===============
;//
MessageId = 1
Facility = SVTVicLpt

Severity = Warning
SymbolicName = SVMSG_INVALID_LINE_STATE

Language = USEnglish
SVTVicLpt Invalid Line State - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
;//
;// =============== End SVTVicLpt Messages ===============
;//

;//
;// =============== SVMatroxLibrary Messages ===============
;//
MessageId = 0
Facility = SVMatroxLibrary

Severity = Success
SymbolicName = SVMSG_SVMATROXLIBRARY_NO_ERROR

Language = USEnglish
No Error - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
;//
MessageId = 0
Facility = SVMatroxLibrary

Severity = Fatal
SymbolicName = SVMSG_SVMATROXLIBRARY_UNKNOWN_FATAL_ERROR

Language = USEnglish
Error - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
;//
;// =============== End SVMATROXGIGE Messages ===============
;//

;//
;// =============== SVCI Messages ===============
;//
MessageId = 0
Facility = SVCI

Severity = Success
SymbolicName = SVMSG_SVCI_NO_ERROR

Language = USEnglish
No Error - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
;//
MessageId = 0
Facility = SVCI

Severity = Informational
SymbolicName = SVMSG_SVCI_UNKNOWN_INFORMATIONAL

Language = USEnglish
Informational - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
;//
MessageId = 0
Facility = SVCI

Severity = Fatal
SymbolicName = SVMSG_SVCI_UNKNOWN_FATAL_ERROR

Language = USEnglish
Error - %11
%1#Details#
%1Source File: %2:[%3] (%4)
%1Program Code: %9 (%10)
%1OSError:     %5 (%6)
%1Compiled:    %7 %8
.
;// ------------------------------------------------------------
;//
;// =============== End SVCI Messages ===============
;//
