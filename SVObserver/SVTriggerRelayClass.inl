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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTriggerRelayClass.inl_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:52:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Oct 2008 16:28:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
