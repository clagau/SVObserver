//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCRFontTrainingPropPage
//* .File Name       : $Workfile:   SVOCRFontTrainingPpg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 16:38:10  $
//******************************************************************************

#if !defined(AFX_SVOCRFONTTRAININGPPG_H__9AEFBD78_F061_11D2_A7B2_00106F000C7D__INCLUDED_)
#define AFX_SVOCRFONTTRAININGPPG_H__9AEFBD78_F061_11D2_A7B2_00106F000C7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////////
// CSVOCRFontTrainingPropPage : See SVOCRFontTrainingPpg.cpp.cpp for implementation.

class CSVOCRFontTrainingPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CSVOCRFontTrainingPropPage)
	DECLARE_OLECREATE_EX(CSVOCRFontTrainingPropPage)

// Constructor
public:
	CSVOCRFontTrainingPropPage();

// Dialog Data
	//{{AFX_DATA(CSVOCRFontTrainingPropPage)
	enum { IDD = IDD_PROPPAGE_SVOCRFONTTRAINING };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CSVOCRFontTrainingPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOCRFONTTRAININGPPG_H__9AEFBD78_F061_11D2_A7B2_00106F000C7D__INCLUDED)

