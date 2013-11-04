//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandStreamManager
//* .File Name       : $Workfile:   SVCommandStreamManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:55:26  $
//******************************************************************************

#pragma once

#include <boost/config.hpp>
#include <boost/function.hpp>

#include "SVObjectLibrary/SVObserverNotificationFunctor.h"
#include "SVObjectLibrary/SVObserverTemplate.h"
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVUtilityLibrary/SVGUID.h"

#include "SVInfoStructs.h"

class SVCommandStreamManager
{
public:
	typedef boost::function< HRESULT ( const SVInspectionCompleteInfoStruct& ) > SVCommandObserverFunction;
	typedef boost::function< HRESULT () > SVCommandRebuildFunction;

	static SVCommandStreamManager& Instance();

	virtual ~SVCommandStreamManager();

	HRESULT InsertCommandCallback( SVCommandObserverFunction p_ObserverFunction, SVCommandRebuildFunction p_RebuildFunction );
	HRESULT EraseCommandCallback();

	HRESULT RebuildCommandObserver();

	HRESULT InsertInspection( const SVGUID& p_rObjectId );
	HRESULT EraseInspection( const SVGUID& p_rObjectId );

	HRESULT EnableInspectionCallback( const SVGUID& p_rObjectId );

	HRESULT EnableAllInspections();
	HRESULT DisableAllInspections();

protected:
	class SVInspectionObserver : 
		public SVObserverNotificationFunctor,
		public SVObserverTemplate< SVInspectionCompleteInfoStruct >
	{
	public:
		typedef boost::function< HRESULT ( const SVInspectionCompleteInfoStruct& ) > SVObserverFunction;

		SVInspectionObserver( SVObserverFunction p_Function ) : m_Function( p_Function ) {}

		virtual HRESULT ObserverUpdate( const SVInspectionCompleteInfoStruct& p_rData )
		{
			HRESULT l_Status = S_OK;

			if( !( m_Function.empty() ) )
			{
				l_Status = m_Function( p_rData );
			}
			else
			{
				l_Status = E_FAIL;
			}

			return l_Status;
		}

	private:
		SVInspectionObserver() : m_Function() {}

		SVObserverFunction m_Function;

	};

	struct SVCookieEnableItem
	{
		SVCookieEnableItem() : m_Cookie( 0 ), m_Enabled( 0 ) {}
		SVCookieEnableItem( long p_Cookie, long p_Enable ) : m_Cookie( p_Cookie ), m_Enabled( p_Enable ) {}
		SVCookieEnableItem( const SVCookieEnableItem& p_rObject ) : m_Cookie( p_rObject.m_Cookie ), m_Enabled( p_rObject.m_Enabled ) {}

		long m_Cookie;
		long m_Enabled;
	};

	typedef std::map< SVGUID, SVCookieEnableItem > SVGuidCookieMap;

	SVCommandStreamManager();

	HRESULT CommandCallback( const SVInspectionCompleteInfoStruct& p_rData );

	SVCommandObserverFunction m_ObserverFunction;
	SVCommandRebuildFunction m_RebuildFunction;

	SVGuidCookieMap m_Cookies;

	mutable SVCriticalSection m_Lock;

};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCommandStreamManager.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:55:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jul 2012 16:39:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
