//*****************************************************************************
/// \copyright (c) 2020,2020 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file MessageNotificationHandler.cpp
/// All Rights Reserved 
//*****************************************************************************
/// (see header file of the same name)
//******************************************************************************

#include "stdafx.h"
#include "MessageNotificationHandler.h"
#include "RemoteControl.h"
#include "SVProtoBuf/Envelope.h"
#include "SVProtoBuf/SVRC.h"
#include "SVStatusLibrary/ErrorUtil.h"
#include "SVSystemLibrary/SVFuture.h"

namespace SvRc
{
MessageNotificationHandler::MessageNotificationHandler(const NotifyFunctor& prNotifier)
	:m_prNotifier{ prNotifier }
{
}

SvSyl::SVFuture<void> MessageNotificationHandler::OnNext(const SvPb::GetMessageStreamResponse& rResponse)
{

	NotificationType type = NotificationType::MessageNotification;

	boost::property_tree::ptree propTree;
	propTree.put("SVRC.Notification", "MessageNotification");
	if(rResponse.messagelist().messages_size() > 0)
	{
		//Get only first message
		const SvPb::MessageContainer& rMessage = rResponse.messagelist().messages(0);
		propTree.put("SVRC.DataItems.MessageText", rMessage.messagetext());
		propTree.put("SVRC.DataItems.MessageType", rResponse.type());
		propTree.put("SVRC.DataItems.MessageNumber", rMessage.messagecode());
		propTree.put("SVRC.DataItems.Severity", rMessage.severity());
	}

	std::stringstream OutStream;
	boost::property_tree::xml_writer_settings< boost::property_tree::ptree::key_type > Settings(' ', 4);
	boost::property_tree::write_xml(OutStream, propTree, Settings);
	_variant_t notifyData(OutStream.str().c_str());
	if (nullptr != m_prNotifier)
	{
		m_prNotifier(notifyData, static_cast<long> (type));
	}
	return  SvSyl::SVFuture<void>::make_ready();
}
void MessageNotificationHandler::OnFinish()
{
	/*SV_LOG_GLOBAL(info) << "Last Notification" << std::endl;
	return;*/
}
void MessageNotificationHandler::OnError(const SvPenv::Error& er)
{
	SvStl::errorToException(er);
	return;
}
} //namespace SvRc