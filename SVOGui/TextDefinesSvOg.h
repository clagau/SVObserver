//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the central location for strings used in the SVOGui project.
//******************************************************************************

#pragma once

namespace Seidenader { namespace SVOGui
{
	const TCHAR* const RingBuffer_Depth_Invalid_ValueString = _T( "RingBuffer Depth has to be between %d and %d, current %s" );
	const TCHAR* const RingBuffer_ImageIndex_Invalid_ValueString = _T( "RingBuffer Image Index %d value invalid." );
	const TCHAR* const GlobalConstantTypes[] = {_T( "Decimal" ), _T( "Text" )};
	const TCHAR* const GlobalConflictHeader[] = {_T( "" ), _T( "Current Name" ), _T( "Current Type" ), _T( "Current Value" ), _T(""),  _T( "Imported name" ), _T( "Imported Type" ), _T( "Imported Value" ) };
	const TCHAR* const RegExp_AllRealNumbers = _T( "[-+]?([0-9]{1,16})([.][0-9]{1,6})?" );
	const TCHAR* const RegExp_Name = _T( "[a-zA-Z][0-9a-zA-Z_ ]*" );
	const TCHAR* const RangeValue_EmptyString = _T( "Empty String in %s." );
	const TCHAR* const RangeValue_WrongRange = _T( "%s must between %d and %d." );
	const TCHAR* const Filter_NoFilterAvailable = _T( "(No Filter Available)" );
	const TCHAR* const Filter_NoFilter = _T( "(No Filter)" );
	const TCHAR* const ScintillaControlName = _T("Scintilla");
	const TCHAR* const   Error_InvalidFormula = _T("An Invalid Formula has been detected.\nThe Formula must be valid to exit this dialog.");
	const TCHAR* const   Error_CreationFilterFailed = _T("Creation of Filter Failed");
	const TCHAR* const   Error_ToolPositionError = _T("Changing image source has placed the tool outside of the selected source image.  Please either: "
		"select a new image source, resize the image source to contain this tool, or resize/position this tool to fit on the source image.");
	const TCHAR* const   UnKnownFileFormat = _T("Unknown File Format!");
	const TCHAR* const   FailedToSaveImage = _T("Unable to Save Image");
} /* namespace SVOGui */ } /* namespace Seidenader */

namespace SvOg = Seidenader::SVOGui;
