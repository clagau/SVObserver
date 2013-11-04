//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorThresholdOutput
//* .File Name       : $Workfile:   SVTADlgColorThresholdOutput.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:20:22  $
//******************************************************************************

#if !defined(SVTADLGCOLORTHRESHOLDOUTPUT_H)
#define SVTADLGCOLORTHRESHOLDOUTPUT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVTADlgColorThresholdOutput.h : header file
//

#include "SVTADlgColorThresholdBasePage.h"
#include "SVDlgImage.h"
#include "SVDlgImageEditor.h"

class SVColorThresholdClass;

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorThresholdOutput dialog

class SVTADlgColorThresholdOutput : public SVTADlgColorThresholdBasePage
{
	DECLARE_DYNCREATE(SVTADlgColorThresholdOutput)

// Construction
public:
	SVTADlgColorThresholdOutput();
	~SVTADlgColorThresholdOutput();

// Dialog Data
	//{{AFX_DATA(SVTADlgColorThresholdOutput)
	enum { IDD = IDD_TA_COLOR_THRESHOLD_OUTPUT_DIALOG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgColorThresholdOutput)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVTADlgColorThresholdOutput)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(SVTADLGCOLORTHRESHOLDOUTPUT_H)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgColorThresholdOutput.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:20:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Aug 2006 15:48:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  574
 * SCR Title:  Fix Color Tool so it can properly handle images larger than 640x480
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed Zoom / overlay bug
 * changed histogram scaling
 * added ROI feedback on threshold adjust pages
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
 *    Rev 1.1   22 Apr 2003 15:43:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Sep 2000 16:41:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  179
 * SCR Title:  Create Color SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New files added to the SVObserver project for color SVIM changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/