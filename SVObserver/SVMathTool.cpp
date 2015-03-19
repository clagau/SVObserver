//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMathTool
//* .File Name       : $Workfile:   SVMathTool.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Feb 2015 18:21:56  $
//******************************************************************************

#include "stdafx.h"
#include "SVMathTool.h"
#include "SVGlobal.h"
#include "SVMathEquation.h"
#include "SVResultDouble.h"
#include "SVInspectionProcess.h"
#include "SVRange.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVMathToolClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************

SV_IMPLEMENT_CLASS( SVMathToolClass, SVMathToolClassGuid );

//******************************************************************************
// Constructor(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : Constructor of class SVMathToolClass
// -----------------------------------------------------------------------------
// .Description : This constructor inits all own members and the members of
//				: the parent class SVToolClass
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:SVImageClass*		Source				Pointer to an image object which is
//											the complete or partly source of
//											this tool
//	:LPCSTR				ObjectName			Pointer to a null terminated const
//	                                        string that contents an optional
//                                          class name.
//                                          Initiated in SVObjectClass.  
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :04.05.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVMathToolClass::SVMathToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
: SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void SVMathToolClass::init(void)
{
	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVMathToolObjectType;

	// Hide and Remove Embedded Extents
	removeEmbeddedExtents();

	// Set up the friend
	pMathEquation = new SVMathEquationClass( this );
	AddFriend( pMathEquation->GetUniqueObjectID() );

	// Setup the result
	SVClassInfoStruct resultClassInfo;
	CString strTitle;
	SVObjectTypeInfoStruct interfaceInfo;

	// Declare Input Interface of Math Equation Result...
	interfaceInfo.EmbeddedID = SVMathEquationResultObjectGuid;
	resultClassInfo.DesiredInputInterface.Add( interfaceInfo );

	resultClassInfo.ObjectTypeInfo.ObjectType = SVResultObjectType;
	resultClassInfo.ObjectTypeInfo.SubType	= SVResultDoubleObjectType;
	resultClassInfo.ClassId = SVDoubleResultClassGuid;
	resultClassInfo.ClassName.LoadString( IDS_OBJECTNAME_RESULT );
	strTitle.LoadString( IDS_CLASSNAME_RESULT_DOUBLE );
	resultClassInfo.ClassName += SV_TSTR_SPACE + strTitle;

	// Construct the result
	SVDoubleResultClass* pResult = ( SVDoubleResultClass * )resultClassInfo.Construct();
	Add( pResult );
}

//******************************************************************************
// Destructor(s):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : Standard Destructor of class SVMathToolClass
// -----------------------------------------------------------------------------
// .Description : Standard destructor
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :04.05.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVMathToolClass::~SVMathToolClass()
{ 
}

//******************************************************************************
// Operator(s):
//******************************************************************************

///////////////////////////////////////////////////////////////////////////////
// Create Operator
////////////////////////////////////////////////////////////////////////////////
BOOL SVMathToolClass::CreateObject(SVObjectLevelCreateStruct* PCreateStruct )
{
	isCreated = SVToolClass::CreateObject( PCreateStruct );
	return isCreated;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : OnValidate
// -----------------------------------------------------------------------------
// .Description : 
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	:None
// .Return Value
//	:BOOL
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment
//  :27.05.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVMathToolClass::OnValidate()
{
	if( SVToolClass::OnValidate() )
	{
		return TRUE;
	}
	SetInvalid();
	return FALSE;
}

void SVMathToolClass::HideRangeIndirectValueObjects()
{
	// override the attributes for the range indirect value objects
	SVResultClass* pResult = dynamic_cast<SVResultClass*>(GetAt(0));
	if (pResult)
	{
		SVRangeClass* pRange = pResult->GetResultRange();
		if (pRange)
		{
			pRange->HideIndirectValueObjects();
		}
	}
}

DWORD_PTR SVMathToolClass::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;

	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
		case SVMSGID_CREATE_ALL_OBJECTS:
		{
			DwResult = SVToolClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext );
			HideRangeIndirectValueObjects();
			return DwResult;
		}
	}

	DWORD_PTR l_Status = ( SVTaskObjectListClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMathTool.cpp_v  $
 * 
 *    Rev 1.1   17 Feb 2015 18:21:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  985
 * SCR Title:  Range Indirect Objects exists for Math Tool but cannot be set via GUI
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised CreateObject to call SVRange::HideIndirectValueObjects method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:29:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   02 Jul 2012 17:19:28   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   27 Jan 2011 11:33:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   08 Dec 2010 13:05:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   17 Feb 2005 14:48:40   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   22 Apr 2003 11:16:54   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   10 Jan 2003 15:41:02   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  305
   SCR Title:  Implement the ability to perform RunOnce from a SIAC client
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   09 Dec 2002 19:39:24   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
