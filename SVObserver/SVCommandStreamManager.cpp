//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandStreamManager
//* .File Name       : $Workfile:   SVCommandStreamManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:56:50  $
//******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>

#include "SVCommandStreamManager.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSystemLibrary/SVAutoLockAndReleaseTemplate.h"


SVCommandStreamManager& SVCommandStreamManager::Instance()
{
	static SVCommandStreamManager l_Object;

	return l_Object;
}

SVCommandStreamManager::~SVCommandStreamManager()
{
	SVGuidCookieMap::iterator l_Iter = m_Cookies.begin();

	while( l_Iter != m_Cookies.end() )
	{
		SVObjectManagerClass::Instance().EraseObserver( l_Iter->second.m_Cookie );

		m_Cookies.erase( l_Iter );

		l_Iter = m_Cookies.begin();
	}
}

HRESULT SVCommandStreamManager::InsertCommandCallback( SVCommandObserverFunction p_ObserverFunction, SVCommandRebuildFunction p_RebuildFunction )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		EnableAllInspections();

		m_ObserverFunction = p_ObserverFunction;
		m_RebuildFunction = p_RebuildFunction;
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVCommandStreamManager::EraseCommandCallback()
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		m_ObserverFunction.clear();
		m_RebuildFunction.clear();

		DisableAllInspections();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVCommandStreamManager::RebuildCommandObserver()
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		if( !( m_RebuildFunction.empty() ) )
		{
			l_Status = m_RebuildFunction();

			EnableAllInspections();
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVCommandStreamManager::InsertInspection( const SVGUID& p_rObjectId )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		SVGuidCookieMap::iterator l_Iter = m_Cookies.find( p_rObjectId );

		if( l_Iter == m_Cookies.end() )
		{
			long l_Cookie = 0;

			SVObserverNotificationFunctorPtr l_NotifyPtr = new SVInspectionObserver( boost::bind( &SVCommandStreamManager::CommandCallback, this, _1 ) );
			l_Status = SVObjectManagerClass::Instance().InsertObserver( l_NotifyPtr, l_Cookie );

			if( S_OK == l_Status )
			{
				l_Status = SVObjectManagerClass::Instance().AttachObserver( "SVInspectionProcess", p_rObjectId, l_Cookie );

				if( S_OK == l_Status )
				{
					l_Status = SVObjectManagerClass::Instance().DisableObserver( "SVInspectionProcess", p_rObjectId, l_Cookie );
				}

				if( S_OK == l_Status )
				{
					m_Cookies[ p_rObjectId ] = SVCookieEnableItem( l_Cookie, 0 );
				}
				else
				{
					SVObjectManagerClass::Instance().EraseObserver( l_Cookie );
				}
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVCommandStreamManager::EraseInspection( const SVGUID& p_rObjectId )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		SVGuidCookieMap::iterator l_Iter = m_Cookies.find( p_rObjectId );

		if( l_Iter != m_Cookies.end() )
		{
			DWORD l_Cookie = l_Iter->second.m_Cookie;

			m_Cookies.erase( l_Iter );

			SVObjectManagerClass::Instance().EraseObserver( l_Cookie );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVCommandStreamManager::EnableInspectionCallback( const SVGUID& p_rObjectId )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		SVGuidCookieMap::iterator l_Iter = m_Cookies.find( p_rObjectId );

		if( l_Iter != m_Cookies.end() )
		{
			l_Iter->second.m_Enabled = 1;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVCommandStreamManager::EnableAllInspections()
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		SVGuidCookieMap::iterator l_Iter = m_Cookies.begin();

		while( l_Iter != m_Cookies.end() )
		{
			if( l_Iter->second.m_Enabled == 1 )
			{
				SVObjectManagerClass::Instance().EnableObserver( "SVInspectionProcess", l_Iter->first, l_Iter->second.m_Cookie );
			}

			++l_Iter;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVCommandStreamManager::DisableAllInspections()
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		SVGuidCookieMap::iterator l_Iter = m_Cookies.begin();

		while( l_Iter != m_Cookies.end() )
		{
			SVObjectManagerClass::Instance().DisableObserver( "SVInspectionProcess", l_Iter->first, l_Iter->second.m_Cookie );

			++l_Iter;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVCommandStreamManager::SVCommandStreamManager()
: m_ObserverFunction(), m_Cookies()
{
}

HRESULT SVCommandStreamManager::CommandCallback( const SVInspectionCompleteInfoStruct& p_rData )
{
	HRESULT l_Status = S_OK;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	if( l_AutoLock.Assign( &m_Lock ) )
	{
		if( !( m_ObserverFunction.empty() ) )
		{
			l_Status = m_ObserverFunction( p_rData );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

