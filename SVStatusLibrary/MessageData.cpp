//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
//*****************************************************************************
/// The structure contains the data for the message handler
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "MessageData.h"
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
#pragma region Constructor
	MessageData::MessageData() :
	  m_Logged( false )
	, m_MessageCode( 0 )
	, m_SourceLine( 0 )
	, m_ProgramCode( 0 )
	, m_OSErrorCode( 0 )
	, m_DateTime( 0 )
	{
	}

	MessageData::MessageData( DWORD MessageCode, LPCTSTR AdditionalText ) :
	 m_Logged( false )
	, m_MessageCode( MessageCode )
	, m_AdditionalText( AdditionalText )
	, m_SourceLine( 0 )
	, m_ProgramCode( 0 )
	, m_OSErrorCode( 0 )
	, m_DateTime( 0 )
	{
	}

	MessageData::MessageData( const MessageData& rRhs ) :
	  m_Logged( rRhs.m_Logged )
	, m_MessageCode( rRhs.m_MessageCode )
	, m_AdditionalText( rRhs.m_AdditionalText )
	, m_CompileDate( rRhs.m_CompileDate )
	, m_CompileTime( rRhs.m_CompileTime )
	, m_SourceFile( rRhs.m_SourceFile )
	, m_SourceLine( rRhs.m_SourceLine )
	, m_SourceDateTime( rRhs.m_SourceDateTime )
	, m_ProgramCode( rRhs.m_ProgramCode )
	, m_OSErrorCode( rRhs.m_OSErrorCode )
	, m_User( rRhs.m_User )
	, m_DateTime( rRhs.m_DateTime )
	{
	}

	const MessageData& MessageData::operator=( const MessageData& rRhs )
	{
		if( &rRhs != this )
		{
			m_Logged = rRhs.m_Logged;
			m_MessageCode = rRhs.m_MessageCode;
			m_AdditionalText = rRhs.m_AdditionalText;
			m_CompileDate = rRhs.m_CompileDate;
			m_CompileTime = rRhs.m_CompileTime;
			m_SourceFile = rRhs.m_SourceFile;
			m_SourceLine = rRhs.m_SourceLine;
			m_SourceDateTime = rRhs.m_SourceDateTime;
			m_ProgramCode = rRhs.m_ProgramCode;
			m_OSErrorCode = rRhs.m_OSErrorCode;
			m_User = rRhs.m_User;
			m_DateTime = rRhs.m_DateTime;
		}

		return *this;
	}

	MessageData::~MessageData()
	{
	}
#pragma endregion Constructor

#pragma region Public Methods
	void MessageData::clear()
	{
		m_Logged = false;
		m_MessageCode = 0;
		m_AdditionalText.clear();
		m_CompileDate.clear();
		m_CompileTime.clear();
		m_SourceFile.clear();
		m_SourceLine = 0;
		m_SourceDateTime.clear();
		m_ProgramCode = 0;
		m_OSErrorCode = 0;
		m_User.clear();
		m_DateTime = 0;
	}
#pragma endregion Public Methods
} /* namespace SVStatusLibrary */ } /* namespace Seidenader */