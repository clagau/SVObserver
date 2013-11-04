//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerClass
//* .File Name       : $Workfile:   SVSoftwareTriggerClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:08:32  $
//******************************************************************************

#include "stdafx.h"
#include "SVSoftwareTriggerClass.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"

SVSoftwareTriggerClass::SVSoftwareTriggerClass(LPCTSTR deviceName)
: SVTriggerClass(deviceName)
{
}

SVSoftwareTriggerClass::~SVSoftwareTriggerClass()
{
}

HRESULT CALLBACK SVSoftwareTriggerClass::TriggerCallback( void *p_pvOwner, void *p_pvData )
{
	HRESULT hrOk = S_OK;

	if ( p_pvOwner )
	{
		try
		{
			SVSoftwareTriggerClass* pDevice = (SVSoftwareTriggerClass *)p_pvOwner;
			hrOk = pDevice->FireAcquisitionTrigger();
		}
		catch ( ... )
		{
		}
	}
	return hrOk;
}

HRESULT CALLBACK SVSoftwareTriggerClass::TriggerCompleteCallback( void *p_pvOwner, void *p_pvData )
{
	HRESULT hrOk = S_OK;

	if ( p_pvOwner )
	{
		try
		{
			SVSoftwareTriggerClass* pDevice = (SVSoftwareTriggerClass *)p_pvOwner;

			SVOResponseClass l_Response;

			l_Response.Reset();
			l_Response.SetIsValid( TRUE );
			l_Response.SetIsComplete( TRUE );

			pDevice->Notify( l_Response );
		}
		catch ( ... )
		{
		}
	}
	return hrOk;
}

HRESULT SVSoftwareTriggerClass::RegisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller )
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = SVODeviceClass::RegisterCallback( pCallback, pvOwner, pvCaller );

	if ( m_pDLLTrigger != NULL )
	{
		SVCallbackStruct localCallback;
		localCallback.m_pCallback = SVSoftwareTriggerClass::TriggerCallback;
		localCallback.m_pOwner = this;
		localCallback.m_pData = pvOwner;

		SVCallbackStruct l_Callback;
		l_Callback.m_pCallback = SVSoftwareTriggerClass::TriggerCompleteCallback;
		l_Callback.m_pOwner = this;
		l_Callback.m_pData = pvOwner;

		if ( l_hrOk == S_OK )
		{
			l_hrOk = m_pDLLTrigger->Register( m_ulHandle, localCallback );
		}

		if ( l_hrOk == S_OK )
		{
			l_hrOk = m_acquisitionInitiator.RegisterCallback( l_Callback );
		}

		if ( l_hrOk != S_OK )
		{
			m_pDLLTrigger->Unregister( m_ulHandle, localCallback );
			m_acquisitionInitiator.UnRegisterCallback(l_Callback);
		}
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVSoftwareTriggerClass::UnregisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller )
{
	HRESULT l_hrOk = S_OK;

	if ( SVODeviceClass::UnregisterCallback( pCallback, pvOwner, pvCaller ) != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( m_pDLLTrigger != NULL )
	{
		SVCallbackStruct localCallback;

		localCallback.m_pCallback = SVSoftwareTriggerClass::TriggerCallback;
		localCallback.m_pOwner = this;
		localCallback.m_pData = pvOwner;

		SVCallbackStruct l_Callback;
		l_Callback.m_pCallback = SVSoftwareTriggerClass::TriggerCompleteCallback;
		l_Callback.m_pOwner = this;
		l_Callback.m_pData = pvOwner;

		if ( m_pDLLTrigger->Unregister( m_ulHandle, localCallback ) != S_OK )
		{
			l_hrOk = S_FALSE;
		}
		m_acquisitionInitiator.UnRegisterCallback(l_Callback);
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

void SVSoftwareTriggerClass::RegisterAcquistionInitiator( SVAcquisitionInitiator& rFunc)
{
	m_acquisitionInitiator = rFunc;
}

HRESULT SVSoftwareTriggerClass::EnableInternalTrigger()
{
	return m_acquisitionInitiator.EnableInternalTrigger();
}

HRESULT SVSoftwareTriggerClass::FireAcquisitionTrigger()
{
	return m_acquisitionInitiator.Exec();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVSoftwareTriggerClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:08:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Feb 2010 12:42:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2009 12:54:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 12:27:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 11:19:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
