//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLuminanceAnalyzerClass
//* .File Name       : $Workfile:   SVAnalyzerLuminance.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:26:22  $
//******************************************************************************

#ifndef SVANALYZERLUMINANCE_H
#define SVANALYZERLUMINANCE_H

#include "SVMatroxLibrary\SVMatroxImageInterface.h"
#include "SVAnalyzer.h"

////////////////////////////////////////////////////////////////////////////////
//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************

//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVLuminanceAnalyzerClass
//* Note(s)    : Luminance Analyzer.
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/



////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVLuminanceAnalyzerClass
// -----------------------------------------------------------------------------
// .Description : e.g. This base class capsules an Image and provides its ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.yyyy				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
class SVLuminanceAnalyzerClass : public SVImageAnalyzerClass
{
	SV_DECLARE_CLASS( SVLuminanceAnalyzerClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVLuminanceAnalyzerClass( LPCSTR ObjectName /* = "Luminace Analyzer" */ );
	SVLuminanceAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLUMINANCEANALYZER );

public:
   long             msvlHistValueArraySize;

   SVLongValueObjectClass msvLuminanceValue;

	SVDoubleValueObjectClass msvVarianceValue;
	SVDoubleValueObjectClass msvStdDevValue;
	SVBoolValueObjectClass msvCalcStdDevValue;

	virtual        ~SVLuminanceAnalyzerClass();

	virtual BOOL   CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

/*- CloseObject () ------------------------------------------*/
/*- This function should only be called from the destructor  */
/*- and from derived functions CloseObject () functions. ----*/
	virtual BOOL   CloseObject();

    SVResultClass* GetResultObject();
   
	virtual BOOL   OnValidate ();


private:
/*- USED BY CONSTRUCTORS ------------------------------------*/
/*- Common initialization routine used by all constructors. -*/
   void init();
/*- End of USED BY CONSTRUCTORS. ----------------------------*/

protected:
	virtual double calculateVariance( double aNumberOfSamples, double aAverageValue, double aAccumulatedSquares );

   virtual BOOL   onRun( SVRunStatusClass& RRunStatus );

/*- USED BY onRun () ------------------------------------------*/
/*- These values are only used by the Run () function, and   */
/*- are maintained here to avoid time loss in allocating and */
/*- freeing these buffers for each function call. -----------*/
    SVMatroxImageResult        msvHistResultID;
	SVMatroxLongArray          msvplHistValues;
/*- End of USED BY Run (). ----------------------------------*/

};



#endif   // _SVANALYZERLUMINANCE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVAnalyzerLuminance.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:26:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   18 Jul 2012 13:23:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   02 Jul 2012 16:11:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   10 Feb 2011 14:24:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   27 Jan 2011 10:47:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   24 Jul 2007 10:49:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   08 Aug 2005 15:21:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   17 Apr 2003 16:33:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   Apr 17 2001 15:03:08   steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Header file chages due to restructuring of include files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   17 Apr 2001 14:13:02   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen S. Steffan
 * Change Description:  
 *   Changes due to restructure of SVObserver.h and other
 * Header files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   03 Mar 2000 12:09:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised declaration of init() method to be private and not virtual.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   02 Feb 2000 18:01:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  95
 * SCR Title:  Standard Deviation on Luminance Histogram
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added capability to calculte standard deviation on the
 * luminance histogram
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   22 Nov 1999 17:19:20   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   03 Sep 1999 14:47:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Run method to onRun
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Aug 1999 15:10:32   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Introduced register input interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Aug 1999 11:22:42   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  14
 * SCR Title:  Integrate luminance analyzer into window tool.
 * Checked in by:  Jim;  James A. Brown
 * Change Description:  
 *   Member variable and functions
 *  msvError,
 *  Serialize (), and
 *  SetObjectDepth ()
 * were all removed because they are being handled by the base class.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Aug 1999 15:35:00   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Replaced DoDetailDialog() with SetupDialog().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Aug 25 1999 22:50:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  1
 * SCR Title:  Integrate a common validity check for all tool base class inputs and outputs.
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised Validate method to be OnValidate method.
 *  ( OnValidate - local scope validation).
 * Added Validate method (Routes to all owned objects)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Aug 1999 11:28:50   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  14
 * SCR Title:  Integrate luminance analyzer into window tool.
 * Checked in by:  James A. Brown
 * Change Description:  
 *   Changed  
 *   SVLuminanceAnalyzerClass( ..., , int StringResourceID = IDS_CLASSNAME_SVWHITEPIXELANALYZER );
 * 
 * to
 *   SVLuminanceAnalyzerClass( ..., , int StringResourceID = IDS_CLASSNAME_SVLUMINANCEANALYZER );
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Aug 16 1999 13:49:48   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  14
 * SCR Title:  Integrate luminance analyzer into window tool.
 * Checked in by:  James A. Brown
 * Change Description:  
 *   First check in.  Integration of Luminance Analyzer.  Most of this code was copied from White Pixel Analyzer. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
