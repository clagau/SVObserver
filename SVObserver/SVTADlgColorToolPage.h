//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorToolPage
//* .File Name       : $Workfile:   SVTADlgColorToolPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:16  $
//******************************************************************************

#if !defined(SVTADLGCOLORTOOLPAGE_H)
#define SVTADLGCOLORTOOLPAGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVTADlgColorToolPage.h : header file
//

#include "SVTaskObjectInterfaceClass.h"
#include "PictureDisplay.h"

class SVColorToolClass;
class SVToolClass;
class SVBoolValueObjectClass;

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorToolPage dialog

class SVTADlgColorToolPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
	
// Construction
public:
	SVTADlgColorToolPageClass( SVToolClass* PTool );
	~SVTADlgColorToolPageClass();

	virtual HRESULT SetInspectionData();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgColorToolPageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVTADlgColorToolPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnConvertToHsi();
	afx_msg void OnTrainColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void setImages();

protected:
	// Dialog Data
	//{{AFX_DATA(SVTADlgColorToolPageClass)
	enum { IDD = IDD_TA_COLOR_TOOL_DIALOG };
	PictureDisplay	m_dialogImage;
	BOOL	m_convertToHSI;
	//}}AFX_DATA

	SVColorToolClass* m_pTool;
	SVBoolValueObjectClass* m_pConvertToHSI;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(SVTADLGCOLORTOOLPAGE_H)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgColorToolPage.h_v  $
 * 
 *    Rev 1.1   26 Jun 2014 18:21:16   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * cleanup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:14:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Aug 2005 08:15:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 16:11:54   rschock
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
