//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// \file MessageNotificationHandler.h
/// All Rights Reserved 
//*****************************************************************************
// Notification Handler for status changes
//******************************************************************************

#pragma once

typedef std::function<void(const _variant_t&, long)> NotifyFunctor;

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

namespace SvRc
{

class MessageNotificationHandler
{
public:
	explicit MessageNotificationHandler(const NotifyFunctor& prNotifier);
	SvSyl::SVFuture<void> OnNext(const SvPb::GetMessageStreamResponse& resp);
	void OnFinish();
	void OnError(const SvPenv::Error& er);
private:
	const NotifyFunctor& m_prNotifier;
};
} //namespace SvRc