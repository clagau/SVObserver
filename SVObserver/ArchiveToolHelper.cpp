//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "ArchiveToolHelper.h"
#pragma endregion Includes

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
void ArchiveToolHelper::Init(const SVString& sPath)
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

bool ArchiveToolHelper::isTokensValid()
{
	return m_ValidTokens;
}

SVString ArchiveToolHelper::TranslatePath(const SVString& sPath)
{
	SVString sReturnPath = sPath;
	SVString sTmpPath = sPath;
	SYSTEMTIME stime;
	memset( &stime, 0, sizeof( SYSTEMTIME));
	::GetLocalTime( &stime );
	SVString sDay;
	sDay.Format("%02d", stime.wDay);
	SVString sMonth;
	sMonth.Format("%02d", stime.wMonth);
	SVString sYear;
	sYear.Format("%04d", stime.wYear);
	SVString sHour;
	sHour.Format("%02d", stime.wHour);
	SVString sMin;
	sMin.Format("%02d", stime.wMinute);
	SVString sSec;
	sSec.Format("%02d", stime.wSecond);
	
	//replace all Keywords
	sReturnPath = sReturnPath.replace(KW_DAY.c_str(),sDay.c_str());
	sReturnPath = sReturnPath.replace(KW_MONTH.c_str(), sMonth.c_str());
	sReturnPath = sReturnPath.replace(KW_YEAR.c_str(), sYear.c_str());
	sReturnPath = sReturnPath.replace(KW_HOUR.c_str(), sHour.c_str());
	sReturnPath = sReturnPath.replace(KW_MINUTE.c_str(), sMin.c_str());
	sReturnPath = sReturnPath.replace(KW_SECONDS.c_str(), sSec.c_str());

	return sReturnPath;
}
#pragma endregion Public Methods

#pragma region Private Methods
void ArchiveToolHelper::ParseTokens(const SVString& sPath)
{
	SVString::size_type iPos = -1;
	bool bDone = false;
	
	SVString::size_type BeginToken = SVString::SVDataType::npos;

	SVString::size_type EndToken = -1;
	
	SVString sToken;
	bool bBeginQuote = false;
	bool bEndQuote = false;

	while (!bDone)
	{
		bBeginQuote = false;
		bEndQuote = false;

		BeginToken = sPath.find('"',iPos+1);

		if ( BeginToken != SVString::SVDataType::npos )
		{
			//if the path contains a " then keywords are being used
			m_IsUsingKeyWords = true;
			bBeginQuote = true;

			EndToken = sPath.find('"',BeginToken+1);
			if ( EndToken != SVString::SVDataType::npos )
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

bool ArchiveToolHelper::ValidateToken(const SVString& sToken)
{
	SVString Token = sToken;
	m_ValidTokens = false;
	if (KeywordList.find(Token) != KeywordList.end())
	{
		m_ValidTokens = true;
	}

	return m_ValidTokens;
}
#pragma endregion Private Methods