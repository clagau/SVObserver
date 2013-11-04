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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOCRGrayFontTrain\SVOCRGrayFontTrainCtl.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 10:31:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Aug 2000 12:58:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  173
 * SCR Title:  Fix OCR Font Training - Pressing ROI Ok button when no ROI is selected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OCR Gray FontTraining OCX to trap the error associated with trying to process a font image when no ROI was selected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 15:07:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 15:21:16   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Aug 26 1999 14:22:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Port code and bug fixes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/