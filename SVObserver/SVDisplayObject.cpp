//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDisplayObject
//* .File Name       : $Workfile:   SVDisplayObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   02 Jun 2014 09:38:20  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDisplayObject.h"

#include "SVLibrary/SVWinUtility.h"
#include "SVMessage/SVMessage.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSVIMStateClass.h"
#include "SVStatusLibrary/SVException.h"
#include "SVTimerLibrary/SVClock.h"

#include "SVSystemLibrary/SVCrash.h"
#include "SVInspectionProcess.h"
#include "SVIPDoc.h"
#include "SVObserver.h"
#include "EnvironmentObject.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::SVObserver;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

SVDisplayObject::SVDisplayObject( LPCSTR ObjectName )
: SVObjectClass( ObjectName )
, m_PendingTrigger( -1 )
, m_CurrentTrigger( -1 )
, m_DisplayedTrigger( -1 )
, m_IPDocDisplayComplete( 1 )
, m_FrameRate( 10 )
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
}

DWORD WINAPI SVDisplayObject::SVDisplayThreadFunc( LPVOID lpParam )
{
	SetThreadType( DISPLAYOBJTHREAD );

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
	BOOL ImageUpdate = TRUE;
	BOOL ResultUpdate = TRUE;

	EnvironmentObject::getEnvironmentValue( ::EnvironmentImageUpdate, ImageUpdate );
	EnvironmentObject::getEnvironmentValue( ::EnvironmentResultUpdate, ResultUpdate );

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDisplayObject.cpp_v  $
 * 
 *    Rev 1.5   02 Jun 2014 09:38:20   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Changed the Online display flag into the Image and Result display update flags.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   25 Jul 2013 14:15:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  844
 * SCR Title:  Add logic for display to always display images and data during regression test
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added check to finish inspection to always display if not running - Ignore online display flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 May 2013 12:15:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   07 May 2013 13:20:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new SVClock conversions functions, which will clarify timing tolerance checks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 May 2013 13:26:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 013.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   01 May 2013 08:46:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Update Next Display Event to fix problem with calculation of the next display time stamp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:02:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   29 Apr 2013 14:10:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:14:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   11 Feb 2013 14:13:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code that uses the subject/observer design patten as part of the object manager with the new parameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   18 Jul 2012 13:30:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   09 Jul 2012 13:55:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove display attributes and SVIM State functoinality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   02 Jul 2012 16:45:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   28 Jun 2012 17:52:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  768
 * SCR Title:  Add the display time-out and throttling options for 5.50
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed display control functionally.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   20 Jun 2012 14:09:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed display issues with run once logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   19 Jun 2012 12:40:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   23 May 2012 13:22:44   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  768
 * SCR Title:  Add the display time-out and throttling options for 5.50
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed condition in FinishInspection
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   16 May 2012 18:55:02   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Merge 5.43 changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   04 Jan 2012 08:01:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  745
 * SCR Title:  Fix contention issue during load process and display
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added a sleep 10ms after updating the view in the DisplayThreadFunc to prevent contention.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33.1.0   14 May 2012 14:18:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added logic for freeze display.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   07 Oct 2011 14:16:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Image View and Display Object based on name changes for methods to make them more clear.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   06 Oct 2011 09:03:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed issues with display update problems when the system is going offline or is offline.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   06 Oct 2011 09:01:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed issues with display update problems when the system is going offline or is offline.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   03 Oct 2011 10:04:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated the handshaking between the display object thread and the image view draw functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   23 Sep 2011 11:33:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added State running check to DisplayThreadFunc to only wait when we are running.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   19 Sep 2011 13:07:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added condition to only throttle the display while the SVIM is Running.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   16 Sep 2011 15:53:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated display fucntionality based on new resource management methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   14 Jul 2011 08:42:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to better track resource management, and clear un-used resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   14 Jul 2011 08:16:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  729
 * SCR Title:  Adjust SVObserver to fix perfomance problems due to logging
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove logging in places that cause performance issues and added a registry controlled variable to handle the go offline data manager dumps to files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   05 Jul 2011 14:45:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  724
 * SCR Title:  Add Resource Logging to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to for Last Inspected to defined reference catagories.  Added new logging based on low availiable resources.  Added offline Data Manager dumps of reference counts.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   29 Apr 2011 08:06:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow for inspection to for the display to give up when an inspection get queued.  The display is now regulated to a maximum of 10 frames per second.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   10 Feb 2011 13:41:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Display to force a delay between display operation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   08 Dec 2010 07:59:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   05 Nov 2010 14:54:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove the un-necessary IP View Control from the application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   18 Feb 2010 14:19:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use an event to cause the display to update instead of the APC functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   15 Dec 2009 15:56:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   24 Sep 2009 09:38:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code with branch changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   03 Sep 2009 10:07:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   30 Jul 2009 11:24:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14.1.0   22 Sep 2009 18:05:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to handle changes in adding output requests to the inspections.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   14 Jul 2009 13:00:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   02 Nov 2005 08:04:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added m_hResultImagePublishedCircleBuffer
 * changed DataManager SV_OTHER to SV_DISPLAY
 * changed long m_lResultImageIndex to SVImageIndexStruct m_svResultImageIndex
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   20 Oct 2005 08:59:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new control variable for the camera info struct map to determine if a new display operation is allowed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   31 Aug 2005 08:37:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update source code call to handle new refresh methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   22 Aug 2005 10:11:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   10 Aug 2005 12:28:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Feb 2005 13:47:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   07 May 2003 13:16:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  361
 * SCR Title:  SVObserver's display may become incorrect when running at very high speeds
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Modified display logic so that image view displaying a SourceImage will look into the ProductInfoStruct to find the source image index for the camera from which they are derived instead of assuming it is the first camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Apr 2003 17:49:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  361
 * SCR Title:  SVObserver's display may become incorrect when running at very high speeds
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Changed display logic to make a copy of the result indexes from the product before starting the SLOW display process so that the product can't be recycled before the display finishes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 09:02:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Feb 2003 16:15:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  314
 * SCR Title:  SVIM stops inspecting when triggered very fast
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Updated FinishInspection method to only display inspected product.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Feb 2003 17:29:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Made modifications to the display logic to display the current source and result images indexes, respectively. This should make it work correctly regardless of whether it is online or performing RunOnce/Regression.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jan 2003 14:02:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to how the images are displayed while running and while doing a RunOnce operation to use the image that is specified in the SVProductInfoStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 12:54:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
