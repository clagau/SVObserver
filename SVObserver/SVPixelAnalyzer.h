//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : PixelAnalyzer
//* .File Name       : $Workfile:   SVPixelAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:22:46  $
//******************************************************************************

#ifndef SVPIXELANALYZER_H
#define SVPIXELANALYZER_H

#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVAnalyzer.h"     // Required by PixelAnalyzer.h

////////////////////////////////////////////////////////////////////////////////
// .Title       : Class SVPixelAnalyzerClass
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
class SVPixelAnalyzerClass : public SVImageAnalyzerClass
{

	SV_DECLARE_CLASS( SVPixelAnalyzerClass );

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVPixelAnalyzerClass( LPCSTR ObjectName /* = "Pixel Analyzer" */ );
	SVPixelAnalyzerClass(BOOL BCreateDefaultTaskList = FALSE, 
                         SVObjectClass* POwner = NULL ,
                         int StringResourceID = IDS_CLASSNAME_SVPIXELANALYZER );

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
	virtual ~SVPixelAnalyzerClass();
	virtual BOOL CloseObject();

//******************************************************************************
// Operator(s):
//******************************************************************************
public:
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );

private:
	void init();

//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************
public:
	virtual BOOL OnValidate();

//******************************************************************************
// Data Element(s):
//******************************************************************************
   long             msvlHistValueArraySize;
	SVLongValueObjectClass	pixelCount;
	SVByteValueObjectClass	pixelCountColor;


protected:
	virtual BOOL onRun(SVRunStatusClass &RRunStatus);

	SVMatroxImageResult		histResultID;
	SVMatroxLongArray		m_alHistValues;

};


//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	// SVPIXELANALYZER_H

//** EOF **

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPixelAnalyzer.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:22:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   18 Jul 2012 14:19:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   10 Feb 2011 15:09:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   27 Jan 2011 11:50:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   24 Jul 2007 15:44:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Apr 2003 13:52:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   20 Nov 2002 10:40:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   CreateObject
 * 
 * Call SVImageProcessingClass method instead of MIL method
 * 
 * Changed ValueObject semantics to work with buckets
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   03 Mar 2000 12:11:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised declaration of init() method to be private and not virtual.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:26:08   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Sep 1999 18:41:10   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  30
 * SCR Title:  Change White Pixel Analyzer to work on all Gray Scale values.
 * Checked in by:  Jim;  James A. Brown
 * Change Description:  
 *   Converted old "Run" standard to new "onRun" standard. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Sep 1999 13:17:44   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  30
 * SCR Title:  Change White Pixel Analyzer to work on all Gray Scale values.
 * Checked in by:  Jim;  James A. Brown
 * Change Description:  
 *   Changed
 * SVLongValueObjectClass pixelCount; and
 * SVByteValueObjectClass pixelCountColor;
 * to public instead of private.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Aug 1999 22:08:54   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Changed references of  Validate() to OnValidate, and
 * referrences of  DoDetailDialog( ) to SetupDialog ( ).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Aug 1999 11:52:10   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Upgrading White Pixel Analyzer to new i/o object interface.  
 * 
 * Changing White Pixel Analyzer to more generic Pixel Analyzer.
 * 
 * 
 * 
 * Separating "Pixel Analyzer" class out from SVAnalyzer.cpp file.  
 * 
 * First Checkin.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
