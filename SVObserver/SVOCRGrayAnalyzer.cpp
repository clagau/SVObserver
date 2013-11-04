//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayAnalyzer
//* .File Name       : $Workfile:   SVOCRGrayAnalyzer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:55:20  $
//******************************************************************************

// 19 Jul 1999 - frb.
//
//

#include "stdafx.h"
#include "SVAnalyzerResultDlg.h"
#include "SVOCRGrayAnalyzerResult.h"
#include "SVOCRGrayAnalyzer.h"
#include "SVOCRGrayDialog.h"
#include "SVInspectionProcess.h"
#include "SVToolSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



SV_IMPLEMENT_CLASS( SVOCRGrayAnalyzerClass, SVOCRGrayAnalyzerClassGuid );

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
SVOCRGrayAnalyzerClass::SVOCRGrayAnalyzerClass( 
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
void SVOCRGrayAnalyzerClass::init()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVOCRGrayAnalyzerObjectType;

   SVOCRGrayAnalyzeResultClass* pAnalyzerResult = new SVOCRGrayAnalyzeResultClass;

	// Children list defaults:
	if( pAnalyzerResult )
	{
		Add( pAnalyzerResult );
	}

}


SVOCRGrayAnalyzerClass::~SVOCRGrayAnalyzerClass()
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
BOOL SVOCRGrayAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	isCreated = SVImageAnalyzerClass::CreateObject( PCreateStructure );

	return isCreated;
}



////////////////////////////////////////////////////////////////////////////////
//
//
//
SVResultClass* SVOCRGrayAnalyzerClass::GetResultObject()
{
	SVResultClass* pAnalyzerResult = NULL;

	// Find the result Object in Our List
	pAnalyzerResult = (SVResultClass *)GetAt(0);

	return( pAnalyzerResult );
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL SVOCRGrayAnalyzerClass::OnValidate()
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
BOOL SVOCRGrayAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	if( SVImageAnalyzerClass::onRun( RRunStatus ) )
	{
		return TRUE;

	}
	SetInvalid();
	RRunStatus.SetInvalid();

	return FALSE;
}

BOOL SVOCRGrayAnalyzerClass::IsPtOverResult( CPoint point )
{
	return TRUE;
}

void SVOCRGrayAnalyzerClass::DisplayAnalyzerResult()
{
	SVOCRGrayAnalyzeResultClass* pOCRResult = ( SVOCRGrayAnalyzeResultClass* ) GetResultObject();
	if (pOCRResult->arrayOCRBlobRecords.GetSize() < 0)
		return;

	pOCRResult->arrayOCRBlobRecords.SortArray(SORT_BLOBS_SCORE);

	SVAnalyzerResultDlg dlg;
	dlg.svocrbraResults = &pOCRResult->arrayOCRBlobRecords;
	dlg.psvocvResultArray = NULL;
	dlg.DoModal();

	pOCRResult->arrayOCRBlobRecords.SortArray();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCRGrayAnalyzer.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:55:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   18 Jul 2012 14:14:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   02 Jul 2012 17:23:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   08 Dec 2010 13:15:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   24 Aug 2005 09:43:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed the use of SVIPDoc::SingleModeRunLoop and replaced it with SVInspectionProcess::RunOnce.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   17 Feb 2005 15:01:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Sep 2003 13:02:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   25 Jun 2003 15:03:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  349
 * SCR Title:  Fix Bug I0001101 Enter text in minimum scale field
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   the analyzer will now save the values even if no font file was created
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 12:42:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Jan 2003 15:18:46   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CreateObject method to use the isCreated flag correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Dec 2002 19:46:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Jun 2001 08:30:22   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions: SetupDialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Jul 2000 15:48:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  169
 * SCR Title:  Add Match Score Information to OCR Tools
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   OCR Match Score Updates
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 14:44:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
//EOF//
