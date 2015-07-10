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
} /* namespace SVOGui */ } /* namespace Seidenader */

namespace SvOg = Seidenader::SVOGui;
