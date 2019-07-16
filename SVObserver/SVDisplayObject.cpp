//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDisplayObject
//* .File Name       : $Workfile:   SVDisplayObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.8  $
//* .Check In Date   : $Date:   17 Dec 2014 07:15:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <limits>
#include "SVDisplayObject.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVObserver.h"
#include "SVLibrary/SVWinUtility.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVTimerLibrary/SVClock.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVDisplayObject::SVDisplayObject( LPCSTR ObjectName )
: SVObjectClass( ObjectName )
, m_PendingTrigger( -1 )
, m_CurrentTrigger( -1 )
, m_DisplayedTrigger( -1 )
, m_IPDocDisplayComplete( 1 )
, m_FrameRate( 10 )
, m_LastUpdateTime(std::numeric_limits<double>::max())
, m_pDoc(nullptr)
, m_hStartEvent(nullptr)
, m_hStopEvent(nullptr)
, m_hDisplayThread(nullptr)
{
	m_CriticalSectionCreated = ::InitializeCriticalSectionAndSpinCount( &m_CriticalSection, 5000 ) ? true : false;
}

SVDisplayObject::SVDisplayObject( SVObjectClass* POwner, int StringResourceID )
: SVObjectClass( POwner, StringResourceID )
, m_PendingTrigger( -1 )
, m_CurrentTrigger( -1 )
, m_DisplayedTrigger( -1 )
, m_IPDocDisplayComplete( 1 )
, m_FrameRate( 10 )
, m_pDoc(nullptr)
, m_hStartEvent(nullptr)
, m_hStopEvent(nullptr)
, m_hDisplayThread(nullptr)
{
	m_CriticalSectionCreated = ::InitializeCriticalSectionAndSpinCount( &m_CriticalSection, 5000 ) ? true : false;
}

SVDisplayObject::~SVDisplayObject()
{
	Destroy();

	m_CriticalSectionCreated = false;
	::DeleteCriticalSection( &m_CriticalSection );

	m_hStartEvent = nullptr;
	m_hStopEvent = nullptr;
	m_hDisplayThread = nullptr;
}

HRESULT SVDisplayObject::ObserverUpdate( const std::pair<SVInspectionInfoStruct, long>& p_rData )
{
	HRESULT l_Status = FinishInspection( p_rData );

	return l_Status;
}

HRESULT SVDisplayObject::ObserverUpdate( const SVInspectionNameUpdate& p_rData )
{
	HRESULT l_Status = S_OK;

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		l_pIPDoc->SetTitle( p_rData.m_InspectionName.c_str() );
	}

	return l_Status;
}

HRESULT SVDisplayObject::ObserverUpdate( const SVRemoveImages& p_rData )
{
	HRESULT l_Status = S_OK;

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		SVGuidSet::const_iterator l_Iter = p_rData.m_Images.begin();

		while( l_Iter != p_rData.m_Images.end() )
		{
			l_pIPDoc->RemoveImage( *l_Iter );

			++l_Iter;
		}
	}

	return l_Status;
}

HRESULT SVDisplayObject::ObserverUpdate( const SVRemoveValues& p_rData )
{
	HRESULT l_Status = S_OK;

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		l_pIPDoc->RebuildResultsList();
	}

	return l_Status;
}

HRESULT SVDisplayObject::ObserverUpdate( const SVRemoveSubjectStruct& p_rData )
{
	HRESULT l_Status = S_OK;

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		l_pIPDoc->CloseDocument();
	}

	return l_Status;
}

void SVDisplayObject::SetInspectionID( const SVGUID& p_rInspectionID, SVIPDoc* pDoc )
{
	m_pDoc = pDoc;

	if( !( m_InspectionID.empty() ) )
	{
		SVObjectManagerClass::Instance().DetachObserver( "SVInspectionProcess", m_InspectionID, GetUniqueObjectID() );
	}

	m_InspectionID = p_rInspectionID;

	if( !( m_InspectionID.empty() ) )
	{
		SVObjectManagerClass::Instance().AttachObserver( "SVInspectionProcess", m_InspectionID, GetUniqueObjectID() );
	}
}

SVIPDoc* SVDisplayObject::GetIPDoc() const
{
	return m_pDoc;
}

BOOL SVDisplayObject::Create()
{
	DWORD dwThreadID;

	m_hStartEvent = ::CreateEvent( nullptr, false, false, nullptr );
	if( !m_hStartEvent )
		return FALSE;

	m_hStopEvent = ::CreateEvent( nullptr, true, false, nullptr );
	if( !m_hStopEvent )
		return FALSE;

	m_hDisplayThread = ::CreateThread(nullptr, 0, SVDisplayThreadFunc, (LPVOID)this, 0, &dwThreadID );
	if( !m_hDisplayThread )
		return FALSE;
	SVThreadManager::Instance().Add( m_hDisplayThread,"Display", SVAffinityUser);

	return TRUE;
}// end Create

BOOL SVDisplayObject::Destroy()
{
	if( !( m_InspectionID.empty() ) )
	{
		SVObjectManagerClass::Instance().DetachObserver( "SVInspectionProcess", m_InspectionID, GetUniqueObjectID() );

		m_InspectionID.clear();
	}

	if (m_hStopEvent && m_hDisplayThread)
	{
		::SetEvent( m_hStopEvent );

		// wait for thread to exit
		DWORD exitCode = 0;
		do
		{
			SVYieldMessages();
			::Sleep(1);
			::GetExitCodeThread(m_hDisplayThread, &exitCode);
		} while (exitCode == STILL_ACTIVE);

		::CloseHandle( m_hStopEvent );
		m_hStopEvent = nullptr;

		::CloseHandle( m_hStartEvent );
		m_hStartEvent = nullptr;

		CloseHandle( m_hDisplayThread );
		SVThreadManager::Instance().Remove(m_hDisplayThread );
		m_hDisplayThread = nullptr;
	}
	return true;
}

bool SVDisplayObject::CanGoOnline()
{
	bool l_Status = m_CriticalSectionCreated;

	l_Status &= ( nullptr != GetIPDoc() );
	l_Status &= ( nullptr != m_hStopEvent );
	l_Status &= ( nullptr != m_hStartEvent );
	l_Status &= ( nullptr != m_hDisplayThread );

	return l_Status;
}// end CanGoOnline

bool SVDisplayObject::GoOnline()
{
	if( !::SetThreadPriority( m_hDisplayThread, THREAD_PRIORITY_IDLE ) )
		return false;

	::InterlockedExchange( &m_IPDocDisplayComplete, 1 );

	m_FrameRate = 10;

	GetIPDoc()->UpdateAllViews( nullptr );

	return true;
}// end GoOnline

bool SVDisplayObject::GoOffline()
{
	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		l_pIPDoc->UpdateWithLastProduct();
	}

	if( !::SetThreadPriority( m_hDisplayThread, THREAD_PRIORITY_NORMAL ) )
		return false;

	return true;
}// end GoOffline

void SVDisplayObject::SetIPDocDisplayComplete()
{
	::InterlockedExchange( &m_IPDocDisplayComplete, 1 );

	::SetEvent( m_hStartEvent );
	m_LastUpdateTime = SvTl::GetTimeStamp();
}

DWORD WINAPI SVDisplayObject::SVDisplayThreadFunc( LPVOID lpParam )
{
	SVDisplayObject *pDisplay = (SVDisplayObject*)lpParam;

	if( nullptr != pDisplay )
	{
		bool l_Processed = true;
		bool l_Running = pDisplay->m_CriticalSectionCreated;

		HANDLE l_pHandles[ 2 ] = { pDisplay->m_hStopEvent, pDisplay->m_hStartEvent };

		::SetThreadPriority( pDisplay->m_hDisplayThread, THREAD_PRIORITY_IDLE );

		while( l_Running )
		{
			if( !l_Processed )
			{
				::WaitForMultipleObjects( 2, l_pHandles, false, INFINITE );
			}

			if( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
			{
				l_Running = ( ::WaitForSingleObject( pDisplay->m_hStopEvent, 10 ) == WAIT_TIMEOUT );
			}
			else
			{
				l_Running = !( SVSVIMStateClass::CheckState( SV_STATE_CLOSING ) );
				l_Running = l_Running && ( ::WaitForSingleObject( pDisplay->m_hStopEvent, 0 ) == WAIT_TIMEOUT );
			}

			if( l_Running )
			{
				pDisplay->ProcessInspectionComplete( l_Processed );

				if( !l_Processed )
				{
					pDisplay->ProcessNotifyIPDoc( l_Processed );
				}
			}
		}
	}

	return 0L;
}// end SVDisplayThreadFunc

HRESULT SVDisplayObject::ProcessInspectionComplete( bool& p_rProcessed )
{
	HRESULT l_Status = S_OK;

	p_rProcessed = ( 0 <= m_PendingTrigger );

	if( p_rProcessed )
	{
		::EnterCriticalSection( &( m_CriticalSection ) );

		m_CurrentTrigger = m_PendingTrigger;
		m_FrameRate = 10;

		if( m_CurrentTrigger == m_DisplayedTrigger )
		{
			if( !( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) ) )
			{
				::InterlockedExchange( &m_DisplayedTrigger, -1 );
			}
		}

		m_PendingTrigger = -1;

		::LeaveCriticalSection( &( m_CriticalSection ) );
	}

	return l_Status;
}

HRESULT SVDisplayObject::ProcessNotifyIPDoc( bool& p_rProcessed )
{
	HRESULT l_Status = S_OK;

	p_rProcessed = ( m_IPDocDisplayComplete == 1 );

	if( p_rProcessed )
	{
		bool l_Process = true;

		if( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
		{
			::Sleep( 0 );

			l_Process = ( m_NextDisplayEvent < SvTl::GetTimeStamp() );

			if( l_Process )
			{
				long l_Indicator = 0;

				if( 0 < SVObjectManagerClass::Instance().GetShortPPQIndicator() )
				{
					l_Indicator += SVObjectManagerClass::Instance().GetProductIndicator();
					l_Indicator -= SVObjectManagerClass::Instance().GetPendingImageIndicator();
				}
				else
				{
					l_Indicator += SVObjectManagerClass::Instance().GetInspectionIndicator();
				}

				l_Process = l_Process && ( m_PendingTrigger < 0 );
				l_Process = l_Process && ( l_Indicator == 0 );

				if( !l_Process )
				{
					m_FrameRate = 1;
				}
				
			}
			// check if forcedUpdate is active
			if (!l_Process && TheSVObserverApp.IsForcedImageUpdateActive())
			{
				double elapsed = SvTl::GetTimeStamp() - m_LastUpdateTime;
				double interval = SvTl::ConvertFrom(SvTl::Seconds, TheSVObserverApp.GetForcedImageUpdateTimeInSeconds());
				// check last update time and if current time is greater than last update time + ForcedImageUpdateTime then Update
				l_Process = (elapsed >= interval);
#if defined (TRACE_THEM_ALL) || defined (TRACE_OTHER)
				TRACE("l_Process = %s\n", (l_Process ) ? "true" : "false");
#endif
			}
		}

		if( l_Process )
		{
			SVIPDoc* l_pIPDoc = GetIPDoc();

			p_rProcessed = ( nullptr != l_pIPDoc );

			if( p_rProcessed )
			{
				p_rProcessed = ( m_CurrentTrigger != m_DisplayedTrigger );

				if( p_rProcessed )
				{
					::InterlockedExchange( &( m_IPDocDisplayComplete ), 0 );

					l_pIPDoc->RefreshDocument();

					::InterlockedExchange( &m_DisplayedTrigger, m_CurrentTrigger );

					UpdateNextDisplayEvent();
				}
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
	}

	return l_Status;
}

HRESULT SVDisplayObject::FinishInspection( const std::pair<SVInspectionInfoStruct, long>& inspectionData )
{	
	HRESULT l_Status = S_OK;
	bool ImageUpdate = true;
	bool ResultUpdate = true;

	SVObjectClass* pObject(nullptr);
	double Value;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(EnvironmentImageUpdateUidGuid, pObject);
	if (nullptr != pObject)
	{
		//Use the getValue with double as it is faster (no dynamic casting)
		pObject->getValue(Value);
		ImageUpdate = 0.0 < Value ? true : false;
	}
	pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(EnvironmentResultUpdateUidGuid, pObject);
	if (nullptr != pObject)
	{
		//Use the getValue with double as it is faster (no dynamic casting)
		pObject->getValue(Value);
		ResultUpdate = 0.0 < Value ? true : false;
	}

	if( ImageUpdate || ResultUpdate  || !SVSVIMStateClass::CheckState( SV_STATE_RUNNING ))
	{
		if( m_CriticalSectionCreated && nullptr != m_hStartEvent )
		{
			bool l_State = !(SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST));

			l_State = l_State || (inspectionData.first.m_InspectedState == PRODUCT_INSPECTION_WARNING);
			l_State = l_State || (inspectionData.first.m_InspectedState == PRODUCT_INSPECTION_FAILED);
			l_State = l_State || (inspectionData.first.m_InspectedState == PRODUCT_INSPECTION_PASSED);

			if (l_State)
			{
				::EnterCriticalSection(&(m_CriticalSection));

				m_PendingTrigger = inspectionData.second;

				::SetEvent(m_hStartEvent);

				::LeaveCriticalSection(&(m_CriticalSection));
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

void SVDisplayObject::UpdateNextDisplayEvent()
{
	double l_OffsetInSeconds = 1.0 / static_cast< double >( m_FrameRate );

	m_NextDisplayEvent = SvTl::GetTimeStamp() + SvTl::ConvertFrom( SvTl::Seconds, l_OffsetInSeconds );

	m_FrameRate = SVObjectManagerClass::Instance().GetNextFrameRate( m_FrameRate );
}

