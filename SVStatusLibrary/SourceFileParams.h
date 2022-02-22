//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file SourceFileParams.h
//*****************************************************************************
/// This structure is a container for the source file parameters
/// Compile date time, file name line number and source date time
//******************************************************************************
#pragma once

#pragma region Includes

#pragma endregion Includes

namespace SvStl
{
	#pragma region Declarations
	//This is a define because it uses the standard macros
	#define StdMessageParams _T(__DATE__), _T(__TIME__), _T(__FILE__), __LINE__, _T(__TIMESTAMP__)
	#pragma endregion Declarations

	struct SourceFileParams
	{

	#pragma region Constructor
	public:
		SourceFileParams() = default;

		SourceFileParams(LPCTSTR CompileDate, LPCTSTR CompileTime, LPCTSTR FileName, long Line, LPCTSTR FileDateTime) :
		  m_CompileDate(CompileDate)
		, m_CompileTime( CompileTime )
		, m_FileName( FileName )
		, m_Line( Line )
		, m_FileDateTime( FileDateTime )
		{
		};

		virtual ~SourceFileParams() = default;
	#pragma endregion Constructor

	#pragma endregion Public Methods
	public:
		void clear()
		{
			m_CompileDate.clear();
			m_CompileTime.clear();
			m_FileName.clear();
			m_Line = 0L;
			m_FileDateTime.clear();
		}
	#pragma endregion Public Methods


	#pragma region Member Variables
	public:
		std::string m_CompileDate;			//Date the file was compiled.
		std::string m_CompileTime;			//Time the file was compiled.
		std::string m_FileName;				//Name of the file where the message was encountered.
		long m_Line{ 0L };					//Line number within the source file where the message was encountered.
		std::string m_FileDateTime;			//Last file modification date/time
	#pragma endregion Member Variables
	};
} //namespace SvStl
