//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerRelayClass
//* .File Name       : $Workfile:   SVTriggerRelayClass.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:52:06  $
//******************************************************************************

template<typename TriggerHandler>
SVTriggerRelayClass<TriggerHandler>::SVTriggerRelayClass()
: m_pTriggerDLL(0), m_ulTriggerHandle(0)
{
}

template<typename TriggerHandler>
SVTriggerRelayClass<TriggerHandler>::~SVTriggerRelayClass()
{
}

template<typename TriggerHandler>
HRESULT SVTriggerRelayClass<TriggerHandler>::RegisterTriggerRelay( SVIOTriggerLoadLibraryClass* pTriggerDLL, unsigned long triggerchannel, TriggerHandler& rTriggerHandler)
{
	m_ulTriggerHandle = triggerchannel;
	m_pTriggerDLL = pTriggerDLL;
	m_triggerHandler = rTriggerHandler;

	SvTh::TriggerDispatcher dispatcher(TriggerHandler::TriggerCallback, SvTh::TriggerParameters(&m_triggerHandler));
	
	HRESULT l_hr = m_pTriggerDLL->Register( m_ulTriggerHandle, dispatcher );
	return l_hr;
}

template<typename TriggerHandler>
HRESULT SVTriggerRelayClass<TriggerHandler>::UnregisterTriggerRelay()
{
	HRESULT l_hr = S_OK;
	if (m_pTriggerDLL && m_ulTriggerHandle > 0)
	{
		SvTh::TriggerDispatcher dispatcher(TriggerHandler::TriggerCallback, SvTh::TriggerParameters(&m_triggerHandler));

		l_hr = m_pTriggerDLL->Unregister( m_ulTriggerHandle, dispatcher );
	}
	return l_hr;
}

