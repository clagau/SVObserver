//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// \file MessageNotificationHandler.h
/// All Rights Reserved 
//*****************************************************************************
// Notification Handler for status changes
//******************************************************************************

#pragma once

class SVControlCommands;

namespace SvPb
{
	class GetMessageStreamResponse;
}
namespace SvPenv
{
	class Error;
}
namespace SvSyl
{
	template<class T>
	class SVFuture;
}

class MessageNotificationHandler
{
public:
	explicit MessageNotificationHandler(const SVControlCommands& rControl);
	SvSyl::SVFuture<void> OnNext(const SvPb::GetMessageStreamResponse& resp);
	void OnFinish();
	void OnError(const SvPenv::Error& er);
private:
	const SVControlCommands& m_rControl;
};
