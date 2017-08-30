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
		void Init();

		bool bValid;
		class SVTriggerObject* pTrigger;
		long lTriggerCount;
		bool m_ToggleState;
		SvTl::SVTimeStamp m_BeginProcess;
		SvTl::SVTimeStamp m_ToggleTimeStamp;
		SvTl::SVTimeStamp m_CallbackReceived;
		SvTl::SVTimeStamp m_PushedOutputs;
		SvTl::SVTimeStamp m_PreviousTrigger;
		boost::any m_Data;
	};
} //namespace SvTi
