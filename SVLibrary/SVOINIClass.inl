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

namespace SvLib
{

	inline SVOINIClass::SVOINIClass( LPCTSTR FileName ) :
	m_FileName( FileName )
	{
	}

	inline SVOINIClass::~SVOINIClass()
	{
	}

	inline std::string SVOINIClass::GetValueString( LPCTSTR Section, LPCTSTR Key, LPCTSTR Default ) const
	{
		std::string Result;

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

	inline double SVOINIClass::GetValueDouble(LPCTSTR Section, LPCTSTR Key, double defaultValue) const
	{
		std::string resultString = GetValueString(Section, Key, std::to_string(defaultValue).c_str());
		double result = defaultValue;
		if (!resultString.empty())
		{
			result = atof(resultString.c_str());
		}
		return result;
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
		_itot_s( Value, ValueText, MAX_PATH, 10 );

		Result = SetValueString( Section, Key, ValueText );

		return Result;
	}
} //namespace SvLib
