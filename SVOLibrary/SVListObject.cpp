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

