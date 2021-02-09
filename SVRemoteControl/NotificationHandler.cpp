//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// \file NotificationHandler.cpp
/// All Rights Reserved 
//*****************************************************************************
/// (see header file of the same name)
//******************************************************************************

#include "stdafx.h"
#include "SVControlCommands.h"
#include "NotificationHandler.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVProtoBuf/SVRC.pb.h"
#include "SVProtoBuf/Envelope.pb.h"
#include "SVSystemLibrary/SVFuture.h"

NotificationHandler::NotificationHandler(const SVControlCommands& rControl) : 
	m_rControl(rControl)
{
}

SvSyl::SVFuture<void> NotificationHandler::OnNext(const SvPb::GetNotificationStreamResponse& rResponse)
{

	SVNotificationTypesEnum type = SVNotificationTypesEnum::UnknownNotificationType;
	_variant_t additionalParameter;
	if (S_OK != SvPb::ConvertProtobufToVariant(rResponse.parameter(), additionalParameter))
	{
		additionalParameter.Clear();
	}
	boost::property_tree::ptree propTree;
	switch (rResponse.type())
	{
		case SvPb::NotifyType::currentMode:
		{
			//For HMI change all Unknownmode notifications to modeChanging
			long parameter = static_cast<long> (additionalParameter);
			bool isUnknownMode = SvPb::unknownMode == static_cast<SvPb::DeviceModeType> (parameter);
			long currentMode = isUnknownMode ? static_cast<long> (SvPb::modeChanging) : parameter;
			propTree.put("SVRC.Notification", "CurrentMode");
			propTree.put("SVRC.DataItems.Mode", currentMode);
			type = SVNotificationTypesEnum::CurrentMode;
			break;
		}
		case SvPb::NotifyType::lastModified:
		{
			propTree.put("SVRC.Notification", "LastModified");
			propTree.put("SVRC.DataItems.TimeStamp", static_cast<__int64> (additionalParameter));
			type = SVNotificationTypesEnum::LastModified;
			break;
		}
		case SvPb::NotifyType::configLoaded:
		{
			std::string loadText{ _T("The following configuration has been loaded:\n") };
			loadText += _bstr_t(additionalParameter.bstrVal);
			propTree.put("SVRC.Notification", "loadConfigNotification");
			propTree.put("SVRC.DataItems.MessageText", loadText);
			type = SVNotificationTypesEnum::ConfigLoaded;
			break;
		}
		case SvPb::NotifyType::configUnloaded:
		{
			std::string loadText{ _T("The following configuration has been unloaded:\n") };
			loadText += _bstr_t(additionalParameter.bstrVal);
			propTree.put("SVRC.Notification", "unloadConfigNotification");
			propTree.put("SVRC.DataItems.MessageText", loadText);
			type = SVNotificationTypesEnum::ConfigUnloaded;
			break;
		}
		default:
			break;
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