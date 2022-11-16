//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogMaskPageClass
//* .File Name       : $Workfile:   SVToolAdjustmentDialogMaskPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:29:28  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "FilesystemUtilities/FileHelper.h"
#include "SVOGuiUtility\PictureDisplay.h"
#include "SVOGuiUtility\ImageController.h"
#include "SVOGuiUtility\DataController.h"
#include "SVOGuiUtility\MaskController.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#include "SVMFCControls\SVEnumerateCombo.h"
#pragma endregion Includes

namespace SvMc
{
class SVMaskEditor;
}

class SVToolAdjustmentDialogMaskPageClass : public CPropertyPage
{
public:
	SVToolAdjustmentDialogMaskPageClass(uint32_t inspectionId, uint32_t taskObjectId, uint32_t maskOperatorId);
	virtual ~SVToolAdjustmentDialogMaskPageClass();

protected:
	enum MaskTypeEnum
	{
		MASK_TYPE_STATIC = 0,
		MASK_TYPE_IMAGE = 1,
		MASK_TYPE_SHAPE = 2,
	};

	void initMask();
	void SetInspectionData();

	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogMaskPageClass)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnEditStaticMaskButton();
	afx_msg void OnEditShapeMaskButton();
	afx_msg void OnExportMaskButton();
	afx_msg void OnImportMaskButton();
	afx_msg void OnActivateMask();
	afx_msg void OnSelchangeMaskOperatorCombo();
	afx_msg void OnRadioImageMask();
	afx_msg void OnRadioStaticMask();
	afx_msg void OnSelchangeComboImageMask();
	afx_msg void OnButtonFillColorMore();
	afx_msg void OnRadioShapeMask();
	afx_msg void OnSelchangeComboFillOptions();
	afx_msg void OnChangeEditFillColor();
	afx_msg void OnSelchangeDrawMaskCriteria();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVToolAdjustmentDialogMaskPageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV
	virtual BOOL OnSetActive() override;
	//}}AFX_VIRTUAL

	//{{AFX_DATA(SVToolAdjustmentDialogMaskPageClass)
	enum { IDD = IDD_TA_MASK };
	CButton	m_btnImportMask;
	CButton	m_btnExportMask;
	CButton	m_btnEditStaticMask;
	CButton	m_btnEditShapeMask;
	CEdit m_ebFillColor;
	SvMc::SVEnumerateCombo m_cbFillOptions;
	CButton	m_btnFillColorMore;
	SvMc::AvailableObjectListComboBox<uint32_t> m_cbAvailableSourceImageList;
	SvMc::SVEnumerateCombo m_cbMaskOperator;
	SvOgu::PictureDisplay m_dialogImage;
	BOOL m_bActivateMask;
	int m_iMaskType;
	CString	m_sFillColor;
	SvMc::SVEnumerateCombo m_DrawCriteriaCombo;
	//}}AFX_DATA

	SvMc::SVMaskEditor* m_pMaskEditorCtl;
	SvFs::FileHelper	m_svfnFileName;    // SES 15-Jan-2001

	static UINT_PTR CALLBACK ColorDlgHookFn(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

private:
	void SetupMaskOperatorComboBox();
	void SetupFillAreaComboBox();
	void SetupDrawCriteriaCombo();

	void SetupImageMaskCombo();
	void ShowControls(int iMaskType);
	void setImages();

	void RetreiveCurrentlySelectedImageNames();

	static SVToolAdjustmentDialogMaskPageClass* m_pThis;
	const uint32_t m_InspectionID;
	const uint32_t m_TaskObjectID;
	SvOgu::ImageController m_ImageController;
	SvOgu::ValueController m_Values;
	SvOgu::MaskController m_maskController;
	std::string m_imageInputName;
	std::string m_imageName;
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

