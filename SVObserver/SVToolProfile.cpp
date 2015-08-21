//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProfileTool
//* .File Name       : $Workfile:   SVToolProfile.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Aug 2013 10:13:52  $
//******************************************************************************

#include "stdafx.h"
#include "SVToolProfile.h"

#include "SVUnaryImageOperatorList.h"
#include "SVThresholdClass.h"
#include "SVUserMaskOperatorClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************



//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVProfileToolClass
//* Note(s)    : 
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/

//******************************************************************************
// Adjustments
//******************************************************************************

SV_IMPLEMENT_CLASS( SVProfileToolClass, SVProfileToolClassGuid );


//******************************************************************************
// Constructor(s):
//******************************************************************************


SVProfileToolClass::SVProfileToolClass( BOOL BCreateDefaultTaskList, SVObjectClass* POwner, int StringResourceID )
				   :SVToolClass( BCreateDefaultTaskList, POwner, StringResourceID )
{
	init();
}

void SVProfileToolClass::init()
{

	// Set up your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVToolObjectType;
	outObjectInfo.ObjectTypeInfo.SubType    = SVToolProfileObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject( &projectAngle, SVProjectAngleObjectGuid, IDS_OBJECTNAME_PROJECT_ANGLE, false, SVResetItemTool );

	// Set Embedded defaults
	projectAngle.SetDefaultValue( 0.0, TRUE );

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

		// Add the UnaruyImageOperatorList to the Tool's List
		Add( pOperatorList );
	}

}


//******************************************************************************
// Destructor(s):
//******************************************************************************

SVProfileToolClass::~SVProfileToolClass()
{
	CloseObject();
}

//******************************************************************************
// Operator(s):
//******************************************************************************

BOOL SVProfileToolClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVToolClass::CreateObject( PCreateStructure ) )
	{
		double projAngle;
		projectAngle.GetValue( projAngle );
		propertyCount	= 2;
		pPropertyArray	= new SVToolPropertyEntryStruct[ propertyCount ];
		if( pPropertyArray )
		{
			pPropertyArray[0].StrName	 = _T( "Column Profile" );
			pPropertyArray[0].DWValue	 = ( projAngle == 0.0 );
			pPropertyArray[0].ID		 = IDC_CHECK1;
			pPropertyArray[0].RadioStart = IDC_CHECK1;
			pPropertyArray[0].RadioEnd	 = IDC_CHECK2;

			pPropertyArray[1].StrName	 = _T( "Row Profile" );
			pPropertyArray[1].DWValue	 = ( projAngle == 90.0 );
			pPropertyArray[1].ID		 = IDC_CHECK2;
			pPropertyArray[1].RadioStart = IDC_CHECK1;
			pPropertyArray[1].RadioEnd	 = IDC_CHECK2;

			bOk = TRUE;
		}
		else
			propertyCount = 0;

	}

	// Set / Reset Printable Flags
	projectAngle.ObjectAttributesAllowedRef() |= SV_PRINTABLE;

	isCreated = bOk;

	return bOk;
}

BOOL SVProfileToolClass::CloseObject()
{
	BOOL bRetVal =  SVToolClass::CloseObject();

	if( pPropertyArray )
	{
		delete [] pPropertyArray;
		pPropertyArray = NULL;
		propertyCount = 0;
	}

	return bRetVal;
}

bool SVProfileToolClass::DoesObjectHaveExtents() const
{
	return true;
}

HRESULT SVProfileToolClass::ResetObject()
{
	HRESULT l_Status = SVToolClass::ResetObject();

	UpdateImageWithExtent( 1 );

	return l_Status;
}

BOOL SVProfileToolClass::onRun( SVRunStatusClass& RRunStatus )
{
	return SVToolClass::onRun( RRunStatus );
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVToolProfile.cpp_v  $
 * 
 *    Rev 1.1   13 Aug 2013 10:13:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Included SVThresholdClass.h and SVUserMaskOperatorClass.h instead of SVOperator.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:46:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   30 Mar 2011 16:28:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   21 Mar 2011 12:19:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   08 Dec 2010 13:45:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   09 Aug 2005 08:49:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   29 Jul 2005 13:32:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   21 Jun 2005 08:36:30   ebeyeler
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
 *    Rev 3.9   17 Feb 2005 13:49:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modifications made to support new Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   22 Apr 2003 16:55:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   31 Jan 2003 09:26:48   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   10 Jan 2003 16:16:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   09 Dec 2002 20:14:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   20 Nov 2002 14:39:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   ValueObject Data Indexes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   23 Mar 2000 16:50:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   revised to work with new SVStdImageOperatorList instead
 * of SVUnaryImageOperatorList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   03 Mar 2000 13:09:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised onUpdateFigure to adjust for changed
 * SVM_GET_EXTENT parameters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   21 Feb 2000 15:05:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised onUpdateFigure to use relative extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:51:38   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Sep 10 1999 16:25:50   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Profile Tool extracted from SVTool.cpp and modified for v3.00.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
//** EOF **
