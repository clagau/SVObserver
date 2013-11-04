//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGageTool
//* .File Name       : $Workfile:   SVGageTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2013 19:45:48  $
//******************************************************************************

#include "stdafx.h"
#include "SVGageTool.h"
#include "SVStatusLibrary/SVStatusCodes.h"
#include "SVLineROI.h"
#include "SVLineAnalyzer.h"
#include "SVInspectionProcess.h"

SV_IMPLEMENT_CLASS( SVGageToolClass, SVGageToolClassGuid );

SVGageToolClass::SVGageToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				:SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : Initialization of class SVGageToolClass
// -----------------------------------------------------------------------------
// .Description : Initialization of newly Instantiated Object
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.08.1999 RO          First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVGageToolClass::init()
{
	m_bToolRegistered = false;

	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVGageToolObjectType;

	// Identify our input type needs
	inputImageObjectInfo.SetInputObjectType( SVImageObjectType );
	inputImageObjectInfo.SetObject( GetObjectInfo() );
	RegisterInputObject( &inputImageObjectInfo, _T( "GageToolImage" ) );

	m_svToolExtent.SetImageType( SVImageTypeVirtual );
	m_svToolExtent.SetTranslation( SVExtentTranslationNone );

	// Remove Embedded Extents
	removeEmbeddedExtents( false );

	// Register Embedded Objects
	RegisterEmbeddedObject( &gageImage, SVOutputImageObjectGuid, IDS_OBJECTNAME_IMAGE1 );

	gageImage.InitializeImage( SVImageTypeVirtual );

	// Default taskObjectList items:

	// Populate the available analyzer list
	SVClassInfoStruct analyzerClassInfo;
	
	// Add the Line Pixel Count Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLinePixelCountingAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVPixelCountingLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVPIXELCOUNTINGLINEANALYZER );
	availableChildren.Add( analyzerClassInfo );

	// Add the Line Edge Count Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineEdgeCountingAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVEdgeCountingLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVEDGECOUNTINGLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);
	
	// Add the Line Edge Position Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineEdgePositionAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVEdgePositionLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVEDGEPOSITIONLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Linear Measurement Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineLinearMeasurementAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVLinearMeasurementLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVLINEARMEASUREMENTANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Foreground Object Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineMaximumForegroundObjectAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVMaximumForegroundObjectLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVMAXIMUMFOREGROUNDOBJECTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Background Object Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineMaximumBackgroundObjectAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVMaximumBackgroundObjectLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVMAXIMUMBACKGROUNDOBJECTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add the Line Maximum Object Analyzer...
	analyzerClassInfo.ObjectTypeInfo.ObjectType = SVAnalyzerObjectType;
	analyzerClassInfo.ObjectTypeInfo.SubType	= SVLineMaximumObjectAnalyzerObjectType;
	analyzerClassInfo.ClassId = SVMaximumObjectLineAnalyzerClassGuid;
	analyzerClassInfo.ClassName.LoadString( IDS_CLASSNAME_SVMAXIMUMOBJECTLINEANALYZER );
	availableChildren.Add(analyzerClassInfo);

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}


//******************************************************************************
// Destructor(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : Standard Destructor of class SVGageToolClass
// -----------------------------------------------------------------------------
// .Description : Standard destructor
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.08.1999 RO          First Implementation
////////////////////////////////////////////////////////////////////////////////
SVGageToolClass::~SVGageToolClass()
{ 
	if( GetInspection() != NULL && m_bToolRegistered )
	{
		GetInspection()->GageToolCountDec();

		m_bToolRegistered = false;
	}
}

//******************************************************************************
// Operator(s):
//******************************************************************************


BOOL SVGageToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVToolClass::CreateObject( PCreateStructure );

	if( GetInspection() != NULL && ! m_bToolRegistered )
	{
		GetInspection()->GageToolCountInc();

		m_bToolRegistered = true;
	}

	bOk &= ( gageImage.InitializeImage( getInputImage() ) == S_OK );

	// Return code for UpdateImageWithExtend not being checked because it may not be valid the first time.
	m_svToolExtent.UpdateImageWithExtent( 1, SVToolExtentClass::SVToolExtent );

	isCreated = bOk;

	RemoveEmbeddedObject( &m_svUpdateAuxilliaryExtents );

	// Add Default Inputs and Outputs
	addDefaultInputObjects();

	return bOk;
}

BOOL SVGageToolClass::CloseObject()
{
	if( SVToolClass::CloseObject() )
	{
		if( GetInspection() != NULL && m_bToolRegistered )
		{
			GetInspection()->GageToolCountDec();

			m_bToolRegistered = false;
		}

		BOOL bRetVal = gageImage.CloseObject();

		return bRetVal;
	}
	return FALSE;
}

BOOL SVGageToolClass::OnValidate()
{

	SetInvalid();
	return FALSE;
}

HRESULT SVGageToolClass::DoesObjectHaveExtents() const
{
	return SV_SUCCESS_SVOBSERVER_2000_HAS_EXTENTS;
}

BOOL SVGageToolClass::SetObjectDepth( int NewObjectDepth )
{
	BOOL l_Status = SVToolClass::SetObjectDepth( NewObjectDepth );

	extentLeft.SetObjectDepth( NewObjectDepth );
	extentTop.SetObjectDepth( NewObjectDepth );
	extentRight.SetObjectDepth( NewObjectDepth );
	extentBottom.SetObjectDepth( NewObjectDepth );
	extentWidth.SetObjectDepth( NewObjectDepth );
	extentHeight.SetObjectDepth( NewObjectDepth );

	return l_Status;
}

BOOL SVGageToolClass::SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex )
{
	BOOL l_Status = SVToolClass::SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );

	extentLeft.SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );
	extentTop.SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );
	extentRight.SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );
	extentBottom.SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );
	extentWidth.SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );
	extentHeight.SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );

	return l_Status;
}

HRESULT SVGageToolClass::ResetObject()
{
	extentLeft.ResetObject();
	extentTop.ResetObject();
	extentRight.ResetObject();
	extentBottom.ResetObject();
	extentWidth.ResetObject();
	extentHeight.ResetObject();

	HRESULT l_hrOk = gageImage.InitializeImage( getInputImage() );

	HRESULT l_Temp = S_OK; //= m_svToolExtent.UpdateImageWithExtent( 1, SVToolExtentClass::SVToolExtent );
	m_svToolExtent.UpdateImageWithExtent( 1, SVToolExtentClass::SVToolExtent );

	if( l_hrOk == S_OK )
	{
		l_hrOk = l_Temp;
	}

//	l_Temp = m_svToolExtent.UpdateOffsetData( true );
	m_svToolExtent.UpdateOffsetData( true );

	if( l_hrOk == S_OK )
	{
		l_hrOk = l_Temp;
	}

	l_Temp = SVToolClass::ResetObject();

	if( l_hrOk == S_OK )
	{
		l_hrOk = l_Temp;
	}

	UpdateImageWithExtent( 1 );
	SetInvalid();
	return l_hrOk;
}

HRESULT SVGageToolClass::IsInputImage( SVImageClass *p_psvImage )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_psvImage != NULL )
	{
		if ( p_psvImage == getInputImage() )
		{
			l_hrOk = S_OK;
		}
		else
		{
			l_hrOk = SVToolClass::IsInputImage( p_psvImage );
		}
	}

	return l_hrOk;
}



//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************

SVImageClass* SVGageToolClass::getInputImage()
{
	if( inputImageObjectInfo.IsConnected() && inputImageObjectInfo.GetInputObjectInfo().PObject )
		return ( SVImageClass* ) inputImageObjectInfo.GetInputObjectInfo().PObject;

	return NULL;
}

BOOL SVGageToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	SetInvalid();
	RRunStatus.SetInvalid();
	return FALSE;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVGageTool.cpp_v  $
 * 
 *    Rev 1.2   15 May 2013 19:45:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 016.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.35   10 May 2013 11:34:18   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  810
   SCR Title:  Remove support for Legacy Tools
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Re-added original reset object code and a call to SetInvalid to make sure the tool is displayed with black square after loading.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.1   08 May 2013 16:16:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.34   25 Apr 2013 16:02:00   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  746
   SCR Title:  Fix SVObserver crash due to discrete outputs after deleting a PPQ
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     change ResetObject to return S_FALSE because the tool is no longer able to be used
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 10:39:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   25 Mar 2013 13:35:26   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  810
   SCR Title:  Remove support for Legacy Tools
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     changed OnRun to make tool invalid
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   08 Jan 2013 11:55:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  805
   SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added new virtual method to classes to fix adjusting Object Depth of Value Object and Last Set Index.  Also, updated code with new methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   25 Jul 2012 14:07:10   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed dead code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   12 Jul 2012 14:37:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   19 Jun 2012 12:45:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
   Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   23 Sep 2011 12:59:16   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Ignored return code in Reset Object for Gage tool when in a color system. 
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   21 Sep 2011 08:29:26   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified CreateObject to ignore return code from UpdateImageWithExtent.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   30 Mar 2011 16:20:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to fix performance problems with the extent changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   21 Mar 2011 12:08:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   16 Mar 2011 13:32:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   14 Mar 2011 08:04:36   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source to perform a partial validation of extents to prevent crashing or causing load failure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   11 Mar 2011 14:20:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix extent and connection issues between image classes and tool classes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   01 Mar 2011 10:39:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source to fix issues with not assigning input image to extents.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   25 Feb 2011 12:08:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   08 Dec 2010 08:05:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   04 Nov 2010 13:35:34   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with image processing and display image processing classes and associated includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   01 Jun 2010 14:24:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   30 Jul 2009 11:39:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Merged branced changes into current code branch with appropriate updates.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   07 Apr 2008 08:09:38   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  621
   SCR Title:  Fix Bug in the SVImageClass
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated methods to use the new GetParentImage method instead of the GetParent method to remove confusion with CWnd GetParent method.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   19 Jan 2006 10:26:58   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  534
   SCR Title:  Add Image Select for Auxiliary Extent Source
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated gage tool to not process Aux Extents and it registers itself to the inspection process to not let any tool use Aux Extents.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   16 Jan 2006 17:02:00   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  534
   SCR Title:  Add Image Select for Auxiliary Extent Source
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to change to using new update methodology for the Auxiliary Extents.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   20 Sep 2005 15:10:16   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  510
   SCR Title:  Add source image extents to all image using tools
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Moved removed embedded object to create object method for load of configuration to happen properly.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   09 Sep 2005 15:11:52   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  510
   SCR Title:  Add source image extents to all image using tools
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Removed Auxilliary Extent option from gage tool class and dlg.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   01 Sep 2005 15:45:44   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  510
   SCR Title:  Add source image extents to all image using tools
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Removed return code checking from update source offset.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   01 Sep 2005 12:06:44   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  510
   SCR Title:  Add source image extents to all image using tools
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modifications to UpdateImageData for Auxilliary Extents.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   17 Feb 2005 14:22:38   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   08 Jul 2003 10:42:52   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  374
   SCR Title:  Add check before changing tool parameters when processing input list
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added new code the processing of InputRequests (from SetToolParameterList) to verify that extents have changed or the change is ignored.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   22 Apr 2003 09:59:30   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   05 Mar 2003 12:13:34   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Restructured and recoded the SetToolParameter code. It now uses a standard struct and each SVToolClass based class is responsible for moving itself based on the values passed in through the struct.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   29 Jan 2003 16:01:26   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated CreateObject method to use new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   14 Jan 2003 13:52:02   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added method SetToolPosition
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   09 Dec 2002 19:21:48   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
