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
HRESULT SVTriggerRelayClass<TriggerHandler>::RegisterTriggerRelay( SVIOTriggerLoadLibraryClass* pTriggerDLL, unsigned long ulHandle, TriggerHandler& rTriggerHandler)
{
	m_ulTriggerHandle = ulHandle;
	m_pTriggerDLL = pTriggerDLL;
	m_triggerHandler = rTriggerHandler;

	SVCallbackStruct callback;
	callback.m_pCallback = TriggerHandler::TriggerCallback;
	callback.m_pOwner = &m_triggerHandler;
	callback.m_pData = NULL;
	
	HRESULT l_hr = m_pTriggerDLL->Register( m_ulTriggerHandle, callback );
	return l_hr;
}

template<typename TriggerHandler>
HRESULT SVTriggerRelayClass<TriggerHandler>::UnregisterTriggerRelay()
{
	HRESULT l_hr = S_OK;
	if (m_pTriggerDLL && m_ulTriggerHandle > 0)
	{
		SVCallbackStruct callback;
		callback.m_pCallback = TriggerHandler::TriggerCallback;
		callback.m_pOwner = &m_triggerHandler;
		callback.m_pData = NULL;

		l_hr = m_pTriggerDLL->Unregister( m_ulTriggerHandle, callback );
	}
	return l_hr;
}

