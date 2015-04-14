//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgAcquisitionSourcePage
//* .File Name       : $Workfile:   SVTADlgAcquisitionSourcePage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:16:26  $
//******************************************************************************

#ifndef SVTADLGACQUISITIONSOURCEPAGE_H
#define SVTADLGACQUISITIONSOURCEPAGE_H

#include "SVMFCControls/SVDottedNameTreeCtrl.h"

class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVCameraImageTemplate;

class SVToolAdjustmentDialogAcquisitionSourcePageClass : public CPropertyPage
{
public:
	SVToolAdjustmentDialogAcquisitionSourcePageClass( SVToolAdjustmentDialogSheetClass* PSheet );

	virtual ~SVToolAdjustmentDialogAcquisitionSourcePageClass();

	//{{AFX_DATA(SVToolAdjustmentDialogAcquisitionSourcePageClass)
	enum { IDD = IDD_TA_ACQUISITION_DIALOG };
	SvMc::SVDottedNameTreeCtrlClass	availableDigitizerTreeCtrl;
	CString	StrSelectedDigitizer;
	BOOL	bChannel0;
	BOOL	bChannel1;
	BOOL	bChannel2;
	BOOL	bChannel3;
	//}}AFX_DATA

protected:
	//{{AFX_MSG(SVToolAdjustmentDialogAcquisitionSourcePageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectButton();
	afx_msg void OnSelchangedAvailableDigitizersTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChannel0Check();
	afx_msg void OnChannel1Check();
	afx_msg void OnChannel2Check();
	afx_msg void OnChannel3Check();
	afx_msg void OnMultibandRadio();
	afx_msg void OnSinglebandRadio();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVToolAdjustmentDialogAcquisitionSourcePageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


	void refresh();

	SVToolAdjustmentDialogSheetClass* pSheet;
	SVToolClass* pTool;
	SVCameraImageTemplate* pMainImage;

	int availableBandNum;
	int availableBandLink;

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgAcquisitionSourcePage.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:16:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   13 Apr 2011 15:35:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Aug 2005 08:15:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 15:43:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Jan 2003 16:41:42   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnSinglebandRadio, OnMultibandRadio, OnChannel3Check, OnChannel2Check, OnCannel1Check, OnChannel0Check, OnselectButton and refresh methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Nov 2002 13:32:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed OLE dispatch map
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Oct 2000 08:31:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  185
 * SCR Title:  Color SVIM Light Reference Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixes channel selection problem in light reference dialog and acquisition tool dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Feb 2000 23:57:46   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Initial Check In.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
//** EOF **
