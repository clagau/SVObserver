//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLineAnalyzer
//* .File Name       : $Workfile:   SVLineAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Feb 2014 11:13:08  $
//******************************************************************************

#ifndef SVLINEANALYZER_H
#define SVLINEANALYZER_H

#include "SVAnalyzer.h"
#include "SVToolExtentClass.h"

class SVLineClass;

class SVLineAnalyzerClass : public SVAnalyzerClass  
{
	SV_DECLARE_CLASS( SVLineAnalyzerClass );

	friend class SVImageClass;

public:
	SVLineAnalyzerClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVLINEANALYZER );
	virtual ~SVLineAnalyzerClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();
	virtual BOOL DisconnectInput(SVInObjectInfoStruct* pInObjectInfo);

	virtual HRESULT ResetObject();

	virtual HRESULT DoesObjectHaveExtents() const;
	virtual HRESULT GetImageExtent( SVImageExtentClass &p_rsvImageExtent );
	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );
	virtual HRESULT SetImageExtentToFit( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );

	virtual HRESULT GetRelativePositionPoints( SVExtentPointStruct &p_rsvPoint1, SVExtentPointStruct &p_rsvPoint2 );

	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint );

	virtual BOOL SetObjectOwner( SVObjectClass* PNewOwner );
	virtual BOOL SetObjectOwner( const GUID& RNewOwnerGUID );

	// Min,Max values used as inputs from the SVImageToLineProjectClass
	virtual BOOL GetMinMaxThresholdValues( DWORD& DWMinValue, DWORD& DWMaxValue ){ return FALSE; };

	// *** TB *** 
	//virtual SVDrawObjectListClass* GetDialogFigure();
	SVObjectClass* getImageToLineProject();

	SVObjectClass* getLineROI();

	virtual BOOL OnValidate();

	virtual HRESULT GetThresholdBarsOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine );
	virtual HRESULT GetHistogramOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine );
	virtual HRESULT GetEdgesOverlay( SVExtentMultiLineStruct &p_rsvMiltiLine );
	virtual HRESULT GetSelectedEdgeOverlays( SVExtentMultiLineStruct &p_MultiLine );

	BOOL m_bEnableDirection;
	BOOL m_bEnableEdgeSelect;
	BOOL m_bEnablePolarisation;
	BOOL m_bEnablePosition;
	BOOL m_bEnableThreshold;

	BOOL m_bSetupEdgeA;

protected:
	SVLineClass* getInputLine();

	// New for Profile Tool - Thresholding Options
	virtual BOOL getThresholdValues( DWORD& DWUpperThresholdValue, DWORD& DWLowerThresholdValue ){ return FALSE; };
	virtual BOOL getThresholdValues( DWORD& DWUpperThresholdValue, DWORD& DWLowerThresholdValue, int index ){ return FALSE; };

	virtual BOOL  onRun( SVRunStatusClass& RRunStatus );

	virtual LONG_PTR processMessage( DWORD DwMessageID, LONG_PTR DwMessageValue, LONG_PTR DwMessageContext );
	virtual void removeEmbeddedExtents();

	virtual HRESULT onCollectOverlays(SVImageClass *p_Image, SVExtentMultiLineStructCArray &p_MultiLineArray );

	void GetOverlayDrawlingAdjustments( bool& p_rIsGageTool, double& p_rHeight, double& p_rLineCorrect );

	// Input: Line
	SVInObjectInfoStruct		inputLineObjectInfo;

	// Embedded Objects
	SVDoubleValueObjectClass	extentX1;
	SVDoubleValueObjectClass	extentY1;
	SVDoubleValueObjectClass	extentX2;
	SVDoubleValueObjectClass	extentY2;
	SVDoubleValueObjectClass	extentLength;

	SVToolExtentClass           m_svLineExtent;

	// Draw Objects:
	//SVDrawObjectClass   m_svLineFigure;

	BOOL m_bDisplayAnalyzeLine;
	BOOL m_bDisplayAnalyzeEdge;

	//SVDrawObjectListClass m_svDialogFigure;

private:
	void addLineAnalyzerFriend();
	
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVLineAnalyzer.h_v  $
 * 
 *    Rev 1.1   01 Feb 2014 11:13:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:47:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.29   01 Aug 2012 12:53:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   27 Jul 2012 08:55:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   25 Jul 2012 14:27:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code and updated overlay functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   18 Jul 2012 13:40:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   16 Jul 2012 09:01:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated overlay collection functionality and removed duplicate fucntionality by consolidating it to the task object class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   10 Jul 2012 12:18:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated on collect overlay method to fix a problem with missing functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   02 Jul 2012 17:15:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   21 Mar 2011 12:11:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix performance issues with extent checks and updates.  Added additional methods to determine if object has extents and moved extent updated to  associated children.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   30 Jul 2009 12:00:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   10 Jun 2008 12:18:10   JSpila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  621
 * SCR Title:  Fix Bug in the SVImageClass
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upated source code to clear parent image information from the embedded image objects and the tool extent object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19.1.0   14 Jul 2009 14:31:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code with new data manager objects and image methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   10 Aug 2005 12:34:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   17 Feb 2005 14:37:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   19 Jan 2004 15:00:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  405
 * SCR Title:  Modify Image Overlay drawing for the display and SIAC
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made short term fix to force overlays to update when an image is requested through the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   04 Dec 2003 12:15:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   08 Oct 2003 10:56:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added BOOL values for the edge flags
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   22 Apr 2003 10:39:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   14 Jan 2003 14:01:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed metholds getImageToLineProject and getLineROI to be public instead of protected
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   10 Jan 2003 14:31:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new function VerifyImageForOverlay to determine which object has overlays on which image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   19 Apr 2001 14:41:46   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   26 Apr 2000 18:37:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  129
 * SCR Title:  Angular Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added getImageToLineProject method.
 * Added getLineROI method.
 * Revised GetTransform method to be public.
 * revised AddAnalyzerFriend method to remove embedded 
 * extents only for the old profile tool.
 * Revised message handler for 
 * SVMSGID_SET_SHADOWED_EXTENT.
 * Added message handler for 
 * SVMSGID_SET_SHADOWED_ABSEXTENT.
 * Revised setShadowedExtents to handle rotation.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   23 Mar 2000 14:05:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  117
 * SCR Title:  Bug Fixes for Integration of conditional Toolset/Tool Drawing.
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Intorduced/Revised checkDrawAllowed(...) override, to
 * check if tool is allowed to be drawn. 
 * (Necessary for conditional tool drawing)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   13 Mar 2000 16:32:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised onDraw to call getLineExtent.
 * Added getLineExtent to provide additional extent information
 * for SVImageToLineExtentClass owned lines.
 * Added removeEmbeddedExtents to hide and remove extents
 * for the lineAnalyzers in the Profile tool.
 * Revised setShadowedExtents to check if hidden/removed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   03 Mar 2000 12:22:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  112
 * SCR Title:  Expose Extents of Images
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVLineInfoClass methods to get/set extents.
 * Added Shadowed Extent Variables as embedded outputs.
 * Added handler for new SVMessage SVM_SET_SHAOWED_EXTENT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   Mar 01 2000 12:39:10   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Revised to make checkDrawAllowed(...) public.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   25 Feb 2000 17:26:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetTransform method to handle contextual drawing.
 * Revised onDraw to call GetTransform and PDrawContext->Transform
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   24 Feb 2000 16:43:28   sMarakkath
 * Project:  SVObserver
 * Change Request (SCR) nbr:  110
 * SCR Title:  Change Configuration Printout to print Tool "extents".
 * Checked in by:  sMarakkath;  Srithaj Marakkath
 * Change Description:  
 *   Made the function 'getInputLine' public, so that it can be accessed from SVConfigurationPrint.cpp
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0.1.0   Feb 22 2000 16:28:06   sMarakkath
 * Made the function 'getInputLine' public, so that it can be accessed from SVConfigurationPrint.cpp.
 * 
 *    Rev 3.4   feb 22 2000 15:31:30   Sri
 * Project:  SVObserver
 * Change Request (SCR) nbr:  110
 * SCR Title:  Change Configuration Printout to print Tool "extents".
 * Checked in by:  Sri;  Srithaj Marakkath
 * Change Description:  
 *    Made the function getInputLine public, so that it can be accessed from SVConfigurationPrint.cpp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVO_SRC\SVLineAnalyzer.h_v  $
 * 
 *    Rev 3.3   21 Feb 2000 14:42:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised onDraw to call checkDrawAllowed().
 * Revised onDraw to use relative extents.
 * Added checkDrawAllowed() method override.
 * Removed dead code - getThresholdValues()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   16 Feb 2000 16:57:10   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  105
 * SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Modified Draw functions to use the DrawContext structure
 * and removed unused methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   04 Feb 2000 09:31:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to add virtual methods for thresholding options.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 12:58:14   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Sep 10 1999 18:12:20   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Overridden SetObjectOwner() to detect Profile Tool or Gage Tool as owner.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Sep 1999 08:20:12   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVLineROI as a Friend Object Entity.
 * Removed Run method declaration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   31 Aug 1999 09:46:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to use SVRunStatusClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Aug 1999 14:24:06   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Introduced register input interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Aug 1999 14:46:34   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Changed to support OnValidate().
 * Changed to support new embedded Draw Objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Aug 1999 13:09:18   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Added Validate().
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Aug 1999 20:52:40   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Initial Check In.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

