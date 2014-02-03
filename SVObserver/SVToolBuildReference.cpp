//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolBuildReference
//* .File Name       : $Workfile:   SVToolBuildReference.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   01 Feb 2014 12:22:14  $
//******************************************************************************

#include "stdafx.h"
#include "SVToolBuildReference.h"
#include "SVEvaluateX.h"
#include "SVEvaluateY.h"
#include "SVEvaluateBeta.h"
#include "SVRotateReference.h"
#include "SVShowDependentsDialog.h"
#include "SVInspectionProcess.h"

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVBuildReferenceToolClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************

SV_IMPLEMENT_CLASS( SVBuildReferenceToolClass, SVBuildReferenceToolClassGuid );

//******************************************************************************
// Constructor(s):
//******************************************************************************

SVBuildReferenceToolClass::SVBuildReferenceToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
						  :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}


void SVBuildReferenceToolClass::init()
{
	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVToolBuildReferenceObjectType;

	// Identify our input type needs

	// Register Embedded Objects

	// Set Embedded defaults

	// Default taskObjectList items:
	RegisterEmbeddedObject( &m_svRotationAngle, SVRotationAngleObjectGuid, IDS_OBJECTNAME_ROTATION_ANGLE, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_svRotationPointX, SVRotationPointXObjectGuid, IDS_OBJECTNAME_ROTATION_POINT_X, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_svRotationPointY, SVRotationPointYObjectGuid, IDS_OBJECTNAME_ROTATION_POINT_Y, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_svOutputRotationPointX, SVOutputRotationPointXObjectGuid, IDS_OBJECTNAME_OUTPUT_ROTATION_POINT_X, false, SVResetItemTool );
	RegisterEmbeddedObject( &m_svOutputRotationPointY, SVOutputRotationPointYObjectGuid, IDS_OBJECTNAME_OUTPUT_ROTATION_POINT_Y, false, SVResetItemTool );

	// Set Embedded defaults
	m_svRotationAngle.SetDefaultValue( 0.0, TRUE );
	m_svRotationPointX.SetDefaultValue( 0.0, TRUE );
	m_svRotationPointY.SetDefaultValue( 0.0, TRUE );
	m_svOutputRotationPointX.SetDefaultValue( 0.0, TRUE );
	m_svOutputRotationPointY.SetDefaultValue( 0.0, TRUE );

	m_svToolExtent.SetImageType( SVImageTypePhysical );
	m_svToolExtent.SetTranslation( SVExtentTranslationBuildReference );
	m_svToolExtent.SetAlwaysUpdate( true );

	m_svToolExtent.SetExtentObject( SVExtentPropertyRotationAngle, &m_svRotationAngle );
	m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointX, &m_svRotationPointX );
	m_svToolExtent.SetExtentObject( SVExtentPropertyPositionPointY, &m_svRotationPointY );
	m_svToolExtent.SetExtentObject( SVExtentPropertyOutputPositionPointX, &m_svOutputRotationPointX );
	m_svToolExtent.SetExtentObject( SVExtentPropertyOutputPositionPointY, &m_svOutputRotationPointY );

	SVEvaluateXClass* pEvaluateX = new SVEvaluateXClass;
	if( pEvaluateX )
		Add( pEvaluateX );

	SVEvaluateYClass* pEvaluateY = new SVEvaluateYClass;
	if( pEvaluateY )
		Add( pEvaluateY );

	SVEvaluateBetaClass* pEvaluateBeta = new SVEvaluateBetaClass;
	if( pEvaluateBeta )
		Add( pEvaluateBeta );

	SVRotateReferenceClass* pRotate = new SVRotateReferenceClass;
	if( pRotate )
	{
		Add( pRotate );
	}

	// Set default inputs and outputs
	addDefaultInputObjects();
	
	SetInvalid();
}


//******************************************************************************
// Destructor(s):
//******************************************************************************

SVBuildReferenceToolClass::~SVBuildReferenceToolClass()
{ 
}

//******************************************************************************
// Operator(s):
//******************************************************************************

BOOL SVBuildReferenceToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	isCreated = SVToolClass::CreateObject( PCreateStructure );

	return isCreated;
}

SVTaskObjectClass *SVBuildReferenceToolClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
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

HRESULT SVBuildReferenceToolClass::DoesObjectHaveExtents() const
{
	return S_OK;
}

BOOL SVBuildReferenceToolClass::IsOkToEdit()
{
	// show dependents dialog
	SVShowDependentsDialog dlg;
	dlg.PTaskObject = this;
	dlg.OnlyImageDependencies = TRUE;
	dlg.StrMessageID = IDS_EDIT_CHECK_DEPENDENTS;

	if( dlg.DoModal() == IDCANCEL )
		return FALSE;

	if( dlg.NumberOfDependents)
	{
		SVOutputInfoListClass l_OutputInfoList;

		GetOutputList( l_OutputInfoList );

		// Disconnect output image from objects using it
		for( int i = 0;i < l_OutputInfoList.GetSize(); ++ i )
		{
			SVOutObjectInfoStruct* pOutObjectInfo = l_OutputInfoList.GetAt( i );
			if( pOutObjectInfo )
			{
				if( pOutObjectInfo->ObjectTypeInfo.ObjectType == SVImageObjectType )
				{
					pOutObjectInfo->DisconnectAllInputs();
				}
			}
		}
	
		GetInspection()->SetDefaultInputs();
	
		::SVSendMessage(this, SVM_RESET_ALL_OBJECTS, NULL, NULL );
	}
	return TRUE;		
}

LONG_PTR SVBuildReferenceToolClass::processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext )
{
	LONG_PTR DwResult = NULL;

	return( SVToolClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}
BOOL SVBuildReferenceToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	SetInvalid();
	RRunStatus.SetInvalid();
	return FALSE;
}

HRESULT SVBuildReferenceToolClass::ResetObject()
{
	return S_FALSE; // tool no longer availabe.  just return S_FALSE
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolBuildReference.cpp_v  $
 * 
 *    Rev 1.3   01 Feb 2014 12:22:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Oct 2013 08:24:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:44:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   25 Apr 2013 16:02:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  746
 * SCR Title:  Fix SVObserver crash due to discrete outputs after deleting a PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added method ResetObject - made so it would only return S_FALSE because the tool can no longer be used.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:42:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   25 Mar 2013 13:37:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added OnRun to set the tool invalid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   25 Jul 2012 14:54:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   21 Mar 2011 12:19:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   16 Mar 2011 13:59:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   08 Dec 2010 13:45:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   01 Jun 2010 15:16:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   16 Jan 2006 17:36:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   21 Sep 2005 07:49:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects to handle invalid items on the input and output lists.  Methods now verify object validity before use.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   29 Jul 2005 13:32:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   18 Feb 2005 07:27:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changes made to support new extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Apr 2003 16:47:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   09 Dec 2002 20:14:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   20 Nov 2002 14:01:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed commented code from GetToolInformationString
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:51:02   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Nov 1999 12:26:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to not allow moving/sizing toolfigure if another
 * object is using the output image.
 * Added IsOkToEdit method which will show dependents of
 * the output image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Sep 14 1999 15:31:56   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced Evaluate Classes.
 * Introduced RotateReference Class.
 * Removed Obsolete functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 10 1999 16:28:38   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Add initial code for Build Reference Tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
