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

	const TCHAR* const EndListDelimiter = _T( "-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------" );
	const TCHAR* const SVTOOLPARAMETERLIST_MARKER = _T( "** SVSetToolParameterList MARKER **" );
	const TCHAR* const SVEXCLUDECHARS_CONFIG_NAME =  _T( "\\/:*\"<>|;&$@'[]{}()^~!%?=§€`´°²³µ#" );
	const TCHAR* const SVEXCLUDECHARS_TOOL_IP_NAME = _T( "\\/:*\"<>|;&$@'[]{}()^~!%?=§€`´°²³µ#,.+-\xd7\xf7" );
	const TCHAR* const SizeAdjustTextNone = _T( "Manual" );
	const TCHAR* const SizeAdjustTextFormula = _T( "Formula" );
	const TCHAR* const SizeAdjustTextFullSize = _T( "Auto Fit" );
	const TCHAR* const RingBuffer_ImageIndex1Connector = _T( "RingBufferImageIndex1Connector" );
	const TCHAR* const RingBuffer_ImageIndex2Connector = _T( "RingBufferImageIndex2Connector" );

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
	///Defines for SVConfigurationObjects
	const TCHAR* const InputObjectList = _T( "Input Object List");
	const TCHAR* const OutputObjectList =  _T( "Output Object List" );

	const TCHAR* const SVObserverDirectoryPath = _T("C:\\SVOBserver\\");///< the path of the SvObserver directory.
	const TCHAR* const SecondSVObserverDirectoryPath = 
		_T("D:\\SVOBserver\\");///< the path of SVObserver directory (not the main one but the one on drive D:)
	const TCHAR* const SVObserverAutosavePath = _T("D:\\SVObserver\\Autosave\\");///< the SVObserver autosave directory
	const TCHAR* const NoneString= 	_T("<none>");
	const TCHAR* const FbwfDllName= _T("fbwflib.dll");///< the name of the DLL providing the file based write filter functionality
	const TCHAR* const ContextMenuImageSaveLocation = _T("D:\\SVObserver\\Image.bmp"); ///< this is the default location for images that are saved using a context menu
	const TCHAR* const AvailableArchiveImageMemory  = _T( "Available Archive Image Memory:  %8.1f MB" );
	const TCHAR* const ARCHIVE_TOOL_MEMORY_POOL_ONLINE_ASYNC_NAME = _T("ArchiveToolOnlineAsync");
	const TCHAR* const ARCHIVE_TOOL_MEMORY_POOL_GO_OFFLINE_NAME = _T("ArchiveToolGoOffline");
} /* namespace SVObserver */ } /* namespace Seidenader */

namespace SvO = Seidenader::SVObserver;