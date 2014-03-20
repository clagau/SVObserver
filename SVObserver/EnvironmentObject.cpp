//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : EnvironmentObject
//* .File Name       : $Workfile:   EnvironmentObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Mar 2014 15:10:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "EnvironmentObject.h"

#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "SVConfigurationObject.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::SVObserver;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SV_IMPLEMENT_CLASS( EnvironmentObject, EnvironmentObjectGuid );
#pragma endregion Declarations

#pragma region Constructor
EnvironmentObject::EnvironmentObject( LPCSTR ObjectName )
: SVObjectClass( ObjectName )
{
}

EnvironmentObject::EnvironmentObject( SVObjectClass* pOwner, int StringResourceID )
: SVObjectClass( pOwner, StringResourceID )
{
}

EnvironmentObject::~EnvironmentObject()
{
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT EnvironmentObject::GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, long Index ) const
{
	HRESULT Result = S_OK;

	rpObject = NULL;
	BasicValueObjects::ValueList::const_iterator ValueIter;
	SVString Name = rNameInfo.GetObjectArrayName( Index );
	//Names are not allowed to have '.' due to Fully Qualified Name being only one object
	Name.replace('.', ':');
	SVObjectNameInfo DotlessNameInfo;
	DotlessNameInfo.ParseObjectName(Name);
	const BasicValueObjects::ValueList& rValues = m_EnvironmentValues.getValueList();
	for( ValueIter = rValues.begin(); rpObject == NULL && ValueIter != rValues.end(); ++ValueIter )
	{
		(*ValueIter)->GetChildObject(rpObject, DotlessNameInfo, Index);
	}

	return Result;
}

void EnvironmentObject::setVariable(LPCSTR Name, LPCTSTR Value)
{
	bool ValueSet = false;
	CString DotlessName = Name;
	//Replace any . in the name since there is only one object storing the value
	DotlessName.Replace('.', ':');
	m_EnvironmentValues.setValueObject(DotlessName, Value, this);
}

CString EnvironmentObject::getVariable(LPCSTR Name) const
{
	CString Value;
	CString DotlessName = Name;
	//Replace any . in the name since there is only one object storing the value
	DotlessName.Replace('.', ':');
	BasicValueObject* pValue = m_EnvironmentValues.getValueObject(DotlessName);
	if(nullptr == pValue)
	{
		pValue->getValue(Value);
	}
	return Value;
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\EnvironmentObject.cpp_v  $
 * 
 *    Rev 1.0   17 Mar 2014 15:10:22   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
