//*****************************************************************************
/// \copyright (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file NotificationHandler.h
/// All Rights Reserved 
//*****************************************************************************
// Notification Handler for status changes
//******************************************************************************

#pragma once

typedef std::function<void(const _variant_t&, long)> NotifyFunctor;

namespace SvPb
{
	class GetNotificationStreamResponse;
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
class RemoteControlImpl;

class NotificationHandler
{
public:
	explicit NotificationHandler(const NotifyFunctor& prNotifier);
	SvSyl::SVFuture<void> OnNext(const SvPb::GetNotificationStreamResponse& resp);
	void OnFinish();
	void OnError(const SvPenv::Error& er);
private:
	const NotifyFunctor& m_prNotifier;
};
} //namespace SvRc