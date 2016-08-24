// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOINIClass
// * .File Name       : $Workfile:   SVOINIClass.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:08:38  $
// ******************************************************************************

namespace Seidenader { namespace SVOMFCLibrary
{

	inline SVOINIClass::SVOINIClass( LPCTSTR FileName ) :
	m_FileName( FileName )
	{
	}

	inline SVOINIClass::~SVOINIClass()
	{
	}

	inline SVString SVOINIClass::GetValueString( LPCTSTR Section, LPCTSTR Key, LPCTSTR Default ) const
	{
		SVString Result;

		TCHAR Value[ SHRT_MAX ];

		::GetPrivateProfileString( Section, Key, Default, Value, sizeof Value, m_FileName.c_str() );

		Result = Value;

		return Result;
	}

	inline int SVOINIClass::GetValueInt( LPCTSTR Section, LPCTSTR Key, int Default ) const
	{
		int Result;

		INT Value = ::GetPrivateProfileInt( Section, Key, Default, m_FileName.c_str() );

		Result = static_cast<int> (Value);
		return Result;
	}

	inline HRESULT SVOINIClass::SetValueString( LPCTSTR Section, LPCTSTR Key, LPCTSTR Value) const
	{
		HRESULT Result = S_OK;

		if ( !::WritePrivateProfileString( Section, Key, Value, m_FileName.c_str() ) )
		{
			Result = E_FAIL;
		}

		return Result;
	}

	inline HRESULT SVOINIClass::SetValueInt( LPCTSTR Section, LPCTSTR Key, int Value ) const
	{
		HRESULT Result = S_FALSE;

		TCHAR ValueText[MAX_PATH];
		_itot( Value, ValueText, 10 );

		Result = SetValueString( Section, Key, ValueText );

		return Result;
	}
} /* namespace SVOMFCLibrary */ } /* namespace Seidenader */

