//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ResultViewPPQInputReferences
//* .File Name       : $Workfile:   ResultViewPPQInputReferences.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Jan 2015 17:35:06  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "ResultViewPPQInputReferences.h"
#include "SVInspectionProcess.h"
#pragma endregion Includes

#pragma region Constructor
ResultViewPPQInputReferences::ResultViewPPQInputReferences()
{
	m_BranchName = CTAG_VIEWEDPPQINPUTS;
}

ResultViewPPQInputReferences::~ResultViewPPQInputReferences()
{
}
#pragma region Constructor

#pragma region Public Methods
void ResultViewPPQInputReferences::BuildReferenceVector(SVInspectionProcess* pInspection)
{
	// Insert all PPQ input items that are picked for viewing
	m_ReferenceVector.clear();
	m_ResultViewItemDefList.clear();
	for( size_t l = 0; pInspection && l < pInspection->m_PPQInputs.size(); ++l )
	{
		SVIOEntryStruct pIOEntry = pInspection->m_PPQInputs[l];
		SVObjectClass * object  = pIOEntry.m_IOEntryPtr->m_pValueObject;
		if( nullptr != object && object->ObjectAttributesSet() & SV_VIEWABLE )
		{
			SVObjectReference ref(object);
			ResultViewItemDef item(ref);
			m_ReferenceVector.push_back(ref);
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ResultViewPPQInputReferences.cpp_v  $
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