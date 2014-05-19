//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVROI
//* .File Name       : $Workfile:   SVROI.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:40:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVROI.h"

#include "SVImageLibrary/SVDrawContext.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"

#include "SVImageClass.h"
#include "SVTool.h"

SV_IMPLEMENT_CLASS( SVROIClass, SVROIClassGuid )

SVROIClass::SVROIClass( SVObjectClass* POwner, int StringResourceID )
		   :SVTaskObjectClass( POwner, StringResourceID ) 
{
	// Identify yourself
	outObjectInfo.ObjectTypeInfo.ObjectType = SVROIObjectType;

	// Identify our input type needs
	inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageObjectInfo, _T( "ROIImage" ) );


	// Register Embedded Objects

	// Set Embedded defaults

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

SVROIClass::~SVROIClass()
{

}

BOOL SVROIClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) )
	{
		if( getInputImage() != NULL )
		{
			// Image input must already exist, and must be created!!!
			
			bOk = TRUE;
		}
	}

	isCreated = bOk;

	return bOk;
}

BOOL SVROIClass::CloseObject()
{
	return SVTaskObjectClass::CloseObject();
}

DWORD_PTR SVROIClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

SVImageClass *SVROIClass::getInputImage()
{
	SVImageClass *l_psvImage = NULL;

	if( inputImageObjectInfo.IsConnected() )
	{
		l_psvImage = dynamic_cast<SVImageClass *>(inputImageObjectInfo.GetInputObjectInfo().PObject);
	}

	return l_psvImage;
}

BOOL SVROIClass::OnValidate()
{
	BOOL bRetVal = getInputImage() != NULL;

	bRetVal = bRetVal && SVTaskObjectClass::OnValidate();

	// Note: Make sure this is called when Validate fails !!!
	if( ! bRetVal )
	{
		SetInvalid();
	}

	return bRetVal;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVROI.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 12:40:08   tbair
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
 *    Rev 1.1   01 Feb 2014 12:04:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:00:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   12 Jul 2012 15:05:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   19 Jun 2012 14:12:36   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
   Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   04 Nov 2010 14:11:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with image processing and display image processing classes and associated includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   01 Jun 2010 15:08:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   16 Dec 2009 12:34:24   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   17 Feb 2005 15:19:22   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modifications to support the new extents which involves removing the SVExtentHolder class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   16 Feb 2005 15:12:36   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   22 Apr 2003 15:11:52   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   30 Jan 2003 16:21:48   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated processMessage method to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   14 Jan 2003 14:07:36   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     changed the class to inherit from SVExtentHolder instead of SVTaskObject
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   10 Jan 2003 15:53:04   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added new function VerifyImageForOverlay to determine which object has overlays on which image.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   09 Dec 2002 20:04:38   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   20 Nov 2002 13:13:32   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  272
   SCR Title:  Outputs do not always get set correctly.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed references to System
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
