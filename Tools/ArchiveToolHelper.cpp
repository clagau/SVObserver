//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by K�rber Pharma Inspection GmbH. All Rights Reserved /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma region Includes
#include "stdAfx.h"
//Moved to precompiled header: #include <io.h>
#include "ArchiveToolHelper.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
ArchiveToolHelper::ArchiveToolHelper(void)
{
	m_IsUsingKeyWords = false;
	m_ValidTokens = false;
}

ArchiveToolHelper::~ArchiveToolHelper()
{

}
#pragma endregion Constructor

#pragma region Public Methods
void ArchiveToolHelper::Init(const std::string& sPath)
{
	m_IsUsingKeyWords = false;
	m_ValidTokens = false;

	//Parse the string
	ParseTokens(sPath);
}

bool ArchiveToolHelper::isUsingKeywords()
{
	return m_IsUsingKeyWords;
}

bool ArchiveToolHelper::areTokensValid()
{
	return m_ValidTokens;
}

std::string ArchiveToolHelper::TranslatePath(const std::string& sPath)
{
	std::string sReturnPath = sPath;
	SYSTEMTIME stime;
	memset( &stime, 0, sizeof( SYSTEMTIME));
	::GetLocalTime( &stime );
	std::string sDay = std::format("{:02d}", stime.wDay);
	std::string sMonth = std::format("{:02d}", stime.wMonth);
	std::string sYear = std::format("{:04d}", stime.wYear);
	std::string sHour = std::format("{:02d}", stime.wHour);
	std::string sMin = std::format("{:02d}", stime.wMinute);
	std::string sSec = std::format("{:02d}", stime.wSecond);
	
	//replace all Keywords
	sReturnPath = SvUl::searchAndReplace(sReturnPath, KW_DAY.c_str(),sDay.c_str());
	sReturnPath = SvUl::searchAndReplace(sReturnPath, KW_MONTH.c_str(), sMonth.c_str());
	sReturnPath = SvUl::searchAndReplace(sReturnPath, KW_YEAR.c_str(), sYear.c_str());
	sReturnPath = SvUl::searchAndReplace(sReturnPath, KW_HOUR.c_str(), sHour.c_str());
	sReturnPath = SvUl::searchAndReplace(sReturnPath, KW_MINUTE.c_str(), sMin.c_str());
	sReturnPath = SvUl::searchAndReplace(sReturnPath, KW_SECONDS.c_str(), sSec.c_str());

	return sReturnPath;
}

#pragma endregion Public Methods

#pragma region Private Methods
void ArchiveToolHelper::ParseTokens(const std::string& sPath)
{
	std::string::size_type iPos = std::string::npos;
	bool bDone = false;
	
	std::string sToken;
	bool bBeginQuote = false;
	bool bEndQuote = false;

	while (!bDone)
	{
		bBeginQuote = false;
		bEndQuote = false;

		std::string::size_type BeginToken = sPath.find('"',iPos+1);

		if ( BeginToken != std::string::npos )
		{
			//if the path contains a " then keywords are being used
			m_IsUsingKeyWords = true;
			bBeginQuote = true;

			std::string::size_type EndToken = sPath.find('"',BeginToken+1);
			if ( EndToken != std::string::npos )
			{
				bEndQuote = true;
				sToken = sPath.substr(BeginToken,(EndToken-BeginToken+1));
				iPos = EndToken;
				if( ValidateToken(sToken) != true )
				{
					bDone = true;
				}
			}
			else
			{
				//no matching end " - keywords are invalid
				bDone = true;
			}
		}
		else
		{
			bDone = true;
		}
	}

	//if there is a begin quote but no end quote keyword path is invalid
	if (bBeginQuote && !bEndQuote)
	{
		m_ValidTokens = false;
	}
}

bool ArchiveToolHelper::ValidateToken(const std::string& sToken)
{
	std::string Token = sToken;
	m_ValidTokens = false;
	if (KeywordList.find(Token) != KeywordList.end())
	{
		m_ValidTokens = true;
	}

	return m_ValidTokens;
}
#pragma endregion Private Methods

} //namespace SvTo
