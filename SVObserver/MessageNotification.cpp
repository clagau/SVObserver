///*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <string>
#include "MessageNotification.h"
#include "JsonLib\include\writer.h"
#include "SVRemoteControlConstants.h"
#include "SVSocketRemoteCommandManager.h"
#include "SVRemoteControlConstants.h"
#include "SVRemoteControlConstants.h"
#include "SVVisionProcessorHelper.h"
#pragma endregion Includes


#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SVN = SVRC::notification;
MessageNotification::MessageNotification() 
	:m_MessageType(SvStl::MsgUknown),
	m_MessageNumber(-1),
	m_MessageText(_T("")),
	m_IsProcessed(false)
{
}

void MessageNotification::ProcessNotification()
{

	Concurrency::critical_section::scoped_lock   scopedLock( m_Protect);

	if( !m_IsProcessed )
	{
		std::string JsonNotification;
		Json::FastWriter Writer;
		Json::Value Object(Json::objectValue);
		Json::Value ElementObject(Json::objectValue);

		ElementObject[SVN::MessageType] = m_MessageType;
		ElementObject[SVN::MessageNumber] = m_MessageNumber;
		ElementObject[SVN::MessageText] = m_MessageText;

		Object[SVN::notification ] = SVN::MessageNotification;
		Object[SVN::dataitems] = ElementObject;

		JsonNotification = Writer.write( Object ).c_str();

		SVSocketRemoteCommandManager::Instance().ProcessJsonNotification( JsonNotification );

		m_IsProcessed = true;
	}

}

void MessageNotification::SetNotification(SvStl::NotificationEnum type, int ErrorNumber, LPCTSTR errormessage)
{
		Concurrency::critical_section::scoped_lock   scopedLock( m_Protect);
		m_MessageType = type;
		m_MessageNumber = ErrorNumber;
		m_MessageText = (nullptr != errormessage) ? errormessage : SVString();
		SvUl_SF::searchAndReplace(m_MessageText, "\r\n","\n");
		m_IsProcessed = false; 

}



INT_PTR  MessageNotification::FireNotify(SvStl::NotificationEnum  type , int errornumber , LPCTSTR errortext )
{
		SVVisionProcessorHelper::Instance().FireMessageNotification(type,errornumber,errortext);
		return 0;
}