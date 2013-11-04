// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVListObject
// * .File Name       : $Workfile:   SVListObject.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:53:06  $
// ******************************************************************************

#include "stdafx.h"
#include "SVListObject.h"

SVListObject::SVListObject()
: m_bCreated( false ), m_List()
{
}

SVListObject::~SVListObject()
{
	Destroy();
}

BOOL SVListObject::Create()
{
	try
	{
		m_bCreated = ::InitializeCriticalSectionAndSpinCount( &m_hCriticalSection, 4000 );
	}
	catch(...)
	{
		m_bCreated = FALSE;
	}

	return m_bCreated;
}

BOOL SVListObject::Destroy()
{
	try
	{
		if( m_bCreated )
		{
			::EnterCriticalSection( &m_hCriticalSection );

			m_List.clear();

			::LeaveCriticalSection( &m_hCriticalSection );
			::DeleteCriticalSection( &m_hCriticalSection );
		}
	}
	catch(...)
	{
		return FALSE;
	}

	m_bCreated = FALSE;
	return TRUE;
}

BOOL SVListObject::IsCreated()
{
	return m_bCreated;
}

BOOL SVListObject::Lock()
{
	if( m_bCreated )
	{
		::EnterCriticalSection( &m_hCriticalSection );

		return TRUE;
	}// end if

	return FALSE;
}// end Lock

BOOL SVListObject::Unlock()
{
	if( m_bCreated )
	{
		::LeaveCriticalSection( &m_hCriticalSection );

		return TRUE;
	}// end if

	return FALSE;
}// end Unlock

BOOL SVListObject::Add( void *pParam )
{
	if( m_bCreated )
	{
		::EnterCriticalSection( &m_hCriticalSection );

		m_List.push_back( pParam );

		::LeaveCriticalSection( &m_hCriticalSection );
		return TRUE;
	}// end if

	return FALSE;
}// end Add

BOOL SVListObject::GetAt( long lPosition, void *&pParam )
{
	if( m_bCreated )
	{
		::EnterCriticalSection( &m_hCriticalSection );

		if( lPosition < 0 || lPosition >= static_cast< long >( m_List.size() ) )
		{
			pParam = NULL;
			::LeaveCriticalSection( &m_hCriticalSection );
			return FALSE;
		}// end if

		pParam = m_List[ lPosition ];

		::LeaveCriticalSection( &m_hCriticalSection );
		return TRUE;
	}// end if

	return FALSE;
}// end GetAt

BOOL SVListObject::GetSize( long &lSize )
{
	if( m_bCreated )
	{
		::EnterCriticalSection( &m_hCriticalSection );

		lSize = static_cast< long >( m_List.size() );

		::LeaveCriticalSection( &m_hCriticalSection );
		return TRUE;
	}// end if

	return FALSE;
}// end GetSize

BOOL SVListObject::RemoveAt( long lPosition )
{
	if( m_bCreated )
	{
		::EnterCriticalSection( &m_hCriticalSection );

		if( lPosition < 0 || lPosition >= static_cast< long >( m_List.size() ) )
		{
			::LeaveCriticalSection( &m_hCriticalSection );
			return FALSE;
		}// end if

		SVVoidDeque::iterator l_Iter = m_List.begin();

		std::advance( l_Iter, lPosition );

		if( l_Iter != m_List.end() )
		{
			m_List.erase( l_Iter );
		}

		::LeaveCriticalSection( &m_hCriticalSection );
		return TRUE;
	}// end if

	return FALSE;
}// end RemoveAt

BOOL SVListObject::RemoveAll()
{
	if( m_bCreated )
	{
		::EnterCriticalSection( &m_hCriticalSection );

		m_List.clear();

		::LeaveCriticalSection( &m_hCriticalSection );
		return TRUE;
	}// end if

	return FALSE;
}// end RemoveAll

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVListObject.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 11:53:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Dec 2010 16:04:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Sep 2009 09:22:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:15:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
