//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// \file NotificationHandler.h
/// All Rights Reserved 
//*****************************************************************************
/// (see header file of the same name)
//******************************************************************************

#include "stdafx.h"
#include "NotificationHandler.h"
#include "RemoteControl.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVProtoBuf/Envelope.h"
#include "SVProtoBuf/SVRC.h"
#include "SVStatusLibrary/ErrorUtil.h"
#include "SVSystemLibrary/SVFuture.h"

namespace SvRc
{
NotificationHandler::NotificationHandler(const NotifyFunctor& prNotifier)
	:m_prNotifier{prNotifier}
{
}

SvSyl::SVFuture<void> NotificationHandler::OnNext(const SvPb::GetNotificationStreamResponse& rResponse)
{

	NotificationType type = NotificationType::UnknownNotificationType;
	_variant_t additionalParameter;
	if (S_OK != SvPb::ConvertProtobufToVariant(rResponse.parameter(), additionalParameter))
	{
		additionalParameter.Clear();
	}
	switch (rResponse.type())
	{
		case SvPb::NotifyType::currentMode:
		{
			type = NotificationType::CurrentMode;
			break;
		}
		case SvPb::NotifyType::lastModified:
		{
			type = NotificationType::LastModified;
			break;
		}
		case SvPb::NotifyType::configLoaded:
		{
			type = NotificationType::ConfigLoaded;
			break;
		}
		case SvPb::NotifyType::configUnloaded:
		{
			type = NotificationType::ConfigUnloaded;
			break;
		}
		default:
			break;
	}
	if (nullptr != m_prNotifier)
	{
		m_prNotifier(additionalParameter, static_cast<long> (type));
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
	SvStl::errorToException(er);
	return;
}

} //namespace SvRc