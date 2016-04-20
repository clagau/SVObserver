///*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// CommandlineArgs Singelton Class for Parsing the command line 
//*****************************************************************************


#pragma region Includes
#include "stdafx.h"
#include "CommandLineArgs.h"
#include <boost/tokenizer.hpp>
#include <string>

#pragma endregion Includes


#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace Seidenader { namespace SVStatusLibrary
{
	
	const LPCTSTR   CommandLineArgs::IniDirFlag = _T( "-iniFile" );

	CommandLineArgs& CommandLineArgs::Inst()
	{
	
		static CommandLineArgs Instance;
		return Instance;
	
	}
	
	CommandLineArgs::CommandLineArgs() :m_IsParsed(false)
	{
	}

	LPCSTR CommandLineArgs::GetStartDirectory()
	{
		if(m_IsParsed == false)
		{
			ParseCommandline();
		}
		return m_startDirectory.c_str();
	}
	LPCSTR CommandLineArgs::GetIniDirectory()
	{
		if(m_IsParsed == false)
		{
			ParseCommandline();
		}
		if(m_iniDirectory.empty())
			return m_startDirectory.c_str();
		else
			return m_iniDirectory.c_str();
	}

	void CommandLineArgs::ParseCommandline()
	{
		std::string StrCommandline  = GetCommandLine(); 
		typedef boost::escaped_list_separator<char> Tels; 
		Tels els("" ," ","\"" ); //escape, separator, quote

		boost::tokenizer<Tels>  tok(StrCommandline, els);
		bool first(true), IsIni(false);
		
		for(boost::tokenizer<Tels>::iterator beg=tok.begin(); beg!=tok.end();beg++)
		{
			std::string::size_type size = beg->size();
			std::string temp = *beg;
			if(size == 0)
			{
				continue;
			}
			if(first)
			{
				m_startDirectory = beg->substr(0, beg->find_last_of("\\"));
				first = false;
			}
			else if (true == IsIni)
			{
				if(size >0  && beg->at(size-1)  == '\\' )
				{
					m_iniDirectory = beg->substr(0, size -1); 
				}
			}
			IsIni = (_stricmp( beg->c_str(),  CommandLineArgs::IniDirFlag) == 0);
		}
		m_IsParsed = true;
	}

}
}