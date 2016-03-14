//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCRGrayFontTrainCtrl
//* .File Name       : $Workfile:   SVOCRGrayFontTrainCtl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 10:31:02  $
//******************************************************************************

#if !defined(AFX_SVOCRGRAYFONTTRAINCTL_H__9AEFBD76_F061_11D2_A7B2_00106F000C7D__INCLUDED_)
#define AFX_SVOCRGRAYFONTTRAINCTL_H__9AEFBD76_F061_11D2_A7B2_00106F000C7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

/////////////////////////////////////////////////////////////////////////////
// CSVOCRGrayFontTrainCtrl : See SVOCRGRAYFONTTRAINCtl.cpp for implementation.

extern int *gplvLastOpStatus;

class CSVOCRGrayFontTrainCtrl : public COleControl
{
	DECLARE_DYNCREATE(CSVOCRGrayFontTrainCtrl)

// Constructor
public:
	CSVOCRGrayFontTrainCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVOCRGrayFontTrainCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CSVOCRGrayFontTrainCtrl();

	DECLARE_OLECREATE_EX(CSVOCRGrayFontTrainCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CSVOCRGrayFontTrainCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CSVOCRGrayFontTrainCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CSVOCRGrayFontTrainCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CSVOCRGrayFontTrainCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CSVOCRGrayFontTrainCtrl)
	afx_msg long DoSVOCRGrayFontTrain(LPCTSTR PathFileSampleCharImage, LPCTSTR PathFileFontImagesAndLabels);
	afx_msg BOOL SetLVLastCallBackErrorCode(long FAR* Code);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CSVOCRGrayFontTrainCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CSVOCRGrayFontTrainCtrl)
	dispidDoSVOCRGrayFontTrain = 1L,
	dispidSetLVLastCallBackErrorCode = 2L,
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOCRGRAYFONTTRAINCTL_H__9AEFBD76_F061_11D2_A7B2_00106F000C7D__INCLUDED)

