//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVLibrary\SVFileNameClass.h"
#include "SVMFCControls\SVMaskEditor.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui\PictureDisplay.h"
#include "SVMFCControls\AvailableObjectListComboBox.h"
#include "SVOGui\ImageController.h"
#include "SVOGui\BoundValue.h"
#include "SVOGui\ValuesAccessor.h"
#include "SVOGui\DataController.h"
#include "SVOGui\MaskController.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVToolAdjustmentDialogMaskPageClass : public CPropertyPage
{
	typedef SvOg::ValuesAccessor<SvOg::BoundValues> Values;
	typedef SvOg::DataController<Values, Values::value_type> ValueController;

public:
	SVToolAdjustmentDialogMaskPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID);
	virtual ~SVToolAdjustmentDialogMaskPageClass();

protected:
	enum MaskTypeEnum
	{
		MASK_TYPE_STATIC = 0,
		MASK_TYPE_IMAGE = 1,
		MASK_TYPE_SHAPE = 2,
	};

	void initMask();
	HRESULT SetInspectionData();

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
	CComboBox m_cbFillOptions;
	CButton	m_btnFillColorMore;
	SvMc::AvailableObjectListComboBox m_cbAvailableSourceImageList;
	CComboBox m_cbMaskOperator;
	SvOg::PictureDisplay m_dialogImage;
	BOOL m_bActivateMask;
	int m_iMaskType;
	CString	m_sFillColor;
	CComboBox m_DrawCriteriaCombo;
	//}}AFX_DATA

	SvMc::CSVMaskEditor* m_pMaskEditorCtl;
	SVFileNameClass	m_svfnFileName;    // SES 15-Jan-2001

	static UINT_PTR CALLBACK ColorDlgHookFn(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

private:
	void SetupMaskOperatorComboBox();
	void SetupFillAreaComboBox();
	void SetupDrawCriteriaCombo();

	void SetupComboBoxes();
	void SetupImageMaskCombo();
	void ShowControls(int iMaskType);
	void setImages();
	void SetData();
	void GetData();

	void RefreshComboBox(const CString& rValue, CComboBox& rComboBox); 
	
	void RetreiveCurrentlySelectedImageNames();
	void RetreiveResultImageNames();

	static SVToolAdjustmentDialogMaskPageClass* m_pThis;
	SVGUID m_InspectionID;
	SVGUID m_TaskObjectID;
	SvOg::ImageController m_ImageController;
	ValueController m_Values;
	SvOg::MaskController m_maskController;
	SVString m_imageInputName;
	SVString m_imageName;
	SVString m_resultImageName;
	SVGUID m_resultImageID;
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

