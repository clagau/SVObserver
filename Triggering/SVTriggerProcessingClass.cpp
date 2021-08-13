//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerProcessingClass
//* .File Name       : $Workfile:   SVTriggerProcessingClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:29:36  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTriggerProcessingClass.h"
#include "SVOLibrary/SVHardwareManifest.h"
#include "Triggering/SVTriggerClass.h"
#include "SVIOTriggerLoadLibraryClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvTrig
{
	SVTriggerProcessingClass& SVTriggerProcessingClass::Instance()
	{
		static SVTriggerProcessingClass l_Object;

		return l_Object;
	}

	SVTriggerProcessingClass::SVTriggerProcessingClass()
	{
	}

	SVTriggerProcessingClass::~SVTriggerProcessingClass()
	{
		Shutdown();
	}

	void SVTriggerProcessingClass::Startup()
	{
		const SVTriggerDeviceParamsVector& l_rTriggerParams = SVHardwareManifest::Instance().GetTriggerDeviceParams();

		SVTriggerDeviceParamsVector::const_iterator l_Iter = l_rTriggerParams.begin();

		while( l_Iter != l_rTriggerParams.end() )
		{
			std::unique_ptr<SvTrig::SVTriggerClass> pTriggerDevice = std::make_unique<SvTrig::SVTriggerClass>(l_Iter->m_Name.c_str());

			if( nullptr != pTriggerDevice)
			{
				pTriggerDevice->setDigitizerNumber(l_Iter->m_Channel);
				m_Triggers[ l_Iter->m_Name ] = std::move(pTriggerDevice);
			}

			++l_Iter;
		}
	}

	void SVTriggerProcessingClass::Shutdown()
	{
		clear();

		m_Triggers.clear();
	}

	void SVTriggerProcessingClass::clear()
	{
		if( !( m_Triggers.empty() ) )
		{
			SVNameTriggerMap::iterator l_Iter = m_Triggers.begin();

			while( l_Iter != m_Triggers.end() )
			{
				if( nullptr != l_Iter->second )
				{
					l_Iter->second->setDLLTrigger(nullptr);
					l_Iter->second->setTriggerChannel(0);
				}
				++l_Iter;
			}
		}

		if( !( m_Subsystems.empty() ) )
		{
			m_Subsystems.clear();
		}

		if( !( m_TriggerSubsystems.empty() ) )
		{
			m_TriggerSubsystems.clear();
		}
	}

	HRESULT SVTriggerProcessingClass::UpdateTriggerSubsystem( SVIOTriggerLoadLibraryClass* p_pDLLTrigger )
	{
		HRESULT result{ S_OK };

		if (nullptr != p_pDLLTrigger )
		{
			unsigned long count = p_pDLLTrigger->GetCount();

			for ( unsigned long i = 0; S_OK == result && i < count; i++ )
			{
				std::string triggerName;

				unsigned long triggerchannel = p_pDLLTrigger->GetHandle(i);

				if (0 < triggerchannel)
				{
					_variant_t value = p_pDLLTrigger->GetName(triggerchannel);

					if(VT_BSTR == value.vt)
					{
						triggerName = SvUl::createStdString(value);
					}
				}
				else
				{
					result = E_FAIL;
				}

				if( S_OK == result )
				{
					result = AddTrigger(triggerName.c_str(), p_pDLLTrigger, triggerchannel);
				}
			}
		}
		else
		{
			result = E_FAIL;
		}

		return result;
	}

	SvTrig::SVTriggerClass* SVTriggerProcessingClass::GetTrigger( LPCTSTR szName ) const
	{
		SvTrig::SVTriggerClass* pTrigger{nullptr};

		SVNameTriggerMap::const_iterator l_Iter = m_Triggers.find( szName );

		if( l_Iter != m_Triggers.end() )
		{
			pTrigger = l_Iter->second.get();
		}

		return pTrigger;
	}

	HRESULT SVTriggerProcessingClass::AddTrigger( LPCTSTR p_szName, SVIOTriggerLoadLibraryClass* p_pTriggerSubsystem, unsigned long p_Handle )
	{
		HRESULT l_Status = S_OK;

		SVNameTriggerMap::iterator l_Iter = m_Triggers.find( p_szName );

		if( l_Iter != m_Triggers.end() && nullptr != l_Iter->second )
		{
			if( l_Iter->second->getDLLTrigger() != p_pTriggerSubsystem )
			{
				l_Iter->second->setDLLTrigger(p_pTriggerSubsystem);
				l_Iter->second->setTriggerChannel(p_Handle);

				SVNameTriggerSubsystemMap::iterator l_SubsystemIter = m_TriggerSubsystems.find( p_szName );

				if( l_SubsystemIter != m_TriggerSubsystems.end() )
				{
					l_SubsystemIter->second = p_pTriggerSubsystem;
				}
				else
				{
					m_TriggerSubsystems[ p_szName ] = p_pTriggerSubsystem;
				}
			}
		}
		else
		{
			l_Status = E_FAIL;
		}

		return l_Status;
	}
} //namespace SvTrig