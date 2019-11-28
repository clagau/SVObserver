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

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <fstream>
#include "SVDigitizerLoadLibraryClass.h"
#include "SVTriggerClass.h"
#include "TriggerDispatcher.h"
#include "SVIOLibrary/SVIOTriggerLoadLibraryClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "TriggerInformation/SVTriggerConstants.h"
#pragma endregion Includes

namespace SvTh
{
	HRESULT CALLBACK TriggerCallbackSvtc(const SvTh::TriggerParameters& rTriggerData)
	{
		HRESULT hrOk = S_OK;

		if ( nullptr != rTriggerData.m_pOwner)
		{
			SVOResponseClass response;

			const SVTriggerClass *pTrigger = reinterpret_cast<const SVTriggerClass*> (rTriggerData.m_pOwner);

			///This sets the m_StartTime value which is the fallback trigger time stamp if the time stamp is not in rTriggerData.m_Data
			response.reset();

			response.setIsValid(true);
			response.setIsComplete(true);
			response.setData(rTriggerData.m_Data);

			pTrigger->Notify( response );

			#ifdef SV_LOG_STATUS_INFO
				std::string LogEntry = SvUl::Format( _T( "FinishProcess %s - HR = 0x%X" ), pTrigger->GetDeviceName(), hr );

				pTrigger->m_StatusLog.push_back( LogEntry );
			#endif
		}

		return hrOk;
	}

	SVTriggerClass::SVTriggerClass(LPCTSTR deviceName)
	: SVODeviceClass(deviceName)
	{
		std::string name{deviceName};
		if (0 == name.compare(0, strlen(SvTi::SoftwareTriggerName), SvTi::SoftwareTriggerName))
		{
			m_type = SvDef::SoftwareTrigger;
		}
		else if (0 == name.compare(0, strlen(SvTi::CameraTriggerName), SvTi::CameraTriggerName))
		{
			m_type = SvDef::CameraTrigger;
		}
	}

	SVTriggerClass::~SVTriggerClass()
	{
	}

	void SVTriggerClass::addAcquisitionTrigger(SVDigitizerLoadLibraryClass* pDllDigitizer, unsigned long triggerChannel)
	{
		AcquisitionParameter acqParameter;
		acqParameter.m_pDllDigitizer = pDllDigitizer;
		acqParameter.m_triggerChannel = triggerChannel;
		auto it = std::find(m_acqTriggerParameters.begin(), m_acqTriggerParameters.end(), acqParameter);
		if(it == m_acqTriggerParameters.end())
		{
			m_acqTriggerParameters.emplace_back(acqParameter);
		}
	}

	void SVTriggerClass::clearAcquisitionTriggers()
	{
		m_acqTriggerParameters.clear();
	}
	
	void SVTriggerClass::enableInternalTrigger() const
	{
		///When software trigger then we need to enable the internal trigger
		for (const auto& rAcquisitionParameter : m_acqTriggerParameters)
		{
			if (nullptr != rAcquisitionParameter.m_pDllDigitizer)
			{
				rAcquisitionParameter.m_pDllDigitizer->InternalTriggerEnable(rAcquisitionParameter.m_triggerChannel);
			}
		}
	}

	HRESULT SVTriggerClass::RegisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller )
	{
		TriggerDispatcher dispatcher(TriggerCallbackSvtc, TriggerParameters(this));

		HRESULT l_hrOk = SVODeviceClass::RegisterCallback( pCallback, pvOwner, pvCaller );

		if( nullptr != m_pDLLTrigger )
		{
			if ( S_OK == l_hrOk )
			{
				l_hrOk = m_pDLLTrigger->Register( m_triggerChannel, dispatcher );
			}

			if ( S_OK != l_hrOk )
			{
				m_pDLLTrigger->Unregister( m_triggerChannel, dispatcher );
			}
		}
		else
		{
			if( S_OK == l_hrOk )
			{
				l_hrOk = E_FAIL;
			}
		}

		return l_hrOk;
	}

	HRESULT SVTriggerClass::UnregisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller )
	{
		HRESULT l_hrOk = S_OK;

		if ( nullptr != m_pDLLTrigger )
		{
			TriggerDispatcher dispatcher(TriggerCallbackSvtc, TriggerParameters(this));

			l_hrOk = m_pDLLTrigger->Unregister( m_triggerChannel, dispatcher );
		}
		else
		{
			l_hrOk = E_FAIL;
		}

		HRESULT l_Temp = SVODeviceClass::UnregisterCallback( pCallback, pvOwner, pvCaller );

		if( S_OK != l_Temp && S_OK == l_hrOk )
		{
			l_hrOk = l_Temp;
		}

		return l_hrOk;
	}

	HRESULT SVTriggerClass::UnregisterAllCallbacks()
	{
		HRESULT l_hrOk = SVODeviceClass::UnregisterAllCallbacks();

		if ( nullptr != m_pDLLTrigger )
		{
			HRESULT l_Temp = m_pDLLTrigger->UnregisterAll( m_triggerChannel );
		
			if( S_OK != l_Temp && S_OK == l_hrOk )
			{
				l_hrOk = l_Temp;
			}
		}
		else
		{
			if( S_OK == l_hrOk )
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

		if ( nullptr != m_pDLLTrigger )
		{
			l_hrOk = m_pDLLTrigger->Start( m_triggerChannel );
		}
		else
		{
			l_hrOk = E_FAIL;
		}

		if ( S_OK == l_hrOk )
		{
			l_hrOk = SVODeviceClass::Start();
		}

		return l_hrOk;
	}

	HRESULT SVTriggerClass::Stop()
	{
		HRESULT l_hrOk = SVODeviceClass::Stop();

		if ( nullptr != m_pDLLTrigger )
		{
			HRESULT l_Temp = m_pDLLTrigger->Stop( m_triggerChannel );

			if( S_OK != l_Temp && S_OK == l_hrOk )
			{
				l_hrOk = l_Temp;
			}
		}
  
		#ifdef SV_LOG_STATUS_INFO
		std::string FileName = SvUl::Format(_T("C:\\SVObserver\\%s.log"), GetDeviceName());

			std::fstream FileStream( FileName.c_str(), std::ios_base::trunc | std::ios_base::out );

			if( FileStream.is_open() )
			{
				for( auto const& rEntry : m_StatusLog )
				{
					FileStream << rEntry.c_str() << std::endl;
				}

				FileStream.close();

				m_StatusLog.clear();
			}
		#endif

		return l_hrOk;
	}

	HRESULT SVTriggerClass::processAcquisitionTriggers(const SVOResponseClass& rResponse) const
	{
		for(const auto& rAcquisitionParameter : m_acqTriggerParameters)
		{
			if(nullptr != rAcquisitionParameter.m_pDllDigitizer)
			{
				rAcquisitionParameter.m_pDllDigitizer->InternalTrigger(rAcquisitionParameter.m_triggerChannel);
			}
		}
		return S_OK;
	}

} //namespace SvTh
