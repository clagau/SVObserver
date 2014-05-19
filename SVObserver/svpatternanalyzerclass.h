//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatternAnalyzerClass
//* .File Name       : $Workfile:   svpatternanalyzerclass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:21:48  $
//******************************************************************************

#ifndef SVPATTERNANALYZERCLASS_H
#define SVPATTERNANALYZERCLASS_H

#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVAnalyzer.h"

class SVPatternAnalyzerClass : public SVImageAnalyzerClass  
{
	SV_DECLARE_CLASS (SVPatternAnalyzerClass);

	friend class SVSetupDialogManager;

public:
	SVPatternAnalyzerClass (BOOL BCreateDefaultTaskList = TRUE, SVObjectClass* POwner = NULL, int StringResourceID = IDS_CLASSNAME_SVPATTERNANALYZER);

	virtual	~SVPatternAnalyzerClass();
	virtual	BOOL CloseObject();
	
	virtual BOOL CreateObject(SVObjectLevelCreateStruct* PCreateStructure);
	virtual BOOL OnValidate();
	virtual BOOL onRun(SVRunStatusClass &RRunStatus);

	SVImageInfoClass m_patBuffer;	// Model Image Buffer
	SVImageInfoClass m_patModel;	// Model Identifier.
	SVImageInfoClass m_patResult; 	// Pattern matching result buffer identifier.

	SVSmartHandlePointer m_patBufferHandlePtr;	// Model Image Buffer Handle
	SVMatroxPatternModel m_patModelHandle;		// Model Identifer Handle
	SVMatroxPatResult m_patResultHandle;	// Pattern matching result buffer identifier handle

	BOOL m_bAngleAccuracy;

// Value Objects for Model Search Parameters.

	SVLongValueObjectClass		msv_lpatMaxOccurances;			// Max. number of occurrences to find.
	SVDoubleValueObjectClass	msv_dpatAcceptanceThreshold;	// Minimum acceptance correlation.
	SVDoubleValueObjectClass	msv_dpatCertaintyThreshold;		// Certainty level in percentage.
	SVLongValueObjectClass		msv_lpatSpeed;					// Search speed factor.		
	SVLongValueObjectClass		msv_lpatAccuracy;				// Required positional accuracy.

	SVBoolValueObjectClass		msv_bpatSearchAngleMode;		// Either Enabled or Disabled.
	SVDoubleValueObjectClass	msv_dpatSearchAngle;
	SVDoubleValueObjectClass	msv_dpatAngleDeltaNeg;
	SVDoubleValueObjectClass	msv_dpatAngleDeltaPos;
	SVDoubleValueObjectClass	msv_dpatAngleTolerance;
	SVDoubleValueObjectClass	msv_dpatAngleAccuracy;
	SVDoubleValueObjectClass	msv_dpatAngleInterpolation;

	SVDoubleValueObjectClass	msv_dpatPreliminaryAcceptanceThreshold;
	SVLongValueObjectClass		msv_lpatFastFind;
	SVLongValueObjectClass		msv_lpatModelStep;
	SVLongValueObjectClass		msv_lpatBeginningResolutionLevel;
	SVLongValueObjectClass		msv_lpatFinalResolutionLevel;
	SVLongValueObjectClass		msv_lpatAdditionalCandidates;
	SVDoubleValueObjectClass	msv_dpatCandidateSpacingXMin;
	SVDoubleValueObjectClass	msv_dpatCandidateSpacingYMin;

// Value Objects for Model Results.
	SVDoubleValueObjectClass	msv_dpatResultMatchScore;
	SVDoubleValueObjectClass	msv_dpatResultX;
	SVDoubleValueObjectClass	msv_dpatResultY;
	SVDoubleValueObjectClass	msv_dpatResultAngle;

	SVLongValueObjectClass		msv_lpatNumFoundOccurances; // define here or just reference result class ?

	vector2d<double>             m_vec2dPatResults;

	void CloseMIL();

	BOOL CreateModelBuffer();
	BOOL DestroyModelBuffer();

	BOOL UpdateModelFromInputImage();
	BOOL UpdateModelFromBuffer();

	BOOL RestorePattern(CString strImageFile, UINT *ErrMsgId = NULL);
	BOOL SetSearchParameters();
	void SetDefaultSearchValues();

	void GetModelExtents(long& xPos, long& yPos, long& width, long& height);
	void SetModelExtents(long xPos, long yPos, long width, long height);
	void SetCircularOverscan(bool bCircularOverscan);

	virtual BOOL IsPtOverResult(CPoint point);
	virtual	void DisplayAnalyzerResult();

	BOOL GetModelImageFileName(CString &csFileName);

	virtual HRESULT ResetObject();

protected:
	void CreateResult();

	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual HRESULT onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructCArray& p_rMultiLineArray );
	
	void ResetResultValues();
	void ResizeResultValues(int nNum);

	int m_nPatternIndex; // index of pattern moused over, for displaying results
	
private:
	BOOL ResetImageFile();

	long m_lpatModelWidth;		// Model Width.
	long m_lpatModelHeight;		// Model Height.
	long m_lpatModelOriginX;	// X coordinate of search region origin with in target image.
	long m_lpatModelOriginY;	// Y coordinate of search region origin with in target image.
	
	SVBoolValueObjectClass msv_bpatCircularOverscan;
	SVFileNameValueObjectClass msv_szModelImageFile; // Name of the Model Image File

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\svpatternanalyzerclass.h_v  $
 * 
 *    Rev 1.2   15 May 2014 11:21:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified processMessage to use DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:00:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:17:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   01 Aug 2012 13:04:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   30 Jul 2012 13:10:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated analyzer to use new extent hit test fucntionality for analyzer results.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   25 Jul 2012 14:43:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code and updated overlay functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   18 Jul 2012 14:19:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   02 Jul 2012 17:30:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   10 Feb 2011 15:09:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   27 Jan 2011 11:50:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   16 Dec 2009 12:25:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Jan 2009 15:36:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  644
 * SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support loading of old configurations.
 * Revised to set default for FailLow and WarnLow
 * Revised to set defaults for results if no occurrences found.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   31 Dec 2008 11:25:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  644
 * SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to suport Circular Overscan.
 * Revised to support advanced search parameters
 * Revised to support Number of Occurences found Result object
 * Revised to use array value object for results
 * Revised to correct problems with figure drawing
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   17 Feb 2005 15:15:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   19 Jan 2004 10:45:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed all non-statndard paint functionality in replace of SVDlgImageClass functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   22 Apr 2003 13:51:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   30 Jan 2003 15:54:46   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SetupDialog, OnValidate, CloseMIL, SetSearchParameters, RestorePattern, InitMIL, onRun and CreateObject methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   20 Nov 2002 10:39:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Cosmetic changes (member variable names)
 * Use SVImageInfoClass instead of direct MIL handle
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   07 Aug 2001 15:32:54   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  217
 * SCR Title:  Fix Dr. Watson errors in Pattern Match Tool
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added synchronization object to the class.
 * Added variable initialization and a synchronization object.calls
 *  to the function AllocateResultValues. 
 * Added synchronization object calls to the onUpdateFigure.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Jul 2001 21:24:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  217
 * SCR Title:  Fix Dr. Watson errors in Patch Match Tool
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new attribute m_lOccurances.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   13 Jun 2001 09:34:04   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Deleted the following functions: 	SelectFontFileName,  SelectMatchStringFileName,
 * GetFontFileName, GetMatchStringFileName.
 * Added the following functions: ResetImageFile.
 * Deleted the following functions: SelectModelImageFile.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 Feb 2001 13:36:42   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified source code to use new file management classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1.1.0   10 May 2001 18:37:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  198
 * SCR Title:  Update Pattern Match Tool to re-load model image when system goes on-line
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added ResetImageFile function
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Jul 2000 13:32:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  172
 * SCR Title:  Remove onUpdateFigure from onRun in SVPatternAnalyzerClass
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Remove onUpdateFigure call from onRun function and process correct information in respective thread.
 * 
 * Cleaned up memory management in onUpdateFigure as well.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
