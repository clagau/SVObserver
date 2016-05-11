//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectManagerClass
//* .File Name       : $Workfile:   SVObjectManagerClass.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Mar 2014 14:18:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectManagerClass.h"
#include "SVObserverTemplate.h"
#include "SVObjectClass.h"
#include "SVObjectNotifyTemplate.h"
#include "GlobalConst.h"
#include "ObjectInterfaces\IObjectManager.h"
#pragma endregion Includes

template< typename SVObjectTypeName >
HRESULT SVObjectManagerClass::ConstructObject( const SVGUID& p_rClassID, SVObjectTypeName*& p_rpObject )
{
	SVObjectClass* l_pObject = nullptr;

	p_rpObject = nullptr;

	HRESULT l_Status = ConstructObject( p_rClassID, l_pObject );

	if( nullptr != l_pObject )
	{
		p_rpObject = dynamic_cast< SVObjectTypeName* >( l_pObject );

		if( nullptr == p_rpObject )
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

	if( nullptr != l_Reference.Object() )
	{
		p_rpObject = dynamic_cast< SVObjectTypeName* >( l_Reference.Object() );
	}
	else
	{
		p_rpObject = nullptr;
	}

	if( S_OK == l_Status && nullptr == p_rpObject )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template< typename SVObjectTypeName >
HRESULT SVObjectManagerClass::GetRootChildObject( SVObjectTypeName*& rpObject, const SVString& rRootChild )
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = GetObject( m_RootNameChildren[rRootChild] );

	if( nullptr != l_pObject )
	{
		rpObject = dynamic_cast< SVObjectTypeName* >( l_pObject );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

template< typename SVObjectTypeName >
HRESULT SVObjectManagerClass::GetConfigurationObject( SVObjectTypeName*& rpObject )
{
	return GetRootChildObject(rpObject, SvOl::FqnConfiguration );
}

template< typename SVNotifyData >
HRESULT SVObjectManagerClass::Notify( const SVGUID& p_rObjectID, SVNotifyData& p_rData )
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = GetObject( p_rObjectID );

	if( nullptr != l_pObject )
	{
		SVObjectNotifyTemplate< SVNotifyData >* l_pCommand = dynamic_cast< SVObjectNotifyTemplate< SVNotifyData >* >( l_pObject );

		if( nullptr != l_pCommand )
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
		//Set to configuration as this used to be the start.
		l_StartingObjectID = GetChildRootObjectID( SvOl::FqnConfiguration );
	}
	SVObjectClass* l_pObject = GetObject( l_StartingObjectID );

	if( nullptr != l_pObject )
	{
		HRESULT l_Temp = S_OK;

		SVObjectClass::SVObjectPtrDeque l_PreObjects = l_pObject->GetPreProcessObjects();

		SVObjectClass::SVObjectPtrDeque::iterator l_PreIter;

		for( l_PreIter = l_PreObjects.begin(); l_PreIter != l_PreObjects.end(); ++l_PreIter )
		{
			SVObjectClass* l_pPreObject = *l_PreIter;

			if( nullptr != l_pPreObject )
			{
				l_Temp = VisitElements( p_rVisitor, l_pPreObject->GetUniqueObjectID() );

				if( S_OK == l_Status )
				{
					l_Status = l_Temp;
				}
			}
		}

		l_Temp = l_pObject->Accept( p_rVisitor );

		if( S_OK == l_Status )
		{
			l_Status = l_Temp;
		}

		SVObjectClass::SVObjectPtrDeque l_PostObjects = l_pObject->GetPostProcessObjects();

		SVObjectClass::SVObjectPtrDeque::iterator l_PostIter;

		for( l_PostIter = l_PostObjects.begin(); l_PostIter != l_PostObjects.end(); ++l_PostIter )
		{
			SVObjectClass* l_pPostObject = *l_PostIter;

			if( nullptr != l_pPostObject )
			{
				l_Temp = VisitElements( p_rVisitor, l_pPostObject->GetUniqueObjectID() );

				if( S_OK == l_Status )
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

	if( nullptr != l_pObject )
	{
		SVObserverTemplate< SVDataType >* l_pObserver = dynamic_cast< SVObserverTemplate< SVDataType >* >( l_pObject );

		if( nullptr != l_pObserver )
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

		if( nullptr != l_pObserver )
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

	if( S_OK == l_Status )
	{
		SVSubjectEnabledObserverMap::iterator l_Iter = l_Observers.begin();

		while( l_Iter != l_Observers.end() )
		{
			if( l_Iter->second == 1 )
			{
				HRESULT l_Temp = UpdateObserver( l_Iter->first, p_rData );

				if( S_OK == l_Status )
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

				if( S_OK == l_Status )
				{
					l_Status = l_Temp;
				}
			}

			++l_CookieIter;
		}
	}

	return l_Status;
}

template<typename ObjectVisitor>
HRESULT SvOi::visitElements(ObjectVisitor& rVisitor, const SVGUID& rStartingObjectID)
{
	return SVObjectManagerClass::Instance().VisitElements(rVisitor, rStartingObjectID);
}

