//*****************************************************************************
/// \copyright (c) $YEAR$,$YEAR$ by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Class to generate a text form a ID (enum) and additional text list depending of language settings
//******************************************************************************
#pragma once

#pragma region Includes
#include <map>
#include <tchar.h>
#include <boost\assign\std\map.hpp>
#include <boost\assign\list_of.hpp>
#include "SVUtilityLibrary\SVString.h"
#include "ObjectInterfaces\MessageTextEnum.h"
#pragma endregion Includes

namespace Seidenader { namespace SVStatusLibrary
{
typedef std::map<SvOi::MessageTextEnum, SVString> MessageTextMap;

MessageTextMap g_MessageTextMap = boost::assign::map_list_of
	(SvOi::Tid_Empty, _T(""))
	(SvOi::Tid_Default, _T("%s"))
	(SvOi::Tid_ErrorClass, _T("Error Nbr = %s, Program Cd = %s"))
	(SvOi::Tid_XmlParserError, _T("%s, Error Code:%s, On line %s position %s"))
	(SvOi::Tid_RestoreVariantError, _T("RestoreVariant returns: %s"))
	(SvOi::Tid_TranslateVariantError, _T("TranslateVariant returns: %s"))
	(SvOi::Tid_ToVariant_InvalidArgument, _T("Invalid Argument in function ToVariant"))

	(SvOi::Tid_XML_UnexpectedType, _T("Unexpected TreeElementType in OnStartElement"))
	(SvOi::Tid_XML_UnexpectedOnElementData, _T("Unexpected OnElementData"))
	(SvOi::Tid_XML_InvalidPointer, _T("Invalid Pointer"))
	(SvOi::Tid_XML_InvalidArrayElement, _T("Invalid ArrayElement in OnEndElement"))
	(SvOi::Tid_XML_Error, _T("Error in Line, Column, Errornumber  : %s , %s , %s  : %s"))
	
	//SVObserver SVDLLToolLoadLibraryClass
	(SvOi::Tid_ToolLoadError_ImageDef, _T("Error in MIL/HBITMAP function definitions."))
	(SvOi::Tid_ToolLoadError_NoImageDef, _T("Neither MIL nor HBITMAP image functions are defined."))
	(SvOi::Tid_ToolLoadError_BothSetsImagesDef, _T("Both sets of MIL and HBITMAP image functions are defined."))
	(SvOi::Tid_ToolLoadError_BothImagesDef,  _T("Both MIL and HBITMAP image functions are defined."))
	(SvOi::Tid_ToolLoadError_InFunctionDef, _T("Error in MIL/HBITMAP image functions definitions."))
	(SvOi::Tid_ToolLoadError_SimpleTest, _T("SimpleTest failed."))
	(SvOi::Tid_ToolLoadError_SimpleTestException, _T("SimpleTest failed - exception."))
	(SvOi::Tid_ToolLoadError_DllStartup, _T("DLL Startup function failed."))
	(SvOi::Tid_ToolLoadError_DllStartupException, _T("DLL Startup function failed.- exception"))
	(SvOi::Tid_ToolLoadError_GetToolName, _T("DLL GetToolName function failed."))
	(SvOi::Tid_ToolLoadError_GetToolNameException, _T("DLL GetToolName failed.- exception"))
	(SvOi::Tid_ToolLoadError_GetToolVersion, _T("DLL GetToolVersion function failed."))
	(SvOi::Tid_ToolLoadError_GetToolVersionException, _T("DLL GetToolVersion function failed - exception."))
	(SvOi::Tid_GetProcAddressError_SimpleTest, _T("GetProcAddress failed SimpleTest"))
	(SvOi::Tid_GetProcAddressError_GetToolName, _T("GetProcAddress failed GetToolName"))
	(SvOi::Tid_GetProcAddressError_GetToolVersion, _T("GetProcAddress failed GetToolVersion"))
	(SvOi::Tid_GetProcAddressError_RunTool, _T("GetProcAddress failed RunTool"))
	(SvOi::Tid_GetProcAddressError_Startup, _T("GetProcAddress failed Startup"))
	(SvOi::Tid_GetProcAddressError_ShutDown, _T("GetProcAddress failed ShutDown"))
	(SvOi::Tid_GetProcAddressError_InitializeRun, _T("GetProcAddress failed InitializeRun"))
	(SvOi::Tid_GetProcAddressError_UninitializeRun, _T("GetProcAddress failed UninitializeRun"))
	(SvOi::Tid_GetProcAddressError_GetInputValueDefinitions, _T("GetProcAddress failed GetInputValueDefinitions"))
	(SvOi::Tid_GetProcAddressError_DestroyInputValueDefinitionStructures, _T("GetProcAddress failed DestroyInputValueDefinitionStructures"))
	(SvOi::Tid_GetProcAddressError_DestroyResultValueDefinitionStructures, _T("GetProcAddress failed DestroyResultValueDefinitionStructures"))
	(SvOi::Tid_GetProcAddressError_SetInputValues, _T("GetProcAddress failed SetInputValues"))
	(SvOi::Tid_GetProcAddressError_GetResultValues, _T("GetProcAddress failed GetResultValues"))
	(SvOi::Tid_GetProcAddressError_GetMessageString, _T("GetProcAddress failed GetMessageString"))
	(SvOi::Tid_GetProcAddressError_ValidateValueParameter, _T("GetProcAddress failed ValidateValueParameter"))
	(SvOi::Tid_GetProcAddressError_GetResultValueDefinitions, _T("GetProcAddress failed GetResultValueDefinitions"))
	(SvOi::Tid_GetProcAddressError_GetNumberOfInputImages, _T("GetProcAddress failed GetNumberOfInputImages"))
	(SvOi::Tid_GetProcAddressError_GetResultImageDefinitions, _T("GetProcAddress failed GetResultImageDefinitions"))
	(SvOi::Tid_GetProcAddressError_DestroyImageDefinitionStructure, _T("GetProcAddress failed DestroyImageDefinitionStructure"))
	(SvOi::Tid_ToolLoadError, _T("Could not load DLL (LoadLibrary failed)."))
	(SvOi::Tid_RunTool_Exception, _T("Exception in SVRunTool of some External Dll %s"))
	(SvOi::Tid_SVInitializeRun_Exception, _T("Exception in SVInitializeRun in some External Dll %s"))

	///Defines for SVConfigurationObjects
	(SvOi::Tid_CreateSFailed, _T("Create %s failed"))
	(SvOi::Tid_NumberOfIosMissing, _T("the Number of Ios Entries is missing."))
	(SvOi::Tid_IOEntryIsMissing, _T("an IO Entry is missing."))
	(SvOi::Tid_CreationOfPPQFailed,  _T("The PPQ Object could not be created."))
	(SvOi::Tid_InputIsMissing, _T("an Input tag is missing"))
	(SvOi::Tid_MsgIOTypeIsMissing, _T("an IO Type is missing"))
	(SvOi::Tid_MsgIONameIsMissing, _T("an Item Name is missing"))
	(SvOi::Tid_MsgPPQPOsitionIsMissing, _T("a PPQ Position is missing"))
	(SvOi::Tid_MsgRemoteIndexIsMissing, _T("a Remote Index is missing"))

	(SvOi::Tid_ActivatingDiskProtection, _T("Activating disk protection.\nAfter the next restart C:\\ will be protected"))
	(SvOi::Tid_DeactivatingDiskProtection, _T("Deactivating disk protection.\nAfter the next restart C:\\ will not be protected"))
	(SvOi::Tid_DiskProtectionRemainsActive, _T("Disk protection will remain active.\n"))
	(SvOi::Tid_DiskProtectionRemainsInactive, _T("Disk protection will remain inactive."))
	(SvOi::Tid_CouldNotExecuteFormatString, _T("could not execute '%s'"))

	//RangeValue
	(SvOi::Tid_RangeValue_EmptyString, _T( "Empty String in %s." ))
	(SvOi::Tid_RangeValue_WrongRange, _T( "%s must between %s and %s." ))
	(SvOi::Tid_ErrorUnknownEnum, _T( "Unknown enum" ))
	(SvOi::Tid_FailHigh, _T("Fail High"))
	(SvOi::Tid_FailLow, _T("Fail Low"))
	(SvOi::Tid_WarnHigh, _T("Warn High"))
	(SvOi::Tid_WarnLow, _T("Warn Low"))
	(SvOi::Tid_IsLessThan, _T("ERROR:\n%s\nis less than\n%s"))
	(SvOi::Tid_IsInvalidRef, _T("ERROR:\n%s: %s\nis an invalid reference."))

	(SvOi::Tid_ErrorWriteMonitorListNoSharedMemory, _T("WriteMonitorListToSharedMemory - No Shared memory"))
	(SvOi::Tid_RingBuffer_Depth_Invalid_Value, _T( "RingBuffer Depth has to be between %s and %s, current %s" ))
	(SvOi::Tid_Error_NoFontFileSpec, _T("No Font File Specified"))
	(SvOi::Tid_Error_FontFileNotExist, _T( "Font File Does Not Exist: %s" ))
	(SvOi::Tid_Error_FontFileEmpty, _T( "Font File Empty: %s" ))
	(SvOi::Tid_SVAcquisitionClass_GetNextIndex, _T( "SVAcquisitionClass::GetNextIndex-Name=%s"))
	(SvOi::Tid_ArchiveTool_CreateFileFailed, _T("ERROR: Create Archive File: %s Failed"))
	(SvOi::Tid_PathFileNotFound, _T("Path/File not found:  %s"))
	(SvOi::Tid_BarCode_UnableToRead, _T("Bar Code Analyzer was unable to read\nthe match string from file. Check the\nanalyzer settings for proper match string."))
	(SvOi::Tid_BarCode_UnableToSave, _T("Bar Code Analyzer was unable to save\nthe match string to file. Check the\nanalyzer settings for proper match string."))
	(SvOi::Tid_BlobAnalyzer_ResultCreationFailed, _T("Creation of Blob Analyzer Result Failed"))
	(SvOi::Tid_CreationFailed, _T("Creation of %s Failed"))
	(SvOi::Tid_CreationOf2Failed, _T("Creation of %s \"%s\" failed."))
	(SvOi::Tid_Command_StartSvimConfig, _T("Informational - Starting PutSVIMConfig"))
	(SvOi::Tid_Command_EndSvimConfig, _T("Informational - Ending PutSVIMConfig"))
	(SvOi::Tid_ErrorMIL, _T("%s - An error has occurred, MIL error code = %s, MIL error Text = %s"))
	(SvOi::Tid_ErrorGettingPPQ, _T( "Error getting PPQ." ))
	(SvOi::Tid_ErrorGettingInputObjectList, _T( "Error getting input object list." ))
	(SvOi::Tid_ErrorFillingInputs, _T( "Error filling inputs." ))
	(SvOi::Tid_ErrorGettingPPQCount, _T( "Error getting PPQ count." ))
	(SvOi::Tid_ErrorGettingOutputs, _T( "Error getting outputs." ))
	(SvOi::Tid_Acquisition_SourceInformationFailed, _T("Failed to get acquisition source information!"))
	(SvOi::Tid_TooManyVariables, _T("%s\nToo many variables."))
	(SvOi::Tid_EquationParserError, _T("%s\nSyntax Error at position %s"))
	(SvOi::Tid_InvalidData, _T("Invalid Data"))
	(SvOi::Tid_Error_NoResultObject, _T("Cannot find the Result Object"))
	(SvOi::Tid_ImageClass_DestroyError, _T("Failed to destroy the image object!"))
	(SvOi::Tid_GetParentImageHandleError, _T("Cannot Get Indexed Child Image Handle"))
	(SvOi::Tid_DisplayedImageIsUnavailable, _T("Displayed image is unavailable"))
	(SvOi::Tid_InspThreadGetResultImageIndexError, _T( "Inspection Thread-Name=%s-Error in getting Result Image Index" ))
	(SvOi::Tid_ErrorNotAllDataItemsFound, _T("Not All Data List items found"))
	(SvOi::Tid_ErrorNotAllImageItemsFound, _T("Not All Image List items found"))
	(SvOi::Tid_ErrorProcessNotifyLastInspected, _T("ProcessNotifyWithLastInspected - %s"))
	(SvOi::Tid_Unknown, _T("Unknown"))
	(SvOi::Tid_ErrorLockingInputImageRequests, _T( "Error locking input image requests." ))
	(SvOi::Tid_ErrorLockingInputRequests, _T( "Error locking input requests." ))
	(SvOi::Tid_ErrorRemovingAllInputRequests, _T( "Error removing all input requests." ))
	(SvOi::Tid_ErrorRemovingAllInputImageRequests, _T( "Error removing all input image requests." ))
	(SvOi::Tid_ErrorUnlockingInputImageRequests, _T( "Error unlocking input image requests." ))
	(SvOi::Tid_ErrorUnlockingInputRequests, _T( "Error unlocking input requests." ))
	(SvOi::Tid_IPDoc_NoCameraColorAttached, _T("Color camera not attached to this IPDoc"))
	(SvOi::Tid_ErrorGettingTimeStamp, _T( "Error getting time stamp." ))
	(SvOi::Tid_ErrorNotEnoughDiskSpace, _T("CanGoOnline %s - Not enough Disk Space Available"))
	(SvOi::Tid_ProcessTrigger, _T("ProcessTrigger - %s"))
	(SvOi::Tid_ErrorNotAllRejectConditionItemsFound, _T("Not All Reject Condition List items found"))
	(SvOi::Tid_ReleaseProduct, _T("Release Product - %s"))
	(SvOi::Tid_ErrorCopyLastInspectedToReject, _T("Copy Last Inspected To Reject failed"))
	(SvOi::Tid_CommitSharedMemory, _T("Commit Shared Memory - %s"))
	(SvOi::Tid_SVIODoc_ClosingImpossible, _T("Invalid Operation!\nThe IO Dialog can not be closed without\nclosing the whole configuration."))
	(SvOi::Tid_SVIPDoc_ClosingImpossible, _T("Invalid Operation!\nIf You wish to remove this inspection\nGo to the Edit menu and Select Edit Configuration."))
	(SvOi::Tid_SVIPDoc_GoIntoRegTestFailed, _T("Inspection is unable to go into Regression Test Mode.  There is an unknown error with the inspection."))
	(SvOi::Tid_ErrorDetachingInput, _T( "Error detaching input." ))
	(SvOi::Tid_ColorToolMustBeFirstMessage, _T( "A Color Tool must ALWAYS be\n the first tool on a color system." ))
	(SvOi::Tid_LightReference_NotAvailable, _T("Light Reference not available for %s"))
	(SvOi::Tid_LUT_NotAvailable, _T("LUT not available for camera(s)"))
	(SvOi::Tid_LUT_ShouldOverwriteAllBands, _T("Selecting this option will overwrite all bands with the currently displayed LUT information. Continue?"))
	(SvOi::Tid_Error_WrongCameraModel, _T("Wrong camera model: Expected %s, Current %s"))
	(SvOi::Tid_Error_WrongCameraVendor, _T("Wrong camera vendor: Expected %s, Current %s"))
	(SvOi::Tid_CreateBufferFailed, _T("Failed to create an image buffer!"))
	(SvOi::Tid_CreateImageChildBuffer_parent, _T("\"%s\" was located partially or completely outside of "
	"the bounds of \"%s\".\nIt has been moved and/or resized "
	"to fit in the bounds.\nThis configuration should be "
	"saved to prevent this message from appearing again."))
	(SvOi::Tid_CreateImageChildBuffer_child, _T("\"%s\" was located partially or completely outside of "
	"the bounds of its parent image.\nIt has been moved "
	"and/or resized to fit in the bounds.\nThis configuration "
	"should be saved to prevent this message from appearing "
	"again."))
	(SvOi::Tid_CreateImageChildBuffer, _T("A child buffer was located partially or completely outside "
	"of the bounds of its parent image.\nIt has been moved "
	"and/or resized to fit in the bounds.\nThis configuration "
	"should be saved to prevent this message from appearing "
	"again."))
	(SvOi::Tid_MatroxImage_UnableToFindFile, _T( "Unable to locate image file:\n%s\n\nBrowse..." ))
	(SvOi::Tid_UnKnownFileFormat, _T("Unknown File Format!"))
	(SvOi::Tid_FailedToLoadImage, _T("Failed to load an image!"))
	(SvOi::Tid_FailedToSaveImage, _T("Unable to Save Image"))

	//Custom Filter 2
	(SvOi::Tid_DataInvalidKernelSize, _T("The Kernel array size [%s] does not match the size of the Kernel Width [%s] and Height [%s]"))
	(SvOi::Tid_DataInvalidKernelWidth, _T("The Kernel Width is not valid [%s], it must be odd and be between the limits including 1 and %s"))
	(SvOi::Tid_DataInvalidKernelHeight, _T("The Kernel Height is not valid [%s], it must be odd and be between the limits including 1 and %s"))
	(SvOi::Tid_DataInvalidNormalizationFactor, _T("The Normalization Factor is not allowed to be 0 or negative"))
	(SvOi::Tid_ExportFailed, _T("Custom filter export failed"))
	(SvOi::Tid_ImportFailed, _T("The Custom Filter file [%s] could not be imported.\n%s"))
	(SvOi::Tid_XmlFormatInvalid, _T("The XML format is invalid."))
	(SvOi::Tid_VersionMismatch, _T("Version mismatch."))
	(SvOi::Tid_ImportFailedDataInvalid, _T("Custom filter import failed, Kernel data is invalid."))

	(SvOi::Tid_True, _T("TRUE"))
	(SvOi::Tid_False, _T("TRUE"))
	(SvOi::Tid_ConditionalValidated, _T("Conditional Equation Validated Successfully\n Condition = %s"))
	(SvOi::Tid_FormulaValidated, _T("Formula Validated Successfully\n Value = %s"))
	(SvOi::Tid_Error_InvalidFormula, _T("An Invalid Formula has been detected.\nThe Formula must be valid to exit this dialog."))
	(SvOi::Tid_Error_CreationFilterFailed, _T("Creation of Filter Failed"))
	(SvOi::Tid_Error_ToolPositionError, _T("Changing image source has placed the tool outside of the selected source image.  Please either: "
		"select a new image source, resize the image source to contain this tool, or resize/position this tool to fit on the source image."))
	(SvOi::Tid_RingBuffer_Depth_Invalid_ValueString, _T( "RingBuffer Depth has to be between %s and %s, current %s" ))
	(SvOi::Tid_RingBuffer_ImageIndex_Invalid_ValueString, _T( "RingBuffer Image Index %s value invalid." ))
	(SvOi::Tid_Error_NoDeviceContext, _T("No device context available!"))
	(SvOi::Tid_Lpt_WrongState, _T("Select Line In wrong State"))
	(SvOi::Tid_Sax_ConInitializeExFailed, _T("CoInitializeEx Failed"))
	(SvOi::Tid_UnknowThread, _T( "Unknown Thread" ))

	//MatroxGigE
	(SvOi::Tid_MatroxGigE_Connect_FindCameraError, _T("HandleConnect - FindCamera Error (%s) - Device %s"))
	(SvOi::Tid_MatroxGigE_Connect_GetDigitizerError, _T("HandleConnect - GetDigitizer Error (%s) - Device %s"))
	(SvOi::Tid_MatroxGigE_Connect_AllocError, _T("HandleConnect - AllocDigitizer Error (%s) - Device %s"))
	(SvOi::Tid_MatroxGigE_Connect_ReadError, _T("HandleConnect - ReadCameraFormat Error (%s) - Device %s"))
	(SvOi::Tid_MatroxGigE_Connect_CreateError, _T("HandleConnect - CreateAcquisitionBuffers Error (%s) - Device %s"))
	(SvOi::Tid_MatroxGigE_Connect_ReloadError, _T("HandleConnect - Reload Camera Parameters Error (%s) - Device %s"))
	(SvOi::Tid_MatroxGigE_Connect_StartDigitizerError, _T("HandleConnect - StartDigitizer Error - Device %s"))
	(SvOi::Tid_MatroxGigE_Disconnect_FindCameraError, _T("HandleDisconnect - FindCamera Error (%s) - Device %s"))
	(SvOi::Tid_MatroxGigE_Disconnect_GetDigitizerError, _T("HandleDisconnect - GetDigitizer Error (%s) - Device %s"))
	(SvOi::Tid_MatroxGigE_Disconnect, _T("CameraPresentCallback - Disconnect - Device %s"))
	(SvOi::Tid_MatroxGigE_Connect, _T("CameraPresentCallback - Connect - Device %s"))
	
	(SvOi::Tid_FileCamera_NoImageFile, _T("E: FileAcquisition::No Image File to Load, possible Invalid or Empty Directory %s"))
	(SvOi::Tid_FileCamera_LoadFailed, _T("E: FileAcquisition::Unable to Load Image File %s"))
	(SvOi::Tid_FileAcquisition_FormatError, _T("E: FileAcquisition::CopyImage - Error in Format"))
	(SvOi::Tid_ObjectBuilder_SetObjectValueError, _T("SVObjectBuilder::SetObjectValue Error:\n Set Object Member Value failed!"))
	(SvOi::Tid_ErrorUsedPreviouslyForSec, _T("This was previously used for .sec files."))

	//ParseError
	(SvOi::Tid_ParseError_NameMustNotContainKeyword, _T("Parser Error:\nName expression expected, valid names must not contain keywords!"))
	(SvOi::Tid_ParseError_OutOfMemory, _T("Parser Error:\nOut of memory!"))
	(SvOi::Tid_ParseError_NameExpressionExpected, _T("Parser Error:\nName expression expected!"))
	(SvOi::Tid_ParseError_OwnerNotExist, _T("Parser Error:\nOwner doesn't exist, cannot access member!"))
	(SvOi::Tid_ParseError_UnexpectedMember, _T("Parser Error:\nUnexpected member access operator!"))
	(SvOi::Tid_ParseError_CannotReadGuid, _T("Parser Error:\nCannot read GUID!"))
	(SvOi::Tid_ParseError_ExpressionExpected, _T("Parser Error:\nExpression expected!"))
	(SvOi::Tid_ParseError_UnknownExpression, _T("Parser Error:\nUnknown expression!"))
	(SvOi::Tid_ParseError_UnexpectedEndOfCommand, _T("Parser Error:\nUnexpected end of command!"))
	(SvOi::Tid_ParseError_CriticalErrorRemovingDependencies, _T("Critical Error removing external dependencies"))
	(SvOi::Tid_ParseError_UnexpectedEndOfParse, _T("ParserState: READ_STRING\nParser Error, unexpected end of parse string while reading string..."))
	(SvOi::Tid_ParseError_UnexpectedEndOfName, _T("ParserState: READ_NAME_STRING\nParser Error, unexpected end of name string while parsing ..."))
	(SvOi::Tid_ParseError_EmptyToken, _T("ParserState: READ_GUID_STRING\nParser Error, empty token found..."))
	(SvOi::Tid_ParseError_GuidCannotEvaluated, _T("ParserState: READ_GUID_STRING\nParser Error, GUID expression cannot be evaluated..."))
	(SvOi::Tid_ParseError_NameOperandExpected, _T("Parser Error:\nName operand expected!"))
	(SvOi::Tid_ParseError_OperatorExpected, _T("Parser Error:\nOperator, expected!"))
	(SvOi::Tid_ParseError_OperandExpected, _T("Parser Error:\nObject operand, class operand or embedded operand expected!"))
	(SvOi::Tid_ParseError_CommandTerminationExpected, _T("Parser Error:\nCommand termination expected!"))
	(SvOi::Tid_ParseError_TooManyExpressions, _T("Parser Error:\nToo many expressions found!"))
	(SvOi::Tid_ParseError_ClassOperandExpected, _T("Parser Error:\nClass operand expected!"))
	(SvOi::Tid_ParseError_OperatorOperand, _T("Parser Error:\nOperator operand expected!"))
	(SvOi::Tid_ParseError_ObjectOperandExpected, _T("Parser Error:\nObject operand expected!"))
	(SvOi::Tid_ParseError_UnableToReadInputList, _T("Parser Error:\nUnable to read input list!"))
	(SvOi::Tid_ParseError_CommandTerminationOrObjectMemberExpected, _T("Parser Error:\nCommand termination or object member definition expected!"))
	(SvOi::Tid_ParseError_NoValidObjectOwner, _T("Parser Error:\nNo valid object owner given!"))
	(SvOi::Tid_ParseError_WrongEmbeddedValues, _T("Parser Error:\nWrong embedded values!"))
	(SvOi::Tid_ParseError_CannotConstructObject, _T("Parser Error:\nCannot construct new object!"))
	(SvOi::Tid_ParseError_ObjectRequired, _T("Parser Error:\nObject required for Member!"))
	(SvOi::Tid_ParseError_ValuesExpected, _T("Parser Error:\nValues expected!"))
	(SvOi::Tid_ParseError_NoValidObject, _T("Parser Error:\nNo valid object given!"))
	(SvOi::Tid_ParseError_SetObjectMemberFailed, _T("Parser Error:\nSet Object Member Value failed!"))
	(SvOi::Tid_Error_ScriptParseFailed, _T("%s parse was not successful"))

	//SVObserver
	(SvOi::Tid_SVObserver_CannotRun_WrongModelNumber, _T( "Configuration cannot enter Run.  The current configuration hardware does not match system hardware. "
	"The system's Model Number is ( Model # %s %s %s %s ). Please verify that the shown model number is correct and contact your system administrator."))
	(SvOi::Tid_SVObserver_WrongModelNumber, _T( "Hardware configuration error. The SVIM hardware does not match the Model Number ( Model # %s %s %s %s ). "
	"%s is either not available or malfunctioning. Please verify that the shown model number is correct and contact your system administrator."))
	(SvOi::Tid_SVObserver_ModelNumberInvalid, _T("The model number specified in OEMINFO.INI is invalid."))
	(SvOi::Tid_SVObserver_AuthorizationFailed_Modification, _T( "Authorization Failed.\n\nUtility Modification requires 'User Manager' privilege." ))
	(SvOi::Tid_SVObserver_AuthorizationFailed_Execution, _T("Authorization Failed.\n\nUtility Execution requires 'User Manager' privilege."))
	(SvOi::Tid_SVObserver_MatroxGigELicenseNotFound, _T("Matrox GigE License not found"))
	(SvOi::Tid_SVObserver_WrongVersionNumber, _T( "This configuration was created by SVObserver %s.\nYou are currently running SVObserver %s.\n"
		"This configuration version may be incompatible with\nthe version of SVObserver that you are running.\nAre you sure you wish to continue ?" ))
	(SvOi::Tid_SVObserver_ConfigurationLoadFailed, _T( "The configuration could not successfully load.\nhr = %s.\n"))
	(SvOi::Tid_SVObserver_SecurityFileLoadFailed, _T("Security File Failed to Load\n Using Default Security"))
	(SvOi::Tid_SVObserver_WrongPathnameEntered, _T("Invalid path or file name found\n\nCheck your inputs and try it again!"))
	(SvOi::Tid_SVObserver_RegisterClassObjectsFailed, _T( "(hr=0x%s), Unable to register class objects. COM interface ISVCommand unavailable."))
	(SvOi::Tid_SVObserver_RegisterClassObjectsFailed_Question, _T( "(hr=0x%s), Unable to register class objects. COM interface ISVCommand unavailable.\nDo you want to exit?"))
	(SvOi::Tid_SVObserver_OleInitFailed, _T( "OLE initialization failed.  Make sure that the OLE libraries are the correct version."))
	(SvOi::Tid_CanGoOnlineFailure_Trigger, _T( "Configuration cannot enter Run.  There is an "
			"unknown error with a Trigger when the system attempted to enter Run." ))
	(SvOi::Tid_CanGoOnlineFailure_Acquisition, _T( "Configuration cannot enter Run.  Cannot connect to camera when the system attempted to enter Run." ))
	(SvOi::Tid_CanGoOnlineFailure_InspectionTool, _T("Configuration cannot enter Run.\n Error with \"%s\" : %s"))
	(SvOi::Tid_CanGoOnlineFailure_Inspection, _T( "Configuration cannot enter Run.  There is an "
							"unknown error with an inspection when the system attempted to enter Run." ))
	(SvOi::Tid_GoOnlineFailure_RecycleProduct, _T( "Configuration cannot enter Run.  There is an "
						"unknown error with a PPQ when it attempted to recycle a product when the system was going online." ))
	(SvOi::Tid_GoOnlineFailure_Inspection, _T( "Configuration cannot enter Run.  There is an "
						"unknown error with an Inspection when the system was going online." ))
	(SvOi::Tid_GoOnlineFailure_Acquisition, _T("Configuration cannot enter Run.  Cannot reach camera when the system was going online." ))
	(SvOi::Tid_GoOnlineFailure_Trigger, _T( "Configuration cannot enter Run.  There is an "
						"unknown error with a Trigger when the system was going online." ))
	(SvOi::Tid_CanGoOnlineFailure_Unknown, _T( "Configuration cannot enter Run.  There is an "
						"unknown error when the system was going online." ))
	(SvOi::Tid_AmountOfSystemMemoryText, _T("Amount of physical memory = %s"))
	(SvOi::Tid_Version, _T("\nVersion %s"))
	(SvOi::Tid_ConfigLoadTime, _T("%s\nload time %s ms"))
	(SvOi::Tid_UserQuestionCloseConfig, _T("Are you sure you want to close the current SVObserver application  %s?\n\nALL CURRENT RUNNING INSPECTIONS WILL BE STOPPED!"))
	(SvOi::Tid_UserQuestionSaveChanges, _T("Save changes to %s?"))
	(SvOi::Tid_UnableConfig, _T("Unable to %s configuration %s !"))
	(SvOi::Tid_Load, _T("load"))
	(SvOi::Tid_Save, _T("save"))
	(SvOi::Tid_GoOnlineTime, _T("%s\nGoOnline time %s ms"))

	(SvOi::Tid_Camera_UseQuestion, _T("Would you like to use this camera file for all camera choices?"))
	(SvOi::Tid_Config_SwitchResetQuestion, _T("By switching the system (product) the camera, trigger, inspection and ppq data will be reset, do you wish to continue?"))
	(SvOi::Tid_Config_SwitchInvalidQuestion, _T("By switching the system (product) the camera files will become invalid, do you wish to continue?"))
	(SvOi::Tid_Config_InspectionImportFailed, _T("Inspection Import failed. Error = %s"))
	(SvOi::Tid_Error_NoMilHostBuffer, _T("No Mil Host Buffer Pointer"))
	(SvOi::Tid_Error_MilImageTypeInvalid, _T("MIL Image Type Not 8 Bit Unsigned: %x"))
	(SvOi::Tid_Error_FontNotMfoExt, _T( "Font File Requires '.mfo' Extension: %s" ))
	(SvOi::Tid_Error_NoFontConstraintsFileSpec, _T("No Font Constraints File Specified"))
	(SvOi::Tid_Error_FontConstraintsRequiredMfoExt, _T( "Font Constraints File Requires '.mfo' Extension: %s" ))
	(SvOi::Tid_Error_FontConstraintsFileNotExist, _T( "Font Constraints File Does Not Exist: %s" ))
	(SvOi::Tid_Error_FontConstraintsFileEmpty, _T( "Font Constraints File Empty: %s" ))
	(SvOi::Tid_Error_NoFontControlsFileSpec, _T("No Font Controls File Specified"))
	(SvOi::Tid_Error_FontControlsRequiredMfoExt, _T( "Font Controls File Requires '.mfo' Extension: %s" ))
	(SvOi::Tid_Error_FontControlsFileNotExist, _T( "Font Controls File Does Not Exist: %s" ))
	(SvOi::Tid_Error_FontControlsFileEmpty, _T( "Font Controls File Empty: %s" ))
	(SvOi::Tid_ImportInspectionError, _T("Inspection was exported using SVObserver %s. \tThis is incompatible with SVObserver %s."))
	(SvOi::Tid_Error_InvalidThresholdValue, _T("Invalid Threshold Value"))
	(SvOi::Tid_Pattern_Model2Large, _T("Model is larger than the ROI of the Window Tool. Do you want to change Model (Yes) or leave dialog (No)?"))
	(SvOi::Tid_ErrorGettingInputs, _T( "Error getting inputs." ))
	(SvOi::Tid_ErrorFindingPPQEntries, _T( "Error finding PPQ entries." ))
	(SvOi::Tid_InvalidReference, _T("%s: Invalid reference"))

	(SvOi::Tid_RegressionTest_NoBmpFileSelected, _T("Selection Error:  A .bmp file must be selected."))
	(SvOi::Tid_RegressionTest_WrongListSize, _T("Selection Error:  Lists must be the same size.  Please select again."))
	(SvOi::Tid_RegressionTest_NoFileSelected, _T("Selection Error: \"Use single file\" is selected for one of the cameras but a file was not selected. Select again?"))
	(SvOi::Tid_RegressionTest_WrongFormat, _T("Selection Error: \"Use list of files\" selected, but the file name does not match\n the acceptable format (<file name>_<sequence number>.bmp). Select again?"))
	(SvOi::Tid_RegressionTest_NoFilesSelected, _T("Selection Error:  \"No files\" selected.  Select again?"))
	(SvOi::Tid_RegressionTest_FileNotExist, _T("Selection Error:  File does not exist.  Select again?"))
	(SvOi::Tid_RemoteOutput_TriggerCountDeleteError, _T("Trigger Count Cannot Be deleted"))
	(SvOi::Tid_RemoteOutput_DeletingOutput, _T("Are You Sure you want to delete this Output?"))
	(SvOi::Tid_RemoteOutput_DeletingAllOutput, _T("Are you sure you want to delete All Entries for %s"))
	(SvOi::Tid_ResultView_InitFailed, _T("Failed to init inspection result view!"))
	(SvOi::Tid_StatTool_ResultFailed, _T("Creation of Statistics Result Failed"))

	(SvOi::Tid_InvalidImagePath, _T("Invalid Keywords in Archive Image Path."))
	(SvOi::Tid_InvalidDrive, _T("Invalid drive: %s"))
	(SvOi::Tid_InvalidFileName, _T("Invalid Keywords in Archive Filename/Path."))
	(SvOi::Tid_AP_InvalidFile, _T("ERROR: Archive File is not unique in system:\nChange archive file name:\n%s"))
	(SvOi::Tid_Error_you_have_Selected_X_Must_less_then_Y, _T("Error: You have selected %s for the Max Images count. The image count must be less then %s"))
	(SvOi::Tid_ArchiveTool_WarningMaxImages, _T("WARNING: You have selected %s for the Max Images count"))
	(SvOi::Tid_AP_NotEnoughImageMemoryInChangeMode, _T("There is not enough Available Archive Image Memory for %s images in Change Mode. Available\nArchive Image Memory is the result of the selected images and the Max Images number.\nThe selection will be reset."))
	(SvOi::Tid_AP_NotEnoughImageMemoryToSelect, _T("Not enough Archive Image Memory to select %s"))
	(SvOi::Tid_AP_NotEnoughMemoryPleaseDeselectImage, _T("Not enough Archive Image Memory for the images selected. Please deselect some images."))
	(SvOi::Tid_AP_NotEnoughMemoryInChangeMode, _T("There is not enough Available Archive Image Memory for your selection in Change Mode.\nAvailable Archive Image Memory is the result of the selected images and the Max Images number."))
	(SvOi::Tid_AP_NotEnoughMemoryPleaseDeselect, _T("Not enough Available Archive Image Memory. Please deselect some\nimages, decrease \"Max Images\" or change \"When to archive\" mode."))
	(SvOi::Tid_Threshold_OutOfRange, _T("Out of Range!\n(0 ... 255)"))
	(SvOi::Tid_CriticalUnableToConnectTo, _T("Unable to connect to %s"))
	(SvOi::Tid_UnknownString, _T("<Unknown>"))
	(SvOi::Tid_Error_DuplicateEmbeddedId, _T("Error - Duplicate Embedded ID found"))
	(SvOi::Tid_Error_CannotAddInputRequest, _T("Cannot Add Input Request.\nPlease contact technical support specialist."))
	(SvOi::Tid_Error_CannotAddInputRequestMarker, _T("Cannot Add Input Request Marker.\nPlease contact technical support specialist."))
	(SvOi::Tid_Error_CannotRunOnce, _T("Cannot Run Once.\nPlease contact technical support specialist."))
	(SvOi::Tid_ErrorIn, _T("Error in %s"))
	(SvOi::Tid_Error_NoAnalyzerDetails, _T("No Analyzer Details available!\nTry to select an Analyzer first.\nNOTICE: Some Tools don�t have an Analyzer!"))
	(SvOi::Tid_Error_AnalyzerCreationFailed, _T("Creation of Analyzer Failed"))
	(SvOi::Tid_Error_AnalyzerInstantiationFailed, _T("Instantiation of Analyzer Failed"))
	(SvOi::Tid_LinearSpecial_OrientationVertical, _T("Tool profile orientation is at vertical.  By turning rotation on the profile orientation will be set to horizontal. Do you wish to continue"))
	(SvOi::Tid_LinearSpecial_IsRotated, _T("Tool is currently rotated.  By turning off rotation the figure will be drawn at a rotation angle of 0. Do you wish to continue"))
	(SvOi::Tid_MaskPage_StartOCXFailed, _T("Static Mask Editor OCX Failed to Start"))
	(SvOi::Tid_Error_CannotOpenFile, _T("Cannot open file!"))
	(SvOi::Tid_Error_CannotOpenTADlg, _T("Unable to open Tool Adjustment Dialog!"))
	(SvOi::Tid_UnableStart_Utility, _T("Unable to start %s\n(%s).\n\nCheck Utility Properties."))
	(SvOi::Tid_SetClipboardDataFailed, _T( "Setting the data into the clipboard failed." ))
	(SvOi::Tid_ClipboardDataConverionFailed, _T( "The clipboard data conversion failed." ))
	(SvOi::Tid_ClipboardMemoryFailed, _T( "Clipboard memory allocation failed." ))
	(SvOi::Tid_GetClipboardDataFailed, _T( "Failed to retrieve the clipboard data." ))
	(SvOi::Tid_ToolInvalid, _T( "The tool to be copied is invalid." ))
	(SvOi::Tid_Clipboard_VersionMismatch, _T( "The current SVObserver Version does not match the version coming from the clipboard." ))
	(SvOi::Tid_ColorToolInsertFailed, _T( "A color tool cannot be inserted into a non-color system." ))
	(SvOi::Tid_NonColorToolInsertFailed, _T( "A color tool must always be the first tool in a color system." ))
	(SvOi::Tid_RenameError_DuplicateName, _T("A duplicate name was found for the item being renamed\n%s"))
	(SvOi::Tid_ErrorInReset, _T("Error in Reset"))
	(SvOi::Tid_InValidNewExtents, _T("The new extents for the %s are not valid"))
	(SvOi::Tid_MilVersion_Error, _T("This version of SVObserver will not run with the installed MIL library. SVObserver needs MIL %4.2f. The installed version is %4.2f"))
	(SvOi::Tid_StatToolInvalidVariable, _T("%s\nError - The Selected Variable To Monitor does not exist."))
	(SvOi::Tid_CannotGoOnline, _T("%s cannot go online"))
	(SvOi::Tid_FailedToGoOnline, _T("%s failed to go online"))
	(SvOi::Tid_PatModelSizeErr, _T("Invalid Model Size"))
	(SvOi::Tid_PatInvalidFilename, _T("Not a Valid Model file"))
	(SvOi::Tid_PatAllocModelFailed, _T("Pattern Model Allocation Failed."))
	(SvOi::Tid_PatAdditionalCandidatesErr, _T("Invalid Additional Candidates Value (range is 1 to 100)"))
	(SvOi::Tid_PatCandidateSpaceErr, _T("Invalid Candidate Spacing Min Value (range is 1.0 to 100.0)"))
	(SvOi::Tid_PatPreliminaryAcceptanceThresholdErr, _T("Invalid Preliminary Acceptance Threshold Value (range is 1.0 to 100.0)"))
	(SvOi::Tid_Security_GainedAccess, _T("Gained Access"))
	(SvOi::Tid_Security_Login, _T("Login"))
	(SvOi::Tid_Security_Access, _T("Access - %s"))
	(SvOi::Tid_Security_UserNoRights, _T("User Does Not Have Rights to This Function"))
	;
} /* namespace SVStatusLibrary */ } /* namespace Seidenader */

namespace SvStl = Seidenader::SVStatusLibrary;