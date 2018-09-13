#include "stdafx.h"
#include "SVControlCommands.h"
#include "NotificationHandler.h"
#include <iosfwd>
#include "SVProtoBuf\ConverterHelper.h"

NotificationHandler::NotificationHandler(SVControlCommands* pControlCommands) :m_pControlCommands(pControlCommands)
{
}

std::future<void>  NotificationHandler::OnNext(SvPb::GetNotificationStreamResponse& resp)
{

	SVNotificationTypes type = UnknownNotificationType;

	boost::property_tree::ptree propTree;
	switch (resp.message_case())
	{

		case SvPb::GetNotificationStreamResponse::kCurrentMode:
		{
			svModeEnum svMode = SvPb::PbDeviceMode_2_SVIMMode(resp.currentmode());
			propTree.put("SVRC.DataItems.Mode", (int)svMode);
			propTree.put("SVRC.Notification", "CurrentMode");
			type = SVNotificationTypes::CurrentMode;
			break;
		}
		case SvPb::GetNotificationStreamResponse::kLastModified:
		{
			type = SVNotificationTypes::LastModified;
			unsigned int  timestamp = resp.lastmodified();
			propTree.put("SVRC.DataItems.TimeStamp", timestamp);
			propTree.put("SVRC.Notification", "LastModified");
			break;
		}
		case SvPb::GetNotificationStreamResponse::kMsgNotification:
		{
			type = SVNotificationTypes::MessageNotification;
			propTree.put("SVRC.Notification", "MessageNotification");
			propTree.put("SVRC.DataItems.MessageText", resp.msgnotification().messagetext());
			propTree.put("SVRC.DataItems.MessageType", resp.msgnotification().type());
			propTree.put("SVRC.DataItems.MessageNumber", resp.msgnotification().errornumber());
			break;
		}
		default:
			break;
	}
	std::stringstream OutStream;
	boost::property_tree::xml_writer_settings< boost::property_tree::ptree::key_type > Settings(' ', 4);
	boost::property_tree::write_xml(OutStream, propTree, Settings);
	m_pControlCommands->m_Notifier(_variant_t(OutStream.str().c_str()), type);
	return std::future<void>();
}
void NotificationHandler::OnFinish()
{
	/*SV_LOG_GLOBAL(info) << "Last Notification" << std::endl;
	return;*/
}
void NotificationHandler::OnError(const SvPenv::Error& er)
{
	SvRpc::errorToException(er);
	return;
}