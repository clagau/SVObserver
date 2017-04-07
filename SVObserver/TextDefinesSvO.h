//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the central location for strings used in the SVObserver project.
//******************************************************************************

#pragma once

namespace Seidenader { namespace SVObserver
{
	const TCHAR* const ToolClipboardFormat = _T( "SVO-Tool" );
	const TCHAR* const ClipboardFileName = _T( "ClipboardFile" );
	const TCHAR* const ZipExtension = _T( ".zip" );
	const TCHAR* const XmlExtension = _T( ".xml" );
	const TCHAR* const XmlNameSpace = _T( "xmlns" );
	const TCHAR* const SvrNameSpace = _T( "x-schema:#SVR00001" );
	const TCHAR* const SV_BaseNode = _T( "SV_BASENODE" );
	const TCHAR* const InsertingTool = _T( "Inserting Tool ..." );
	const TCHAR* const LoadingResourceDllFailed = _T( "Could not load resource dll.\nThe program will terminated." );

	const TCHAR* const EndListDelimiter = _T( "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------" );
	const TCHAR* const SVTOOLPARAMETERLIST_MARKER = _T( "** SVSetToolParameterList MARKER **" );
	const TCHAR* const SVEXCLUDECHARS_CONFIG_NAME =  _T( "\\/:*\"<>|;&$@'[]{}()^~!%?=§€`´°²³µ#" );
	const TCHAR* const SVEXCLUDECHARS_TOOL_IP_NAME = _T( "\\/:*\"<>|;&$@'[]{}()^~!%?=§€`´°²³µ#,.+-\xd7\xf7" );
	const TCHAR* const SizeAdjustTextNone = _T( "Manual" );
	const TCHAR* const SizeAdjustTextFormula = _T( "Formula" );
	const TCHAR* const SizeAdjustTextFullSize = _T( "Auto Fit" );

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
	const TCHAR* const Remote_Input= _T("Remote Input");
	const TCHAR* const DIOInput= _T("DIO.Input");
	const TCHAR* const cModuleReady = _T("Module Ready");
	const TCHAR* const cRaidErrorIndicator = _T("Raid Error Indicator");
	///Defines for SVConfigurationObjects
	const TCHAR* const InputObjectList = _T( "Input Object List");
	const TCHAR* const OutputObjectList =  _T( "Output Object List" );

	const TCHAR* const NoneString= 	_T("<none>");
	const TCHAR* const FbwfDllName= _T("fbwflib.dll");///< the name of the DLL providing the file based write filter functionality
	const TCHAR* const AvailableArchiveImageMemory  = _T( "Available Archive Image Memory:  %8.1f MB" );
	const TCHAR* const ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME = _T("ArchiveToolOnlineAsync");
	const TCHAR* const ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME = _T("ArchiveToolGoOffline");

	const TCHAR* const cInspectionProcessTag = _T( "SVInspectionProcess" );
	const TCHAR* const cPPQObjectTag = _T( "SVPPQObject" );
	const TCHAR* const cRemoteOutputGroupTag = _T( "SVRemoteOutputGroup" );

	const TCHAR* const cCameraContrast = _T( "Contrast" );		//Legacy name for Gain was Contrast and needs to be checked on loading
	const TCHAR* const cPpqFixedName = _T( "PPQ_" );
	const TCHAR* const cTriggerFixedName = _T( "Trigger_" );
	const TCHAR* const cCameraFixedName = _T( "Camera_" );
	const TCHAR* const cInspectionFixedName = _T( "Inspection_" );

	const TCHAR* const cLinkName = _T(" Link");

	const TCHAR* const cInputTag_SortColumn = _T( "SortColumn" );
	const TCHAR* const cInputTag_ExcludeColumn = _T( "ExcludeColumn" );
	const TCHAR* const cInputTag_LongResultValue = _T( "LongResultValue" );

	const TCHAR* const cTrue = _T( "True" );
	const TCHAR* const cFalse = _T( "False" );

} /* namespace SVObserver */ } /* namespace Seidenader */

namespace SvO = Seidenader::SVObserver;