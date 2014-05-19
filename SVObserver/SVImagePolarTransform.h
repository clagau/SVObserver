//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImagePolarTransform
//* .File Name       : $Workfile:   SVImagePolarTransform.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:46:30  $
//******************************************************************************
#ifndef SVIMAGEPOLARTRANSFORM_H
#define SVIMAGEPOLARTRANSFORM_H

#include "SVImageClass.h"
#include "SVPolarTransform.h"
#include "SVValueObjectImpl.h"

class SVImagePolarTransformClass : public SVPolarTransformClass
{
friend class SVPolarTransformationToolClass;
	SV_DECLARE_CLASS( SVImagePolarTransformClass )

public:
	SVImagePolarTransformClass( SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVIMAGEPOLARTRANSFORM );
	virtual ~SVImagePolarTransformClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );

	virtual BOOL OnValidate();

	HRESULT ResetObject();

	SVImageClass* GetOutputImage();

	BOOL SetDefaultFormulas();

	SVImageClass* getInputImage();

protected:
	//BOOL calcPolarDestImageSize( SVExtentClass& RExtent );
	//BOOL checkOutputImageDimensions( double DCenterX, double DCenterY, double DStartRadius, double DEndRadius, double DStartAngle, double DEndAngle, long LInterpolationMode );
	void NewCorrectAngles( double& RDStartAngle, double& RDEndAngle );
	void correctAngles( double& RDStartAngle, double& RDEndAngle );
	void AnglesTo360( double& p_dStart, double& p_dEnd);

	virtual BOOL onRun( SVRunStatusClass& RRunStatus );
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

	SVImageClass				outputImageObject;
	
	// Tool's center x value, comes from mouse positioning...
	SVDoubleValueObjectClass centerX;
	// Tool's center y value, comes from mouse positioning...
	SVDoubleValueObjectClass centerY;
	// Tool's start radius value, comes from mouse positioning...
	SVDoubleValueObjectClass startRadius;
	// Tool's end radius value, comes from mouse positioning...
	SVDoubleValueObjectClass endRadius;
	// Tool's start angle value, comes from mouse positioning...
	SVDoubleValueObjectClass startAngle;
	// Tool's end angle value, comes from mouse positioning...
	SVDoubleValueObjectClass endAngle;

	// If TRUE we use the result inputs from evaluate objects,
	//	which are then used to overwrite centerX, centerY, etc. in onRun(...). 
	//
	// If FALSE we don't overwrite in onRun(...), so the values are free
	//	to be changed by mouse movings.
	SVBoolValueObjectClass useFormulaInput;

	// M_NEAREST_NEIGHBOR or M_BILINEAR or M_BICUBIC
	//	plus: M_OVERSCAN_ENABLE or M_OVERSCAN_DISABLE or M_OVERSCAN_CLEAR
	SVEnumerateValueObjectClass interpolationMode;

	// Inputs
	SVInObjectInfoStruct		inputImageObjectInfo;

private:
	void SetCalculatedPrintableFlags();
	static double g_dMaxAngularDistance;

	HRESULT CollectInputImageNames( SVRunStatusClass& RRunStatus );

};

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVIMAGEPOLARTRANSFORM_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVImagePolarTransform.h_v  $
 * 
 *    Rev 1.2   15 May 2014 12:46:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 10:42:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed sendmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:53:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   25 Jul 2012 14:10:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code and updated overlay functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   06 Mar 2008 15:33:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  616
 * SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Code to collect source Image Names for Image Tools.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   08 Aug 2005 16:01:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Feb 2005 09:12:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  449
 * SCR Title:  Modify Polar Un-Wrap Tool
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added newCorrectAngles( to allow PolarUnwrap to 720 degrees.
 * Modified OnRun to go beyond 360 degrees.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   17 Feb 2005 14:27:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   27 May 2004 07:59:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  423
 * SCR Title:  Do not Print Calculated Values from the Polar Unwrap Tool in Print Report
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method SetCalculatedPrintableFlags.  When use formula is checked, printable flags and remotely setable and online flags are turned off
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 Jan 2004 09:56:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Update header information to make getInputImage method public.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   22 Apr 2003 10:18:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   05 Mar 2003 12:13:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Restructured and recoded the SetToolParameter code. It now uses a standard struct and each SVToolClass based class is responsible for moving itself based on the values passed in through the struct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Jan 2003 18:02:16   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CreateObject, calcPolarDestSize, checkOutputImageDimensions, onRun and processMessage with new image buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Jan 2003 14:24:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new function VerifyImageForOverlay to determine which object has overlays on which image.
 * Made changes related to changing the SVDrawObjectClass to work in a consistent manner with SVDrawObjectListClass.
 * Added method to return the overlay figure from this object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Apr 2001 14:36:26   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Mar 2000 16:05:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Introduced checking of resizing in size...(...) functions.
 * Added and introduced method to correct Angle overrun.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Mar 2000 14:11:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added method to Set Default Formulas.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   Mar 01 2000 12:06:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   NEW Module: Provides image polar transformation.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
