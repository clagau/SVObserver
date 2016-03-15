// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCompressionDlg
// * .File Name       : $Workfile:   SVCompressionDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:22:08  $
// ******************************************************************************

#ifndef SVCOMPRESSIONDLG_H
#define SVCOMPRESSIONDLG_H

#include "lead.h"
/*
This is the class that manages access to the Lead Tools ActiveX Control.
*/
class SVCompressionDlg : public CDialog
{
public:
	SVCompressionDlg(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(SVCompressionDlg)
	enum { IDD = IDD_SVCOMPRESSION_DLG };
	// This attribute contains the interface to the Lead Tools image-processing library.
	CLead	svmLeadControl;
	//}}AFX_DATA


   HRESULT AllocateSaveBuffer (long auRequiredSize);

	// This attribute will point to the location of the image to process.
   BSTR*    svmpbstrOutputImage;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVCompressionDlg)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(SVCompressionDlg)
	afx_msg void OnSaveBufferLeadctrl1(long uRequiredSize);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(SVCompressionDlg)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

