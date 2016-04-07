//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the central location for strings used in the SVObserver project.
//******************************************************************************

#pragma once

namespace Seidenader { namespace SVObserver
{
	const TCHAR* const ErrorDetachingInput = _T( "Error detaching input." );
	const TCHAR* const ErrorFillingInputs = _T( "Error filling inputs." );
	const TCHAR* const ErrorFindingPPQEntries = _T( "Error finding PPQ entries." );
	const TCHAR* const ErrorGettingInputObjectList = _T( "Error getting input object list." );
	const TCHAR* const ErrorGettingInputs = _T( "Error getting inputs." );
	const TCHAR* const ErrorGettingOutputs = _T( "Error getting outputs." );
	const TCHAR* const ErrorGettingPPQ = _T( "Error getting PPQ." );
	const TCHAR* const ErrorGettingPPQCount = _T( "Error getting PPQ count." );
	const TCHAR* const ErrorGettingTimeStamp = _T( "Error getting time stamp." );
	const TCHAR* const ErrorLockingInputImageRequests = _T( "Error locking input image requests." );
	const TCHAR* const ErrorLockingInputRequests = _T( "Error locking input requests." );
	const TCHAR* const ErrorRemovingAllInputRequests = _T( "Error removing all input requests." );
	const TCHAR* const ErrorRemovingAllInputImageRequests = _T( "Error removing all input image requests." );
	const TCHAR* const ErrorUnlockingInputImageRequests = _T( "Error unlocking input image requests." );
	const TCHAR* const ErrorUnlockingInputRequests = _T( "Error unlocking input requests." );
	const TCHAR* const c_textImportInspectionError = _T("Inspection was exported using SVObserver %s. \tThis is incompatible with SVObserver %s.");
	const TCHAR* const ToolClipboardFormat = _T( "SVO-Tool" );
	const TCHAR* const TempFolder = _T( "C:\\Temp" );
	const TCHAR* const RunFolder = _T( "C:\\RUN" );
	const TCHAR* const ClipboardFileName = _T( "ClipboardFile" );
	const TCHAR* const ZipExtension = _T( ".zip" );
	const TCHAR* const XmlExtension = _T( ".xml" );
	const TCHAR* const XmlNameSpace = _T( "xmlns" );
	const TCHAR* const SvrNameSpace = _T( "x-schema:#SVR00001" );
	const TCHAR* const SV_BaseNode = _T( "SV_BASENODE" );
	const TCHAR* const InsertingTool = _T( "Inserting Tool ..." );
	const TCHAR* const LoadingResourceDllFailed = _T( "Could not load resource dll.\nThe program will terminated." );
	const TCHAR* const SetClipboardDataFailed = _T( "Setting the data into the clipboard failed." );
	const TCHAR* const ClipboardMemoryFailed = _T( "Clipboard memory allocation failed." );
	const TCHAR* const GetClipboardDataFailed = _T( "Failed to retrieve the clipboard data." );
	const TCHAR* const ToolInvalid = _T( "The tool to be copied is invalid." );
	const TCHAR* const ClipboardDataConverionFailed = _T( "The clipboard data conversion failed." );
	const TCHAR* const VersionMismatch = _T( "The current SVObserver Version does not match the version coming from the clipboard." );
	const TCHAR* const ColorToolInsertFailed = _T( "A color tool cannot be inserted into a non-color system." );
	const TCHAR* const NonColorToolInsertFailed = _T( "A color tool must always be the first tool in a color system." );
	const TCHAR* const EndListDelimiter = _T( "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------" );
	const TCHAR* const ColorToolMustBeFirstMessage = _T( "A Color Tool must ALWAYS be\n the first tool on a color system." );
	const TCHAR* const SVTOOLPARAMETERLIST_MARKER = _T( "** SVSetToolParameterList MARKER **" );
	const TCHAR* const SVEXCLUDECHARS_CONFIG_NAME =  _T( "\\/:*\"<>|;&$@'[]{}()^~!%?=§€`´°²³µ#" );
	const TCHAR* const SVEXCLUDECHARS_TOOL_IP_NAME = _T( "\\/:*\"<>|;&$@'[]{}()^~!%?=§€`´°²³µ#,.+-\xd7\xf7" );
	const TCHAR* const SizeAdjustTextNone = _T( "Manual" );
	const TCHAR* const AmountOfSystemMemoryText = _T("Amount of physical memory = %d");
	const TCHAR* const SizeAdjustTextFormula = _T( "Formula" );
	const TCHAR* const SizeAdjustTextFullSize = _T( "Auto Fit" );
	const TCHAR* const RingBuffer_Depth_Invalid_Value = _T( "RingBuffer Depth has to be between %d and %d, current %d" );
	const TCHAR* const RingBuffer_ImageIndex1Connector = _T( "RingBufferImageIndex1Connector" );
	const TCHAR* const RingBuffer_ImageIndex2Connector = _T( "RingBufferImageIndex2Connector" );
	const TCHAR* const InvalidFileName = _T("Invalid Keywords in Archive Filename/Path.");
	const TCHAR* const InvalidImagePath = _T("Invalid Keywords in Archive Image Path.");
	const TCHAR* const ErrorUnknownEnum = _T( "Unknown enum" );
	const TCHAR* const RangeValue_EmptyString = _T( "Empty String in %s." );
	const TCHAR* const RangeValue_WrongRange = _T( "%s must between %d and %d." );
	const TCHAR* const GlobalViewHeader[] = {_T("Global Constants"), _T("Type"), _T("Value"), _T("Description")};
	const TCHAR* const Invalid = _T("Invalid");
	const TCHAR* const Auto = _T("Auto");
	const TCHAR* const Average = _T("Average"); 
	const TCHAR* const Bicubic =_T("Bicubic");
	const TCHAR* const Bilinear = _T("Bilinear");
	const TCHAR* const NearestNeighbor = _T("Nearest Neighbor");
	const TCHAR* const Enabled = _T("Enabled");
	const TCHAR* const Disabled = _T("Disabled");
	const TCHAR* const Fast = _T("Fast");
	const TCHAR* const Precise = _T("Precise");
	const TCHAR* const ResizeImage = _T("ResizeImage");
	const TCHAR* const ErrorWriteMonitorListNoSharedMemory = _T("WriteMonitorListToSharedMemory - No Shared memory");
	const TCHAR* const ErrorNotAllDataItemsFound = _T("Not All Data List items found\n");
	const TCHAR* const ErrorNotAllImageItemsFound = _T("Not All Image List items found\n");
	const TCHAR* const ErrorProcessNotifyLastInspected = _T("ProcessNotifyWithLastInspected - %s");
	const TCHAR* const Unknown = _T("Unknown");
	const TCHAR* const ErrorUsedPreviouslyForSec = _T("This was previously used for .sec files.");
	const TCHAR* const ErrorNotEnoughDiskSpace = _T("CanGoOnline %s - Not enough Disk Space Available");
	const TCHAR* const ProcessTrigger =  _T("ProcessTrigger - %s");
	const TCHAR* const ErrorNotAllRejectConditionItemsFound = _T("Not All Reject Condition List items found\n");
	const TCHAR* const ReleaseProduct =  _T("Release Product - %s");
	const TCHAR* const ErrorCopyLastInspectedToReject =  _T("Copy Last Inspected To Reject failed\n");
	const TCHAR* const CommitSharedMemory =  _T("Commit Shared Memory - %s");
	const TCHAR* const ErrorMIL =  _T("%s - An error has occurred, MIL error code = %d, MIL error Text = %s");
	const TCHAR* const AP_NotEnoughMemoryPleaseDeselectImage = 
		_T("Not enough Archive Image Memory for the images selected. Please deselect some images.");
	const TCHAR* const AP_NotEnoughHandlesPleaseDeselectImage = 
		_T("Not enough Archive Image Memory for the images selected. Please deselect some images.");
	const TCHAR* const AP_NotEnoughMemoryInChangeMode = 
		_T("There is not enough Available Archive Image Memory for your selection in Change Mode.\nAvailable Archive Image Memory is the result of the selected images and the Max Images number.");
	const TCHAR* const AP_NotEnoughHandlesInChangeMode = 
		_T("There are not enough Available Archive Image Memory Handles for your selection in Change Mode.\nAvailable Archive Image Memory Handles are the result of the selected images and the Max Images number.");
	const TCHAR* const AP_NotEnoughMemoryPleaseDeselect = 
		_T("Not enough Available Archive Image Memory. Please deselect some\nimages, decrease \"Max Images\" or change \"When to archive\" mode.");
	const TCHAR* const AP_NotEnoughHandlesPleaseDeselect = 
		_T("Not enough Available Archive Image Memory Handles. Please deselect some\nimages, decrease \"Max Images\" or change \"When to archive\" mode.");
	const TCHAR* const AP_NotEnoughMemoryToSelect_S = 
		_T("Not enough Archive Image Memory to select %s");
	const TCHAR* const AP_NotEnoughMemoryHandlesToSelect_S = 
		_T("Not enough Archive Image Memory Handles to select %s");
	const TCHAR* const AP_NotEnoughMemoryForSImages = 
		_T("There is not enough Available Archive Image Memory for %s images in Change Mode. Available\nArchive Image Memory is the result of the selected images and the Max Images number.\nThe selection will be reset.");
	const TCHAR* const AP_NotEnoughHandlesForSImages = 
		_T("There are not enough Available Archive Image Memory Handles for %s images in Change Mode. Available\nArchive Image Memory Handles are the result of the selected images and the Max Images number.\nThe selection will be reset.");
	const TCHAR* const Remote_Input= _T("Remote Input");
	const TCHAR* const DIOInput= _T("DIO.Input");
	///Defines for SVConfigurationObjects
	const TCHAR* const NumberOfIosMissing = _T("the Number of Ios Entries is missing.");
	const TCHAR* const IOEntryIsMissingMissing =  _T("an IO Entry is missing.");
	const TCHAR* const CreationOfPPQFailed =  _T("The PPQ Object could not be created.");
	const TCHAR* const InputIsMissing =  _T("an Input tag is missing");
	const TCHAR* const MsgIONameIsMissing = _T("an Item Name is missing"); 
	const TCHAR* const MsgPPQPOsitionIsMissing   = _T("a PPQ Position is missing"); 
	const TCHAR* const MsgIOTypeIsMissing = _T("an IO Type is missing") ;
	const TCHAR* const MsgRemoteIndexIsMissing = _T("a Remote Index is missing") ;
	const TCHAR* const InputObjectList = _T( "Input Object List");
	const TCHAR* const OutputObjectList =  _T( "Output Object List" );
	const TCHAR* const Create_S_Failed = _T("Create %s failed") ;

	const TCHAR* const ActivatingDiskProtection = 
		_T("Activating disk protection.\nAfter the next restart C:\\ will be protected");
	const TCHAR* const DeactivatingDiskProtection = 
		_T("Deactivating disk protection.\nAfter the next restart C:\\ will not be protected");
	const TCHAR* const DiskProtectionRemainsActive = 
		_T("Disk protection will remain active.\n");
	const TCHAR* const DiskProtectionRemainsInactive = 
		_T("Disk protection will remain inactive.");
	const TCHAR* const SVObserverDirectoryPath = _T("C:\\SVOBserver\\");///< the path of the SvObserver directory.
	const TCHAR* const SecondSVObserverDirectoryPath = 
		_T("D:\\SVOBserver\\");///< the path of SVObserver directory (not the main one but the one on drive D:)
	const TCHAR* const SVObserverAutosavePath = _T("D:\\SVObserver\\Autosave\\");///< the SVObserver autosave directory
	const TCHAR* const NoneString= 	_T("<none>");
	const TCHAR* const FbwfDllName= _T("fbwflib.dll");///< the name of the DLL providing the file based write filter functionality
	const TCHAR* const ContextMenuImageSaveLocation = _T("D:\\SVObserver\\Image.bmp"); ///< this is the default location for images that are saved using a context menu
	const TCHAR* const DisplayedImageIsUnavailable  = _T("Displayed image is unavailable");
	const TCHAR* const AvailableArchiveImageMemory  = _T( "Available Archive Image Memory:  %8.1f MB" );

	const TCHAR* const ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME = _T("ArchiveToolOnlineAsync");
	const TCHAR* const ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME = _T("ArchiveToolGoOffline");

	const TCHAR* const   Error_you_have_Selected_X_Must_less_then_Y = _T("Error: You have selected %ld for the Max Images count. The image count must be less then %ld"); 
	const TCHAR* const   Error_WrongCameraModel = _T("Wrong camera model: Expected %s, Current %s"); 
	const TCHAR* const   Error_WrongCameraVendor = _T("Wrong camera vendor: Expected %s, Current %s"); 
	const TCHAR* const   Error_ScriptParseFailed = _T("%s parse was not successful");
	const TCHAR* const   Error_NoFontFileSpec = _T("No Font File Specified");
	const TCHAR* const   Error_FontNotMfoExt = _T( "Font File Requires '.mfo' Extension: %s" );
	const TCHAR* const   Error_FontFileNotExist = _T( "Font File Does Not Exist: %s" );
	const TCHAR* const   Error_FontFileEmpty = _T( "Font File Empty: %s" );
	const TCHAR* const   Error_ArchiveTool_CreateFileFailed = _T("ERROR: Create Archive File: %s Failed");
	const TCHAR* const   Error_PathFileNotFound = _T("Path/File not found:  %s");
	const TCHAR* const   BarCode_UnableToRead = _T("Bar Code Analyzer was unable to read\nthe match string from file. Check the\nanalyzer settings for proper match string.");
	const TCHAR* const   BarCode_UnableToSave = _T("Bar Code Analyzer was unable to save\nthe match string to file. Check the\nanalyzer settings for proper match string.");
	const TCHAR* const   BlobAnalyzer_ResultCreationFailed = _T("Creation of Blob Analyzer Result Failed");
	const TCHAR* const   Acquisition_SourceInformationFailed = _T("Failed to get acquisition source information!");
	const TCHAR* const   InvalidData = _T("Invalid Data");
	const TCHAR* const   ImageClass_DestroyError = _T("Failed to destroy the image object!");
	const TCHAR* const   IPDoc_NoCameraColorAttached = _T("Color camera not attached to this IPDoc");
	const TCHAR* const   SVIODoc_ClosingImpossible = _T("Invalid Operation!\nThe IO Dialog can not be closed without\nclosing the whole configuration.");
	const TCHAR* const   SVIPDoc_ClosingImpossible = _T("Invalid Operation!\nIf You wish to remove this inspection\nGo to the Edit menu and Select Edit Configuration.");
	const TCHAR* const   SVIPDoc_GoIntoRegTestFailed = _T("Inspection is unable to go into Regression Test Mode.  There is an unknown error with the inspection.");
	const TCHAR* const   LightReference_NotAvailable = _T("Light Reference not available for %s");
	const TCHAR* const   LUT_NotAvailable = _T("LUT not available for camera(s)");
	const TCHAR* const   CreateBufferFailed = _T("Failed to create an image buffer!");
	const TCHAR* const   MatroxImage_UnableToFindFile = _T( "Unable to locate image file:\n%s\n\nBrowse..." );
	const TCHAR* const   FailedToLoadImage = _T("Failed to load an image!");
	const TCHAR* const   ParseError_NameMustNotContainKeyword = _T("Parser Error:\nName expression expected, valid names must not contain keywords!");
	const TCHAR* const   ParseError_OutOfMemory = _T("Parser Error:\nOut of memory!");
	const TCHAR* const   ParseError_NameExpressionExpected = _T("Parser Error:\nName expression expected!");
	const TCHAR* const   ParseError_OwnerNotExist = _T("Parser Error:\nOwner doesn't exist, cannot access member!");
	const TCHAR* const   ParseError_UnexpectedMember = _T("Parser Error:\nUnexpected member access operator!");
	const TCHAR* const   ParseError_CannotReadGuid = _T("Parser Error:\nCannot read GUID!");
	const TCHAR* const   ParseError_ExpressionExpected = _T("Parser Error:\nExpression expected!");
	const TCHAR* const   ParseError_UnknownExpression = _T("Parser Error:\nUnknown expression!");
	const TCHAR* const   ParseError_UnexpectedEndOfCommand = _T("Parser Error:\nUnexpected end of command!");
	const TCHAR* const   ParseError_CriticalErrorRemovingDependencies = _T("Critical Error removing external dependencies");
	const TCHAR* const   ParseError_UnexpectedEndOfParse = _T("ParserState: READ_STRING\nParser Error, unexpected end of parse string while reading string...");
	const TCHAR* const   ParseError_UnexpectedEndOfName = _T("ParserState: READ_NAME_STRING\nParser Error, unexpected end of name string while parsing ...");
	const TCHAR* const   ParseError_EmptyToken = _T("ParserState: READ_GUID_STRING\nParser Error, empty token found...");
	const TCHAR* const   ParseError_GuidCannotEvaluated = _T("ParserState: READ_GUID_STRING\nParser Error, GUID expression cannot be evaluated...");
	const TCHAR* const   ParseError_NameOperandExpected = _T("Parser Error:\nName operand expected!");
	const TCHAR* const   ParseError_OperatorExpected = _T("Parser Error:\nOperator = expected!");
	const TCHAR* const   ParseError_OperandExpected = _T("Parser Error:\nObject operand, class operand or embedded operand expected!");
	const TCHAR* const   ParseError_CommandTerminationExpected = _T("Parser Error:\nCommand termination expected!");
	const TCHAR* const   ParseError_TooManyExpressions = _T("Parser Error:\nToo many expressions found!");
	const TCHAR* const   ParseError_ClassOperandExpected = _T("Parser Error:\nClass operand expected!");
	const TCHAR* const   ParseError_OperatorOperand = _T("Parser Error:\nOperator operand expected!");
	const TCHAR* const   ParseError_ObjectOperandExpected = _T("Parser Error:\nObject operand expected!");
	const TCHAR* const   ParseError_UnableToReadInputList = _T("Parser Error:\nUnable to read input list!");
	const TCHAR* const   ParseError_CommandTerminationOrObjectMemberExpected = _T("Parser Error:\nCommand termination or object member definition expected!");
	const TCHAR* const   ParseError_NoValidObjectOwner = _T("Parser Error:\nNo valid object owner given!");
	const TCHAR* const   ParseError_WrongEmbeddedValues = _T("Parser Error:\nWrong embedded values!");
	const TCHAR* const   ParseError_CannotConstructObject = _T("Parser Error:\nCannot construct new object!");
	const TCHAR* const   ParseError_ObjectRequired = _T("Parser Error:\nObject required for Member!");
	const TCHAR* const   ParseError_ValuesExpected = _T("Parser Error:\nValues expected!");
	const TCHAR* const   ParseError_NoValidObject = _T("Parser Error:\nNo valid object given!");
	const TCHAR* const   ParseError_SetObjectMemberFailed = _T("Parser Error:\nSet Object Member Value failed!");
	const TCHAR* const   SVObserver_CannotRun_WrongModelNumber = _T( "Configuration cannot enter Run.  The current configuration hardware does not match system hardware. "
		"The system's Model Number is ( Model # %s %s %s %s ). Please verify that the shown model number is correct and contact your system administrator.");
	const TCHAR* const   SVObserver_WrongModelNumber = _T( "Hardware configuration error. The SVIM hardware does not match the Model Number ( Model # %s %s %s %s ). "
		"%s is either not available or malfunctioning. Please verify that the shown model number is correct and contact your system administrator.");
	const TCHAR* const   SVObserver_ModelNumberInvalid = _T("The model number specified in OEMINFO.INI is invalid.");
	const TCHAR* const   SVObserver_AuthorizationFailed_Modification = _T( "Authorization Failed.\n\nUtility Modification requires 'User Manager' privilege." );
	const TCHAR* const   SVObserver_AuthorizationFailed_Execution = _T("Authorization Failed.\n\nUtility Execution requires 'User Manager' privilege.");
	const TCHAR* const   SVObserver_MatroxGigELicenseNotFound = _T("Matrox GigE License not found");
	const TCHAR* const   SVObserver_WrongVersionNumber = _T( "This configuration was created by SVObserver %s.\nYou are currently running SVObserver %s.\n"
		"This configuration version may be incompatible with\nthe version of SVObserver that you are running.\nAre you sure you wish to continue ?" );
	const TCHAR* const   SVObserver_ConfigurationLoadFailed = _T( "The configuration could not successfully load.\nhr = %d.\n");
	const TCHAR* const   SVObserver_SecurityFileLoadFailed = _T("Security File Failed to Load\n Using Default Security");
	const TCHAR* const   SVObserver_WrongPathnameEntered = _T("Invalid path or file name found\n\nCheck your inputs and try it again!");
	const TCHAR* const   SVObserver_RegisterClassObjectsFailed = _T( "(hr=0x%08X), Unable to register class objects. COM interface ISVCommand unavailable.");
	const TCHAR* const   SVObserver_OleInitFailed = _T( "OLE initialization failed.  Make sure that the OLE libraries are the correct version.");
	const TCHAR* const   Camera_UseQuestion = _T("Would you like to use this camera file for all camera choices?");
	const TCHAR* const   Config_SwitchResetQuestion = _T("By switching the system (product) the camera, trigger, inspection and ppq data will be reset, do you wish to continue?");
	const TCHAR* const   Config_SwitchInvalidQuestion = _T("By switching the system (product) the camera files will become invalid, do you wish to continue?");
	const TCHAR* const   Config_InspectionImportFailed = _T("Inspection Import failed. Error = %d");
	const TCHAR* const   Error_NoMilHostBuffer = _T("No Mil Host Buffer Pointer");
	const TCHAR* const   Error_MilImageTypeInvalid = _T("MIL Image Type Not 8 Bit Unsigned: %x");
	const TCHAR* const   Error_NoFontConstraintsFileSpec = _T("No Font Constraints File Specified");
	const TCHAR* const   Error_FontConstraintsRequiredMfoExt = _T( "Font Constraints File Requires '.mfo' Extension: %s" );
	const TCHAR* const   Error_FontConstraintsFileNotExist = _T( "Font Constraints File Does Not Exist: %s" );
	const TCHAR* const   Error_FontConstraintsFileEmpty = _T( "Font Constraints File Empty: %s" );
	const TCHAR* const   Error_NoFontControlsFileSpec = _T("No Font Controls File Specified");
	const TCHAR* const   Error_FontControlsRequiredMfoExt = _T( "Font Controls File Requires '.mfo' Extension: %s" );
	const TCHAR* const   Error_FontControlsFileNotExist = _T( "Font Controls File Does Not Exist: %s" );
	const TCHAR* const   Error_FontControlsFileEmpty = _T( "Font Controls File Empty: %s" );
	const TCHAR* const   Error_InvalidThresholdValue = _T("Invalid Threshold Value");
	const TCHAR* const   Pattern_Model2Large = _T("Model is larger than the ROI of the Window Tool. Do you want to change Model (Yes) or leave dialog (No)?");
	const TCHAR* const   RegressionTest_NoBmpFileSelected = _T("Selection Error:  A .bmp file must be selected.");
	const TCHAR* const   RegressionTest_WrongListSize = _T("Selection Error:  Lists must be the same size.  Please select again.");
	const TCHAR* const   RegressionTest_NoFileSelected = _T("Selection Error: \"Use single file\" is selected for one of the cameras but a file was not selected. Select again?");
	const TCHAR* const   RegressionTest_WrongFormat = _T("Selection Error: \"Use list of files\" selected, but the file name does not match\n the acceptable format (<file name>_<sequence number>.bmp). Select again?");
	const TCHAR* const   RegressionTest_NoFilesSelected = _T("Selection Error:  \"No files\" selected.  Select again?");
	const TCHAR* const   RegressionTest_FileNotExist = _T("Selection Error:  File does not exist.  Select again?");
	const TCHAR* const   RemoteOutput_TriggerCountDeleteError = _T("Trigger Count Cannot Be deleted");
	const TCHAR* const   RemoteOutput_DeletingOutput = _T("Are You Sure you want to delete this Output?");
	const TCHAR* const   RemoteOutput_DeletingAllOutput = _T("Are you sure you want to delete All Entries for %s");
	const TCHAR* const   ResultView_InitFailed = _T("Failed to init inspection result view!");
	const TCHAR* const   StatTool_ResultFailed = _T("Creation of Statistics Result Failed");
	const TCHAR* const   Threshold_OutOfRange = _T("Out of Range!\n(0 ... 255)");
	const TCHAR* const   Error_DuplicateEmbeddedId = _T("Error - Duplicate Embedded ID found");
	const TCHAR* const   Error_CannotAddInputRequest = _T("Cannot Add Input Request.\nPlease contact technical support specialist.");
	const TCHAR* const   Error_CannotAddInputRequestMarker = _T("Cannot Add Input Request Marker.\nPlease contact technical support specialist.");
	const TCHAR* const   Error_CannotRunOnce = _T("Cannot Run Once.\nPlease contact technical support specialist.");
	const TCHAR* const   Error_NoAnalyzerDetails = _T("No Analyzer Details available!\nTry to select an Analyzer first.\nNOTICE: Some Tools don´t have an Analyzer!");
	const TCHAR* const   Error_AnalyzerCreationFailed = _T("Creation of Analyzer Failed");
	const TCHAR* const   Error_AnalyzerInstantiationFailed = _T("Instantiation of Analyzer Failed");
	const TCHAR* const   LinearSpecial_OrientationVertical = _T("Tool profile orientation is at vertical.  By turning rotation on the profile orientation will be set to horizontal. Do you wish to continue");
	const TCHAR* const   LinearSpecial_IsRotated = _T("Tool is currently rotated.  By turning off rotation the figure will be drawn at a rotation angle of 0. Do you wish to continue");
	const TCHAR* const   MaskPage_StartOCXFailed = _T("Static Mask Editor OCX Failed to Start");
	const TCHAR* const   Error_CannotOpenFile = _T("Cannot open file!");
	const TCHAR* const   Error_CannotOpenTADlg = _T("Unable to open Tool Adjustment Dialog!");
	const TCHAR* const   Error_NoDeviceContext = _T("No device context available!");
	const TCHAR* const   Error_NoResultObject = _T("Cannot find the Result Object");
	const TCHAR* const   LUT_ShouldOverwriteAllBands = _T("Selecting this option will overwrite all bands with the currently displayed LUT information. Continue?");
} /* namespace SVObserver */ } /* namespace Seidenader */

namespace SvO = Seidenader::SVObserver;