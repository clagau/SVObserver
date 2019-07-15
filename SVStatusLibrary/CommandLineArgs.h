//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Singelton Class for Parsing the command line 
//******************************************************************************
#pragma once

#pragma region Includes

#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvStl
{
/// \briefClass for Parsing the command line 
class CommandLineArgs
{
public:
#pragma region Public Methods
	static CommandLineArgs& Inst();

	//************************************
	//! return the  directory from the command line or Getstartdirectorx  
	//! \returns LPCSTR
	//************************************
	LPCSTR GetIniDirectory();

	//!Retrieves the ModulDir
	//!If the hModul  parameter is NULL,retrieves the dir of the executable file of the current process.

	static	void GetModulDirName(std::string& rDirName, HMODULE hModul=nullptr );
#pragma endregion Public Methods

private:
#pragma region privat constructor 
	CommandLineArgs();
	CommandLineArgs(const CommandLineArgs&);
#pragma endregion privat constructor 

#pragma region privat function 
	void ParseCommandline();
#pragma endregion privat function 

public:
#pragma region public member 
	static const LPCTSTR  IniDirFlag;
#pragma endregion public member 
private:

#pragma region privat member 
	bool        m_IsParsed;
	std::string m_iniDirectory;
	std::string m_startDirectory;
#pragma endregion privat member 
};

} //namespace SvStl
