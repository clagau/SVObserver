//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCRGrayFontTrainPropPage
//* .File Name       : $Workfile:   SVOCRGrayFontTrainPpg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 10:31:32  $
//******************************************************************************

#if !defined(AFX_SVOCRGRAYFONTTRAINPPG_H__9AEFBD78_F061_11D2_A7B2_00106F000C7D__INCLUDED_)
#define AFX_SVOCRGRAYFONTTRAINPPG_H__9AEFBD78_F061_11D2_A7B2_00106F000C7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////////
// CSVOCRGrayFontTrainPropPage : See SVOCRGrayFontTrainPpg.cpp.cpp for implementation.

class CSVOCRGrayFontTrainPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CSVOCRGrayFontTrainPropPage)
	DECLARE_OLECREATE_EX(CSVOCRGrayFontTrainPropPage)

// Constructor
public:
	CSVOCRGrayFontTrainPropPage();

// Dialog Data
	//{{AFX_DATA(CSVOCRGrayFontTrainPropPage)
	enum { IDD = IDD_PROPPAGE_SVOCRGRAYFONTTRAIN };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CSVOCRGrayFontTrainPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOCRGRAYFONTTRAINPPG_H__9AEFBD78_F061_11D2_A7B2_00106F000C7D__INCLUDED)

