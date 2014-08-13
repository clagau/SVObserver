//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObjects
//* .File Name       : $Workfile:   BasicValueObjects.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   12 Aug 2014 12:50:12  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BasicValueObjects.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
	ValueList::const_iterator ValueIter = m_Values.begin();
	while( m_Values.end() != ValueIter )
	{
		SVString ObjectName = (*ValueIter)->GetCompleteObjectName();
		size_t position = ObjectName.find( Name );
		if( SVString::npos != position && position + strlen(Name) >= ObjectName.size() )
		{
			pValue = (*ValueIter);
			break;
		}
		++ValueIter;
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
 *    Rev 1.3   12 Aug 2014 12:50:12   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fixed: getValueObject returning object with sub name
 * Changed methods:getValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Jul 2014 17:39:38   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jun 2014 08:51:32   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Changed method getValueObject.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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