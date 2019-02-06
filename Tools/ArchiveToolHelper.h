//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH /// All Rights Reserved 
/// \Author	Robert Yoho
//*****************************************************************************

#pragma once

#pragma region Includes
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
	const std::string KW_DAY("\"DD\"");
	const std::string KW_MONTH("\"MM\"" );
	const std::string KW_YEAR("\"YYYY\"" );
	const std::string KW_HOUR("\"HH\"" );
	const std::string KW_MINUTE("\"mm\"" );
	const std::string KW_SECONDS( "\"SS\"" );

	typedef SvDef::StringSet KeywordSet;
	static KeywordSet KeywordList
	{
		KW_DAY,
		KW_MONTH,
		KW_YEAR,
		KW_HOUR,
		KW_MINUTE,
		KW_SECONDS
	};
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
	void Init(const std::string& sPath);

	//************************************
	/// Returns if the path is using keywords
	//************************************
	bool isUsingKeywords();

	//************************************
	/// Initialized the class.
	/// \param sToken [in] the token to be validated 
	/// \returns bool – true if the passed in token is valid
	//************************************
	std::string TranslatePath(const std::string& sPath);

	//************************************
	/// Returns if the tokens are valid
	/// \returns bool – true if parsed path is valid
	//************************************
	bool isTokensValid();

	//************************************
	/// this function will check the existence of the drive
	/// \returns bool – true if valid
	//************************************
	static bool ValidateDrive(LPCTSTR szFilePath, std::string& rDrive);
#pragma endregion Public Methods

private:
#pragma region Private Methods
	//************************************
	/// Will parse the path and check if keywords are being used
	/// \param sPath [in] - The string (path) that is to be parsed
	//************************************
	void ParseTokens(const std::string& sPath);

	//************************************
	/// It will validate the token
	/// \param sToken [in] the token to be validated 
	/// \returns bool – true if the passed in token is valid
	//************************************
	bool ValidateToken(const std::string& sToken);
#pragma endregion Private Methods

#pragma region Member Variables
	bool m_IsUsingKeyWords;
	bool m_ValidTokens;
#pragma endregion Member Variables
};

} //namespace SvTo
