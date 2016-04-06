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

#include "SVLibrary/SVWinUtility.h"
#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSVIMStateClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVObserver.h"
#include "RootObject.h"
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
, m_LastUpdateTime(std::numeric_limits<SVClock::SVTimeStamp>::max())
{
	m_hStartEvent = NULL;
	m_hStopEvent = NULL;
	m_hDisplayThread = NULL;

	m_CriticalSectionCreated = ( ::InitializeCriticalSectionAndSpinCount( &m_CriticalSection, 5000 ) != FALSE );
}

SVDisplayObject::SVDisplayObject( SVObjectClass* POwner, int StringResourceID )
: SVObjectClass( POwner, StringResourceID )
, m_PendingTrigger( -1 )
, m_CurrentTrigger( -1 )
, m_DisplayedTrigger( -1 )
, m_IPDocDisplayComplete( 1 )
, m_FrameRate( 10 )
{
	m_hStartEvent = NULL;
	m_hStopEvent			= NULL;
	m_hDisplayThread		= NULL;

	m_CriticalSectionCreated = ( ::InitializeCriticalSectionAndSpinCount( &m_CriticalSection, 5000 ) != FALSE );
}

SVDisplayObject::~SVDisplayObject()
{
	Destroy();

	m_CriticalSectionCreated = false;
	::DeleteCriticalSection( &m_CriticalSection );

	m_hStartEvent = NULL;
	m_hStopEvent			= NULL;
	m_hDisplayThread		= NULL;
}

HRESULT SVDisplayObject::ObserverUpdate( const SVInspectionCompleteInfoStruct& p_rData )
{
	HRESULT l_Status = S_OK;

	l_Status = FinishInspection( p_rData );

	return l_Status;
}

HRESULT SVDisplayObject::ObserverUpdate( const SVInspectionNameUpdate& p_rData )
{
	HRESULT l_Status = S_OK;

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( l_pIPDoc != NULL )
	{
		l_pIPDoc->SetTitle( p_rData.m_InspectionName.c_str() );
	}

	return l_Status;
}

HRESULT SVDisplayObject::ObserverUpdate( const SVRemoveImages& p_rData )
{
	HRESULT l_Status = S_OK;

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( l_pIPDoc != NULL )
	{
		SVRemoveImages::SVImageIdSet::const_iterator l_Iter = p_rData.m_Images.begin();

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

	if( l_pIPDoc != NULL )
	{
		l_pIPDoc->RebuildResultsList();
	}

	return l_Status;
}

HRESULT SVDisplayObject::ObserverUpdate( const SVRemoveSubjectStruct& p_rData )
{
	HRESULT l_Status = S_OK;

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( l_pIPDoc != NULL )
	{
		l_pIPDoc->CloseDocument();
	}

	return l_Status;
}

void SVDisplayObject::SetInspectionID( const SVGUID& p_rInspectionID )
{
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
	SVIPDoc* l_pDoc = NULL;

	if( !( m_InspectionID.empty() ) )
	{
		l_pDoc = SVObjectManagerClass::Instance().GetIPDoc( m_InspectionID );
	}

	return l_pDoc;
}

BOOL SVDisplayObject::Create()
{
	DWORD dwThreadID;

	m_hStartEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );
	if( !m_hStartEvent )
		return FALSE;

	m_hStopEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
	if( !m_hStopEvent )
		return FALSE;

	m_hDisplayThread = ::CreateThread(NULL, 0, SVDisplayThreadFunc, (LPVOID)this, 0, &dwThreadID );
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
		m_hStopEvent = NULL;

		::CloseHandle( m_hStartEvent );
		m_hStartEvent = NULL;

		CloseHandle( m_hDisplayThread );
		SVThreadManager::Instance().Remove(m_hDisplayThread );
		m_hDisplayThread = NULL;
	}
	return true;
}

BOOL SVDisplayObject::CanGoOnline()
{
	BOOL l_Status = m_CriticalSectionCreated;

	l_Status &= ( GetIPDoc() != NULL );
	l_Status &= ( m_hStopEvent != NULL );
	l_Status &= ( m_hStartEvent != NULL );
	l_Status &= ( m_hDisplayThread != NULL );

	return l_Status;
}// end CanGoOnline

BOOL SVDisplayObject::GoOnline()
{
	if( !::SetThreadPriority( m_hDisplayThread, THREAD_PRIORITY_IDLE ) )
		return FALSE;

	::InterlockedExchange( &m_IPDocDisplayComplete, 1 );

	m_FrameRate = 10;

	GetIPDoc()->UpdateAllViews( NULL );

	return TRUE;
}// end GoOnline

BOOL SVDisplayObject::GoOffline()
{
	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( l_pIPDoc != NULL )
	{
		l_pIPDoc->UpdateWithLastProduct();
	}

	if( !::SetThreadPriority( m_hDisplayThread, THREAD_PRIORITY_NORMAL ) )
		return FALSE;

	return TRUE;
}// end GoOffline

void SVDisplayObject::SetIPDocDisplayComplete()
{
	::InterlockedExchange( &m_IPDocDisplayComplete, 1 );

	::SetEvent( m_hStartEvent );
	m_LastUpdateTime = SVClock::GetTimeStamp();
}

DWORD WINAPI SVDisplayObject::SVDisplayThreadFunc( LPVOID lpParam )
{
	SVDisplayObject *pDisplay = (SVDisplayObject*)lpParam;

	if( pDisplay != NULL )
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

			l_Process = ( m_NextDisplayEvent < SVClock::GetTimeStamp() );

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
				double elapsed = SVClock::GetTimeStamp() - m_LastUpdateTime;
				double interval = SVClock::ConvertFrom(SVClock::Seconds, TheSVObserverApp.GetForcedImageUpdateTimeInSeconds());
				// check last update time and if current time is greater than last update time + ForcedImageUpdateTime then Update
				l_Process = (elapsed >= interval);
				TRACE("l_Process = %s\n", (l_Process ) ? "true" : "false");
			}
		}

		if( l_Process )
		{
			SVIPDoc* l_pIPDoc = GetIPDoc();

			p_rProcessed = ( l_pIPDoc != NULL );

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

HRESULT SVDisplayObject::FinishInspection( const SVInspectionCompleteInfoStruct& p_rProduct )
{	
	HRESULT l_Status = S_OK;
	bool ImageUpdate = true;
	bool ResultUpdate = true;

	RootObject::getRootChildValue( ::EnvironmentImageUpdate, ImageUpdate );
	RootObject::getRootChildValue( ::EnvironmentResultUpdate, ResultUpdate );

	if( ImageUpdate || ResultUpdate  || !SVSVIMStateClass::CheckState( SV_STATE_RUNNING ))
	{
		if( m_CriticalSectionCreated && m_hStartEvent != NULL && !( p_rProduct.empty() ) )
		{
			SVGUIDSVInspectionInfoStructMap::const_iterator l_Iter = p_rProduct.m_ProductInfo.m_svInspectionInfos.find( m_InspectionID.ToGUID() );

			if( l_Iter != p_rProduct.m_ProductInfo.m_svInspectionInfos.end() )
			{
				bool l_State = !( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) );

				l_State = l_State || ( l_Iter->second.oInspectedState == PRODUCT_INSPECTION_WARNING );
				l_State = l_State || ( l_Iter->second.oInspectedState == PRODUCT_INSPECTION_FAILED );
				l_State = l_State || ( l_Iter->second.oInspectedState == PRODUCT_INSPECTION_PASSED );

				if( l_State )
				{
					::EnterCriticalSection( &( m_CriticalSection ) );

					m_PendingTrigger = p_rProduct.m_ProductInfo.ProcessCount();

					::SetEvent( m_hStartEvent );

					::LeaveCriticalSection( &( m_CriticalSection ) );
				}
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

	m_NextDisplayEvent = SVClock::GetTimeStamp() + SVClock::ConvertFrom( SVClock::Seconds, l_OffsetInSeconds );

	m_FrameRate = SVObjectManagerClass::Instance().GetNextFrameRate( m_FrameRate );
}

