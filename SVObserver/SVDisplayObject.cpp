//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVUtilityLibrary/SVClock.h"
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
}

SVDisplayObject::~SVDisplayObject()
{
	Destroy();

	m_hStartEvent = nullptr;
	m_hStopEvent = nullptr;
	m_hDisplayThread = nullptr;
}

HRESULT SVDisplayObject::ObserverUpdate( const std::pair<long, SVInspectionInfoStruct>& rData )
{
	return FinishInspection(rData);
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
		auto l_Iter = p_rData.m_Images.begin();

		while( l_Iter != p_rData.m_Images.end() )
		{
			l_pIPDoc->RemoveImage( *l_Iter );

			++l_Iter;
		}
	}

	return l_Status;
}

HRESULT SVDisplayObject::ObserverUpdate( const SVRemoveValues&  )
{
	HRESULT l_Status = S_OK;

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		l_pIPDoc->RebuildResultsList();
	}

	return l_Status;
}

HRESULT SVDisplayObject::ObserverUpdate(const SVRemoveSubjectStruct& )
{
	HRESULT l_Status = S_OK;

	SVIPDoc* l_pIPDoc = GetIPDoc();

	if( nullptr != l_pIPDoc )
	{
		l_pIPDoc->CloseDocument();
	}

	return l_Status;
}

void SVDisplayObject::SetInspectionID(uint32_t inspectionID, SVIPDoc* pDoc)
{
	m_pDoc = pDoc;

	if (m_InspectionID != inspectionID)
	{
		auto* pObject = dynamic_cast<SVInspectionProcess*>(SVObjectManagerClass::Instance().GetObject(m_InspectionID));

		if (nullptr != pObject)
		{
			pObject->detachObserver(this);
		}

		m_InspectionID = inspectionID;

		pObject = dynamic_cast<SVInspectionProcess*>(SVObjectManagerClass::Instance().GetObject(m_InspectionID));
		if (nullptr != pObject)
		{
			pObject->attachObserver(this);
		}
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

	return TRUE;
}// end Create

BOOL SVDisplayObject::Destroy()
{
	auto* pObject = dynamic_cast<SVInspectionProcess*>(SVObjectManagerClass::Instance().GetObject(m_InspectionID));
	if (nullptr != pObject)
	{
		pObject->detachObserver(this);
	}
	m_InspectionID = SvDef::InvalidObjectId;

	if (m_hStopEvent && m_hDisplayThread)
	{
		::SetEvent( m_hStopEvent );

		// wait for thread to exit
		DWORD exitCode = 0;
		do
		{
			MSG msg;

			while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			::Sleep(1);
			::GetExitCodeThread(m_hDisplayThread, &exitCode);
		} while (exitCode == STILL_ACTIVE);

		::CloseHandle( m_hStopEvent );
		m_hStopEvent = nullptr;

		::CloseHandle( m_hStartEvent );
		m_hStartEvent = nullptr;

		CloseHandle( m_hDisplayThread );
		m_hDisplayThread = nullptr;
	}
	return true;
}

bool SVDisplayObject::CanGoOnline()
{
	bool l_Status = ( nullptr != GetIPDoc() );
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
	m_LastUpdateTime = SvUl::GetTimeStamp();
}

DWORD WINAPI SVDisplayObject::SVDisplayThreadFunc( LPVOID lpParam )
{
	SVDisplayObject *pDisplay = reinterpret_cast<SVDisplayObject*>(lpParam);

	if( nullptr != pDisplay )
	{
		bool l_Processed = true;
		bool l_Running = true;

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
		std::lock_guard<std::mutex> lock {m_displayMutex};

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

			l_Process = ( m_NextDisplayEvent < SvUl::GetTimeStamp() );

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
			if (!l_Process && TheSVObserverApp().IsForcedImageUpdateActive())
			{
				double elapsed = SvUl::GetTimeStamp() - m_LastUpdateTime;
				double interval = SvUl::ConvertFrom(SvUl::Seconds, TheSVObserverApp().GetForcedImageUpdateTimeInSeconds());
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

HRESULT SVDisplayObject::FinishInspection( const std::pair<long, SVInspectionInfoStruct>& rData )
{	
	HRESULT l_Status = S_OK;
	bool ImageUpdate = true;
	bool ResultUpdate = true;

	SVObjectClass* pObject(nullptr);
	double Value;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(ObjectIdEnum::EnvironmentImageUpdateId, pObject);
	if (nullptr != pObject)
	{
		//Use the getValue with double as it is faster (no dynamic casting)
		pObject->getValue(Value);
		ImageUpdate = 0.0 < Value ? true : false;
	}
	pObject = nullptr;
	SVObjectManagerClass::Instance().GetObjectByIdentifier(ObjectIdEnum::EnvironmentResultUpdateId, pObject);
	if (nullptr != pObject)
	{
		//Use the getValue with double as it is faster (no dynamic casting)
		pObject->getValue(Value);
		ResultUpdate = 0.0 < Value ? true : false;
	}

	if( ImageUpdate || ResultUpdate  || !SVSVIMStateClass::CheckState( SV_STATE_RUNNING ))
	{
		if( nullptr != m_hStartEvent )
		{
			bool l_State = !(SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST));

			l_State = l_State || (rData.second.m_InspectedState == PRODUCT_INSPECTION_WARNING);
			l_State = l_State || (rData.second.m_InspectedState == PRODUCT_INSPECTION_FAILED);
			l_State = l_State || (rData.second.m_InspectedState == PRODUCT_INSPECTION_PASSED);

			if (l_State)
			{
				std::lock_guard<std::mutex> lock {m_displayMutex};

				m_PendingTrigger = rData.first;

				::SetEvent(m_hStartEvent);
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

	m_NextDisplayEvent = SvUl::GetTimeStamp() + SvUl::ConvertFrom( SvUl::Seconds, l_OffsetInSeconds );

	m_FrameRate = SVObjectManagerClass::Instance().GetNextFrameRate( m_FrameRate );
}

