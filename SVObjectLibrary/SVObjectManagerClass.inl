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
#include "Definitions/GlobalConst.h"
#include "ObjectInterfaces/IObjectManager.h"
#pragma endregion Includes

template< typename SVObjectTypeName >
HRESULT SVObjectManagerClass::ConstructObject(SvPb::ClassIdEnum classID, SVObjectTypeName*& rpObject)
{
	SVObjectClass* pObject = nullptr;
	rpObject = nullptr;

	HRESULT Result = ConstructObject(classID, pObject);

	if (nullptr != pObject)
	{
		rpObject = dynamic_cast<SVObjectTypeName*> (pObject);

		if (nullptr == rpObject)
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
	return GetRootChildObject( rpObject, SvDef::FqnConfiguration );
}

template< typename ObjectVisitor>
HRESULT SVObjectManagerClass::VisitElements( ObjectVisitor& rVisitor, uint32_t startingObjectID )
{
	HRESULT Result = S_OK;

	if( SvDef::InvalidObjectId == startingObjectID )
	{
		//Set to configuration as this used to be the start.
		startingObjectID = GetChildRootObjectID( SvDef::FqnConfiguration );
	}
	SVObjectClass* pObject = GetObject( startingObjectID );

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
				Temp = VisitElements( rVisitor, pPreObject->getObjectId() );

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
				Temp = VisitElements( rVisitor, pPostObject->getObjectId() );

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
HRESULT SVObjectManagerClass::UpdateObserver( uint32_t observerID, const SVDataType& rData )
{
	HRESULT Result = E_FAIL;

	SVObjectClass* pObject = GetObject( observerID );

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

	if(nullptr != pCookie)
	{
		SVObserverTemplate<SVDataType>* pObserver = dynamic_cast< SVObserverTemplate<SVDataType>* > ( pCookie->m_pFunctor.get() );
		// cppcheck-suppress knownConditionTrueFalse //dynamic_cast can change pObserver to nullptr
		if( nullptr != pObserver )
		{
			Result = pObserver->ObserverUpdate( rData );
		}
	}

	return Result;
}

template< typename SVDataType >
HRESULT SVObjectManagerClass::UpdateObservers( const std::string& rSubjectDataName, uint32_t subjectID, const SVDataType& rData )
{
	HRESULT Result = S_OK;

	SVSubjectEnabledObserverMap Observers;
	SVSubjectEnabledCookieMap Cookies;

	Result = GetObservers( rSubjectDataName, subjectID, Observers, Cookies );

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

template<typename ObjectVisitor>
HRESULT SvOi::visitElements( ObjectVisitor& rVisitor, uint32_t startingObjectID )
{
	return SVObjectManagerClass::Instance().VisitElements(rVisitor, startingObjectID );
}

