//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#pragma once




#pragma region Includes
#include <string>
#pragma endregion Includes


#pragma region Declarations
#pragma endregion Declarations


/// \brief ENTER SHORT DESCRIPTION (ONE SENTENCE)
/// ENTER ...
/// ... MORE ... 
/// ... DETAILED ... 
/// ... DESCRIPTION
class CommandLineArgs
{
public:
	 CommandLineArgs();
	 LPCSTR GetStartDirectory();
	 LPCSTR GetIniDirectory();
	 void ParseCommandline();
	static const std::string  IniDirFlag;
private:
	bool        m_IsParsed;
	std::string m_iniDirectory;
	std::string m_startDirectory;

};

