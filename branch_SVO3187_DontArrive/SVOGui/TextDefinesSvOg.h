//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************

// This is the central location for strings used in the SVOGui project.
//******************************************************************************

#pragma once

namespace SvOg
{
	const TCHAR* const GlobalConstantTypes[] = {_T( "Decimal" ), _T( "Text" )};
	const TCHAR* const GlobalConflictHeader[] = {_T( "" ), _T( "Current Name" ), _T( "Current Type" ), _T( "Current Value" ), _T(""),  _T( "Imported name" ), _T( "Imported Type" ), _T( "Imported Value" ) };
	const TCHAR* const RegExp_AllRealNumbers = _T( "[-+]?([0-9]{1,16})([.][0-9]{1,6})?" );
	const TCHAR* const RegExp_Name = _T( "[a-zA-Z][0-9a-zA-Z_ ]*" );
	const TCHAR* const Filter_NoFilterAvailable = _T( "(No Filter Available)" );
	const TCHAR* const Filter_NoFilter = _T( "(No Filter)" );
	const TCHAR* const Table_NoAvailable = _T("(No Table Available)");
	const TCHAR* const Table_NoSelected = _T("(No Table Selected)");
	const TCHAR* const Analyzer_NoAnalyzerAvailable = _T( "(No Analyzer Available)" );
	const TCHAR* const Analyzer_NoAnalyzer = _T( "(No Analyzer)" );
	const TCHAR* const ScintillaControlName = _T("Scintilla");
} //namespace SvOg
