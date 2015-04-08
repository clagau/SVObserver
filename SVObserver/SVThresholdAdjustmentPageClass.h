//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThresholdAdjustmentPageClass
//* .File Name       : $Workfile:   SVThresholdAdjustmentPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:31:50  $
//******************************************************************************

#ifndef _SVTHRESHOLDADJUSTMENTPAGECLASS_H
#define _SVTHRESHOLDADJUSTMENTPAGECLASS_H

#include "SVEdge.h"
#include "SVEditNumbers.h"
#include "SVValueBaseNormalizerClass.h"
#include "SVValueObjectImpl.h"
#include "SVTaskObjectInterfaceClass.h"
#include "SVMFCControls/SVSliderCtrl.h"

class SVLineAnalyzerClass;
class SVTaskObjectClass;
class SVIPDoc;

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVThresholdAdjustmentPageClass 
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
//  :10.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVThresholdAdjustmentPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
	DECLARE_DYNCREATE(SVThresholdAdjustmentPageClass)

public:
// Dialogfelddaten
	//{{AFX_DATA(SVThresholdAdjustmentPageClass)
	enum { IDD = IDD_GR_THRESHOLD_ADJUST_DIALOG };
	SVSliderCtrl	UpperSliderCtrl;
	SVSliderCtrl	LowerSliderCtrl;
	CString	StrUpper;
	CString	StrLower;
	//}}AFX_DATA

protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(SVThresholdAdjustmentPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	SVThresholdAdjustmentPageClass( UINT nIDCaption = 0, int id = IDD );

public:
	~SVThresholdAdjustmentPageClass();

	virtual HRESULT SetInspectionData();

// Überschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVThresholdAdjustmentPageClass)
	public:
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL
	
protected:
	void updateDisplay();

public:
//#ifdef _DEBUG
//	virtual void AssertValid() const;
//	virtual void Dump(CDumpContext& dc) const;
//#endif

public:
	//edge value objects
	SVBoolValueObjectClass		*m_pvoUseLowerThresholdSelectable;
	SVBoolValueObjectClass		*m_pvoUseLowerThresholdMaxMinusPercentDiff;
	SVBoolValueObjectClass		*m_pvoUseLowerThresholdMaxMinusOffset;
	SVBoolValueObjectClass		*m_pvoUseLowerThresholdMinPlusOffset;
	SVDWordValueObjectClass		*m_pvoLowerThresholdMaxMinusPercentDiff;
	SVDWordValueObjectClass		*m_pvoLowerThresholdMaxMinusOffset;
	SVDWordValueObjectClass		*m_pvoLowerThresholdMinPlusOffset;
	SVBoolValueObjectClass		*m_pvoUseUpperThresholdSelectable;
	SVBoolValueObjectClass		*m_pvoUseUpperThresholdMaxMinusPercentDiff;
	SVBoolValueObjectClass		*m_pvoUseUpperThresholdMaxMinusOffset;
	SVBoolValueObjectClass		*m_pvoUseUpperThresholdMinPlusOffset;
	SVDWordValueObjectClass		*m_pvoUpperThresholdMaxMinusPercentDiff;
	SVDWordValueObjectClass		*m_pvoUpperThresholdMaxMinusOffset;
	SVDWordValueObjectClass		*m_pvoUpperThresholdMinPlusOffset;

	SVDWordValueObjectClass		*m_pvoUpperThreshold;
	SVDWordValueObjectClass		*m_pvoLowerThreshold;

	SVIPDoc						*PCurrentDocument;
	SVLineAnalyzerClass			*PCurrentAnalyzer;
	SVValueBaseNormalizerClass	Normalizer;
protected:

	SVTaskObjectClass*  pAnalyzerOwner;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif	// _SVTHRESHOLDADJUSTMENTPAGECLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVThresholdAdjustmentPageClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:31:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   09 Aug 2005 08:33:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   17 Feb 2005 14:51:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   04 Dec 2003 13:08:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Redid edge control value objects: creation, setup dialogs, validation, runtime, persistence.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   08 Oct 2003 16:23:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made more changes to the line analyzers to support printing and correct use of the edge related value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   08 Oct 2003 11:06:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   using the value objects for the edge parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   22 Apr 2003 11:24:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   20 Nov 2002 08:34:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed COM support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   19 Apr 2001 14:43:04   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   06 Sep 2000 16:31:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to include color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   14 Jun 2000 13:41:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  158
 * SCR Title:  Profile Tool Lower Threshold Setup problem
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added initEdgeParameters method.
 * Revised OnInitDialog to call initEdgeParameters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   29 Feb 2000 14:28:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  106
 * SCR Title:  Math Tool - List processing
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed Close Button on caption.
 * Added Handler for WM_SYSCOMMAND to restrict SC_CLOSE message.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   07 Feb 2000 15:06:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to correct problem with constructors in
 * SVEdgeMarkerAdjustmentPageClass and
 * SVThresholdAdjustmentPageClass which were changed for
 * the new profile tool
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   04 Feb 2000 09:36:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  94
 * SCR Title:  New Profile Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to reduce redundant code.
 * Revised constructor so this class can be used as a base class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:18:42   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Nov 11 1999 08:54:08   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Add code to 'save' the location of the measurement property sheet.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Nov 1999 14:22:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  33
 * SCR Title:  Port Profile Tool from v2.42
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added updateLineThresholds method called by 
 * OnSetActive and OnVscroll 
 * (SVImageToLineProject needs to be updated first before
 * running)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Oct 05 1999 09:52:44   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Edit controls repaired.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Aug 1999 13:35:10   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Ported to 3.0.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Aug 26 1999 18:31:04   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  Steve Jones
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/