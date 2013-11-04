//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerClass
//* .File Name       : $Workfile:   SVTriggerClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:27:56  $
//******************************************************************************

#include "stdafx.h"
#include <fstream>
#include "SVTriggerClass.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"

HRESULT CALLBACK SVTriggerClass::SVTriggerCallback( void *p_pvOwner, void *p_pvData )
{
	HRESULT hrOk = S_OK;

	if ( p_pvOwner )
	{
		SVOResponseClass l_Response;

		SVTriggerClass *pDevice = (SVTriggerClass *)p_pvOwner;

		l_Response.Reset();
		l_Response.SetIsValid( TRUE );
		l_Response.SetIsComplete( TRUE );

		HRESULT hr = pDevice->Notify( l_Response );

		//TRACE( "Process Trigger Callback - %s\n", pDevice->GetDeviceName() );

		#ifdef SV_LOG_STATUS_INFO
			SVString l_String;

			l_String.Format( _T( "FinishProcess %s - HR = 0x%X" ), pDevice->GetDeviceName(), hr );

			pDevice->m_StatusLog.push_back( l_String );
		#endif
	}

	return hrOk;
}

SVTriggerClass::SVTriggerClass(LPCTSTR deviceName)
: SVODeviceClass(deviceName)
, m_pDLLTrigger(NULL)
, m_ulHandle(NULL)
, miChannelNumber(-1)
{
	#ifdef SV_LOG_STATUS_INFO
		m_StatusLog.Create();
	#endif
}

SVTriggerClass::~SVTriggerClass()
{
}

HRESULT SVTriggerClass::RegisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller )
{
	HRESULT l_hrOk = S_OK;

	SVCallbackStruct l_Callback;

	l_Callback.m_pCallback = SVTriggerClass::SVTriggerCallback;
	l_Callback.m_pOwner = this;
	l_Callback.m_pData = pvOwner;

	l_hrOk = SVODeviceClass::RegisterCallback( pCallback, pvOwner, pvCaller );

	if( m_pDLLTrigger != NULL )
	{
		if ( l_hrOk == S_OK )
		{
			l_hrOk = m_pDLLTrigger->Register( m_ulHandle, l_Callback );
		}

		if ( l_hrOk != S_OK )
		{
			m_pDLLTrigger->Unregister( m_ulHandle, l_Callback );
		}
	}
	else
	{
		if( l_hrOk == S_OK )
		{
			l_hrOk = E_FAIL;
		}
	}

	return l_hrOk;
}

HRESULT SVTriggerClass::UnregisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller )
{
	HRESULT l_hrOk = S_OK;

	if ( m_pDLLTrigger != NULL )
	{
		SVCallbackStruct l_Callback;

		l_Callback.m_pCallback = SVTriggerCallback;
		l_Callback.m_pOwner = this;
		l_Callback.m_pData = pvOwner;

		l_hrOk = m_pDLLTrigger->Unregister( m_ulHandle, l_Callback );
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	HRESULT l_Temp = SVODeviceClass::UnregisterCallback( pCallback, pvOwner, pvCaller );

	if( l_Temp != S_OK && l_hrOk == S_OK )
	{
		l_hrOk = l_Temp;
	}

	return l_hrOk;
}

HRESULT SVTriggerClass::UnregisterAllCallbacks()
{
	HRESULT l_hrOk = SVODeviceClass::UnregisterAllCallbacks();

	if ( m_pDLLTrigger != NULL )
	{
		HRESULT l_Temp = m_pDLLTrigger->UnregisterAll( m_ulHandle );
		
		if( l_Temp != S_OK && l_hrOk == S_OK )
		{
			l_hrOk = l_Temp;
		}
	}
	else
	{
		if( l_hrOk == S_OK )
		{
			l_hrOk = E_FAIL;
		}
	}

	return l_hrOk;
}

HRESULT SVTriggerClass::Start()
{
	HRESULT l_hrOk = S_FALSE;

	#ifdef SV_LOG_STATUS_INFO
		m_StatusLog.clear();
	#endif

	if ( m_pDLLTrigger != NULL )
	{
		l_hrOk = m_pDLLTrigger->Start( m_ulHandle );
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	if ( l_hrOk == S_OK )
	{
		l_hrOk = SVODeviceClass::Start();
	}

	return l_hrOk;
}

HRESULT SVTriggerClass::Stop()
{
	HRESULT l_hrOk = SVODeviceClass::Stop();

	if ( m_pDLLTrigger != NULL )
	{
		HRESULT l_Temp = m_pDLLTrigger->Stop( m_ulHandle );

		if( l_Temp != S_OK && l_hrOk == S_OK )
		{
			l_hrOk = l_Temp;
		}
	}
  
	#ifdef SV_LOG_STATUS_INFO
		SVString l_FileName;

		l_FileName.Format( _T( "C:\\SVObserver\\%s.log" ), GetDeviceName() );

		std::fstream l_Stream( l_FileName.ToString(), std::ios_base::trunc | std::ios_base::out );

		if( l_Stream.is_open() )
		{
			for( int i = 0; i < m_StatusLog.GetCount(); ++i )
			{
				SVString l_String;

				m_StatusLog.GetAt( i, &l_String );

				l_Stream << l_String.ToString() << std::endl;
			}

			l_Stream.close();

			m_StatusLog.clear();
		}
	#endif

	return l_hrOk;
}

HRESULT SVTriggerClass::EnableInternalTrigger()
{
	return E_NOTIMPL;
}

//******************************************************************************

//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTriggerClass.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 12:27:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   16 Jan 2013 16:46:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated fucntioality associated with triggers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   16 Sep 2011 16:23:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with new device and resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   14 Jul 2011 16:19:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove un-used product info struct attribute and inspection attribute.  Added logging attribute to product info struct for product state.  Remove extra Inspection Complete notification.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   15 Feb 2010 12:42:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 Jan 2010 15:56:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.  Updated inspection queueing functionality to handle floundering inspections.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Dec 2009 13:22:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Jul 2009 13:09:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Sep 2008 17:00:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised constructor to require name string
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4.1.0   14 Jul 2009 15:27:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code with new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Oct 2003 13:04:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated exposed function prototypes to change parameters from a class to individual items.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Sep 2003 08:48:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Changed the type of the variable that interfaces with the hardware from SVIOConfigurationInterfaceClass to SVIOTriggerLoadLibraryClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Jul 2003 08:59:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added the SVTriggerCallback function, RegisterCallback method, UnregisterCallback method, Start method, and Stop method to use the new I/O configuration interface to manage the Trigger Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 17:07:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:41:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
