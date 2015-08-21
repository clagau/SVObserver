//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPolarTransformationTool
//* .File Name       : $Workfile:   SVPolarTransformationTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:31:46  $
//******************************************************************************

#include "stdafx.h"
#include "SVPolarTransformationTool.h"

#include "SVImageLibrary/SVDrawContext.h"

#include "SVAnalyzer.h"
#include "SVEvaluate.h"
#include "SVImagePolarTransform.h"
#include "SVInspectionProcess.h"

SV_IMPLEMENT_CLASS( SVPolarTransformationToolClass, SVPolarTransformationToolClassGuid );

SVPolarTransformationToolClass::SVPolarTransformationToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
							   :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	// BoolValueObject to use new method
	RegisterEmbeddedObject( &m_svAngularMethod, SVOutputAngularMethodObjectGuid, IDS_OBJECTNAME_ANGULAR_METHOD,	false, SVResetItemNone );

	init();
}


void SVPolarTransformationToolClass::init()
{
	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVPolarTransformationToolObjectType;

	// Identify our input type needs

	m_svToolExtent.SetTool( this );
	m_svToolExtent.SetImageType( SVImageTypePhysical );
	m_svToolExtent.SetTranslation( SVExtentTranslationPolarUnwrap );
	m_svToolExtent.SetAlwaysUpdate( true );

	// Hide and Remove Embedded Extents
	removeEmbeddedExtents();

	// Register Embedded Objects
	RegisterEmbeddedObject( &m_svSourceImageNames, SVSourceImageNamesGuid, IDS_OBJECTNAME_SOURCE_IMAGE_NAMES, false, SVResetItemTool );
		

	// Default taskObjectList items:

	// Setup the evaluation clases
	SVEvaluateClass* pEvaluate = new SVEvaluateCenterXClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateCenterYClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateStartRadiusClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateEndRadiusClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateStartAngleClass;
	if( pEvaluate )
		Add( pEvaluate );

	pEvaluate = new SVEvaluateEndAngleClass;
	if( pEvaluate )
		Add( pEvaluate );

	// Set Default Value to Use Angular Method1
	CString strEnumTypes;
	strEnumTypes = "Angular Method1=0,Angular Method2=1";
	m_svAngularMethod.SetEnumTypes( strEnumTypes );

	m_svAngularMethod.SetDefaultValue( _T("Angular Method1"), TRUE );
	m_svAngularMethod.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	// Add the Image Transformation Class
	SVImagePolarTransformClass* pTransform = new SVImagePolarTransformClass;
	if( pTransform )
		Add( pTransform );

	// Set default inputs and outputs
	addDefaultInputObjects();
}

//******************************************************************************
// Destructor(s):
//******************************************************************************

SVPolarTransformationToolClass::~SVPolarTransformationToolClass()
{ 
}

//******************************************************************************
// Operator(s):
//******************************************************************************

BOOL SVPolarTransformationToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = SVToolClass::CreateObject( PCreateStructure );

	m_svSourceImageNames.ObjectAttributesAllowedRef() &=~SV_REMOTELY_SETABLE & ~SV_SETABLE_ONLINE;

	isCreated = bOk;

	return bOk;
}

SVTaskObjectClass *SVPolarTransformationToolClass::GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint )
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

bool SVPolarTransformationToolClass::DoesObjectHaveExtents() const
{
	return true;
}

// Sets the formulas of the evaluation inputs to the standard
// outputs of this image polar transform object. 
// This is very useful, if you want that mouse movings will 
// work also if use formula is selected.
// So this should be called if the user adds a new blank polar
// unwrap tool for easier handling. If he wants he can then  
// change the formulas.
// Another possibilty to call this function is in the setup
// dialog, if a kind of reset formula button exists.
BOOL SVPolarTransformationToolClass::SetDefaultFormulas()
{
	// Find image polar transform child...
	SVObjectTypeInfoStruct objectInfo;
	objectInfo.SubType = SVImagePolarTransformObjectType;
	SVImagePolarTransformClass* pImagePolarTransform = reinterpret_cast<SVImagePolarTransformClass*>(::SVSendMessage( this, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&objectInfo) ));
	if( pImagePolarTransform )
	{
		return pImagePolarTransform->SetDefaultFormulas();
	}
	return FALSE;
}

HRESULT SVPolarTransformationToolClass::SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent )
{
	HRESULT l_hrOk = S_FALSE;
	double l_dInnerRadius;
	double l_dOuterRadius;

	if( (l_hrOk = p_svImageExtent.GetExtentProperty( SVExtentPropertyOuterRadius, l_dOuterRadius )) == S_OK )
	{
		l_hrOk = p_svImageExtent.GetExtentProperty( SVExtentPropertyOuterRadius, l_dInnerRadius );
	}
	// Validate that at least one radius is greater than or equal to 1.
	if(l_hrOk == S_OK && (l_dOuterRadius >= 1 || l_dInnerRadius >= 1) )
	{
		l_hrOk = SVToolClass::SetImageExtent( p_ulIndex, p_svImageExtent );
	}

	return l_hrOk;
}


HRESULT SVPolarTransformationToolClass::GetInputImageNames( SVStringValueObjectClass*& p_pSourceNames )
{
	p_pSourceNames = &m_svSourceImageNames;
	return S_OK;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPolarTransformationTool.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 12:31:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SendMessage to use proper type cast of DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:00:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:30:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   16 Jul 2012 12:59:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   02 Jul 2012 17:30:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   21 Mar 2011 12:13:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   16 Mar 2011 13:48:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   04 Nov 2010 14:08:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   01 Jun 2010 15:05:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to improve ability to track performance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   16 Dec 2009 12:29:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   07 Mar 2008 07:46:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added functions to support source Image Names.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   16 Jan 2006 17:11:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   10 Aug 2005 12:03:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  487
 * SCR Title:  Add new blob feature for source extents of blob box center
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added SetImageExtent Function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   29 Jul 2005 13:18:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   21 Jun 2005 08:27:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   24 Feb 2005 10:23:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  449
 * SCR Title:  Modify Polar Un-Wrap Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Made changes to use a Angular Method combo instead of a use new method check box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   18 Feb 2005 10:44:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  449
 * SCR Title:  Modify Polar Un-Wrap Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes to implement "New Angular Method".
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   17 Feb 2005 15:19:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   16 Feb 2005 15:01:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Jul 2003 10:42:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new code the processing of InputRequests (from SetToolParameterList) to verify that extents have changed or the change is ignored.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Apr 2003 13:52:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Mar 2003 12:13:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Restructured and recoded the SetToolParameter code. It now uses a standard struct and each SVToolClass based class is responsible for moving itself based on the values passed in through the struct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   03 Jan 2003 15:32:42   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added method SetToolPosition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Dec 2002 19:56:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Mar 2000 14:11:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added method to Set Default Formulas.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Mar 2000 13:06:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added call to removeEmbeddedExtents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 01 2000 12:08:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   NEW Module: Provides polar transform ( contains Image Polar Transform Object ) 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
