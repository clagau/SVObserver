//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObjects
//* .File Name       : $Workfile:   BasicValueObjects.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Mar 2014 15:10:20  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BasicValueObjects.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::SVObserver;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

#pragma region Constructor
BasicValueObjects::BasicValueObjects()
{
	Create();
}

BasicValueObjects::~BasicValueObjects()
{
	Destroy();
}
#pragma endregion Constructor

#pragma region Public Methods
BasicValueObject* BasicValueObjects::getValueObject(LPCSTR Name) const
{
	BasicValueObject* pValue = nullptr;
	ValueList::const_iterator ValueIter;
	for( ValueIter = m_Values.begin(); ValueIter != m_Values.end(); ++ValueIter )
	{
		CString ObjectName = (*ValueIter)->GetName();
		if(ObjectName == Name)
		{
			pValue = (*ValueIter);
		}
	}
	return pValue;
}

#pragma endregion Public Methods

#pragma region Private Methods
BOOL BasicValueObjects::Create()
{
	return TRUE;
}

BOOL BasicValueObjects::Destroy()
{
	ValueList::const_iterator ValueIter;
	for( ValueIter = m_Values.begin(); ValueIter != m_Values.end(); ++ValueIter )
	{
		delete *ValueIter;
	}
	m_Values.clear();
	return TRUE;
}
#pragma endregion Private Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\BasicValueObjects.cpp_v  $
 * 
 *    Rev 1.0   17 Mar 2014 15:10:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
