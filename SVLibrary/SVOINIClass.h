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

namespace SvLib
{
	class SVOINIClass
	{
	public:
		explicit SVOINIClass(LPCTSTR FileName) : m_FileName(FileName) {};
		virtual ~SVOINIClass() = default;

		std::string GetValueString(LPCTSTR Section, LPCTSTR Key, LPCTSTR defaultValue) const
		{
			TCHAR Value[SHRT_MAX];
			::GetPrivateProfileString(Section, Key, defaultValue, Value, sizeof Value, m_FileName.c_str());
			return std::string {Value};
		}

		int GetValueInt(LPCTSTR Section, LPCTSTR Key, int defaultValue) const
		{
			return ::GetPrivateProfileInt(Section, Key, defaultValue, m_FileName.c_str());
		}

		double GetValueDouble(LPCTSTR Section, LPCTSTR Key, double defaultValue) const
		{
			std::string resultString = GetValueString(Section, Key, std::to_string(defaultValue).c_str());
			double result = defaultValue;
			if (false == resultString.empty())
			{
				result = ::atof(resultString.c_str());
			}
			return result;
		}

		HRESULT SetValueString(LPCTSTR Section, LPCTSTR Key, LPCTSTR Value) const
		{
			return ::WritePrivateProfileString(Section, Key, Value, m_FileName.c_str()) ? S_OK : E_FAIL;
		}

		HRESULT SetValueInt(LPCTSTR Section, LPCTSTR Key, int Value) const
		{
			return SetValueString(Section, Key, std::to_string(Value).c_str());
		}

	protected:
		std::string m_FileName;
	};
} //namespace SvLib
