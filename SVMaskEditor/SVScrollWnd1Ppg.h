//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVScrollWnd1Ppg
//* .File Name       : $Workfile:   SVScrollWnd1Ppg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Apr 2013 14:05:46  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "resource.h"
#pragma endregion Includes

class SVScrollWnd1PropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(SVScrollWnd1PropPage)
	DECLARE_OLECREATE_EX(SVScrollWnd1PropPage)

// Constructor
public:
	SVScrollWnd1PropPage();
	virtual ~SVScrollWnd1PropPage();

// Dialog Data
	//{{AFX_DATA(SVScrollWnd1PropPage)
	enum { IDD = IDD_PROPPAGE_SVSCROLLWND1 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(SVScrollWnd1PropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
