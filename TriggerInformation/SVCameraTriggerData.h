//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraTriggerData
//* .File Name       : $Workfile:   SVCameraTriggerData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:52:48  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTriggerInfoStruct.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVIOLibrary/SVIOEntryHostStruct.h"
#pragma endregion Includes

namespace SvTi
{
	class SVCameraTriggerData : public SVObjectClass 
	{
	public:
		SVCameraTriggerData();
		virtual ~SVCameraTriggerData();

		SVIOEntryHostStructPtr SetupLineStateInput();
		SVIOEntryHostStructPtr SetupTimestampInput();

		std::string GetTimestampName() const;
		std::string GetLineStateName() const;

		void Set(const IntVariantMap& rSettings);

	private:
		void init();
		void destroy();
		void SetLineState(double timeStamp, bool state);

	private:
		SvVol::SVDoubleValueObjectClass m_timestamp;
		SvVol::SVBoolValueObjectClass m_lineInState;
	};
} //namespace SvTi