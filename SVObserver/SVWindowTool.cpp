//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVWindowTool
//* .File Name       : $Workfile:   SVWindowTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   15 May 2014 15:03:02  $
//******************************************************************************

#include "stdafx.h"
#include "SVWindowTool.h"

#include "SVAnalyzer.h"
#include "SVInspectionProcess.h"
#include "SVLUTEquation.h"
#include "SVLUTOperator.h"
#include "SVObserver.h"
#include "SVUnaryImageOperatorList.h"
#include "SVOLicenseManager/SVOLicenseManager.h"
#include "SVThresholdClass.h"
#include "SVUserMaskOperatorClass.h"
#include "ToolSizeAdjustTask.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"

SV_IMPLEMENT_CLASS( SVWindowToolClass, SVWindowToolClassGuid );

SVWindowToolClass::SVWindowToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
	:SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

void SVWindowToolClass::init()
{
	BuildEmbeddedObjectList ();

	BuildAvailableAnalyzerList (); 

	AddUnaryImageOperatorListAsChild ();

	// Set up your type... in this case this will reference that this tool is a 
	// Window Tool.
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVWindowToolObjectType;
}

void SVWindowToolClass::BuildEmbeddedObjectList ()
{
	// Register Embedded Object
	RegisterEmbeddedObject( &m_svSourceImageNames, 
							SVSourceImageNamesGuid, 
							IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, 
							false, 
							SVResetItemTool );

}


void SVWindowToolClass::BuildAvailableAnalyzerList ()
{
	// Default taskObjectList items:

	// Populate the available analyzer list
	SVClassInfoStruct analyzerClassInfo;

	// Add the Blob analyzer
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType = SVBlobAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVBlobAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVBLOBANALYZER );
	availableChildren.Add( analyzerClassInfo );

	// Add the White Pixel Analyzer
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType = SVPixelAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVPixelAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVPIXELANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Luminance Analyzer
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType = SVLuminanceAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLuminanceAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLUMINANCEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Histogram Analyzer
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType = SVHistogramAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVHistogramAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVHISTOGRAMANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Sri 04-12-00
	// Add the Pattern Analyzer	
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType = SVPatternAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVPatternAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVPATTERNANALYZER );
	availableChildren.Add(analyzerClassInfo);

	if ( TheSVOLicenseManager().HasMatroxIdentificationLicense() )
	{
		// Add the BarCode Analyzer
		analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
		analyzerClassInfo.ObjectTypeInfo.SubType = SVBarCodeAnalyzerObjectType;
		analyzerClassInfo.ClassId = SVBarCodeAnalyzerClassGuid;
		analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVBARCODEANALYZER );
		availableChildren.Add(analyzerClassInfo);
	}

	//
	// The Fast OCR Analyzer (Wit Library) is loaded only if needed.
	//
	if ( TheSVOLicenseManager().HasFastOCRLicense() )
	{
		analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
		analyzerClassInfo.ObjectTypeInfo.SubType = SVOCRAnalyzerObjectType;
		analyzerClassInfo.ClassId = SVOCRAnalyzerClassGuid;
		analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVOCRANALYZER );
		availableChildren.Add(analyzerClassInfo);

		analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
		analyzerClassInfo.ObjectTypeInfo.SubType = SVOCRGrayAnalyzerObjectType;
		analyzerClassInfo.ClassId = SVOCRGrayAnalyzerClassGuid;
		analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVOCRGRAYANALYZER );
		availableChildren.Add(analyzerClassInfo);
	}// end if

	if ( TheSVOLicenseManager().HasMatroxIdentificationLicense() )
	{
		// Add the Matrox OCV Analyzer	
		analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
		analyzerClassInfo.ObjectTypeInfo.SubType = SVOCVAnalyzerObjectType;
		analyzerClassInfo.ClassId = SVOCVAnalyzerClassGuid;
		analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVOCVANALYZER );
		availableChildren.Add(analyzerClassInfo);
	}
}

void SVWindowToolClass::AddUnaryImageOperatorListAsChild ()
{
	// Build an operator list...
	// ...use Standard image operator list, because we need an output image! RO_20Mar2000
	SVUnaryImageOperatorListClass* pOperatorList = new SVStdImageOperatorListClass;

	// Operator list defaults:
	if( pOperatorList )
	{
		// Requires a SVThresholdClass Object
		pOperatorList->Add( new SVThresholdClass( pOperatorList ) );

		// and Requires a SVUsermaskOperatorClass Object
		pOperatorList->Add( new SVUserMaskOperatorClass( pOperatorList ) );

		// and Requires a SVLUTOperatorClass Object
		pOperatorList->Add( new SVLUTOperatorClass( pOperatorList ) );

		// Add the UnaryImageOperatorList to the Tool's List
		Add( pOperatorList );
	}
	
	ToolSizeAdjustTask::AddToFriendlist(this,true,true,true);
	
}

SVWindowToolClass::~SVWindowToolClass()
{ 
}


BOOL SVWindowToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVToolClass::CreateObject( PCreateStructure );
	
	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;
	
	if(bOk)
	{
		bOk  = (S_OK == ToolSizeAdjustTask::EnsureInFriendList(this,true,true,true)); 
	}
	
	isCreated = bOk;
	return bOk;
}

BOOL SVWindowToolClass::SetDefaultFormulas()
{
	BOOL bRetVal = TRUE;

	// Set Default Formula of LUTEquation, if any...
	SVObjectTypeInfoStruct lutEquationInfo;
	lutEquationInfo.ObjectType	= SVEquationObjectType;
	lutEquationInfo.SubType		= SVLUTEquationObjectType;
	SVLUTEquationClass* pLUTEquation = reinterpret_cast<SVLUTEquationClass*>(::SVSendMessage( this, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&lutEquationInfo) ));
	if( pLUTEquation )
	{
		bRetVal = pLUTEquation->SetDefaultFormula() && bRetVal;
	}

	return bRetVal;
}


HRESULT SVWindowToolClass::SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent )
{
	HRESULT l_hrOk = m_svToolExtent.ValidExtentAgainstParentImage( p_svImageExtent );

	if( l_hrOk == S_OK )
	{
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, p_svImageExtent );
	}

	return l_hrOk;
}

HRESULT SVWindowToolClass::SetImageExtentToParent(unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;
	SVImageExtentClass l_NewExtent;

	l_hrOk = m_svToolExtent.UpdateExtentToParentExtents( p_ulIndex, l_NewExtent );

	if( l_hrOk == S_OK )
	{
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, l_NewExtent );
	}
	return l_hrOk;
}


HRESULT SVWindowToolClass::SetImageExtentToFit( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent )
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = m_svToolExtent.UpdateExtentAgainstParentImage( p_ulIndex, p_svImageExtent );

	return l_hrOk;
}

SVTaskObjectClass *SVWindowToolClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	SVImageExtentClass l_svExtents;

	SVTaskObjectClass *l_psvObject = NULL;

	if( m_svToolExtent.GetImageExtent( l_svExtents ) == S_OK &&
		l_svExtents.GetLocationPropertyAt( p_rsvPoint ) != SVExtentLocationPropertyUnknown )
	{
		l_psvObject = this;
	}

	return l_psvObject;
}

bool SVWindowToolClass::DoesObjectHaveExtents() const
{
	return true;
}

SVStaticStringValueObjectClass* SVWindowToolClass::GetInputImageNames()
{
	return &m_svSourceImageNames;
}

HRESULT SVWindowToolClass::ResetObject()
{
	HRESULT l_Status = SVToolClass::ResetObject();

	//SetExtendPropertyAutoReset();

	UpdateImageWithExtent( 1 );

	return l_Status;
}

BOOL SVWindowToolClass::IsValid()
{
	BOOL l_bValid = TRUE;
	for( SVTaskObjectPtrVector::iterator it = m_aTaskObjects.begin() ; it != m_aTaskObjects.end() ; ++it)
	{
		SVTaskObjectClass* pTask = *it;
		l_bValid &= pTask->OnValidate();
	}


	ToolSizeAdjustTask* pToolSizeAdjustTask = nullptr;
	pToolSizeAdjustTask = ToolSizeAdjustTask::GetToolSizeAdjustTask(this);
	if(pToolSizeAdjustTask)
	{
		l_bValid = l_bValid &&  pToolSizeAdjustTask->OnValidate();
	}
	
	return SVToolClass::IsValid() & l_bValid ;
}

BOOL SVWindowToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	return SVToolClass::onRun( RRunStatus );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVWindowTool.cpp_v  $
* 
*    Rev 1.6   15 May 2014 15:03:02   sjones
* Project:  SVObserver
* Change Request (SCR) nbr:  852
* SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
* Checked in by:  tBair;  Tom Bair
* Change Description:  
*   Revised SVSendMessage to use DWORD_PTR
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.5   01 Feb 2014 12:26:10   tbair
* Project:  SVObserver
* Change Request (SCR) nbr:  852
* SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
* Checked in by:  tBair;  Tom Bair
* Change Description:  
*   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.4   13 Aug 2013 10:44:18   bWalter
* Project:  SVObserver
* Change Request (SCR) nbr:  814
* SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
* Checked in by:  bWalter;  Ben Walter
* Change Description:  
*   Added include of SVThresholdClass.h and SVUserMaskOperatorClass.h.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
*    Rev 1.3   15 May 2013 19:46:50   bWalter
* Project:  SVObserver
* Change Request (SCR) nbr:  814
* SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
* Checked in by:  bWalter;  Ben Walter
* Change Description:  
*   Merged with svo_src label SVO 6.10 Beta 016.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
Rev 3.33   14 May 2013 10:32:50   tbair
Project:  SVObserver
Change Request (SCR) nbr:  796
SCR Title:  Add a License Manager that will handle the Matrox Licenses
Checked in by:  rYoho;  Rob Yoho
Change Description:  
Added IsValid override to check task objects.

/////////////////////////////////////////////////////////////////////////////////////

*    Rev 1.2   07 May 2013 08:29:20   bWalter
* Project:  SVObserver
* Change Request (SCR) nbr:  814
* SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
* Checked in by:  bWalter;  Ben Walter
* Change Description:  
*   Merged with svo_src label SVO 6.10 Beta 011.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
Rev 3.32   19 Apr 2013 09:26:32   tbair
Project:  SVObserver
Change Request (SCR) nbr:  827
SCR Title:  Add button to the adjust tool size/position to set size to parent image.
Checked in by:  tBair;  Tom Bair
Change Description:  
Added Full Image button support to Window Tool by added trait AllowResizeToParent and override UpdateExtentToParentExtents.

/////////////////////////////////////////////////////////////////////////////////////

*    Rev 1.1   06 May 2013 14:38:34   bWalter
* Project:  SVObserver
* Change Request (SCR) nbr:  814
* SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
* Checked in by:  bWalter;  Ben Walter
* Change Description:  
*   Merged with svo_src label SVO 6.10 Beta 009.
* 
* /////////////////////////////////////////////////////////////////////////////////////
* 
Rev 3.31   10 Apr 2013 14:55:24   ryoho
Project:  SVObserver
Change Request (SCR) nbr:  796
SCR Title:  Add a License Manager that will handle the Matrox Licenses
Checked in by:  rYoho;  Rob Yoho
Change Description:  
when adding analyzer, check to see if the LicenseManager has FastOCRLicense for the Ocr analyzers

/////////////////////////////////////////////////////////////////////////////////////

*    Rev 1.0   23 Apr 2013 16:31:42   bWalter
* Project:  SVObserver
* Change Request (SCR) nbr:  814
* SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
* Checked in by:  bWalter;  Ben Walter
* Change Description:  
*   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
* 
* /////////////////////////////////////////////////////////////////////////////////////

Rev 3.30   27 Feb 2013 11:28:16   ryoho
Project:  SVObserver
Change Request (SCR) nbr:  796
SCR Title:  Add a License Manager that will handle the Matrox Licenses
Checked in by:  rYoho;  Rob Yoho
Change Description:  
Put checks around the barcode analyzer and Matrox OCV analyzer so they will not show if the license is not present

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.29   25 Jul 2012 14:55:00   jspila
Project:  SVObserver
Change Request (SCR) nbr:  769
SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
Checked in by:  Joe;  Joe Spila
Change Description:  
Removed dead code.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.28   16 Jul 2012 13:04:20   jspila
Project:  SVObserver
Change Request (SCR) nbr:  769
SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
Checked in by:  Joe;  Joe Spila
Change Description:  
Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.27   02 Jul 2012 17:52:00   jspila
Project:  SVObserver
Change Request (SCR) nbr:  769
SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
Checked in by:  Joe;  Joe Spila
Change Description:  
Updated source code to promote new display functionality.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.26   30 Mar 2011 16:36:12   jspila
Project:  SVObserver
Change Request (SCR) nbr:  712
SCR Title:  Fix issues with black images when using command interface (SIAC)
Checked in by:  Joe;  Joe Spila
Change Description:  
Updated code to fix performance problems with the extent changes.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.25   21 Mar 2011 12:20:52   jspila
Project:  SVObserver
Change Request (SCR) nbr:  712
SCR Title:  Fix issues with black images when using command interface (SIAC)
Checked in by:  Joe;  Joe Spila
Change Description:  
Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.24   16 Mar 2011 14:01:22   jspila
Project:  SVObserver
Change Request (SCR) nbr:  712
SCR Title:  Fix issues with black images when using command interface (SIAC)
Checked in by:  Joe;  Joe Spila
Change Description:  
Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.23   27 Oct 2009 18:53:52   jspila
Project:  SVObserver
Change Request (SCR) nbr:  633
SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
Checked in by:  jSpila;  Joseph Spila
Change Description:  
Updated code to fix issues with disconnecting cameras and invalid image behavior.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.22   30 Jul 2009 13:16:36   jspila
Project:  SVObserver
Change Request (SCR) nbr:  665
SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
Checked in by:  Joe;  Joe Spila
Change Description:  
Merged branched changes into current code branch with appropriate updates.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.21   21 Oct 2008 15:07:28   jhanebach
Project:  SVObserver
Change Request (SCR) nbr:  629
SCR Title:  Add Histogram analyzer
Checked in by:  jHanebach;  Jack Hanebach
Change Description:  
Added Histogram Analyzer

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.20   07 Mar 2008 12:57:26   tbair
Project:  SVObserver
Change Request (SCR) nbr:  616
SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
Checked in by:  tBair;  Tom Bair
Change Description:  
Added functions to support source image names

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.19   25 Jul 2007 07:58:44   tbair
Project:  SVObserver
Change Request (SCR) nbr:  597
SCR Title:  Upgrade Matrox Imaging Library to version 8.0
Checked in by:  tBair;  Tom Bair
Change Description:  
Upgrade to MIL 8.0

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.18   10 Aug 2005 12:49:02   Joe
Project:  SVObserver
Change Request (SCR) nbr:  500
SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
Checked in by:  Joe;  Joe Spila
Change Description:  
Updated methods to improve performance and reduce system delay.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.17   17 Feb 2005 15:13:44   tbair
Project:  SVObserver
Change Request (SCR) nbr:  456
SCR Title:  Update Image and Tool Objects to use the new Extent Classes
Checked in by:  Joe;  Joe Spila
Change Description:  
Made changes to support new Extents and new reset metnodology.
Added SetImageExtent(, GetObjectAtPoint(, onCollectOverlays(.
Removed GetObjectScript(, SetObjectDepth(, onUpdateFigure(.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.16   05 Dec 2003 14:53:04   rschock
Project:  SVObserver
Change Request (SCR) nbr:  389
SCR Title:  Present Error Messages for Invalid Hardware using User Interface
Checked in by:  Joe;  Joe Spila
Change Description:  
Fixed include issue with global variable access.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.15   05 Dec 2003 14:47:28   rschock
Project:  SVObserver
Change Request (SCR) nbr:  389
SCR Title:  Present Error Messages for Invalid Hardware using User Interface
Checked in by:  Joe;  Joe Spila
Change Description:  
Changed code to check different flag before loading Coreco OCR analyzers.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.14   17 Sep 2003 13:02:54   rschock
Project:  SVObserver
Change Request (SCR) nbr:  384
SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
Checked in by:  rSchock;  Rosco Schock
Change Description:  
Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.13   22 Apr 2003 17:30:38   rschock
Project:  SVObserver
Change Request (SCR) nbr:  346
SCR Title:  Update SVObserver to Version 4.21 Release
Checked in by:  Joe;  Joe Spila
Change Description:  
Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.12   31 Jan 2003 09:40:34   joe
Project:  SVObserver
Change Request (SCR) nbr:  301
SCR Title:  Implement Result Image Circle Buffer
Checked in by:  Joe;  Joe Spila
Change Description:  
Updated CreateObject method to use the new image circle buffer methodology.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.11   10 Jan 2003 16:16:02   rschock
Project:  SVObserver
Change Request (SCR) nbr:  305
SCR Title:  Implement the ability to perform RunOnce from a SIAC client
Checked in by:  rSchock;  Rosco Schock
Change Description:  
Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.10   09 Dec 2002 20:22:32   Joe
Project:  SVObserver
Change Request (SCR) nbr:  295
SCR Title:  Remove Result Data from Configuration Printout
Checked in by:  Joe;  Joe Spila
Change Description:  
CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.

/////////////////////////////////////////////////////////////////////////////////////

Rev 3.9   20 Nov 2002 15:02:22   ryoho
Project:  SVObserver
Change Request (SCR) nbr:  226
SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
Checked in by:  rYoho;  Rob Yoho
Change Description:  
Removed Code from GetToolInformationString  but Still calls Parent function

/////////////////////////////////////////////////////////////////////////////////////
*/
//** EOF **
