//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// \file NotificationHandler.h
/// All Rights Reserved 
//*****************************************************************************
/// (see header file of the same name)
//******************************************************************************

#include "stdafx.h"
#include "NotificationHandler.h"
#include "RemoteControlImpl.h"
#include "SVProtoBuf\SVRC.pb.h"
#include "SVProtoBuf\Envelope.pb.h"
#include <iosfwd>

namespace SvRc
{
NotificationHandler::NotificationHandler(const RemoteControlImpl& rRemoteCtrl) : 
	m_rRemoteCtrl(rRemoteCtrl)
{
}

SvSyl::SVFuture<void> NotificationHandler::OnNext(const SvPb::GetNotificationStreamResponse& rResponse)
{

	NotificationType type = NotificationType::UnknownNotificationType;

	boost::property_tree::ptree propTree;
	switch (rResponse.message_case())
	{

		case SvPb::GetNotificationStreamResponse::kCurrentMode:
		{
			//For HMI change all Unknown mode notifications to modeChanging
			bool isUnknownMode = SvPb::unknownMode == rResponse.currentmode();
			int currentMode = static_cast<int> (isUnknownMode ? SvPb::modeChanging : rResponse.currentmode());
			propTree.put("SVRC.Notification", "CurrentMode");
			propTree.put("SVRC.DataItems.Mode", currentMode);
			type = NotificationType::CurrentMode;
			break;
		}
		case SvPb::GetNotificationStreamResponse::kLastModified:
		{
			type = NotificationType::LastModified;
			unsigned int  timestamp = rResponse.lastmodified();
			propTree.put("SVRC.Notification", "LastModified");
			propTree.put("SVRC.DataItems.TimeStamp", timestamp);
			break;
		}
		case SvPb::GetNotificationStreamResponse::kMsgNotification:
		{
			type = NotificationType::MessageNotification;
			propTree.put("SVRC.Notification", "MessageNotification");
			propTree.put("SVRC.DataItems.MessageText", rResponse.msgnotification().messagetext());
			propTree.put("SVRC.DataItems.MessageType", rResponse.msgnotification().type());
			propTree.put("SVRC.DataItems.MessageNumber", rResponse.msgnotification().errornumber());
			break;
		}
		case SvPb::GetNotificationStreamResponse::kConfigFileLoaded:
		{
			type = NotificationType::ConfigLoaded;
			std::string loadText{_T("The following configuration has been loaded:\n")};
			loadText += rResponse.configfileloaded();
			propTree.put("SVRC.Notification", "loadConfigNotification");
			propTree.put("SVRC.DataItems.MessageText", loadText);
			break;
		}
		default:
			break;
	}
	std::stringstream OutStream;
	boost::property_tree::xml_writer_settings< boost::property_tree::ptree::key_type > Settings(' ', 4);
	boost::property_tree::write_xml(OutStream, propTree, Settings);
	_variant_t notifyName(OutStream.str().c_str());
	if (nullptr != m_rRemoteCtrl.GetNotifyFunctor())
	{
		m_rRemoteCtrl.GetNotifyFunctor()(notifyName, static_cast<long> (type));
	}
	return  SvSyl::SVFuture<void>::make_ready();
}
void NotificationHandler::OnFinish()
{
	/*SV_LOG_GLOBAL(info) << "Last Notification" << std::endl;
	return;*/
}
void NotificationHandler::OnError(const SvPenv::Error& er)
{
	SvUl::errorToException(er);
	return;
}

} //namespace SvRc