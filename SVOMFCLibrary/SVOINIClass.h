// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOINIClass
// * .File Name       : $Workfile:   SVOINIClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:08:22  $
// ******************************************************************************

#pragma once

//Moved to precompiled header: #include <io.h>
#include "SVUtilityLibrary/SVString.h"

namespace Seidenader { namespace SVOMFCLibrary
{
	class SVOINIClass
	{
	public:
		SVOINIClass( LPCTSTR FileName );
		virtual ~SVOINIClass();

		SVString GetValueString( LPCTSTR Section, LPCTSTR Key, LPCTSTR Default ) const;

		int GetValueInt( LPCTSTR Section, LPCTSTR Key, int Default ) const;

		HRESULT SetValueString( LPCTSTR Section, LPCTSTR Key, LPCTSTR Value ) const;

		HRESULT SetValueInt( LPCTSTR Section, LPCTSTR Key, int Value ) const;
	protected:
		SVString m_FileName;
	};
} /* namespace SVOMFCLibrary */ } /* namespace Seidenader */

#include "SVOINIClass.inl"

namespace SvOml = Seidenader::SVOMFCLibrary;
