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
#include "SVTriggerObject.h"
#include "SVTriggerConstants.h"
#include "SVTriggerInfoStruct.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"
#include "TriggerHandling/SVTriggerClass.h"
#include "TriggerHandling/TriggerDispatcher.h"
#include "SVUtilityLibrary/StringHelper.h"
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
	, m_timerPeriod(TimerPeriod) 
	{
	}

	SVTriggerObject::SVTriggerObject( SVObjectClass* POwner, int StringResourceID )
	: SVObjectClass( POwner, StringResourceID )
	, m_timerPeriod(TimerPeriod) 
	{
	}

	SVTriggerObject::~SVTriggerObject()
	{
		Destroy();
	}

	bool SVTriggerObject::Create( SvTh::SVTriggerClass* pTrigger )
	{
		bool bOk = true;

		if ( nullptr != pTrigger )
		{
			m_pTriggerDevice = pTrigger;

			m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVTriggerObjectType;

			bOk = S_OK == m_pTriggerDevice->Create();
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

		if ( nullptr != m_pTriggerDevice )
		{
			bOk = S_OK == m_pTriggerDevice->Destroy();

			m_pTriggerDevice = nullptr;
		}
		return bOk;
	}

	bool SVTriggerObject::CanGoOnline()
	{
		///This needs to be done before the cameras are started
		if (nullptr != m_pTriggerDevice)
		{
			if (SvDef::TriggerType::SoftwareTrigger == getType())
			{
				m_pTriggerDevice->enableInternalTrigger();
			}
		}
		return nullptr != m_pTriggerDevice && m_pTriggerDevice->IsValid();
	}

	bool SVTriggerObject::GoOnline()
	{
		bool bOk = true;

		#ifdef SV_LOG_STATUS_INFO
			m_StatusLog.clear();
		#endif

		if ( nullptr != m_pTriggerDevice && !m_pTriggerDevice->IsRegistered())
		{
			bOk = S_OK == m_pTriggerDevice->RegisterCallback( SVOTriggerObjectCallbackPtr, this, m_pTriggerDevice );
		}
		return bOk;
	}

	bool SVTriggerObject::GoOffline()
	{
	  bool bOk = false;

	  if ( nullptr != m_pTriggerDevice )
	  {

		bOk = S_OK == m_pTriggerDevice->Stop();

		if ( m_pTriggerDevice->IsRegistered() )
		{
		  bOk = S_OK == m_pTriggerDevice->UnregisterCallback( SVOTriggerObjectCallbackPtr, this, m_pTriggerDevice ) && bOk;
		}
	  }
  
		#ifdef SV_LOG_STATUS_INFO
			std::string FileName = SvUl::Format( _T( "C:\\SVObserver\\%s.log" ), GetName() );

			std::fstream l_Stream( FileName.c_str(), std::ios_base::trunc | std::ios_base::out );

			if( l_Stream.is_open() )
			{
				for( auto const & rEntry : m_StatusLog )
				{
					l_Stream << rEntry.c_str() << std::endl;
				}

				l_Stream.close();

				m_StatusLog.clear();
			}
		#endif

	  return bOk;  
	}// end GoOffline

	bool SVTriggerObject::Start()
	{ 
		return (nullptr != m_pTriggerDevice) && (S_OK == m_pTriggerDevice->Start());
	}

	bool SVTriggerObject::RegisterFinishProcess( void *pOwner, LPSVFINISHPROC pFunc )
	{
		bool bOk = false;

		if ( nullptr != m_pTriggerDevice )
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
			SVTriggerInfoStruct triggerInfo;
			triggerInfo.m_Data = pResponse->getData();

			//If in the input data it has a valid time stamp value then it is more accurate then use it
			SvTh::IntVariantMap::const_iterator iterData {triggerInfo.m_Data.end()};
			iterData = triggerInfo.m_Data.find(SvTh::TriggerDataEnum::TimeStamp);
			if (triggerInfo.m_Data.end() != iterData && 0.0 < static_cast<double> (iterData->second))
			{
				triggerInfo.m_triggerTimeStamp = static_cast<double> (iterData->second);
			}
			else
			{
				///This is the fallback triiger time stamp
				triggerInfo.m_triggerTimeStamp = pResponse->getStartTime();
			}

			triggerInfo.lTriggerCount = ++m_triggerCount;
			triggerInfo.bValid = pResponse->isValid();

			if ( m_pFinishProc )
			{
				(m_pFinishProc)( m_pOwner, this, &triggerInfo );
			}
		}

		#ifdef SV_LOG_STATUS_INFO
			std::string LogEntry = SvUl::Format( _T( "FinishProcess %s - TC = %d" ), GetName(), m_triggerCount );

			m_StatusLog.push_back(LogEntry);
		#endif
	}

	void SVTriggerObject::Fire(double timeStamp)
	{
		SVOResponseClass response;

		 response.setStartTime(timeStamp);
		 response.setIsValid(true);

		 FinishProcess(&response);
	}

	long SVTriggerObject::GetSoftwareTriggerPeriod() const
	{
		return m_timerPeriod;
	}

	void SVTriggerObject::SetSoftwareTriggerPeriod(long period, bool setTimer)
	{
		m_timerPeriod = period;

		if( setTimer && nullptr != m_pTriggerDevice )
		{
			SVIOTriggerLoadLibraryClass* pDllTrigger = m_pTriggerDevice->getDLLTrigger();

			if( nullptr != pDllTrigger )
			{
				unsigned long triggerHandle = 0;
				pDllTrigger->GetHandle(&triggerHandle, m_pTriggerDevice->getDigitizerNumber());
				_variant_t var;
				var.vt = VT_I4;
				var.intVal = period;
				pDllTrigger->SetParameterValue(triggerHandle, 0, &var);
			}
		}
	}

	SvDef::TriggerType SVTriggerObject::getType() const
	{
		SvDef::TriggerType result{SvDef::TriggerType::HardwareTrigger};
		if(nullptr != m_pTriggerDevice)
		{
			result = m_pTriggerDevice->getType();
		}
		
		return result;
	}
} //namespace SvTi
