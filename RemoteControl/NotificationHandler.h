//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// \file NotificationHandler.h
/// All Rights Reserved 
//*****************************************************************************
// Notification Handler for status changes
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVSystemLibrary/SVFuture.h"
#pragma endregion Includes


namespace SvPb
{
	class GetNotificationStreamResponse;
}
namespace SvPenv
{
	class Error;
}

namespace SvRc
{
class RemoteControlImpl;

class NotificationHandler
{
public:
	explicit NotificationHandler(const RemoteControlImpl& rRemoteCtrl);
	SvSyl::SVFuture<void> OnNext(const SvPb::GetNotificationStreamResponse& resp);
	void OnFinish();
	void OnError(const SvPenv::Error& er);
private:
	const RemoteControlImpl& m_rRemoteCtrl;
};
} //namespace SvRc