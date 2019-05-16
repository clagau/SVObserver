+//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RootChildren
//* .File Name       : $Workfile:   RootChidren.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   07 Jan 2015 17:33:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "RootChildren.h"

#include "SVConfigurationObject.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVObjectLibrary\SVObjectLibrary.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
RootChildren::RootChildren()
{
}

RootChildren::~RootChildren()
{
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT RootChildren::GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index ) const
{
	rpObject =  dynamic_cast<SVObjectClass*> ( m_RootChildrenValues.getValueObject( rNameInfo.GetObjectArrayName( Index ).c_str() ).get() );

	return S_OK;
}

SvVol::BasicValueObjectPtr RootChildren::getValueObject( LPCTSTR DottedName ) const
{
	return m_RootChildrenValues.getValueObject( DottedName );
}

void RootChildren::getObjectList(SvVol::BasicValueObjects::ValueVector& rObjectList, LPCTSTR DottedBranch, UINT AttributesAllowedFilter ) const
{
	m_RootChildrenValues.getValueList( rObjectList, DottedBranch, AttributesAllowedFilter );
}

HRESULT RootChildren::deleteValue( LPCTSTR DottedName )
{
	return m_RootChildrenValues.deleteValueObject( DottedName );
}

#pragma endregion Public Methods

