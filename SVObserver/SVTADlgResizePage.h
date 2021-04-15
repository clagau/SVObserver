//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015, 2020 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// SVTADlgResizePage.h
//*****************************************************************************
/// Classes and function declarations for the ToolAdjustment Dialog for the Resize Tool
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMatroxLibrary/SVMatroxEnums.h"
#include "SVRPropertyTree/SVRPropTree.h"
#include "SVOGui/PictureDisplay.h"
#include "SVOGui/ISVPropertyPageDialog.h"  // for QueryAllowExit()
#include "SVOGui/ImageController.h"
#include "SVOGui/ObjectSelectorController.h"
#include "SVOGui/DataController.h"
#include "SVOGui/ValueEditWidgetHelper.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;
class SVRPropertyItem;


struct ResizeParameters;

enum ScaleFactorDimension { Width, Height, Max };

class SVTADlgResizePage :
	public CPropertyPage, 
	public SvOg::ISVPropertyPageDialog
{

public:
	SVTADlgResizePage(uint32_t inspectionID, uint32_t taskObjectID, SVToolAdjustmentDialogSheetClass* Parent, int id = IDD);
	virtual ~SVTADlgResizePage();

	bool QueryAllowExit() override { return true; } //invalid resize parameters would already have been caught in OnAnyItemChanged()

private:
	void ResetAndFillInterpolationModeComboBox();
	void ResetAndFillOverscanModeComboBox();

	HRESULT SetupResizeImageControl();

	std::string GetToolname() const;

	void getInterpolationModeFromDialog();
	void getOverscanModeFromDialog();
	bool CommitAndCheckNewParameterValues();

	void CommitValuesFromDialog();

	InterpolationMode m_selectedInterpolationMode = InterpolationMode::Default;
	OverscanMode m_selectedOverscanMode = OverscanMode::OverscanEnable;

	void UpdateImages();

	void GetAndDisplayValuesFromTool();
	void PickValue(CEdit& rEdit, UINT ResourceID);

	void traceScalefactorValues(const std::string &rHeading, bool alsoAsDouble=false) const ;

	SVToolAdjustmentDialogSheetClass* m_ParentDialog;

	static const std::vector<SvPb::EmbeddedIdEnum> ms_allScaleFactorEIDs;

	long m_ROITabHandle;
	long m_OutputTabHandle;

	SvOg::ImageController m_ImageController;
	uint32_t m_ROIImageID = SvDef::InvalidObjectId;
	uint32_t m_OutputImageID = SvDef::InvalidObjectId;

	SvOg::ObjectSelectorController m_resizeValueSelector;

	const uint32_t m_inspectionID;
	const uint32_t m_toolID;

	SvOg::ValueController m_resizeValueController;

	CEdit m_contentScaleEdit[ScaleFactorDimension::Max];
	CButton m_contentScaleButton[ScaleFactorDimension::Max];

	CEdit m_formatScaleEdit[ScaleFactorDimension::Max];
	CButton m_formatScaleButton[ScaleFactorDimension::Max];

	SvOg::ValueEditWidgetHelper	m_formatWidthHelper;
	SvOg::ValueEditWidgetHelper	m_formatHeightHelper;

	SvOg::ValueEditWidgetHelper	m_contentWidthHelper;
	SvOg::ValueEditWidgetHelper	m_contentHeightHelper;

	CComboBox m_InterpolationModeCombo;
	std::vector<std::string> m_InterpolationModeNames;
	CComboBox m_OverscanModeCombo;
	std::vector<std::string> m_OverscanModeNames;

	std::vector<std::reference_wrapper<SvOg::ValueEditWidgetHelper>> m_allEditHelpers;

	//{{AFX_DATA(SVTADlgResizePage)
	enum { IDD = IDD_TA_RESIZE_DIALOG };
	SvOg::PictureDisplay m_DialogImage;
	//}}AFX_DATA

	// Overrides
#pragma region AFX Virtual Methods
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgResizePage)
	virtual BOOL OnInitDialog() override;
	virtual BOOL OnSetActive() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL
#pragma endregion AFX Virtual Methods

#pragma region AFX MSG
	// Generated message map functions
	//{{AFX_MSG(SVTADlgResizePage)
	afx_msg void OnAnyItemChanged();
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
