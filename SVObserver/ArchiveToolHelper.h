//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma once

#pragma region Includes
#include <afx.h>
#include <boost/assign.hpp>
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

#pragma region Declarations
	const SVString KW_DAY("\"DD\"");
	const SVString KW_MONTH("\"MM\"" );
	const SVString KW_YEAR("\"YYYY\"" );
	const SVString KW_HOUR("\"HH\"" );
	const SVString KW_MINUTE("\"mm\"" );
	const SVString KW_SECONDS( "\"SS\"" );

	typedef std::set<SVString> KeywordSet;
	static KeywordSet KeywordList = boost::assign::list_of
		(KW_DAY)
		(KW_MONTH)
		(KW_YEAR)
		(KW_HOUR)
		(KW_MINUTE)
		(KW_SECONDS);
#pragma endregion Declarations

class ArchiveToolHelper
{
public:
#pragma region Constructor 
	ArchiveToolHelper();
	virtual ~ArchiveToolHelper();
#pragma endregion Constructor 

#pragma region Public Methods
	//************************************
	/// Initializes the path
	/// \param sPath [in] path to be initialized
	//************************************
	void Init(const SVString& sPath);

	//************************************
	/// Returns if the path is using keywords
	//************************************
	bool isUsingKeywords();

	//************************************
	/// Initialized the class.
	/// \param sToken [in] the token to be validated 
	/// \returns bool � true if the passed in token is valid
	//************************************
	SVString TranslatePath(const SVString& sPath);

	//************************************
	/// Returns if the tokens are valid
	/// \returns bool � true if parsed path is valid
	//************************************
	bool isTokensValid();
#pragma endregion Public Methods

private:
#pragma region Private Methods
	//************************************
	/// Will parse the path and check if keywords are being used
	/// \param sPath [in] - The string (path) that is to be parsed
	//************************************
	void ParseTokens(const SVString& sPath);

	//************************************
	/// It will validate the token
	/// \param sToken [in] the token to be validated 
	/// \returns bool � true if the passed in token is valid
	//************************************
	bool ValidateToken(const SVString& sToken);
#pragma endregion Private Methods

#pragma region Member Variables
	bool m_IsUsingKeyWords;
	bool m_ValidTokens;
#pragma endregion Member Variables
};

