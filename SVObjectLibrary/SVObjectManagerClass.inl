//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectManagerClass
//* .File Name       : $Workfile:   SVObjectManagerClass.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:57:52  $
//******************************************************************************

#ifndef SVOBJECTMANAGERCLASS_INL
#define SVOBJECTMANAGERCLASS_INL

#include "SVObjectManagerClass.h"
#include "SVObserverTemplate.h"
#include "SVObjectClass.h"
#include "SVObjectNotifyTemplate.h"

template< typename SVObjectTypeName >
HRESULT SVObjectManagerClass::ConstructObject( const SVGUID& p_rClassID, SVObjectTypeName*& p_rpObject )
{
	SVObjectClass* l_pObject = NULL;

	p_rpObject = NULL;

	HRESULT l_Status = ConstructObject( p_rClassID, l_pObject );

	if( l_pObject != NULL )
	{
		p_rpObject = dynamic_cast< SVObjectTypeName* >( l_pObject );

		if( p_rpObject == NULL )
		{
			l_Status = E_FAIL;

			delete l_pObject;
		}
	}

	return l_Status;
}

template< typename SVObjectTypeName >
HRESULT SVObjectManagerClass::GetObjectByDottedName( const SVString& p_rFullName, SVObjectTypeName*& p_rpObject )
{
	HRESULT l_Status = S_OK;

	SVObjectReference l_Reference;

	l_Status = GetObjectByDottedName( p_rFullName, l_Reference );

	if( l_Reference.Object() != NULL )
	{
		p_rpObject = dynamic_cast< SVObjectTypeName* >( l_Reference.Object() );
	}
	else
	{
		p_rpObject = NULL;
	}

	if( l_Status == S_OK && p_rpObject == NULL )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template< typename SVObjectTypeName >
HRESULT SVObjectManagerClass::GetConfigurationObject( SVObjectTypeName*& p_rpObject )
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = GetObject( m_ConfigurationID );

	if( l_pObject != NULL )
	{
		p_rpObject = dynamic_cast< SVObjectTypeName* >( l_pObject );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template< typename SVNotifyData >
HRESULT SVObjectManagerClass::Notify( const SVGUID& p_rObjectID, SVNotifyData& p_rData )
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = GetObject( p_rObjectID );

	if( l_pObject != NULL )
	{
		SVObjectNotifyTemplate< SVNotifyData >* l_pCommand = dynamic_cast< SVObjectNotifyTemplate< SVNotifyData >* >( l_pObject );

		if( l_pCommand != NULL )
		{
			l_Status = l_pCommand->ProcessNotifyData( p_rData );
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template< typename ObjectVisitor>
HRESULT SVObjectManagerClass::VisitElements( ObjectVisitor& p_rVisitor, const SVGUID& p_rStartingObjectID )
{
	HRESULT l_Status = S_OK;

	GUID l_StartingObjectID = p_rStartingObjectID;
	if (l_StartingObjectID == GUID_NULL)
	{
		l_StartingObjectID = GetConfigurationObjectID();
	}
	SVObjectClass* l_pObject = GetObject( l_StartingObjectID );

	if( l_pObject != NULL )
	{
		HRESULT l_Temp = S_OK;

		SVObjectClass::SVObjectPtrDeque l_PreObjects = l_pObject->GetPreProcessObjects();

		SVObjectClass::SVObjectPtrDeque::iterator l_PreIter;

		for( l_PreIter = l_PreObjects.begin(); l_PreIter != l_PreObjects.end(); ++l_PreIter )
		{
			SVObjectClass* l_pPreObject = *l_PreIter;

			if( l_pPreObject != NULL )
			{
				l_Temp = VisitElements( p_rVisitor, l_pPreObject->GetUniqueObjectID() );

				if( l_Status == S_OK )
				{
					l_Status = l_Temp;
				}
			}
		}

		l_Temp = l_pObject->Accept( p_rVisitor );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		SVObjectClass::SVObjectPtrDeque l_PostObjects = l_pObject->GetPostProcessObjects();

		SVObjectClass::SVObjectPtrDeque::iterator l_PostIter;

		for( l_PostIter = l_PostObjects.begin(); l_PostIter != l_PostObjects.end(); ++l_PostIter )
		{
			SVObjectClass* l_pPostObject = *l_PostIter;

			if( l_pPostObject != NULL )
			{
				l_Temp = VisitElements( p_rVisitor, l_pPostObject->GetUniqueObjectID() );

				if( l_Status == S_OK )
				{
					l_Status = l_Temp;
				}
			}
		}
	}

	return l_Status;
}

template< typename SVDataType >
HRESULT SVObjectManagerClass::UpdateObserver( const SVGUID& p_rObserverID, const SVDataType& p_rData )
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = GetObject( p_rObserverID );

	if( l_pObject != NULL )
	{
		SVObserverTemplate< SVDataType >* l_pObserver = dynamic_cast< SVObserverTemplate< SVDataType >* >( l_pObject );

		if( l_pObserver != NULL )
		{
			l_Status = l_pObserver->ObserverUpdate( p_rData );
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template< typename SVDataType >
HRESULT SVObjectManagerClass::UpdateObserver( long p_Cookie, const SVDataType& p_rData )
{
	HRESULT l_Status = S_OK;

	SVCookieEntryStructPtr l_CookiePtr = GetCookieEntry( p_Cookie );

	if( !( l_CookiePtr.empty() ) )
	{
		SVObserverTemplate< SVDataType >* l_pObserver = dynamic_cast< SVObserverTemplate< SVDataType >* >( l_CookiePtr->m_FunctorPtr.get() );

		if( l_pObserver != NULL )
		{
			l_Status = l_pObserver->ObserverUpdate( p_rData );
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template< typename SVDataType >
HRESULT SVObjectManagerClass::UpdateObservers( const SVString& p_rSubjectDataName, const SVGUID& p_rSubjectID, const SVDataType& p_rData )
{
	HRESULT l_Status = S_OK;

	SVSubjectEnabledObserverMap l_Observers;
	SVSubjectEnabledCookieMap l_Cookies;

	l_Status = GetObservers( p_rSubjectDataName, p_rSubjectID, l_Observers, l_Cookies );

	if( l_Status == S_OK )
	{
		SVSubjectEnabledObserverMap::iterator l_Iter = l_Observers.begin();

		while( l_Iter != l_Observers.end() )
		{
			if( l_Iter->second == 1 )
			{
				HRESULT l_Temp = UpdateObserver( l_Iter->first, p_rData );

				if( l_Status == S_OK )
				{
					l_Status = l_Temp;
				}
			}

			++l_Iter;
		}

		SVSubjectEnabledCookieMap::iterator l_CookieIter = l_Cookies.begin();

		while( l_CookieIter != l_Cookies.end() )
		{
			if( l_CookieIter->second == 1 )
			{
				HRESULT l_Temp = UpdateObserver( l_CookieIter->first, p_rData );

				if( l_Status == S_OK )
				{
					l_Status = l_Temp;
				}
			}

			++l_CookieIter;
		}
	}

	return l_Status;
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVObjectManagerClass.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:57:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   11 Feb 2013 12:19:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update object manager to fix issues with subject/observer pattern functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Jul 2012 12:32:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object manager to change the functionality for the object visitor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   27 Jul 2012 07:59:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the visitor fucntionality to make it more flexable without having to add new elements to the framework.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Jul 2012 13:51:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove display attributes and SVIM State functoinality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Jul 2012 16:02:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Jun 2012 17:31:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
 * Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 May 2012 19:00:28   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Merge 5.43 changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Jan 2012 15:04:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated object manager to include the new notification functionality to managed object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.0   14 May 2012 13:36:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  763
 * SCR Title:  Add the display time-out and throttling options.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added functionallity for display update rate and display timeout feature.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Dec 2010 15:05:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
