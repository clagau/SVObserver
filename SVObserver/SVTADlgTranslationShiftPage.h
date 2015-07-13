//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgTranslationShiftPage
//* .File Name       : $Workfile:   SVTADlgTranslationShiftPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:24:52  $
//******************************************************************************

#ifndef SVTADLGTRANSLATIONSHIFTPAGE_H
#define SVTADLGTRANSLATIONSHIFTPAGE_H

#include "SVEnumerateCombo.h"
#include "SVTaskObjectInterfaceClass.h"
#include "PropertyTree/PropTree.h"

class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVEvaluateClass;
class SVDoubleValueObjectClass;
class SVBoolValueObjectClass;
class SVEnumerateValueObjectClass;

enum SVShiftResultEnum
{
	PROP_SHIFT_TRANS_X		= 100,
	PROP_SHIFT_TRANS_Y		= 101,
	PROP_SHIFT_DISP_X		= 102,
	PROP_SHIFT_DISP_Y		= 103,
	PROP_SHIFT_RESULT_TOP	= 104,
	PROP_SHIFT_RESULT_LEFT	= 105,
};

class SVTADlgTranslationShiftPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
//	DECLARE_DYNAMIC(SVTADlgTranslationShiftPage)

public:
	SVTADlgTranslationShiftPageClass( SVToolAdjustmentDialogSheetClass* Parent );
	virtual ~SVTADlgTranslationShiftPageClass();

	virtual HRESULT SetInspectionData();

// Dialog Data
	enum { IDD = IDD_TA_TRANSLATION_SHIFT_DIALOG };

	virtual BOOL OnSetActive() override; 
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	SVRPropTree	m_Tree;


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLearn();
	afx_msg void OnBnClickedBtnNormalize();

protected:
	void refresh();
	void UpdateControls();
	void SetupShiftPropertyTree();
	void FillShiftProperties();

	SVToolAdjustmentDialogSheetClass* pParentDialog;
	SVToolClass* pTool;

	SVEvaluateClass* pEvaluateTranslationX;
	SVEvaluateClass* pEvaluateTranslationY;


public:
	BOOL m_performTranslation;
	afx_msg void OnBnClickedPerformTranslation();
	afx_msg void OnBnClickedTranslationXFormulaButton();
	afx_msg void OnBnClickedTranslationYFormulaButton();
	CString StrTranslationXValue;
	CString StrTranslationYValue;
	SVEnumerateComboClass m_ctlShiftModeCombo;

public:
	SVEnumerateValueObjectClass *m_pvoShiftMode;
	SVBoolValueObjectClass *m_pvoEnableSourceImageExtents;
	afx_msg void OnCbnSelchangeComboShiftMode();
	CButton m_btnFormulaX;
	CButton m_btnFormulaY;
	CButton m_btnSetReference;
	CButton m_btnNormalize;
	CListBox m_ctlShiftValueList;
	CStatic m_ShiftValueStatic;

private:
	long m_lShiftType;
public:
	CButton m_ctlEnableSourceImageExtents;
	afx_msg void OnBnClickedChkEnableSourceImageExtents();
};
#endif	// SVTADLGTRANSLATIONSHIFTPAGE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgTranslationShiftPage.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:24:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Jan 2013 16:43:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new checkbox for Enable source image Extents
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Jan 2013 15:13:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  801
 * SCR Title:  Add new Shift Tool to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added property tree
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/