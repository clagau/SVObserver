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
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVLibrary/SVIOEntryHostStruct.h"
#pragma endregion Includes

namespace SvTi
{
	class SVCameraTriggerData : public SVObjectClass 
	{
	public:
		typedef  std::map<std::string, _variant_t> NameVariantMap;

	private:
		SVDoubleValueObjectClass m_timestamp;
		SVBoolValueObjectClass m_lineInState;

	public:
		SVCameraTriggerData();
		virtual ~SVCameraTriggerData();

		SVIOEntryHostStructPtr SetupLineStateInput(long objectDepth);
		SVIOEntryHostStructPtr SetupTimestampInput(long objectDepth);

		std::string GetTimestampName() const;
		std::string GetLineStateName() const;

		void Set(const NameVariantMap& rSettings);

	private:
		void init();
		void destroy();
		void SetLineState(double timeStamp, bool state);
	};
} //namespace SvTi