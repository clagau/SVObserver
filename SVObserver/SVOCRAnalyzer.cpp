//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRAnalyzer
//* .File Name       : $Workfile:   SVOCRAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:50:38  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRAnalyzerResult.h"
#include "SVOCRAnalyzer.h"
#include "SVOCRDialog.h"
#include "SVInspectionProcess.h"
#include "SVToolSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



SV_IMPLEMENT_CLASS( SVOCRAnalyzerClass, SVOCRAnalyzerClassGuid );

SVOCRAnalyzerClass::SVOCRAnalyzerClass( 
	BOOL BCreateDefaultTaskList, 
	SVObjectClass* POwner, 
	int StringResourceID )
		:SVImageAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCRAnalyzerClass::init()
{

	outObjectInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVOCRAnalyzerObjectType;

   SVOCRAnalyzeResultClass* pAnalyzerResult = new SVOCRAnalyzeResultClass;

	// Children list defaults:
	if( pAnalyzerResult )
	{
		Add( pAnalyzerResult );
	}

}


SVOCRAnalyzerClass::~SVOCRAnalyzerClass()
{
	//delete( pAnalyzerResult ); 
	//Error!!!! Must be ONLY killed by SVResultListClass or PreDestroy()
}



////////////////////////////////////////////////////////////////////////////////
// .Title       : Create
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: SVToolClass*		PTool 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: FALSE
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :25.08.1997 RO			First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
BOOL SVOCRAnalyzerClass::CreateObject( 
	SVObjectLevelCreateStruct* PCreateStructure )
{
	isCreated = SVImageAnalyzerClass::CreateObject( PCreateStructure );

	return isCreated;
}



////////////////////////////////////////////////////////////////////////////////
//
//
//
SVResultClass* SVOCRAnalyzerClass::GetResultObject()
{
	SVResultClass* pAnalyzerResult = NULL;

	// Find the result Object in Our List
	pAnalyzerResult = (SVResultClass *)GetAt(0);

	//ASSERT( GetTool() != NULL );
	//if( pAnalyzerResult == NULL )
	//{
	//	if( pAnalyzerResult	= new SVWhitePixelAnalyzeResultClass( this ) )
	//	{
	//		if( ! pAnalyzerResult->Create( this, GetTool() ) )
	//		{
	//			AfxMessageBox( "Unable to create white pixel Analyze result!" );
	//			pAnalyzerResult->Destroy();
	//			pAnalyzerResult = NULL;
	//		}
	//	}
	//}
	return( pAnalyzerResult );
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL SVOCRAnalyzerClass::OnValidate()
{
	BOOL bRetVal = FALSE;
	//if( inputLongObjectInfo.IsConnected &&
	//	inputLongObjectInfo.InputObjectInfo.PObject )
	//{
		bRetVal = TRUE;
		bRetVal = SVImageAnalyzerClass::OnValidate() && bRetVal;
	//}

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
		SetInvalid();

	return bRetVal;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL SVOCRAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	if( SVImageAnalyzerClass::onRun( RRunStatus ) )
	{
		return TRUE;

	}
	SetInvalid();
	RRunStatus.SetInvalid();

	return FALSE;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCRAnalyzer.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:50:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   18 Jul 2012 14:14:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   02 Jul 2012 17:23:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   08 Dec 2010 13:15:26   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   24 Aug 2005 09:43:46   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  504
   SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed the use of SVIPDoc::SingleModeRunLoop and replaced it with SVInspectionProcess::RunOnce.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   17 Feb 2005 15:01:22   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   25 Jun 2003 15:03:20   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  349
   SCR Title:  Fix Bug I0001101 Enter text in minimum scale field
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     the analyzer will now save the values even if no font file was created
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   22 Apr 2003 12:41:54   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   30 Jan 2003 15:18:46   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated CreateObject method to use the isCreated flag correctly.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   09 Dec 2002 19:46:34   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/