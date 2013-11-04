//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVAnalyzer
//* .File Name       : $Workfile: 
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   13 May 2013 12:36:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVAnalyzerResultDlg.h"
#include "SVOCVAnalyzerResult.h"
#include "SVOCVAnalyzer.h"
#include "SVOCVDialog.h"
#include "SVInspectionProcess.h"
#include "SVToolSet.h"
#include "SVTool.h"
#include "SVOLicenseManager/SVOLicenseManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



SV_IMPLEMENT_CLASS( SVOCVAnalyzerClass, SVOCVAnalyzerClassGuid );

/////////////////////////////////////////////////////////////////////////////
SVOCVAnalyzerClass::SVOCVAnalyzerClass( 
	BOOL BCreateDefaultTaskList, 
	SVObjectClass* POwner, 
	int StringResourceID )
		:SVImageAnalyzerClass( BCreateDefaultTaskList, POwner, StringResourceID ) 
{
	init();

	if ( TheSVOLicenseManager().HasMatroxIdentificationLicense() )
	{
		m_bHasLicenseError = false;
	}
	else
	{
		m_bHasLicenseError = true;
	}

}

////////////////////////////////////////////////////////////////////////////////
void SVOCVAnalyzerClass::init()
{
	outObjectInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVOCVAnalyzerObjectType;

	SVOCVAnalyzeResultClass* pAnalyzerResult = new SVOCVAnalyzeResultClass( this );

	// Children list defaults:
	if( pAnalyzerResult )
	{
		Add( pAnalyzerResult );
	}

}


SVOCVAnalyzerClass::~SVOCVAnalyzerClass()
{
	if ( m_bHasLicenseError )
	{
		m_bHasLicenseError = false;
		TheSVOLicenseManager().RemoveLicenseErrorFromList(GetUniqueObjectID());
	}
		//remove error from LicenseManager
	//delete( pAnalyzerResult ); 
	//Error!!!! Must be ONLY killed by SVResultListClass or PreDestroy()
}



////////////////////////////////////////////////////////////////////////////////
BOOL SVOCVAnalyzerClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	isCreated = SVImageAnalyzerClass::CreateObject( PCreateStructure );
	
	if ( m_bHasLicenseError )
	{
		TheSVOLicenseManager().AddLicenseErrorToList(GetUniqueObjectID());
	}

	return isCreated;
}

HRESULT SVOCVAnalyzerClass::ResetObject()
{
	HRESULT	l_hrOk = SVImageAnalyzerClass::ResetObject();

	return l_hrOk;
}

////////////////////////////////////////////////////////////////////////////////
SVResultClass* SVOCVAnalyzerClass::GetResultObject()
{
	SVResultClass* pAnalyzerResult = NULL;

	// Find the result Object in Our List
	pAnalyzerResult = (SVResultClass *) GetAt(0);

	return( pAnalyzerResult );
}


/////////////////////////////////////////////////////////////////////////////
BOOL SVOCVAnalyzerClass::OnValidate()
{
	BOOL bRetVal = SVImageAnalyzerClass::OnValidate();

	// Note: Make sure this is called when Validate fails !!!
	if (! bRetVal)
	{
		SetInvalid();
		bRetVal = FALSE;
	}
	if( m_bHasLicenseError )
	{
		bRetVal = FALSE;
	}

	return bRetVal;
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL SVOCVAnalyzerClass::onRun( SVRunStatusClass& RRunStatus )
{
	if ( !m_bHasLicenseError )
	{
		if( SVImageAnalyzerClass::onRun( RRunStatus ) )
		{
			return TRUE;
		}
	}
	SetInvalid();
	RRunStatus.SetInvalid();

	return FALSE;
}


BOOL SVOCVAnalyzerClass::IsPtOverResult( CPoint point )
{
	return TRUE;
}

void SVOCVAnalyzerClass::DisplayAnalyzerResult()
{
	SVOCVAnalyzeResultClass* pOCVResult = ( SVOCVAnalyzeResultClass* ) GetResultObject();

	SVAnalyzerResultDlg dlg;
	dlg.svocrbraResults = NULL;
	dlg.psvocvResultArray = &pOCVResult->arrayOCVCharacterResults;
	dlg.DoModal();
}

DWORD SVOCVAnalyzerClass::processMessage( DWORD DwMessageID, DWORD DwMessageValue, DWORD DwMessageContext )
{
	DWORD DwResult = NULL;
	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
	case SVMSGID_RESET_ALL_OBJECTS:
		{
			HRESULT l_ResetStatus = ResetObject();
			if( l_ResetStatus != S_OK )
			{
				ASSERT( SUCCEEDED( l_ResetStatus ) );

				DwResult = SVMR_NO_SUCCESS;
			}
			else
			{
				DwResult = SVMR_SUCCESS;
			}
			break;
		}
	}

	return( SVImageAnalyzerClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCVAnalyzer.cpp_v  $
 * 
 *    Rev 1.2   13 May 2013 12:36:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   02 May 2013 11:21:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 14:38:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   10 Apr 2013 14:53:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   in the destructor if there was a license error remove it from the list.
 * changed CreateObejct to add license error to list instead of OnRun
 * changed OnValidate to only return false if there is a license error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:00:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   25 Mar 2013 12:54:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed license error check in GetResultObject, OnRun now calls TheLicenseManager to add the error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   05 Mar 2013 08:45:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed error string text
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   27 Feb 2013 11:26:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Put in checks to see if it has licnese errors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   25 Jul 2012 14:39:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removal of dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   18 Jul 2012 14:14:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   02 Jul 2012 17:25:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Dec 2010 13:15:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Aug 2005 09:44:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed the use of SVIPDoc::SingleModeRunLoop and replaced it with SVInspectionProcess::RunOnce.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Mar 2005 07:06:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed return code check within ResetObject method and moved validity check to OnValidate.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Feb 2005 07:33:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Mar 2004 13:29:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  412
 * SCR Title:  Fix Character Analyzer to show correct number of character results
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Moved hiding/unhiding of result objects to happen everytime the font file is reloaded.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Mar 2004 13:29:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  412
 * SCR Title:  Fix Character Analyzer to show correct number of character results
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Moved hiding/unhiding of result objects to happen everytime the font file is reloaded.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Sep 2003 13:07:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
//EOF//
