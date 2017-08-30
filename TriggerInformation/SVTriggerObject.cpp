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

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <fstream>
#include "TriggerHandling/TriggerBasics.h"
#include "TriggerHandling/SVTriggerClass.h"
#include "SVTriggerObject.h"
#include "TriggerHandling/SVTriggerConstants.h"
#include "SVTriggerLibrary/SVTriggerEnums.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace SvTi
{
	HRESULT CALLBACK SVOTriggerObjectCallbackPtr( void *pvOwner, void *pvCaller, void *pvResponse )
	{
		HRESULT hrOk = S_OK;

		SVTriggerObject* pTrigger = reinterpret_cast< SVTriggerObject* >( pvOwner );	
		SVOResponseClass* pResponse = reinterpret_cast< SVOResponseClass* >( pvResponse );	

		//Only do an assert check so that in release mode no check is made
		assert( nullptr != pTrigger && nullptr != pResponse );

		pTrigger->FinishProcess( pResponse );

		return hrOk;
	}

	SVTriggerObject::SVTriggerObject( LPCSTR ObjectName )
	: SVObjectClass( ObjectName )
	, mpsvDevice(nullptr)
	, m_pFinishProc(nullptr)
	, m_pOwner(nullptr)
	, m_lTriggerCount(0)
	, m_bSoftwareTrigger(false)
	, m_timerPeriod(SvTh::TimerPeriod) 
	{
		#ifdef SV_LOG_STATUS_INFO
			m_StatusLog.Create();
		#endif
	}

	SVTriggerObject::SVTriggerObject( SVObjectClass* POwner, int StringResourceID )
	: SVObjectClass( POwner, StringResourceID )
	, mpsvDevice(nullptr)
	, m_pFinishProc(nullptr)
	, m_pOwner(nullptr)
	, m_lTriggerCount(0)
	, m_bSoftwareTrigger(false)
	, m_timerPeriod(SvTh::TimerPeriod) 
	{
		#ifdef SV_LOG_STATUS_INFO
			m_StatusLog.Create();
		#endif
	}

	SVTriggerObject::~SVTriggerObject()
	{
		Destroy();
		mpsvDevice = nullptr;

		m_pOwner = nullptr;
		m_pFinishProc = nullptr;
		m_lTriggerCount	= 0;
	}

	bool SVTriggerObject::Create( SvTh::SVTriggerClass *psvDevice )
	{
		bool bOk = true;

		if ( nullptr != psvDevice )
		{
			mpsvDevice = psvDevice;

			m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVTriggerObjectType;

			bOk = S_OK == psvDevice->Create();
		}
		else 
		{
			bOk = false;
		}
		return bOk;
	}

	bool SVTriggerObject::Destroy()
	{
		bool bOk = false;

		if ( nullptr != mpsvDevice )
		{
			bOk = S_OK == mpsvDevice->Destroy();

			mpsvDevice = nullptr;
		}
		return bOk;
	}

	bool SVTriggerObject::CanGoOnline()
	{
		return nullptr != mpsvDevice && mpsvDevice->IsValid();
	}

	bool SVTriggerObject::GoOnline()
	{
		bool bOk = true;

		#ifdef SV_LOG_STATUS_INFO
			m_StatusLog.clear();
		#endif

		if ( nullptr != mpsvDevice && ! mpsvDevice->IsStarted() )
		{
		  bOk = S_OK == mpsvDevice->RegisterCallback( SVOTriggerObjectCallbackPtr, this, mpsvDevice );
		}
		return bOk && nullptr != mpsvDevice && ( S_OK == mpsvDevice->Start() );
	}

	bool SVTriggerObject::GoOffline()
	{
	  bool bOk = false;

	  if ( nullptr != mpsvDevice )
	  {
		bool l_bIsStarted = mpsvDevice->IsStarted();
  
		bOk = S_OK == mpsvDevice->Stop();

		if ( l_bIsStarted )
		{
		  bOk = S_OK == mpsvDevice->UnregisterCallback( SVOTriggerObjectCallbackPtr, this, mpsvDevice ) && bOk;
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

	bool SVTriggerObject::RegisterFinishProcess( void *pOwner, LPSVFINISHPROC pFunc )
	{
		bool bOk = false;

		if ( nullptr != mpsvDevice )
		{
			bOk = true;

			m_pFinishProc	= pFunc;
			m_pOwner		= pOwner;
		}
		return bOk;
	}

	bool SVTriggerObject::UnregisterFinishProcess( void *pOwner )
	{
		bool bOk = ( m_pOwner == pOwner );

		if( bOk )
		{
			m_pFinishProc = nullptr;
			m_pOwner = nullptr;
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

		if (nullptr != mpsvDevice)
		{
			SvTh::SVTriggerClass* pTrigger = dynamic_cast<SvTh::SVTriggerClass *>(mpsvDevice);
			if (nullptr != pTrigger)
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

		if( setTimer && nullptr != mpsvDevice )
		{
			SVIOTriggerLoadLibraryClass* l_pLib = mpsvDevice->m_pDLLTrigger;

			if( nullptr != l_pLib )
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
			bRet = (0 == name.find(SvTh::CameraTriggerName) );
		}
		return bRet;
	}

	void SVTriggerObject::SetAcquisitionTriggered(bool bAcquisitionTriggered)
	{
		if (nullptr != mpsvDevice)
		{
			SVIOTriggerLoadLibraryClass* l_pLib = mpsvDevice->m_pDLLTrigger;

			if (nullptr != l_pLib)
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
} //namespace SvTi
