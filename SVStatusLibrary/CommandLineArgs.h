//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Singelton Class for Parsing the command line 
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
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
		//! return the  directory of the executable
		//! \returns LPCSTR
		//************************************
		LPCSTR GetStartDirectory();
		//************************************
		//! return the  directory from the command line or Getstartdirectorx  
		//! \returns LPCSTR
		//************************************
		LPCSTR GetIniDirectory();
#pragma endregion Public Methods

	private: 
#pragma region privat constructor 
		CommandLineArgs();
		CommandLineArgs( const CommandLineArgs& );
#pragma endregion privat constructor 

#pragma region privat function 
		void ParseCommandline();
#pragma endregion privat function 

	public:	
#pragma region public member 
		static const LPCTSTR  IniDirFlag ;
#pragma endregion public member 
	private:

#pragma region privat member 
		bool        m_IsParsed;
		SVString m_iniDirectory;
		SVString m_startDirectory;
#pragma endregion privat member 
	};

} //namespace SvStl
