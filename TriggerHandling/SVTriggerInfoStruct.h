//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file SVTriggerInfoStruct.h
/// All Rights Reserved
//*****************************************************************************
  
/// Contains the struct SVTriggerInfoStruct 
//******************************************************************************

namespace Seidenader { namespace TriggerHandling {
	struct SVTriggerInfoStruct 
	{
		SVTriggerInfoStruct();
		SVTriggerInfoStruct( const SVTriggerInfoStruct& p_rsvObject );
		virtual ~SVTriggerInfoStruct();

		const SVTriggerInfoStruct& operator=( const SVTriggerInfoStruct& p_rsvObject );

		void Reset();
		void Init();

		BOOL bValid;
		class SVTriggerObject* pTrigger;
		long lTriggerCount;
		bool m_ToggleState;
		SVClock::SVTimeStamp m_BeginProcess;
		SVClock::SVTimeStamp m_ToggleTimeStamp;
		SVClock::SVTimeStamp m_CallbackReceived;
		SVClock::SVTimeStamp m_PushedOutputs;
		SVClock::SVTimeStamp m_PreviousTrigger;
		boost::any m_Data;
	};
} /* namespace TriggerHandling */ } /* namespace Seidenader */

namespace SvTh = Seidenader::TriggerHandling;