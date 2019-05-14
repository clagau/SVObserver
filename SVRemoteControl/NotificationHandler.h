#pragma once
#include "SVProtoBuf\SVRC.pb.h"
#include "SVProtoBuf\Envelope.pb.h"
#include "SVControlCommands.h"
class NotificationHandler
{
public:
	NotificationHandler(SVControlCommands* pControlCommands);
	SvSyl::SVFuture<void>  OnNext(SvPb::GetNotificationStreamResponse& resp);
	void OnFinish();
	void OnError(const SvPenv::Error& er);
private:
	SVControlCommands* m_pControlCommands {NULL};
};
