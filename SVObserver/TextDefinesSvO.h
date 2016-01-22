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
	const TCHAR* const SVObserverExecutableDirectoryPath = 
		_T("C:\\SVOBserver\\bin\\");///< the path of the Directory containing SvObserver executables.
	const TCHAR* const SVObserverAutosavePath = _T("D:\\SVObserver\\Autosave\\");///< the SVObserver autosave directory
	const TCHAR* const NoneString= 	_T("<none>");
	const TCHAR* const FbwfDllName= _T("fbwflib.dll");///< the name of the DLL providing the file based write filter functionality
	const TCHAR* const ContextMenuImageSaveLocation = _T("D:\\SVObserver\\Image.bmp"); ///< this is the default location for images that are saved using a context menu
	const TCHAR* const DisplayedImageIsUnavailable  = _T("Displayed image is unavailable");


	const TCHAR* const   Error_you_have_Selected_X_Must_less_then_Y =  _T("Error: You have selected %ld for the Max Images count. The image count must be less then %ld"); 
} /* namespace SVObserver */ } /* namespace Seidenader */

namespace SvO = Seidenader::SVObserver;