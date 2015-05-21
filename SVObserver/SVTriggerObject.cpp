//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerObject
//* .File Name       : $Workfile:   SVTriggerObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:44:14  $
//******************************************************************************

#include "stdafx.h"
#include <fstream>
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"
#include "SVTriggerLibrary/SVTriggerEnums.h"
#include "SVTriggerObject.h"
#include "SVTriggerClass.h"
#include "SVSoftwareTriggerDefaults.h"
#include "SVTriggerConstants.h"

HRESULT CALLBACK SVOTriggerObjectCallbackPtr( void *pvOwner, void *pvCaller, void *pvResponse )
{
	HRESULT hrOk = S_OK;

	SVTriggerObject* pTrigger = reinterpret_cast< SVTriggerObject* >( pvOwner );	
	SVOResponseClass* pResponse = reinterpret_cast< SVOResponseClass* >( pvResponse );	

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != pTrigger && nullptr != pResponse );

	pTrigger->FinishProcess( pResponse );

	return hrOk;
}

SVTriggerObject::SVTriggerObject( LPCSTR ObjectName )
: SVObjectClass( ObjectName )
, mpsvDevice(NULL)
, m_pFinishProc(NULL)
, m_pOwner(NULL)
, m_lTriggerCount(0)
, m_bSoftwareTrigger(false)
, m_timerPeriod(SVSoftwareTriggerDefaults::TimerPeriod) 
{
	#ifdef SV_LOG_STATUS_INFO
		m_StatusLog.Create();
	#endif
}

SVTriggerObject::SVTriggerObject( SVObjectClass* POwner, int StringResourceID )
: SVObjectClass( POwner, StringResourceID )
, mpsvDevice(NULL)
, m_pFinishProc(NULL)
, m_pOwner(NULL)
, m_lTriggerCount(0)
, m_bSoftwareTrigger(false)
, m_timerPeriod(SVSoftwareTriggerDefaults::TimerPeriod) 
{
	#ifdef SV_LOG_STATUS_INFO
		m_StatusLog.Create();
	#endif
}

SVTriggerObject::~SVTriggerObject()
{
	Destroy();
	mpsvDevice = NULL;

	m_pOwner = NULL;
	m_pFinishProc	= NULL;
	m_lTriggerCount	= 0;
}

BOOL SVTriggerObject::Create( SVTriggerClass *psvDevice )
{
	BOOL bOk = TRUE;

	if ( psvDevice != NULL )
	{
		mpsvDevice = psvDevice;

		outObjectInfo.ObjectTypeInfo.ObjectType = SVTriggerObjectType;

		bOk = psvDevice->Create() == S_OK;
	}
    else 
    {
        bOk = FALSE;
    }
	return bOk;
}

BOOL SVTriggerObject::Destroy()
{
	BOOL bOk = FALSE;

	if ( mpsvDevice != NULL )
	{
		bOk = mpsvDevice->Destroy() == S_OK;

		mpsvDevice = NULL;
	}
	return bOk;
}

BOOL SVTriggerObject::CanGoOnline()
{
	return mpsvDevice != NULL && mpsvDevice->IsValid();
}

BOOL SVTriggerObject::GoOnline()
{
    BOOL bOk = TRUE;

	#ifdef SV_LOG_STATUS_INFO
		m_StatusLog.clear();
	#endif

    if ( mpsvDevice != NULL && ! mpsvDevice->IsStarted() )
    {
      bOk = mpsvDevice->RegisterCallback( SVOTriggerObjectCallbackPtr, this, mpsvDevice ) == S_OK;
    }
	return bOk && mpsvDevice != NULL && ( mpsvDevice->Start() == S_OK );
}

BOOL SVTriggerObject::GoOffline()
{
  BOOL bOk = FALSE;

  if ( mpsvDevice != NULL )
  {
    bool l_bIsStarted = mpsvDevice->IsStarted();
  
    bOk = mpsvDevice->Stop() == S_OK;

    if ( l_bIsStarted )
    {
      bOk = mpsvDevice->UnregisterCallback( SVOTriggerObjectCallbackPtr, this, mpsvDevice )== S_OK && bOk;
    }
  }
  
	#ifdef SV_LOG_STATUS_INFO
		SVString l_FileName;

		l_FileName.Format( _T( "C:\\SVObserver\\%s.log" ), GetName() );

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

  return bOk;  
}// end GoOffline

BOOL SVTriggerObject::RegisterFinishProcess( void *pOwner, LPSVFINISHPROC pFunc )
{
	BOOL bOk = FALSE;

	if ( mpsvDevice != NULL )
	{
		bOk = TRUE;

		m_pFinishProc	= pFunc;
		m_pOwner		= pOwner;
	}
	return bOk;
}

BOOL SVTriggerObject::UnregisterFinishProcess( void *pOwner )
{
	BOOL bOk = ( m_pOwner == pOwner );

	if( bOk )
	{
		m_pFinishProc = NULL;
		m_pOwner = NULL;
	}
	return bOk;
}

void SVTriggerObject::FinishProcess(SVOResponseClass *pResponse)
{
	if ( pResponse )
	{
		SVTriggerInfoStruct l_Info;
		pResponse->GetExtraData(l_Info.m_Data);

		pResponse->GetStartTick( l_Info.m_BeginProcess );

		l_Info.lTriggerCount = ++m_lTriggerCount;
		l_Info.bValid = pResponse->IsValid();
		l_Info.pTrigger = this;

		if ( m_pFinishProc )
		{
			(m_pFinishProc)( m_pOwner, this, &l_Info );
		}
	}

	#ifdef SV_LOG_STATUS_INFO
		SVString l_String;

		l_String.Format( _T( "FinishProcess %s - TC = %d" ), GetName(), m_lTriggerCount );

		m_StatusLog.push_back( l_String );
	#endif
}

HRESULT SVTriggerObject::EnableInternalTrigger()
{
	HRESULT hr = S_FALSE;

	if (mpsvDevice != NULL)
	{
		SVTriggerClass* pTrigger = dynamic_cast<SVTriggerClass *>(mpsvDevice);
		if (pTrigger != NULL)
		{
			hr = pTrigger->EnableInternalTrigger();
		}
	}
	return hr;
}

bool SVTriggerObject::IsSoftwareTrigger() const
{
	return m_bSoftwareTrigger;
}

void SVTriggerObject::SetSoftwareTrigger(bool bSoftwareTrigger)
{
	m_bSoftwareTrigger = bSoftwareTrigger;
}

long SVTriggerObject::GetSoftwareTriggerPeriod() const
{
	return m_timerPeriod;
}

void SVTriggerObject::SetSoftwareTriggerPeriod(long period, bool setTimer)
{
	m_timerPeriod = period;

	if( setTimer && mpsvDevice != NULL )
	{
		SVIOTriggerLoadLibraryClass* l_pLib = mpsvDevice->m_pDLLTrigger;

		if( l_pLib != NULL )
		{
			unsigned long triggerHandle = 0;
			l_pLib->GetHandle(&triggerHandle, mpsvDevice->miChannelNumber);
			_variant_t var;
			var.vt = VT_I4;
			var.intVal = period;
			l_pLib->SetParameterValue(triggerHandle, 0, &var);
		}
	}
}

bool SVTriggerObject::IsAcquisitionTrigger() const
{
	bool bRet = false;
	if (mpsvDevice)
	{
		SVString name = mpsvDevice->GetDeviceName();
		bRet = (name.find(SV_CAMERA_TRIGGER_NAME) == 0);
	}
	return bRet;
}

void SVTriggerObject::SetAcquisitionTriggered(bool bAcquisitionTriggered)
{
	if (mpsvDevice != NULL)
	{
		SVIOTriggerLoadLibraryClass* l_pLib = mpsvDevice->m_pDLLTrigger;

		if (l_pLib != NULL)
		{
			unsigned long triggerHandle = 0;
			l_pLib->GetHandle(&triggerHandle, mpsvDevice->miChannelNumber);
			_variant_t var;
			var.vt = VT_BOOL;
			var.boolVal = (bAcquisitionTriggered) ? VARIANT_TRUE : VARIANT_FALSE;
			l_pLib->SetParameterValue(triggerHandle, SVAcquisitionTriggered, &var);
		}
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTriggerObject.cpp_v  $
 * 
 *    Rev 1.1   08 May 2013 16:44:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   29 Apr 2013 14:33:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 12:29:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   21 Jan 2013 11:23:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added IsAcquisitionTrigger method
 * Added SetAcquisitionTriggered method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   16 Jan 2013 16:46:14   jspila
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
 *    Rev 1.16   03 Oct 2011 10:47:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix a unregister problem when changing PPQ connections and update inspection when adding new image to image view.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   15 Feb 2010 12:42:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   19 Jan 2010 15:56:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.  Updated inspection queueing functionality to handle floundering inspections.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   30 Jul 2009 13:09:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   23 Jun 2009 15:04:14   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  654
 * SCR Title:  Software Trigger Runtime Adjustments
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Changed SetSoftwareTriggerPeriod to allow for setting the timer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   06 Feb 2009 08:49:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Constructors to initialize m_pOwner to NULL to correct a problem with File Acquisition when triggers aren't attached to the PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   17 Oct 2008 10:34:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added EnableInternalTrigger method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   03 Oct 2008 17:30:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Changed Frequency to Period for the Software Timer Trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   10 Sep 2008 17:01:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods and member variables for Software Trigger
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   31 Oct 2007 08:27:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added destroy to destructor to clean up resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Jun 2007 15:03:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5.1.1   14 Jul 2009 15:29:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code with new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5.1.0   30 Oct 2007 10:48:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added a call to destroy in destructor to free resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Nov 2004 15:26:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  459
 * SCR Title:  Fix a problem with inspection lock-up
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added logging
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Jul 2003 08:53:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated Create, Destroy, CanGoOnline, and GoOnline methods to use new SVODevice menthods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 17:07:46   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Feb 2003 10:42:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated GoOffline and GoOnline methods to fix issues with ViperRGB trigger.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Jan 2003 09:23:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed the following methods.  Create()  took out the RegisterCallback and moved it to the GoOnline() method.  Destroy() took out UnRegisterCallback() and moved it to the GoOffline() method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:42:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226, 272
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
