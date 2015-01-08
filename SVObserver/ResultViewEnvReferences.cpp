//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ResultViewEnvReferences
//* .File Name       : $Workfile:   ResultViewEnvReferences.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   07 Jan 2015 17:35:04  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVObjectLibrary/GlobalConst.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "ResultViewEnvReferences.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#pragma endregion Includes

#pragma region Constructor
ResultViewEnvReferences::ResultViewEnvReferences()
{
	m_BranchName = CTAG_VIEWEDENVARIABLES;
}

ResultViewEnvReferences::~ResultViewEnvReferences()
{
}
#pragma endregion Constructor

#pragma region Public Methods
void ResultViewEnvReferences::BuildReferenceVector(SVInspectionProcess* pProcess)
{
	m_ResultViewItemDefList.clear();
	m_ReferenceVector.clear();
	// Add Environment variables
	SVObjectReferenceVector RefVector;
	SVObjectManagerClass::Instance().getTreeList( Seidenader::SVObjectLibrary::FqnEnvironmentMode, RefVector, SV_VIEWABLE );

	// Add ppq variables
	SVPPQObject *ppq = pProcess->GetPPQ();
	if(ppq)
	{
		SVString PPQName = ppq->GetName();
		SVObjectManagerClass::Instance().getTreeList( PPQName, RefVector, SV_VIEWABLE );
	}

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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ResultViewEnvReferences.cpp_v  $
 * 
 *    Rev 1.0   07 Jan 2015 17:35:04   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/