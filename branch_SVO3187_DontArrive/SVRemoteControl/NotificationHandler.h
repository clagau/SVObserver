//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// \file NotificationHandler.h
/// All Rights Reserved 
//*****************************************************************************
// Notification Handler for status changes
//******************************************************************************

#pragma once

class SVControlCommands;
namespace SvPenv
{
	class Error;
}
namespace SvPb
{
	class GetNotificationStreamResponse;
}
namespace SvSyl
{
	template<class T>
	class SVFuture;
}

class NotificationHandler
{
public:
	NotificationHandler(const SVControlCommands& rControl);
	SvSyl::SVFuture<void>  OnNext(const SvPb::GetNotificationStreamResponse& resp);
	void OnFinish();
	void OnError(const SvPenv::Error& er);
private:
	const SVControlCommands& m_rControl;
};
