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
#include "SVHardwareManifest.h"
#include "TriggerHandling/SVTriggerClass.h"
#include "SVSoftwareTriggerClass.h"
#include "TriggerInformation/SVCameraTriggerClass.h"
#include "SVTriggerConstants.h"
#pragma endregion Includes

static int i = 0;

namespace SvTi
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
			SvTh::SVTriggerClass* l_pTrigger = nullptr;

			if( 0 == l_Iter->m_Name.find( SoftwareTriggerName ) )
			{
				l_pTrigger = new SVSoftwareTriggerClass( l_Iter->m_Name.c_str() );
			}
			else if( 0 == l_Iter->m_Name.find( _T( "IO_Board" ) ) )
			{
				l_pTrigger = new SvTh::SVTriggerClass( l_Iter->m_Name.c_str() );
			}
			else if( 0 == l_Iter->m_Name.find( CameraTriggerName ) )
			{
				l_pTrigger = new SVCameraTriggerClass( l_Iter->m_Name.c_str() );
			}

			if( nullptr != l_pTrigger )
			{
				l_pTrigger->miChannelNumber = l_Iter->m_Channel;

				m_Triggers[ l_Iter->m_Name ] = l_pTrigger;
			}

			++l_Iter;
		}
	}

	void SVTriggerProcessingClass::Shutdown()
	{
		clear();

		if( !( m_Triggers.empty() ) )
		{
			SVNameTriggerMap::iterator l_Iter = m_Triggers.begin();

			while( l_Iter != m_Triggers.end() )
			{
				SvTh::SVTriggerClass* l_pTrigger = l_Iter->second;

				if( nullptr != l_pTrigger )
				{
					delete l_pTrigger;
				}

				l_Iter = m_Triggers.erase( l_Iter );
			}
		}
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
					l_Iter->second->m_pDLLTrigger = nullptr;
					l_Iter->second->m_triggerchannel = 0;
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
		HRESULT l_hrOk = S_OK;

		if (nullptr != p_pDLLTrigger )
		{
			unsigned long l_ulSize = 0;

			l_hrOk = p_pDLLTrigger->GetCount( &l_ulSize );

			for ( unsigned long i = 0; S_OK == l_hrOk && i < l_ulSize; i++ )
			{
				SVString l_Name;

				unsigned long triggerchannel = 0;

				l_hrOk = p_pDLLTrigger->GetHandle( &triggerchannel, i );

				if ( S_OK == l_hrOk )
				{
					BSTR l_bstrName = nullptr;

					l_hrOk = p_pDLLTrigger->GetName( triggerchannel, &l_bstrName );

					if( S_OK == l_hrOk )
					{
						l_Name = SvUl_SF::createSVString(l_bstrName);

						if ( nullptr != l_bstrName )
						{
							::SysFreeString( l_bstrName );

							l_bstrName = nullptr;
						}
					}
				}
				else
				{
					l_hrOk = S_FALSE;
				}

				if( S_OK == l_hrOk )
				{
					l_hrOk = AddTrigger( l_Name.c_str(), p_pDLLTrigger, triggerchannel );
				}
			}
		}
		else
		{
			l_hrOk = S_FALSE;
		}

		return l_hrOk;
	}

	SvTh::SVTriggerClass* SVTriggerProcessingClass::GetTrigger( LPCTSTR szName ) const
	{
		SvTh::SVTriggerClass* l_pTrigger = nullptr;

		SVNameTriggerMap::const_iterator l_Iter = m_Triggers.find( szName );

		if( l_Iter != m_Triggers.end() )
		{
			l_pTrigger = l_Iter->second;
		}

		return l_pTrigger;
	}

	HRESULT SVTriggerProcessingClass::AddTrigger( LPCTSTR p_szName, SVIOTriggerLoadLibraryClass* p_pTriggerSubsystem, unsigned long p_Handle )
	{
		HRESULT l_Status = S_OK;

		SVNameTriggerMap::iterator l_Iter = m_Triggers.find( p_szName );

		if( l_Iter != m_Triggers.end() && nullptr != l_Iter->second )
		{
			if( l_Iter->second->m_pDLLTrigger != p_pTriggerSubsystem )
			{
				l_Iter->second->m_pDLLTrigger = p_pTriggerSubsystem;
				l_Iter->second->m_triggerchannel = p_Handle;

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
} //namespace SvTi