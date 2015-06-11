//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the central location for strings used in the SVObserver project.
//******************************************************************************

#pragma once

namespace Seidenader
{
	namespace SVObserver
	{
		const TCHAR* const ErrorDetachingInput = _T( "Error detaching input." );
		const TCHAR* const ErrorFillingInputs = _T( "Error filling inputs." );
		const TCHAR* const ErrorFindingPPQEntries = _T( "Error finding PPQ entries." );
		const TCHAR* const ErrorGettingInputObjectList = _T( "Error getting input object list." );
		const TCHAR* const ErrorGettingInputs = _T( "Error getting inputs." );
		const TCHAR* const ErrorGettingOutputs = _T( "Error getting outputs." );
		const TCHAR* const ErrorGettingPLCData = _T( "Error getting PLC data." );
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
		const TCHAR* const SVEXCLUDECHARS_CONFIG_NAME = _T( "\\/:*\"<>|;&$@`[]{}()^~!#%" );
		const TCHAR* const SVEXCLUDECHARS_TOOL_NAME = _T( "\\/:*\";,.+-$@'[]{}()<>|!%&~^\327\367" );
		const TCHAR* const SizeAdjustTextNone = _T( "Manual" );
		const TCHAR* const AmountOfSystemMemoryText = _T("Amount of physical memory = %d");
		const TCHAR* const SizeAdjustTextFormula = _T( "Formula" );
		const TCHAR* const SizeAdjustTextFullSize = _T( "Auto Fit" );
		const TCHAR* const RingBuffer_Depth_Invalid_Value = _T( "RingBuffer Depth has to be between %d and %d, current %d" );
		const TCHAR* const RingBuffer_ImageIndex1Connector = _T( "RingBufferImageIndex1Connector" );
		const TCHAR* const RingBuffer_ImageIndex2Connector = _T( "RingBufferImageIndex2Connector" );
	}
}
namespace SvO = Seidenader::SVObserver;