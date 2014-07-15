//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAcquisition
//* .File Name       : $Workfile:   SVToolAcquisition.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   14 Jul 2014 12:44:44  $
//******************************************************************************

#include "stdafx.h"
#include "SVToolAcquisition.h"
#include "SVStatusLibrary/SVStatusCodes.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************


//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************



//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVAcquisitionToolClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************

SV_IMPLEMENT_CLASS( SVAcquisitionToolClass, SVAcquisitionToolClassGuid );

//******************************************************************************
// Constructor(s):
//******************************************************************************

SVAcquisitionToolClass::SVAcquisitionToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
					   :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}


void SVAcquisitionToolClass::init()
{
	// Indentify our type in the Output List
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType = SVToolAcquisitionObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &mainImageObject, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	// Register SourceImageNames Value Object
	RegisterEmbeddedObject( &m_svSourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );

	addDefaultInputObjects();
}


SVAcquisitionToolClass::~SVAcquisitionToolClass()
{
}


BOOL SVAcquisitionToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVToolClass::CreateObject( PCreateStructure ) )
	{
		bOk = ( SetImageExtent( 1, mainImageObject.GetImageExtents() ) == S_OK );
		mainImageObject.ObjectAttributesAllowedRef() |= SV_PUBLISH_RESULT_IMAGE;
	}

	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	isCreated = bOk;

	return bOk;
}

HRESULT SVAcquisitionToolClass::ResetObject()
{
	HRESULT l_hrOk = SVToolClass::ResetObject();
	HRESULT l_Temp = SetImageExtent( 1, mainImageObject.GetImageExtents() );

	if( l_hrOk == S_OK )
	{
		l_hrOk = l_Temp;
	}

	UpdateImageWithExtent( 1 );

	return l_hrOk;
}

BOOL SVAcquisitionToolClass::OnValidate()
{
	BOOL bRetVal = FALSE;

	bRetVal = SVToolClass::OnValidate();

	bRetVal &= mainImageObject.GetCamera() != NULL;

	if( !bRetVal )
	{
		SetInvalid();
	}

	return bRetVal;	
}

HRESULT SVAcquisitionToolClass::DoesObjectHaveExtents() const
{
	return SV_SUCCESS_SVOBSERVER_2000_HAS_EXTENTS;
}

BOOL SVAcquisitionToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	BOOL bRetVal = SVToolClass::onRun( RRunStatus );

	CollectInputImageNames( RRunStatus );

	return bRetVal;
}

DWORD_PTR SVAcquisitionToolClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	return( SVToolClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}


SVCameraImageTemplate* SVAcquisitionToolClass::GetMainImageClass()
{
	return &mainImageObject;
}

// Set String value object for Source Image Names
HRESULT SVAcquisitionToolClass::CollectInputImageNames( SVRunStatusClass& RRunStatus )
{
	CString l_strName = mainImageObject.GetCompleteObjectName();

	m_svSourceImageNames.SetValue( RRunStatus.m_lResultDataIndex, 0, l_strName );

	return S_OK;
}

HRESULT SVAcquisitionToolClass::GetInputImageNames( SVStringValueObjectClass*& p_pSourceNames )
{
	p_pSourceNames = &m_svSourceImageNames;
	return S_OK;
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolAcquisition.cpp_v  $
 * 
 *    Rev 1.3   14 Jul 2014 12:44:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed CreateObject to add the SV_PUBLISH_RESULT_IMAGE attribute
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 May 2014 13:10:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed processMessage signature to use DWORD_PTR.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:18:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:32:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   28 Sep 2011 13:16:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   19 Apr 2011 10:23:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated tool to fix extent issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   13 Apr 2011 15:35:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   30 Mar 2011 16:28:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   21 Mar 2011 12:19:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   08 Dec 2010 13:45:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   01 Jun 2010 15:16:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   07 Mar 2008 08:08:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Code to collect source Image Names for Image Tools.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   26 Jan 2006 23:50:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent information to set tool input image to NULL.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   06 Oct 2005 09:45:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to handle the issues when a camera gets removed from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Feb 2005 07:22:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changes to support new Reset Methodology.
 * Changes to support new Extents. Added OnUpdateFigure(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 16:37:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Apr 2003 13:29:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  339
 * SCR Title:  Deleting a camera used by an Acquisition Tool causes problems
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added method GetMainImageClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Jan 2003 08:56:26   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CreateObject method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Dec 2002 20:14:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Nov 2002 13:58:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Add MainImageObject.destroy in CreateObject
 * Removed run, OnDraw, calls to createObject in processMessage
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Feb 2000 00:00:32   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Initial Check In.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
