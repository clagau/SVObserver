//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOperator
//* .File Name       : $Workfile:   SVOperator.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   05 Feb 2014 09:35:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVOperator.h"
#include "SVImageLibrary/SVDrawContext.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVObjectLibrary/SVAnalyzerLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObserver.h"
#include "SVAnalyzer.h"
#include "SVAutoThresholdEquation.h"
#include "SVGlobal.h"
#include "SVInspectionProcess.h"
#include "SVLowerThresholdEquation.h"
#include "SVUnaryImageOperatorList.h"
#include "SVUpperThresholdEquation.h"
#include "SVDataBuffer.h"
#include "SVImageProcessingClass.h"
#include "SVToolSet.h"
#include "SVTool.h"
#include "SVMaskShape.h"
#include "SVObjectLibrary\SVToolSetScriptTags.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SV_IMPLEMENT_CLASS( SVOperatorClass, SVOperatorClassGuid )

SVOperatorClass::SVOperatorClass( SVObjectClass* POwner, int StringResourceID )
				:SVTaskObjectClass( POwner, StringResourceID )
{
	init();
}

BOOL SVOperatorClass::CreateObject( SVObjectLevelCreateStruct* PCreateStructure )
{
	BOOL bOk = FALSE;

	if( SVTaskObjectClass::CreateObject( PCreateStructure ) && GetTool() != NULL )
	{
		bOk = TRUE;
	}

	isCreated = bOk;

	return bOk;
}

LONG_PTR SVOperatorClass::processMessage(DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext)
{
	LONG_PTR DwResult = NULL;
	// Try to process message by yourself...
	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
	case SVMSGID_RESET_ALL_OBJECTS :
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
	return( SVTaskObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}

void SVOperatorClass::init()
{
	m_bUseOverlays = false;	// in general, operators don't have overlays

	// Identify our output type
	outObjectInfo.ObjectTypeInfo.ObjectType = SVOperatorObjectType;

	// Register Embedded Object(s)

	// Set Embedded defaults

	// Set default inputs and outputs
	addDefaultInputObjects();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOperator.cpp_v  $
 * 
 *    Rev 1.4   05 Feb 2014 09:35:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modifications to processMessage to use LONG_PTR  instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Feb 2014 11:55:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Aug 2013 10:30:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Moved all extra classes to separate files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 May 2013 12:36:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.79   02 May 2013 11:21:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  810
   SCR Title:  Remove support for Legacy Tools
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 13:08:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.78   20 Mar 2013 09:24:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  823
   SCR Title:  Fix Mask Overlays where the mask is not being drawn.
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added OnCollectOverlays to SVUserMaskOperatorClass. And removed value object index in BuildMaskLines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.77   21 Jan 2013 10:59:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  801
   SCR Title:  Add new Shift Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated missing SetObjectValue and GetObjectValue methods based on 6.01 branch changes for the shift tool.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.76   04 Sep 2012 15:17:30   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to add new scripting and object creation/update funcitonality to the object.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.75   25 Jul 2012 14:39:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removal of dead code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.74   18 Jul 2012 14:15:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.73   16 Jul 2012 12:59:04   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.72   12 Jul 2012 15:03:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to add in an additional string identifier for the input and fixed clear functionality assoicated with input structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.71   02 Jul 2012 17:24:04   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.70   19 Jun 2012 13:10:58   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
   Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.69   11 Apr 2011 18:55:36   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated child run status objects with update counters flag
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.68   30 Mar 2011 15:29:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.40 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to use STL min/max functions instead of MFC macros.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.67   16 Mar 2011 13:45:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.66   25 Feb 2011 12:22:24   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.65   10 Feb 2011 15:05:26   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.64   27 Jan 2011 11:41:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.63   08 Dec 2010 13:15:28   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.62   04 Nov 2010 14:08:08   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with image processing and display image processing classes and associated includes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.61   28 Oct 2010 14:08:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.33 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.60   29 Jun 2010 14:08:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source to change object validity test and casting changes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.59   01 Jun 2010 14:58:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.58   05 Mar 2010 11:01:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  686
   SCR Title:  Update Laptop version to load all valid configurations
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Added new element to run status for critical errors.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.57   16 Dec 2009 14:05:42   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.56   30 Jul 2009 12:14:56   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  665
   SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Merged branched changes into current code branch with appropriate updates.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.55   19 Nov 2008 08:36:48   jbrown
   Project:  SVObserver
   Change Request (SCR) nbr:  641
   SCR Title:  BoundsChecker results
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     Correct previous checkin.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.54   14 Nov 2008 12:45:12   jbrown
   Project:  SVObserver
   Change Request (SCR) nbr:  641
   SCR Title:  BoundsChecker results
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     Increased error handling within
   SVSVThresholdClass::onRun ().
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.53   21 Oct 2008 14:55:22   jhanebach
   Project:  SVObserver
   Change Request (SCR) nbr:  625
   SCR Title:  Functional Requirement for Tool Adjustment Dialog - Histogram Control
   Checked in by:  jHanebach;  Jack Hanebach
   Change Description:  
     Changes for histogram control.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.52   22 May 2008 11:40:26   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  597
   SCR Title:  Upgrade Matrox Imaging Library to version 8.0
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Temperary Buffer for OnPaint message handler so GetHostAddress and GetBitmapinfo will succeed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.51   24 Jul 2007 15:17:36   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  597
   SCR Title:  Upgrade Matrox Imaging Library to version 8.0
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Update MIL 8.0
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.50   22 Jun 2007 10:42:56   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.49   21 Jun 2007 14:34:04   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.48   19 Feb 2007 16:40:08   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  594
   SCR Title:  Remove issue with interpreting status response incorrectly
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.47   15 Feb 2007 11:53:02   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  594
   SCR Title:  Remove issue with interpreting status response incorrectly
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated flags for the mappgeterror fucntion to remove the M_MESSAGE flag.  Also remove the unused string attribute that held the last matrox error.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.46   07 Apr 2006 13:41:42   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  549
   SCR Title:  Fix Shape Mask Editor
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed GetFillProperties to properly get the fill color
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.45   31 Mar 2006 09:20:44   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  554
   SCR Title:  Add Shape Mask capability
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added Auto Resize capability to the Shape Mask
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.44   20 Mar 2006 13:57:10   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  547
   SCR Title:  Fix Image overlay data
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Fixed overlays issue
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.43   20 Mar 2006 13:31:38   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  549
   SCR Title:  Fix Shape Mask Editor
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Added GetFillProperties functionality
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.42   02 Nov 2005 07:33:16   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  512
   SCR Title:  Add Published Result Images functionality
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed RunStatus.m_lResultImageIndex to .Images
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.41   21 Oct 2005 14:04:50   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed un-necessary setting of the IsConnected flag.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.40   05 Oct 2005 11:09:28   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Added code to perform the proper create object functionality when shape mask helper object is re-created.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.39   14 Sep 2005 09:20:40   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed the status color of the shape helper
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.38   09 Sep 2005 10:30:42   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  484
   SCR Title:  Display Mask on Source Image
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Changed BuildMaskLines to draw every line and changed meaning of the drawObjectList Flag to be DrawHatchedFigure instead of LossOnZoom.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.37   07 Sep 2005 12:36:10   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Fixed ResetObject in the UserMaskOperator to fix the Viewable flag.  Some items were being put into the Result View when using Shape Mask.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.36   23 Aug 2005 14:24:28   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  484
   SCR Title:  Display Mask on Source Image
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated Build fucntion to allow the mask to draw if the options other than use image as mask is selected.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35   18 Aug 2005 10:19:50   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     hide Mask Fill Area and Mask Fill Color if not Shape mask
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   17 Aug 2005 14:20:02   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     fixed dlg updating mask operator
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   17 Aug 2005 10:36:32   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     implemented Input Request Queue tool updating
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   16 Aug 2005 15:28:20   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  484
   SCR Title:  Display Mask on Source Image
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified onDraw of SVUserMaskOperatorClass::OnDraw
   set m_bLossOnZoom in svdrawobjectlistclass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   09 Aug 2005 10:10:08   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added attributes to Mask value objects
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   09 Aug 2005 07:54:48   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated objects and dialogs to use the ActiveX Interface to update inspection data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   29 Jul 2005 13:08:04   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated object initialization to change the parameters for RegisterEmbeddedObject for a SVValueObjectClass.  The two additional parameters will inform the preperation process before toolset execution.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   19 Jul 2005 14:36:26   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed cancel bug
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   07 Jul 2005 13:44:26   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Fixed merge in onUpdateFigure. Commented out code that overwrites the tool figure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   29 Jun 2005 16:09:24   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   22 Jun 2005 12:37:06   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     value objects use SVHRESULT
   object attributes use accessor functions
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   22 Jun 2005 10:25:00   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     object attributes use accessor functions
   SetObjectValue now returns SVHRESULT
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   19 May 2005 15:02:24   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  482
   SCR Title:  New vector based mask
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented Shape Mask
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   19 May 2005 08:49:14   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  484
   SCR Title:  Display Mask on Source Image
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified IsInputImage
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   18 May 2005 14:08:12   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  484
   SCR Title:  Display Mask on Source Image
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added functions to support drawing the mask on the input Image.  AddLine(.. , BuildMaskLines( , modified UpdateFigure(.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   17 Feb 2005 15:08:58   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   16 Feb 2005 14:53:48   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   06 Nov 2003 08:31:26   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  385
   SCR Title:  Expose additional parameters for SIAC
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Made changes for threshold equations to be remotely settable and correct in the config print.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   07 Aug 2003 12:01:48   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  380
   SCR Title:  Add Image Mask option
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added new image mask.  added methods for this new mask
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   08 Jul 2003 10:40:06   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated methods CreateObject, CloseObject, Refresh, and Resize in most classes to include changes in SVImageProcessingClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   08 Jul 2003 10:15:54   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  378
   SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Added new code and classes for the new custom filters objects and dialogs.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   22 Apr 2003 13:17:12   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   06 Feb 2003 10:27:16   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  320
   SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Modified to use new ImageProcessing stuff
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   30 Jan 2003 15:28:38   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated processMessage, init, CloseObject, CreateObject, Resize, Refresh and onRun methods to use the new image circle buffer methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   03 Jan 2003 15:31:32   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added attributes to the objects
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   09 Dec 2002 19:48:28   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  295
   SCR Title:  Remove Result Data from Configuration Printout
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   20 Nov 2002 10:29:42   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  272
   SCR Title:  Outputs do not always get set correctly.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed references to System
   Changed references to System to SVImageProcessingClass 
         GetIPDocument
             Changed method of access
   Changed ValueObject semantics to work with buckets
   CreateObject
   Call SVImageProcessingClass method instead of MIL method
   onRun
   removed ErrorEvent.PulseEvent
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   13 Jun 2001 09:20:06   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  200
   SCR Title:  Update code to implement SVIM COM Server
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
     Modified the following functions: SVUserMaskOperatorClass::SetObjectValue.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   19 Apr 2001 18:53:58   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  196
   SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
   Checked in by:  Steve;  Stephen E. Steffan
   Change Description:  
     Header file changes due to include file restructuring.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   19 Apr 2001 15:14:02   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  191
   SCR Title:  Restructure File Management
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Header file changes due to include file restructuring.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   23 Mar 2000 16:41:58   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  118
   SCR Title:  Integration of LUT Operator
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed Unary Image Operator List 
   (now in SVUnaryImageOperatorList.cpp/h)
   Revised all onRun(...) to return FALSE, if not run and perform
   the required invalidation.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   13 Mar 2000 16:16:06   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  112
   SCR Title:  Expose Extents of Images
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised CreateObject to set OwnerTool before setting extents.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   03 Mar 2000 16:02:00   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  113
   SCR Title:  Integrate new Polar Unwrap Tool
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Integrated Resizing Message handler into
   SVUnaryImageOperatorListClass::processMessage(...).
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   03 Mar 2000 13:49:12   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  112
   SCR Title:  Expose Extents of Images
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to use SVImageInfoClass methods to get image extents.
   
   Revised init() method to be private and not virtual.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   Jan 03 2000 14:36:50   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  73
   SCR Title:  Mask Editor Mil 5.0 to Mil 6.0 repairs.
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Some test code for checking that masks operator works.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   Nov 30 1999 13:50:34   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  65
   SCR Title:  Remove obsolute class SVToolSetViewClass
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Removed references to SVToolSetViewClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 13:23:18   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.13   28 Oct 1999 08:45:36   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised GetObjectScript to address performance issues.
   Revised onRun methods for operators.
   Removed obsolete code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.12   19 Oct 1999 17:38:52   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  48
   SCR Title:  Threshold Page Formula
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised SVThresholdClass to use formulas
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.11   Oct 14 1999 13:44:44   steve
   Latest Configuration file printing changes.
   
      Rev 1.10   Sep 22 1999 18:16:36   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  33
   SCR Title:  Port Profile Tool from v2.42
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Added code to implement the SVDoubleValueObject for the auto thresholding value for the Threshold operator.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.9   16 Sep 1999 11:06:56   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised SVUserMaskOperatorClass::GetObjectScript to
   script the serialized stream of the SVGraphixClass.
   Revised SVUserMaskOperatorClass::SetObjectValue to
   restore from the script the serialized stream of the 
   SVGraphixClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.8   15 Sep 1999 17:57:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Removed Unneccessary code.
   Revised SVUserMaskOperatorClass to expose and script
   member variables.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.7   07 Sep 1999 08:19:38   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Fixed Run() to support new Run(RunStatus).
   Removed obsolete code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   04 Sep 1999 17:27:08   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Moved initialization of output image extents to SVImageClass.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.5   02 Sep 1999 08:26:24   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised to call immediate base class CreateObject
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   28 Aug 1999 17:57:20   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  9
   SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised Destructor in SVUnaryImageOperatorListClass to
    not call RemoveAll.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/