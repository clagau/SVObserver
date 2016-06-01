//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file SourceFileParams.h
//*****************************************************************************
/// This structure is a container for the source file parameters
/// Compile date time, file name line number and source date time
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace Seidenader { namespace SVStatusLibrary
{
	#pragma region Declarations
	//This is a define because it uses the standard macros
	#define StdMessageParams _T(__DATE__), _T(__TIME__), _T(__FILE__), __LINE__, _T(__TIMESTAMP__)
	#pragma endregion Declarations

	struct SourceFileParams
	{

	#pragma region Constructor
	public:
		SourceFileParams() :
		  m_Line( 0 )
		{

		};

		SourceFileParams(LPCTSTR CompileDate, LPCTSTR CompileTime, LPCTSTR FileName, long Line, LPCTSTR FileDateTime) :
		  m_CompileDate(CompileDate)
		, m_CompileTime( CompileTime )
		, m_FileName( FileName )
		, m_Line( Line )
		, m_FileDateTime( FileDateTime )
		{

		};

		  virtual ~SourceFileParams() {};
	#pragma endregion Constructor

	#pragma endregion Public Methods
	public:
		void clear()
		{
			m_CompileDate.clear();
			m_CompileTime.clear();
			m_FileName.clear();
			m_Line = 0;
			m_FileDateTime.clear();
		}
	#pragma endregion Public Methods


	#pragma region Member Variables
	public:
		SVString m_CompileDate;				//Date the file was compiled.
		SVString m_CompileTime;				//Time the file was compiled.
		SVString m_FileName;				//Name of the file where the message was encountered.
		long m_Line;						//Line number within the source file where the message was encountered.
		SVString m_FileDateTime;			//Last file modification date/time
	#pragma endregion Member Variables
	};
} /* namespace SVStatusLibrary */ } /* namespace Seidenader */

namespace SvStl = Seidenader::SVStatusLibrary;
