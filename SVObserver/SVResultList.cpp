//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultList.cpp
//* .File Name       : $Workfile:   SVResultList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   07 Jan 2015 17:48:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVResultList.h"

#include "SVObjectLibrary/GlobalConst.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVResult.h"
#include "SVInspectionProcess.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVToolSet.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"

#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
SVResultListClass::SVResultListClass():
m_ResultViewReferences(CTAG_VIEWEDRESULTS)
{
}

SVResultListClass::~SVResultListClass()
{
	Destroy();
	m_results.RemoveAll();
}
#pragma endregion Constructor

void SVResultListClass::Destroy()
{
	m_ResultViewReferences.Clear();
}

void SVResultListClass::SetToolSet(SVToolSetClass* pToolSet)
{
	m_pToolSet = pToolSet;
}

void SVResultListClass::Refresh(SVTaskObjectClass* pRootObject)
{
	m_results.RemoveAll();

	// find all result classes
	// pRootObject is a pointer to SVToolSetClass at this time
	SVObjectTypeInfoStruct info;
	info.ObjectType = SVResultObjectType;

	SVGetObjectDequeByTypeVisitor l_Visitor( info );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, pRootObject->GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
	{
		SVResultClass* pResult = dynamic_cast< SVResultClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		m_results.Add( pResult );
	}
	
	m_ResultViewReferences.RebuildReferenceVector(m_pToolSet->GetInspection());
}

SVClock::SVTimeStamp SVResultListClass::getUpdateTimeStamp()
{
	return m_ResultViewReferences.getUpdateTimeStamp();
}

void SVResultListClass::Save(SVObjectWriter& rWriter)
{
	m_ResultViewReferences.Save(rWriter);
}

bool SVResultListClass::LoadViewedVariables(SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent)
{
	m_ResultViewReferences.Clear();
	SVInspectionProcess* pInspec(nullptr);
	if(m_pToolSet)
	{
		pInspec = m_pToolSet->GetInspection();
	}
	
	bool SevenOneCfg(false);
	bool SevenTwoCfg(false);

	SVTreeType::SVBranchHandle htiChild = nullptr;


	if (SVNavigateTreeClass::GetItemBranch(rTree, CTAG_VIEWEDVARIABLES, htiParent, htiChild))
	{

		SevenTwoCfg = m_ResultViewReferences.Load( rTree, htiChild );
		if(SevenTwoCfg == false)
		{
			///older configurations may have a ViewedEnvVariables entry! 
			m_ResultViewReferences.Load( rTree, htiChild, CTAG_VIEWEDENVARIABLES);
			SevenOneCfg = m_ResultViewReferences.Load( rTree, htiChild, CTAG_VIEWEDTOOLVARIABLES);
		}

		
	}
	
	if(!SevenTwoCfg )
	{
		m_ResultViewReferences.InsertFromPPQInputs(pInspec);
	}


	if(!SevenTwoCfg && !SevenOneCfg )
	{
		///older configurations may have no  ViewedToolVariables entry!
		m_ResultViewReferences.InsertFromOutputList(pInspec);
	}

	return true;
}

void SVResultListClass::RebuildReferenceVector(SVInspectionProcess* pInspection )
{
	return m_ResultViewReferences.RebuildReferenceVector(pInspection);
}

HRESULT  SVResultListClass::GetResultData( SVIPResultData& p_rResultData) const
{
	return m_ResultViewReferences.GetResultData( p_rResultData);
}

int SVResultListClass::AddResults(  SVResultsWrapperClass* pSVRWC, LPCTSTR lptitle )
{
	return m_ResultViewReferences.AddResults(pSVRWC,lptitle);
}

HRESULT SVResultListClass::GetResultDefinitions( ResultViewReferences::SVResultDefinitionDeque& rDefinitions )  const
{
	return m_ResultViewReferences.GetResultDefinitions(rDefinitions);
}



SVProductInspectedState SVResultListClass::GetInspectionState()
{
	// Only if both values are FALSE the product is good !!!!
	BOOL masterFailed = FALSE;
	BOOL masterWarned = FALSE;

	for( int i = 0; i < m_results.GetSize(); ++i )
	{
		masterFailed |= m_results.GetAt( i )->IsFailed();
		masterWarned |= m_results.GetAt( i )->IsWarned();
	}

	if( masterFailed )
		return( PRODUCT_INSPECTION_FAILED );

	if( masterWarned )
		return( PRODUCT_INSPECTION_WARNING );

	return( PRODUCT_INSPECTION_PASSED );
}

void SVResultListClass::Clear()
{
	m_ResultViewReferences.Clear();
}

void SVResultListClass::GetNameSet(SVStringSet& SelectedNamesRaw)
{
	m_ResultViewReferences.GetNameSet(SelectedNamesRaw);
}

bool SVResultListClass::Insert(const SVString& param1)
{
	return m_ResultViewReferences.Insert(param1);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVResultList.cpp_v  $
 * 
 *    Rev 1.3   07 Jan 2015 17:48:18   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Add new members for referenced PPQ variables, tool variables, and environment variables.
 * Added method SetToolSet.
 * Removed methods GetView and SetView.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 May 2014 13:48:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:43:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.9   29 Apr 2013 14:27:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  830
   SCR Title:  Consolidate all Time Stamp and Clock Functionality
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 14:56:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   30 Jul 2012 13:05:36   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated the parameter change for the visitor fucntionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   27 Jul 2012 09:01:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed include information and updated overlay collection functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   02 Jul 2012 17:33:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   01 Jun 2010 15:08:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   14 Jul 2005 12:46:00   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented array object capabilities
   cleaned up code
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   22 Apr 2003 14:55:06   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   20 Nov 2002 13:01:02   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  272
   SCR Title:  Outputs do not always get set correctly.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Changed return type of GetInspectionState from
   SVInspectionStateEnum to SVProductInspectedState struct
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   void SVResultListClass::Save(SVObjectWriter& rWriter)
   {
   }
   */