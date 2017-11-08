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
HRESULT SVObjectManagerClass::ConstructObject( const SVGUID& rClassID, SVObjectTypeName*& rpObject )
{
	SVObjectClass* pObject = nullptr;

	rpObject = nullptr;

	HRESULT Result = ConstructObject( rClassID, pObject );

	if( nullptr != pObject )
	{
		rpObject = dynamic_cast<SVObjectTypeName*> ( pObject );

		if( nullptr == rpObject )
		{
			Result = E_FAIL;

			delete pObject;
		}
	}

	return Result;
}

template< typename SVObjectTypeName >
HRESULT SVObjectManagerClass::GetObjectByDottedName( const std::string& rFullName, SVObjectTypeName*& rpObject )
{
	HRESULT Result = S_OK;

	SVObjectReference ObjRef;

	Result = GetObjectByDottedName( rFullName, ObjRef );

	if( nullptr != ObjRef.getObject() )
	{
		rpObject = dynamic_cast<SVObjectTypeName*> ( ObjRef.getObject() );
	}
	else
	{
		rpObject = nullptr;
	}

	if( S_OK == Result && nullptr == rpObject )
	{
		Result = E_FAIL;
	}

	return Result;
}

template< typename SVObjectTypeName >
HRESULT SVObjectManagerClass::GetRootChildObject( SVObjectTypeName*& rpObject, const std::string& rRootChild )
{
	HRESULT Result = E_FAIL;

	SVObjectClass* pObject = GetObject( m_RootNameChildren[rRootChild] );

	if( nullptr != pObject )
	{
		rpObject = dynamic_cast<SVObjectTypeName*> (pObject);
		Result = S_OK;
	}

	return Result;
}

template< typename SVObjectTypeName >
HRESULT SVObjectManagerClass::GetConfigurationObject( SVObjectTypeName*& rpObject )
{
	return GetRootChildObject( rpObject, SvOl::FqnConfiguration );
}

template< typename SVNotifyData >
HRESULT SVObjectManagerClass::Notify( const SVGUID& rObjectID, SVNotifyData& rData )
{
	HRESULT Result = E_FAIL;

	SVObjectClass* pObject = GetObject( rObjectID );

	if( nullptr != pObject )
	{
		SVObjectNotifyTemplate< SVNotifyData >* pCommand = dynamic_cast< SVObjectNotifyTemplate< SVNotifyData >* >( pObject );

		if( nullptr != pCommand )
		{
			Result = pCommand->ProcessNotifyData( rData );
		}
	}

	return Result;
}

template< typename ObjectVisitor>
HRESULT SVObjectManagerClass::VisitElements( ObjectVisitor& rVisitor, const SVGUID& rStartingObjectID )
{
	HRESULT Result = S_OK;

	GUID StartingObjectID = rStartingObjectID;
	if( GUID_NULL == StartingObjectID )
	{
		//Set to configuration as this used to be the start.
		StartingObjectID = GetChildRootObjectID( SvOl::FqnConfiguration );
	}
	SVObjectClass* pObject = GetObject( StartingObjectID );

	if( nullptr != pObject )
	{
		HRESULT Temp = S_OK;

		SVObjectClass::SVObjectPtrDeque PreObjects = pObject->GetPreProcessObjects();

		SVObjectClass::SVObjectPtrDeque::iterator PreIter;

		for( PreIter = PreObjects.begin(); PreObjects.end() != PreIter; ++PreIter )
		{
			SVObjectClass* pPreObject = *PreIter;

			if( nullptr != pPreObject )
			{
				Temp = VisitElements( rVisitor, pPreObject->GetUniqueObjectID() );

				if( S_OK == Result )
				{
					Result = Temp;
				}
			}
		}

		Temp = pObject->Accept( rVisitor );

		if( S_OK == Result )
		{
			Result = Temp;
		}

		SVObjectClass::SVObjectPtrDeque PostObjects = pObject->GetPostProcessObjects();

		SVObjectClass::SVObjectPtrDeque::iterator PostIter;

		for( PostIter = PostObjects.begin(); PostObjects.end() != PostIter; ++PostIter )
		{
			SVObjectClass* pPostObject = *PostIter;

			if( nullptr != pPostObject )
			{
				Temp = VisitElements( rVisitor, pPostObject->GetUniqueObjectID() );

				if( S_OK == Result )
				{
					Result = Temp;
				}
			}
		}
	}

	return Result;
}

template< typename SVDataType >
HRESULT SVObjectManagerClass::UpdateObserver( const SVGUID& rObserverID, const SVDataType& rData )
{
	HRESULT Result = E_FAIL;

	SVObjectClass* pObject = GetObject( rObserverID );

	if( nullptr != pObject )
	{
		SVObserverTemplate< SVDataType >* pObserver = dynamic_cast<SVObserverTemplate<SVDataType>*> (pObject);

		if( nullptr != pObserver )
		{
			Result = pObserver->ObserverUpdate( rData );
		}
	}

	return Result;
}

template< typename SVDataType >
HRESULT SVObjectManagerClass::UpdateObserver( long Cookie, const SVDataType& rData )
{
	HRESULT Result = E_FAIL;

	SVCookieEntryStructPtr pCookie = GetCookieEntry( Cookie );

	if( !( pCookie.empty() ) )
	{
		SVObserverTemplate<SVDataType>* pObserver = dynamic_cast< SVObserverTemplate<SVDataType>* > ( pCookie->m_pFunctor.get() );

		if( nullptr != pObserver )
		{
			Result = pObserver->ObserverUpdate( rData );
		}
	}

	return Result;
}

template< typename SVDataType >
HRESULT SVObjectManagerClass::UpdateObservers( const std::string& rSubjectDataName, const SVGUID& rSubjectID, const SVDataType& rData )
{
	HRESULT Result = S_OK;

	SVSubjectEnabledObserverMap Observers;
	SVSubjectEnabledCookieMap Cookies;

	Result = GetObservers( rSubjectDataName, rSubjectID, Observers, Cookies );

	if( S_OK == Result )
	{
		SVSubjectEnabledObserverMap::iterator Iter = Observers.begin();

		for( ; Observers.end() != Iter;  ++Iter )
		{
			if( Iter->second == 1 )
			{
				HRESULT Temp = UpdateObserver( Iter->first, rData );

				if( S_OK == Result )
				{
					Result = Temp;
				}
			}
		}

		SVSubjectEnabledCookieMap::iterator CookieIter = Cookies.begin();

		for( ; Cookies.end() != CookieIter;  ++CookieIter )
		{
			if( CookieIter->second == 1 )
			{
				HRESULT Temp = UpdateObserver( CookieIter->first, rData );

				if( S_OK == Result )
				{
					Result = Temp;
				}
			}
		}
	}

	return Result;
}

template< typename DataType >
bool SVObjectManagerClass::createBucket( std::unique_ptr<std::vector<DataType>>& rpBucket, const DataType& rDefault, int NumberOfBuckets )
{
	if(nullptr != rpBucket.get())
	{
		rpBucket.reset();
	}
	
	try
	{
		rpBucket = std::unique_ptr<std::vector<DataType>> (new std::vector<DataType>);
		rpBucket->resize( NumberOfBuckets, rDefault );
		return true;
	}
	catch( std::exception& )
	{
		return false;
	}
}

template<typename ObjectVisitor>
HRESULT SvOi::visitElements( ObjectVisitor& rVisitor, const SVGUID& rStartingObjectID )
{
	return SVObjectManagerClass::Instance().VisitElements(rVisitor, rStartingObjectID );
}

