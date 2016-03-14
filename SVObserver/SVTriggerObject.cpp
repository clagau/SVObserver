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

