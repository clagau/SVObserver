//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTimerCallback
//* .File Name       : $Workfile:   SVTimerCallback.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:36:24  $
//******************************************************************************
#pragma once

#pragma region Includes

#pragma endregion Includes

namespace SvTl
{
	class SVTimerCallback
	{
	public:
		virtual ~SVTimerCallback() {}
		virtual void Notify(const std::string& listenerTag)=0;
	};
} //namespace SvTl
