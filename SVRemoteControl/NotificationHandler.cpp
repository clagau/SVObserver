#include "stdafx.h"
#include "SVControlCommands.h"
#include "NotificationHandler.h"
#include <iosfwd>

NotificationHandler::NotificationHandler(SVControlCommands* pControlCommands) :m_pControlCommands(pControlCommands)
{
}

std::future<void>  NotificationHandler::OnNext(SvPb::GetNotificationStreamResponse& resp)
{
	//@Todo[MEC][8.00] [07.05.2018] we need XML fo rthe strings 
	SVNotificationTypes type = UnknownNotificationType;
	std::string value;
	switch (resp.message_case())
	{
		
		case SvPb::GetNotificationStreamResponse::kCurrentMode:
		{
			type = SVNotificationTypes::CurrentMode;
			value = SvPb::DeviceModeType_Name(resp.currentmode());

			break;
		}
		case SvPb::GetNotificationStreamResponse::kLastModified:
		{
			type = SVNotificationTypes::LastModified;
			unsigned int  lastMod = resp.lastmodified();
			std::stringstream strst;
			strst << lastMod;
			value = strst.str();
			break;
		}
		case SvPb::GetNotificationStreamResponse::kMsgNotification:
		{
			type = SVNotificationTypes::MessageNotification;
			value = resp.msgnotification().messagetext();
			break;
		}
		default:
			break;
	}

	m_pControlCommands->m_Notifier(_variant_t(value.c_str()), type);

	return std::future<void>();
}
void NotificationHandler::OnFinish()
{
	/*BOOST_LOG_TRIVIAL(info) << "Last Notification" << std::endl;
	return;*/
}
void NotificationHandler::OnError(const SvPenv::Error& er)
{
	/*SvRpc::errorToException(er);
	return;*/
}