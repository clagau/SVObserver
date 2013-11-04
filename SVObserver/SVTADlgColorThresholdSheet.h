//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorThresholdSheet
//* .File Name       : $Workfile:   SVTADlgColorThresholdSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:21:12  $
//******************************************************************************

#if !defined(SVTADLGCOLORTHRESHOLDSHEET_H)
#define SVTADLGCOLORTHRESHOLDSHEET_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVTADlgColorThresholdSheet.h : header file
//

#include "SVTADlgColorThresholdROI.h"
#include "SVTADlgColorThresholdAdjustment.h"
#include "SVTADlgColorThresholdOutput.h"
#include "SVROIFigureEditor.h"

class SVColorToolClass;

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdSheet

class SVTADlgColorThresholdSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(SVTADlgColorThresholdSheet)

// Construction
public:
	SVTADlgColorThresholdSheet(SVColorToolClass* aPTool, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	SVTADlgColorThresholdSheet(SVColorToolClass* aPTool, LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	virtual ~SVTADlgColorThresholdSheet();

	SVColorToolClass* GetTool() { return m_pTool; }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgColorThresholdSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions

protected:
	//{{AFX_MSG(SVTADlgColorThresholdSheet)
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	SVTADlgColorThresholdROI		mSheetROI;	
	SVTADlgColorThresholdAdjustment	mSheetBand0;	
	SVTADlgColorThresholdAdjustment	mSheetBand1;	
	SVTADlgColorThresholdAdjustment	mSheetBand2;	
	SVTADlgColorThresholdOutput		mSheetOutput;

	SVColorToolClass* m_pTool;

public:
	// Shared dialog data
	int		m_iZoom;
	CPoint  m_ptOrigin;

	CRect m_ImageRect;
	CRect m_rectROI;

	SVROIFigureEditor* m_pFigureEditor;

private:
	void AddTabs();
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(SVTADLGCOLORTHRESHOLDSHEET_H)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgColorThresholdSheet.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:21:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Aug 2006 16:03:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Made changes for color threshold dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 15:43:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Sep 2000 16:41:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New files added to the SVObserver project for color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/