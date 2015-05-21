//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ResultViewToolReferences
//* .File Name       : $Workfile:   ResultViewToolReferences.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Jan 2015 17:35:06  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "ResultViewToolReferences.h"
#include "SVToolSet.h"
#pragma endregion Includes

#pragma region Constructor
ResultViewToolReferences::ResultViewToolReferences()
{
	m_BranchName = CTAG_VIEWEDTOOLVARIABLES;
}

ResultViewToolReferences::~ResultViewToolReferences()
{
}
#pragma endregion Constructor

#pragma region Public Methods
void ResultViewToolReferences::BuildReferenceVector(SVInspectionProcess* pInspection)
{
	SVToolSetClass* pToolSet = nullptr;
	if( nullptr != pInspection )
	{
		pToolSet = pInspection->GetToolSet();
	}

	if(nullptr == pToolSet)
	{
		ASSERT(FALSE);
		return;
	}

	m_ReferenceVector.clear();
	m_ResultViewItemDefList.clear();

	SVObjectReferenceVector RefVector;

	// Find all outputs marked as selected for viewing
	SVOutputInfoListClass OutputList;
	pToolSet->GetOutputList( OutputList );
	OutputList.GetSetAttributesList( SV_VIEWABLE, RefVector );

	for(SVObjectReferenceVector::const_iterator iter = RefVector.begin(); iter != RefVector.end(); ++iter)
	{
		if( nullptr != iter->Object() && iter->ObjectAttributesSet() & SV_VIEWABLE )
		{
			m_ReferenceVector.push_back(*iter);
			ResultViewItemDef item(*iter);
			m_ResultViewItemDefList.push_back(item);
		}
	}

	m_LastUpdateTimeStamp = SVClock::GetTimeStamp();
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ResultViewToolReferences.cpp_v  $
 * 
 *    Rev 1.0   07 Jan 2015 17:35:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/