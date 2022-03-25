// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
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


namespace SvLib
{
	class SVOINIClass
	{
	public:
		explicit SVOINIClass(LPCTSTR FileName);
		virtual ~SVOINIClass();

		std::string GetValueString( LPCTSTR Section, LPCTSTR Key, LPCTSTR Default ) const;

		int GetValueInt( LPCTSTR Section, LPCTSTR Key, int Default ) const;

		double GetValueDouble(LPCTSTR Section, LPCTSTR Key, double Default) const;

		HRESULT SetValueString( LPCTSTR Section, LPCTSTR Key, LPCTSTR Value ) const;

		HRESULT SetValueInt( LPCTSTR Section, LPCTSTR Key, int Value ) const;
	protected:
		std::string m_FileName;
	};
} //namespace SvLib

#include "SVOINIClass.inl"
