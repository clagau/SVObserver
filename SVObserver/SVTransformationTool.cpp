//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTransformationTool
//* .File Name       : $Workfile:   SVTransformationTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:27:00  $
//******************************************************************************

#include "stdafx.h"
#include "SVTransformationTool.h"
#include "SVEvaluate.h"
#include "SVImageTransform.h"
#include "SVInspectionProcess.h"

SV_IMPLEMENT_CLASS( SVTransformationToolClass, SVTransformationToolClassGuid );

SVTransformationToolClass::SVTransformationToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
						  :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}


void SVTransformationToolClass::init()
{
	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVTransformationToolObjectType;

	// Identify our input type needs

	// Register Embedded Objects

	RegisterEmbeddedObject( &m_svSourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );
		
	// Set Embedded defaults

	// Default taskObjectList items:

	// Setup the evaluation clases
	SVEvaluateClass* pEvaluate = new SVEvaluateTranslationXClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateTranslationYClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateRotationXClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateRotationYClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateRotationAngleClass;
	if( pEvaluate )
		Add( pEvaluate );

	// Add the Image Transformation Class
	SVImageTransformClass* pTransform = new SVImageTransformClass;
	if( pTransform )
		Add( pTransform );

	// Set default inputs and outputs
	addDefaultInputObjects();
	
}

//******************************************************************************
// Destructor(s):
//******************************************************************************

SVTransformationToolClass::~SVTransformationToolClass()
{ 
}

//******************************************************************************
// Operator(s):
//******************************************************************************

BOOL SVTransformationToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVToolClass::CreateObject( PCreateStructure );

	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	isCreated = bOk;

	return bOk;
}

HRESULT SVTransformationToolClass::ResetObject()
{
	HRESULT l_Status = SVToolClass::ResetObject();

	UpdateImageWithExtent( 1 );

	return l_Status;
}


SVTaskObjectClass *SVTransformationToolClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
{
	return NULL;
}

bool SVTransformationToolClass::DoesObjectHaveExtents() const
{
	return true;
}

HRESULT SVTransformationToolClass::IsInputImage( SVImageClass *p_psvImage )
{
	return S_FALSE;
}

HRESULT SVTransformationToolClass::UpdateImageWithExtent( unsigned long p_Index )
{
	HRESULT l_Status = S_OK;

	l_Status = m_svToolExtent.UpdateImageWithExtent( p_Index, SVToolExtentClass::SVTransformationToolExtent );

	return l_Status;
}

HRESULT SVTransformationToolClass::GetInputImageNames( SVStringValueObjectClass*& p_pSourceNames )
{
	p_pSourceNames = &m_svSourceImageNames;
	return S_OK;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTransformationTool.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 12:27:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   30 Mar 2011 16:28:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   21 Mar 2011 12:19:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   16 Mar 2011 13:59:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   11 Mar 2011 14:33:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix extent and connection issues between image classes and tool classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   01 Mar 2011 10:39:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to fix issues with not assigning input image to extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   25 Feb 2011 12:25:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   01 Jun 2010 15:16:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   30 Jul 2009 13:09:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   09 Apr 2008 08:51:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  624
 * SCR Title:  Fix problems with Color Tool when it gets deleted
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Replaced Set Parent operator with Set Parent Image operator when accessing the parent information of the Image Class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   07 Mar 2008 08:08:44   tbair
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
 *    Rev 1.8.1.1   14 Jul 2009 15:27:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code with new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8.1.0   07 Apr 2008 09:33:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated methods to use the new GetParentImage method instead of the GetParent method to remove confusion with CWnd GetParent method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   16 Jan 2006 17:37:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   01 Sep 2005 12:38:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed UpdateSourceExtents return code from consideration for validity of operator during a Reset Object Call.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Sep 2005 12:06:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modifications to UpdateImageData for Auxilliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Aug 2005 10:00:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed un-unsed local variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Mar 2005 11:42:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Made change to UpdateImageData so it uses the input image's output width and height for output image and tool extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Feb 2005 14:10:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications made to support new Extents and Create / Reset Methodology.
 * Added GetObjectAtPoint(
 * Added IsInputImage(
 * Added UpdateImageData(
 * Removed  OnUpdateFigure(
 * Removed OnDraw(
 * Removed ProcessMessage(
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 17:07:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Dec 2002 20:15:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Feb 2000 09:29:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  104
 * SCR Title:  New Build Reference Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * New class for theTransformation Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
