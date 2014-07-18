//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RootObject
//* .File Name       : $Workfile:   RootObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Jul 2014 17:56:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "RootObject.h"

#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVObjectLibrary\SVObjectLibrary.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( RootObject, RootObjectGuid );
#pragma endregion Declarations

#pragma region Constructor
RootObject::RootObject( LPCSTR ObjectName )
: SVObjectClass( ObjectName )
, m_EnvironmentObject()
, m_pConfigurationObject(nullptr)

{
	createEnvironmentObject();
	createConfigurationObject();
}

RootObject::RootObject( SVObjectClass* POwner, int StringResourceID )
: SVObjectClass( POwner, StringResourceID )
, m_EnvironmentObject()
, m_pConfigurationObject(nullptr)
{
	createEnvironmentObject();
	createConfigurationObject();
}

RootObject::~RootObject()
{
	destroyConfigurationObject();
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT RootObject::GetChildObject( SVObjectClass*& p_rpObject, const SVObjectNameInfo& p_rNameInfo, long p_Index ) const
{
	HRESULT l_Status = S_OK;

	return l_Status;
}

BOOL RootObject::createConfigurationObject()
{
	destroyConfigurationObject();
	m_pConfigurationObject = new SVConfigurationObject;
	SVObjectManagerClass::Instance().setRootChildID(SVObjectManagerClass::Configuration, m_pConfigurationObject->GetUniqueObjectID());
	m_pConfigurationObject->SetObjectOwner(this);

	return TRUE;
}
#pragma endregion Public Methods

#pragma region Private Methods
BOOL RootObject::createEnvironmentObject()
{
	SVObjectManagerClass::Instance().setRootChildID(SVObjectManagerClass::Environment, m_EnvironmentObject.GetUniqueObjectID());
	m_EnvironmentObject.SetObjectOwner(this);
	return TRUE;
}

BOOL RootObject::destroyConfigurationObject()
{
	if(nullptr != m_pConfigurationObject)
	{
		delete m_pConfigurationObject;
		m_pConfigurationObject = nullptr;
	}
	return TRUE;
}
#pragma endregion Private Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\RootObject.cpp_v  $
 * 
 *    Rev 1.1   17 Jul 2014 17:56:30   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Mar 2014 15:10:24   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/