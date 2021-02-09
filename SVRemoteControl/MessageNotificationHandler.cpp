//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// \file MessageNotificationHandler.cpp
/// All Rights Reserved 
//*****************************************************************************
/// (see header file of the same name)
//******************************************************************************

#include "stdafx.h"
#include "MessageNotificationHandler.h"
#include "SVControlCommands.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVProtoBuf/Envelope.pb.h"
#include "SVProtoBuf/SVRC.pb.h"
#include <iosfwd>

MessageNotificationHandler::MessageNotificationHandler(const SVControlCommands& rControl) :
	m_rControl(rControl)
{
}

SvSyl::SVFuture<void> MessageNotificationHandler::OnNext(const SvPb::GetMessageNotificationStreamResponse& rResponse)
{

	SVNotificationTypesEnum type = SVNotificationTypesEnum::MessageNotification;

	boost::property_tree::ptree propTree;
	propTree.put("SVRC.Notification", "MessageNotification");
	if(rResponse.msglist().messages_size() > 0)
	{
		//Get only first message
		const SvPb::MessageContainer& rMessage = rResponse.msglist().messages(0);
		propTree.put("SVRC.DataItems.MessageText", rMessage.messagetext());
		propTree.put("SVRC.DataItems.MessageType", rResponse.type());
		propTree.put("SVRC.DataItems.MessageNumber", rMessage.messagecode());
		propTree.put("SVRC.DataItems.Severity", rMessage.severity());
	}

	std::stringstream OutStream;
	boost::property_tree::xml_writer_settings< boost::property_tree::ptree::key_type > Settings(' ', 4);
	boost::property_tree::write_xml(OutStream, propTree, Settings);
	_variant_t notifyData(OutStream.str().c_str());
	NotifyFunctor pNotifier = m_rControl.getNotifier();
	if (nullptr != pNotifier)
	{
		pNotifier(notifyData, type);
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
