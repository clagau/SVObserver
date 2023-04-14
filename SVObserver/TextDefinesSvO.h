//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************

// This is the central location for strings used in the SVObserver project.
//******************************************************************************

#pragma once

namespace SvO
{
	const TCHAR* const ToolClipboardFormat = _T( "SVO-Tool" );
	const TCHAR* const ClipboardFileName = _T( "ClipboardFile" );
	const TCHAR* const ZipExtension = _T( ".7z" );
	const TCHAR* const XmlExtension = _T( ".xml" );
	const TCHAR* const ModuleExtension = _T(".svm");
	const TCHAR* const InsertingTool = _T( "Inserting Tool ..." );
	const TCHAR* const LoadingResourceDllFailed = _T( "Could not load resource dll.\nThe program will terminated." );

	const TCHAR* const EndListDelimiter	  = _T("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------" );
	const TCHAR* const LoopToolDelimiter = EndListDelimiter;
	const TCHAR* const SVTOOLPARAMETERLIST_MARKER = _T( "** SVSetToolParameterList MARKER **" );

	const TCHAR* const GlobalViewHeader[] = {_T("Global Constants"), _T("Type"), _T("Value"), _T("Description")};
	const TCHAR* const Invalid = _T("Invalid");
	const TCHAR* const Average = _T("Average"); 
	///Defines for SVConfigurationObjects
	const TCHAR* const InputObjectList = _T( "Input Object List");
	const TCHAR* const OutputObjectList =  _T( "Output Object List" );

	const TCHAR* const NoneString= 	_T("<none>");
	const TCHAR* const AvailableArchiveImageMemory = _T("Remaining Archive Image Memory:  {:8.1f} MB");
	const TCHAR* const AvailableArchiveImageMemoryExceeded = _T("Available Archive Image Memory EXCEEDED by {:6.1f} MB!!!");

	const TCHAR* const cCameraContrast = _T( "Contrast" );		//Legacy name for Gain was Contrast and needs to be checked on loading

	const TCHAR* const cTrue = _T( "True" );
	const TCHAR* const cFalse = _T( "False" );

	const TCHAR* const cRemoteInputNumberLabel = _T("Remote Input ");

} //namespace SvO
