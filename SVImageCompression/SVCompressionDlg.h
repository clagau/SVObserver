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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageCompression\SVCompressionDlg.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:22:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Feb 2010 08:19:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Feb 2003 18:06:10   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Changes to debug Image Compression.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/