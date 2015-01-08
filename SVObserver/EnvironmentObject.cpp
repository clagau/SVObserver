+//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : EnvironmentObject
//* .File Name       : $Workfile:   EnvironmentObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   07 Jan 2015 17:33:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "EnvironmentObject.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVObjectLibrary\SVObjectLibrary.h"
#include "SVConfigurationObject.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
	rpObject =  dynamic_cast<SVObjectClass*> ( m_EnvironmentValues.getValueObject( rNameInfo.GetObjectArrayName( Index ).c_str() ) );

	return S_OK;
}

void EnvironmentObject::getEnvironmentObjectNameList(SVStringArray& rObjectNameList, const SVString& rPath, UINT AttributesAllowedFilter) const
{
	SVString path = rPath + _T(".");
	const BasicValueObjects::ValueList& list = m_EnvironmentValues.getValueList();
	BasicValueObjects::ValueList::const_iterator it = list.cbegin();
	for(it = list.cbegin(); it != list.cend(); ++it)
	{
		SVString completeName = (*it)->GetCompleteObjectName();
		if( ((*it)->ObjectAttributesAllowed() & AttributesAllowedFilter) == AttributesAllowedFilter )
		{
			// Check if current object is a sub-object of this path.
			size_t completeSize = completeName.size();
			size_t pathSize = path.size();
			if ( completeSize >= pathSize )
			{
				SVString checkName = completeName.Left( path.size() );
				int diff = checkName.Compare( path );

				if ( rPath.empty() || diff == 0 )
				{
					rObjectNameList.push_back( completeName );
				}
			}
		}
	}
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\EnvironmentObject.cpp_v  $
 * 
 *    Rev 1.5   07 Jan 2015 17:33:50   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Changed conditions in method getEnvironmentObjectNameList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Dec 2014 17:53:30   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added method getEnvironmentObjectNameList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Aug 2014 12:51:52   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fixed: GetChildObject not returning object when it is a node
 * Changed methods:GetChildObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Jul 2014 17:39:40   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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