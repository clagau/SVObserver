//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOperator
//* .File Name       : $Workfile:   SVOperator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   05 Feb 2014 09:35:20  $
//******************************************************************************

#ifndef SVOPERATOR_H
#define SVOPERATOR_H

#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVRunControlLibrary/SVRunStatus.h"
#include "SVTaskObject.h"
#include "SVOMFCLibrary/SVGraphix.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageClass.h"
#include "SVValueObjectImpl.h"
#include "ISVCancel.h"

// Necessary for SVUnaryImageOperatorClass, SVFilterListBoxClass
class SVClassInfoStructListClass;
class SVUnaryImageOperatorListClass;
class SVToolClass;
class SVIPDoc;

class SVOperatorClass : public SVTaskObjectClass
{
	SV_DECLARE_CLASS( SVOperatorClass );

public:
	SVOperatorClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVOPERATOR );
	
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

protected:
	virtual LONG_PTR processMessage(DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext);

private:
	void init();

};

#endif	//	SVOPERATOR_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOperator.h_v  $
 * 
 *    Rev 1.3   05 Feb 2014 09:35:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modifications to processMessage to use LONG_PTR  instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Feb 2014 11:55:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   13 Aug 2013 10:30:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Moved all extra classes to separate files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:06:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.36   20 Mar 2013 09:24:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  823
 * SCR Title:  Fix Mask Overlays where the mask is not being drawn.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OnCollectOverlays to SVUserMaskOperatorClass. And removed value object index in BuildMaskLines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.35   21 Jan 2013 10:59:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated missing SetObjectValue and GetObjectValue methods based on 6.01 branch changes for the shift tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.34   04 Sep 2012 15:17:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.33   25 Jul 2012 14:39:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removal of dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.32   18 Jul 2012 14:14:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.31   16 Jul 2012 12:59:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.30   02 Jul 2012 17:24:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   10 Feb 2011 15:05:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   28 Oct 2010 14:08:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   16 Dec 2009 14:05:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   03 Sep 2009 10:35:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   21 Oct 2008 14:55:22   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  625
 * SCR Title:  Functional Requirement for Tool Adjustment Dialog - Histogram Control
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Changes for histogram control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   24 Jul 2007 15:17:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   15 Feb 2007 11:53:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated flags for the mappgeterror fucntion to remove the M_MESSAGE flag.  Also remove the unused string attribute that held the last matrox error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   31 Mar 2006 09:20:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  554
 * SCR Title:  Add Shape Mask capability
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Auto Resize capability to the Shape Mask
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   20 Mar 2006 13:31:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  549
 * SCR Title:  Fix Shape Mask Editor
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added GetFillProperties functionality
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   14 Sep 2005 09:20:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed the status color of the shape helper
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   17 Aug 2005 14:20:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed dlg updating mask operator
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   17 Aug 2005 10:36:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   implemented Input Request Queue tool updating
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   09 Aug 2005 07:54:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   19 Jul 2005 14:36:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed cancel bug
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   22 Jun 2005 09:44:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SetObjectValue now returns SVHRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   19 May 2005 15:02:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented Shape Mask
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   18 May 2005 14:08:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  484
 * SCR Title:  Display Mask on Source Image
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added functions to support drawing the mask on the input Image.  AddLine(.. , BuildMaskLines( , modified UpdateFigure(.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   17 Feb 2005 15:08:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   16 Feb 2005 14:53:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   19 Jan 2004 10:03:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Update header information to make getInputImage method public.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   06 Nov 2003 08:31:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made changes for threshold equations to be remotely settable and correct in the config print.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   07 Aug 2003 12:01:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  380
 * SCR Title:  Add Image Mask option
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new image mask.  added methods for this new mask
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   22 Apr 2003 13:17:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   30 Jan 2003 15:28:38   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated processMessage, init, CloseObject, CreateObject, Resize, Refresh and onRun methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   20 Nov 2002 10:30:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed references to System
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   04 Apr 2002 10:16:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  286
 * SCR Title:  Upgrade SVObserver to Version 3.37 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code syntax to allow rational rose to reverse engineer properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   19 Apr 2001 18:54:54   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   19 Apr 2001 15:14:44   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   23 Mar 2000 16:41:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed Unary Image Operator List 
 * (now in SVUnaryImageOperatorList.cpp/h)
 * Revised all onRun(...) to return FALSE, if not run and perform
 * the required invalidation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/