//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultList.cpp
//* .File Name       : $Workfile:   SVResultList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 13:48:26  $
//******************************************************************************

#include "stdafx.h"
#include "SVResultList.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVGetObjectDequeByTypeVisitor.h"
#include "SVResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


SVResultListClass::SVResultListClass()
: m_LastUpdateTimestamp( 0 )
{
	m_pCurrentResultView = NULL;
}

SVResultListClass::~SVResultListClass()
{
	Destroy();

	m_results.RemoveAll();

}

void SVResultListClass::Destroy()
{
	m_vecObjects.clear();
}

void SVResultListClass::SetView( SVResultViewClass* p_pResultView )
{
	m_pCurrentResultView = p_pResultView;
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

	/*
	SVResultClass* pResult = dynamic_cast <SVResultClass*> ( reinterpret_cast <SVObjectClass*> (::SVSendMessage(pRootObject, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast <DWORD_PTR> (&info) ) ) );
	while( pResult )
	{
		m_results.Add( pResult );
		pResult = dynamic_cast <SVResultClass*> ( reinterpret_cast <SVObjectClass*> (::SVSendMessage(pRootObject, SVM_GETNEXT_OBJECT, reinterpret_cast <DWORD> (pResult), reinterpret_cast <DWORD_PTR> (&info) ) ) );
	}
	*/

	m_vecObjects.clear();

	// Find all outputs marked as selected for viewing
	SVOutputInfoListClass l_OutputList;

	pRootObject->GetOutputList( l_OutputList );

	l_OutputList.GetSetAttributesList( SV_VIEWABLE, m_vecObjects );

	/*
	SVResultView* pView = GetView();
	if( pView )
		pView->Rebuild();
	*/

	m_LastUpdateTimestamp = SVClock::GetTimeStamp();
}

SVResultViewClass* SVResultListClass::GetView()
{
	return m_pCurrentResultView;
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVResultList.cpp_v  $
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
*/

