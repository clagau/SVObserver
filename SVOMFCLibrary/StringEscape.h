//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
#pragma once

namespace Seidenader { namespace SVOMFCLibrary
{
	////////////////////////////////////////////////////////////////////////////////
	// Places escape character before double quotes in a String
	////////////////////////////////////////////////////////////////////////////////
	bool AddEscapeSpecialCharacters( CString& rString, bool bConvertCtrl );

	////////////////////////////////////////////////////////////////////////////////
	// Removes any escape characters before double quotes in a String
	////////////////////////////////////////////////////////////////////////////////
	bool RemoveEscapedSpecialCharacters( CString& rString, bool bConvertCtrl );
} /*SVOMFCLibrary*/ } /*Seidenader*/

namespace SvOml = Seidenader::SVOMFCLibrary;