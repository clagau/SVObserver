//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : EnvironmentObject
//* .File Name       : $Workfile:   EnvironmentObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Jun 2014 09:02:58  $
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
	BasicValueObject* pBasicValueObject=nullptr;

	rpObject = nullptr;
	pBasicValueObject = m_EnvironmentValues.getValueObject( rNameInfo.GetObjectArrayName( Index ).c_str() );
	
	//Only if it is not a node return the object
	if( ( nullptr != pBasicValueObject ) && !pBasicValueObject->isNode() )
	{
		rpObject = dynamic_cast<SVObjectClass*> (pBasicValueObject);
	}

	return Result;
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\EnvironmentObject.cpp_v  $
 * 
 *    Rev 1.1   02 Jun 2014 09:02:58   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Changed GetChildObject.
 * Removed getVariable and setVariable methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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
