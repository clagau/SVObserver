//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015, 2020 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// TADlgResizePage.h
//*****************************************************************************
/// Classes and function declarations for the ToolAdjustment Dialog for the Resize Tool
//******************************************************************************

#pragma once

#pragma region Includes
#include "Definitions/MatroxEnums.h"
#include "SVOGuiUtility/PictureDisplay.h"
#include "SVOGui/ISVPropertyPageDialog.h"  // for QueryAllowExit()
#include "SVOGuiUtility/ImageController.h"
#include "SVOGuiUtility/DataController.h"
#include "SVOGuiUtility/LinkedValueWidgetHelper.h"
#pragma endregion Includes

class TADlgSheetClass;
struct ResizeParameters;

enum ScaleFactorDimension { Width, Height, Max };

class TADlgResizePage :
	public CPropertyPage, 
	public SvOg::ISVPropertyPageDialog
{

public:
	TADlgResizePage(uint32_t inspectionID, uint32_t taskObjectID, TADlgSheetClass* Parent, int id = IDD);
	virtual ~TADlgResizePage();

	bool QueryAllowExit() override { return true; } //invalid resize parameters would already have been caught in OnAnyItemChanged()

private:
	void ResetAndFillInterpolationModeComboBox();
	void ResetAndFillOverscanModeComboBox();

	HRESULT SetupResizeImageControl();

	void getInterpolationModeFromDialog();
	void getOverscanModeFromDialog();
	bool CommitAndCheckNewParameterValues();

	void CommitValuesFromDialog();

	InterpolationMode m_selectedInterpolationMode = InterpolationMode::Default;
	OverscanMode m_selectedOverscanMode = OverscanMode::OverscanEnable;

	void UpdateImages();

	void GetAndDisplayValuesFromTool();

	TADlgSheetClass* m_ParentDialog;

	long m_ROITabHandle;
	long m_OutputTabHandle;

	SvOgu::ImageController m_ImageController;
	uint32_t m_ROIImageID = SvDef::InvalidObjectId;
	uint32_t m_OutputImageID = SvDef::InvalidObjectId;

	const uint32_t m_inspectionID;
	const uint32_t m_toolID;

	SvOgu::ValueController m_resizeValueController;

	CEdit m_contentScaleEdit[ScaleFactorDimension::Max];
	CButton m_contentScaleButton[ScaleFactorDimension::Max];
	std::array<std::unique_ptr<SvOgu::LinkedValueWidgetHelper>, ScaleFactorDimension::Max > m_contentScaleWidgets;

	CEdit m_formatScaleEdit[ScaleFactorDimension::Max];
	CButton m_formatScaleButton[ScaleFactorDimension::Max];
	std::array<std::unique_ptr<SvOgu::LinkedValueWidgetHelper>, ScaleFactorDimension::Max > m_formatScaleWidgets;

	CComboBox m_InterpolationModeCombo;
	std::vector<std::string> m_InterpolationModeNames;
	CComboBox m_OverscanModeCombo;
	std::vector<std::string> m_OverscanModeNames;

	//{{AFX_DATA(TADlgResizePage)
	enum { IDD = IDD_TA_RESIZE_DIALOG };
	SvOgu::PictureDisplay m_DialogImage;
	//}}AFX_DATA

	// Overrides
#pragma region AFX Virtual Methods
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(TADlgResizePage)
	virtual BOOL OnInitDialog() override;
	virtual BOOL OnSetActive() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL
#pragma endregion AFX Virtual Methods

#pragma region AFX MSG
	// Generated message map functions
	//{{AFX_MSG(TADlgResizePage)
	afx_msg void OnInterpolationModeChanged();
	afx_msg void OnOverscanChanged();
	afx_msg void OnContentWidthFactorChanged();
	afx_msg void OnContentHeightFactorChanged();
	afx_msg void OnFormatWidthFactorChanged();
	afx_msg void OnFormatHeightFactorChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg void OnButtonContentWidthFactor();
	afx_msg void OnButtonContentHeightFactor();
	afx_msg void OnButtonFormatWidthFactor();
	afx_msg void OnButtonFormatHeightFactor();
	
#pragma endregion AFX MSG
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
