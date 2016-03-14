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

#pragma once

#pragma region Includes
#include "SVMFCControls/SVDottedNameTreeCtrl.h"
#include "SVUtilityLibrary\SVGUID.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVCameraImageTemplate;

class SVToolAdjustmentDialogAcquisitionSourcePageClass : public CPropertyPage
{
public:
	SVToolAdjustmentDialogAcquisitionSourcePageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* PSheet );

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

