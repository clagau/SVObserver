//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file SVTriggerInfoStruct.h
/// All Rights Reserved
//*****************************************************************************
  
/// Contains the struct SVTriggerInfoStruct 
//******************************************************************************

#pragma once
namespace SvTi
{
	struct SVTriggerInfoStruct 
	{
		SVTriggerInfoStruct();
		SVTriggerInfoStruct( const SVTriggerInfoStruct& p_rsvObject );
		virtual ~SVTriggerInfoStruct();

		const SVTriggerInfoStruct& operator=( const SVTriggerInfoStruct& p_rsvObject );

		void Reset();

		bool bValid;
		long lTriggerCount;
		bool m_ToggleState;
		double m_BeginProcess;
		double m_ToggleTimeStamp;
		double m_CallbackReceived;
		double m_PushedOutputs;
		double m_PreviousTrigger;
		boost::any m_Data;
	};
} //namespace SvTi
