//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogPassFailPageClass
//* .File Name       : $Workfile:   SVTADlgPassFailPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:16:30  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************

#ifndef SVTADLGPASSFAILPAGE_H
#define SVTADLGPASSFAILPAGE_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// Other Necessary Include File(s) - Module Link(s)
////////////////////////////////////////////////////////////////////////////////
#include "SVEditNumbers.h"

#include "SVTaskObjectInterfaceClass.h"

class SVRangeClass;
class SVToolClass;
////////////////////////////////////////////////////////////////////////////////
// Includes which are necessary to declare or define types, constants and macros
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// .Title       : Dialog Class SVToolAdjustmentDialogPassFailPageClass 
// -----------------------------------------------------------------------------
// .Description : e.g. This dialog class ...
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
class SVToolAdjustmentDialogPassFailPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{

//******************************************************************************
// Class Wizard Generated Message Map Entries:
//******************************************************************************
	// Generated message map functions
protected:
	//{{AFX_MSG(SVToolAdjustmentDialogPassFailPageClass)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
// Standard constructor
	SVToolAdjustmentDialogPassFailPageClass( SVToolClass* PTool );
	SVToolAdjustmentDialogPassFailPageClass();

	virtual HRESULT SetInspectionData();

	void OnOK();
	void UpdateRangeValues();

//******************************************************************************
// Destructor(s):
//******************************************************************************
public:
// Standard destructor
	~SVToolAdjustmentDialogPassFailPageClass();

	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();

//******************************************************************************
// Class Wizard Generated Virtual Function(s):
//******************************************************************************
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogPassFailPageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

//******************************************************************************
// Class Wizard Generated Dialog Data:
//******************************************************************************
protected:
	//{{AFX_DATA(SVToolAdjustmentDialogPassFailPageClass)
	enum { IDD = IDD_TA_PASS_FAIL_DIALOG };
	CEditNumbersFloat	m_warnLowEditCtrl;
	CEditNumbersFloat	m_warnHighEditCtrl;
	CEditNumbersFloat	m_failLowEditCtrl;
	CEditNumbersFloat	m_failHighEditCtrl;
	double	failHigh;
	double	failLow;
	double	warnHigh;
	double	warnLow;
	//}}AFX_DATA

//******************************************************************************
// Data Element(s):
//******************************************************************************
protected:
	SVRangeClass* pRange;
};

//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

#endif	//	SVTADLGPASSFAILPAGE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgPassFailPage.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:16:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   09 Aug 2005 08:18:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 16:11:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:43:10   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Oct 1999 16:31:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added OnOk method.
 * Revised to use CEditNumbersFloat control to restrict
 * entries to doubles.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Sep 1999 19:59:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  16
 * SCR Title:  Integrate Math Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised for new architecture.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
