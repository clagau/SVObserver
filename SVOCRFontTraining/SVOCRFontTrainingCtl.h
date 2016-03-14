//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCRFontTrainingCtrl
//* .File Name       : $Workfile:   SVOCRFontTrainingCtl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 16:37:40  $
//******************************************************************************

#if !defined(AFX_SVOCRFONTTRAININGCTL_H__9AEFBD76_F061_11D2_A7B2_00106F000C7D__INCLUDED_)
#define AFX_SVOCRFONTTRAININGCTL_H__9AEFBD76_F061_11D2_A7B2_00106F000C7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSVOCRFontTrainingCtrl : public COleControl
{
	DECLARE_DYNCREATE(CSVOCRFontTrainingCtrl)

// Constructor
public:
	CSVOCRFontTrainingCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVOCRFontTrainingCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CSVOCRFontTrainingCtrl();

	DECLARE_OLECREATE_EX(CSVOCRFontTrainingCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CSVOCRFontTrainingCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CSVOCRFontTrainingCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CSVOCRFontTrainingCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CSVOCRFontTrainingCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CSVOCRFontTrainingCtrl)
	afx_msg long DoSVOCRFontTraining(LPCTSTR PathFileSampleCharImage, LPCTSTR PathFileFontImagesAndLabels);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CSVOCRFontTrainingCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CSVOCRFontTrainingCtrl)
	dispidDoSVOCRFontTraining = 1L,
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOCRFONTTRAININGCTL_H__9AEFBD76_F061_11D2_A7B2_00106F000C7D__INCLUDED)

