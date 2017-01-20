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
	SVString sDay = SvUl_SF::Format("%02d", stime.wDay);
	SVString sMonth = SvUl_SF::Format("%02d", stime.wMonth);
	SVString sYear = SvUl_SF::Format("%04d", stime.wYear);
	SVString sHour = SvUl_SF::Format("%02d", stime.wHour);
	SVString sMin = SvUl_SF::Format("%02d", stime.wMinute);
	SVString sSec = SvUl_SF::Format("%02d", stime.wSecond);
	
	//replace all Keywords
	sReturnPath = SvUl_SF::searchAndReplace(sReturnPath, KW_DAY.c_str(),sDay.c_str());
	sReturnPath = SvUl_SF::searchAndReplace(sReturnPath, KW_MONTH.c_str(), sMonth.c_str());
	sReturnPath = SvUl_SF::searchAndReplace(sReturnPath, KW_YEAR.c_str(), sYear.c_str());
	sReturnPath = SvUl_SF::searchAndReplace(sReturnPath, KW_HOUR.c_str(), sHour.c_str());
	sReturnPath = SvUl_SF::searchAndReplace(sReturnPath, KW_MINUTE.c_str(), sMin.c_str());
	sReturnPath = SvUl_SF::searchAndReplace(sReturnPath, KW_SECONDS.c_str(), sSec.c_str());

	return sReturnPath;
}

 bool ArchiveToolHelper::ValidateDrive(LPCTSTR szFilePath, SVString& rDrive)
{
	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];

	//Get the drive text
	_tsplitpath(szFilePath, szDrive, szDir, szFName, szExt);

	if(rDrive.empty())
	{
		rDrive = szDrive;
	}
	return ( _access( szDrive, 0 ) ) ? false : true;
}
#pragma endregion Public Methods

#pragma region Private Methods
void ArchiveToolHelper::ParseTokens(const SVString& sPath)
{
	SVString::size_type iPos = -1;
	bool bDone = false;
	
	SVString::size_type BeginToken = SVString::npos;

	SVString::size_type EndToken = -1;
	
	SVString sToken;
	bool bBeginQuote = false;
	bool bEndQuote = false;

	while (!bDone)
	{
		bBeginQuote = false;
		bEndQuote = false;

		BeginToken = sPath.find('"',iPos+1);

		if ( BeginToken != SVString::npos )
		{
			//if the path contains a " then keywords are being used
			m_IsUsingKeyWords = true;
			bBeginQuote = true;

			EndToken = sPath.find('"',BeginToken+1);
			if ( EndToken != SVString::npos )
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