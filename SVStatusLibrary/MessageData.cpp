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
#include "MessageTextGenerator.h"
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

	MessageData::MessageData( DWORD MessageCode, SvOi::MessageTextEnum AdditionalTextId, SVStringArray AdditionalTextList ) :
	m_Logged( false )
	, m_Displayed( false )
	, m_MessageCode( MessageCode )
	, m_AdditionalTextId( AdditionalTextId )
	, m_AdditionalTextList( AdditionalTextList )
	, m_SourceLine( 0 )
	, m_ProgramCode( 0 )
	, m_OSErrorCode( 0 )
	, m_DateTime( 0 )
	{
	}

	MessageData::MessageData( const MessageData& rRhs ) :
	  m_Logged( rRhs.m_Logged )
	, m_Displayed( rRhs.m_Displayed )
	, m_MessageCode( rRhs.m_MessageCode )
	, m_AdditionalTextId( rRhs.m_AdditionalTextId )
	, m_AdditionalTextList( rRhs.m_AdditionalTextList )
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
			m_Displayed = rRhs.m_Displayed;
			m_MessageCode = rRhs.m_MessageCode;
			m_AdditionalTextId = rRhs.m_AdditionalTextId;
			m_AdditionalTextList = rRhs.m_AdditionalTextList;
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
		m_Displayed = false;
		m_MessageCode = 0;
		m_AdditionalTextId = SvOi::Tid_Empty;
		m_AdditionalTextList.clear();
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

	SVString MessageData::getAdditionalText() const
	{
		return MessageTextGenerator::Instance().getText(m_AdditionalTextId, m_AdditionalTextList);
	}

	SVString MessageData::convertId2AddtionalText(SvOi::MessageTextEnum id)
	{
		return MessageTextGenerator::convertId2AddtionalText(id);
	}
#pragma endregion Public Methods
} /* namespace SVStatusLibrary */ } /* namespace Seidenader */