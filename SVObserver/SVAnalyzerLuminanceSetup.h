//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAnalyzerLuminanceSetup
//* .File Name       : $Workfile:   SVAnalyzerLuminanceSetup.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:32:28  $
//******************************************************************************

#ifndef SVANALYZERLUMINANCESETUPCLASS_H
#define SVANALYZERLUMINANCESETUPCLASS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVTaskObjectInterfaceClass.h"

class SVLuminanceAnalyzerClass;

/////////////////////////////////////////////////////////////////////////////
// SVLuminanceAnalyzerSetupClass dialog

class SVLuminanceAnalyzerSetupClass : public CDialog, public SVTaskObjectInterfaceClass
{
public:
	SVLuminanceAnalyzerClass* m_pAnalyzer;

	virtual HRESULT SetInspectionData();

protected:
	SVErrorClass  msvError;

	// Construction
public:
	SVLuminanceAnalyzerSetupClass(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVLuminanceAnalyzerSetupClass)
	enum { IDD = IDD_LUMINANCE_ANALYZER_SETUP };
	BOOL	m_calcStdDev;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVLuminanceAnalyzerSetupClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVLuminanceAnalyzerSetupClass)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnRange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVANALYZERLUMINANCESETUPCLASS_H__FCF01565_CE72_11D3_A87A_00106F000C7D__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVAnalyzerLuminanceSetup.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:32:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Aug 2005 15:21:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Apr 2003 16:33:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Feb 2000 17:41:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  95
 * SCR Title:  Standard Deviation on Luminance Histogram
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin.
 * Setup Dialog for the Luminance Analyzer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/