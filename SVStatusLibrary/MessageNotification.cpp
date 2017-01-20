///*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "MessageNotification.h"
#pragma endregion Includes


#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace Seidenader { namespace SVStatusLibrary
{
	MessageNotification::MessageNotification() :
	 m_Type( SvStl::MsgUknown )
	,m_MessageNumber( -1 )
	,m_MessageText( _T("") )
	,m_IsProcessed( false )
	{
	}

	void MessageNotification::SetNotification( SvStl::NotificationEnum Type, int MessageNumber, LPCTSTR MessageText )
	{
		Concurrency::critical_section::scoped_lock   scopedLock( m_Lock );
		m_Type = Type;
		m_MessageNumber = MessageNumber;
		m_MessageText = (nullptr != MessageText) ? MessageText : SVString();
		SvUl_SF::searchAndReplace( m_MessageText, "\r\n","\n" );
		m_IsProcessed = false; 
	}
} /* namespace SVStatusLibrary */ } /* namespace Seidenader */
