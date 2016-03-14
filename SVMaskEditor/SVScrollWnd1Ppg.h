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

#if !defined(AFX_SVSCROLLWND1PPG_H__6DF99A49_EBB8_11D2_A759_00106F000C7A__INCLUDED_)
#define AFX_SVSCROLLWND1PPG_H__6DF99A49_EBB8_11D2_A759_00106F000C7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SVScrollWnd1Ppg.h : Declaration of the SVScrollWnd1PropPage property page class.

////////////////////////////////////////////////////////////////////////////
// SVScrollWnd1PropPage : See SVScrollWnd1Ppg.cpp.cpp for implementation.

class SVScrollWnd1PropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(SVScrollWnd1PropPage)
	DECLARE_OLECREATE_EX(SVScrollWnd1PropPage)

// Constructor
public:
	SVScrollWnd1PropPage();

// Dialog Data
	//{{AFX_DATA(SVScrollWnd1PropPage)
	enum { IDD = IDD_PROPPAGE_SVSCROLLWND1 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

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

#endif // !defined(AFX_SVSCROLLWND1PPG_H__6DF99A49_EBB8_11D2_A759_00106F000C7A__INCLUDED)

