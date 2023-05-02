//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	auto [Result, pObject, _] = getObjectByDottedName(rFullName);

	if( nullptr != pObject)
	{
		rpObject = dynamic_cast<SVObjectTypeName*> (pObject);
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
