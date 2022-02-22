//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
//*****************************************************************************
/// Property Page: Position and Size Page for The ToolAdjustmen Dialog
//******************************************************************************

#pragma once
#pragma region Includes
#include "Definitions/SVExtentEnums.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#include "ISVPropertyPageDialog.h"
#include "ToolSizeController.h"
#include "ImageController.h"
#include "SVMFCControls/AvailableObjectListComboBox.h"
#pragma endregion Includes

namespace SvOg
{
//! \brief Property Page: Position and Size for The ToolAdjustmen Dialog
class SVToolAdjustmentDialogSizePage : public CPropertyPage, public ISVPropertyPageDialog
{
public:
	SVToolAdjustmentDialogSizePage(uint32_t inspectionId, uint32_t taskObjectId);
	virtual ~SVToolAdjustmentDialogSizePage();
	HRESULT SetInspectionData();
	virtual BOOL OnInitDialog() override;
	
	// ISVPropertyPageDialog
	virtual bool QueryAllowExit() override;
	virtual BOOL OnSetActive() override;
	virtual BOOL OnKillActive() override;  

	afx_msg void OnCbnSelchangeComboPositionX();
	afx_msg void OnCbnSelchangeComboPositionY();
	afx_msg void OnCbnSelchangeComboWidthMode();
	afx_msg void OnCbnSelchangeComboHeightMode();
	afx_msg void OnBnClickedButtonTaWidthFormula();
	afx_msg void OnBnClickedButtonPositionY();
	afx_msg void OnBnClickedButtonPositionFormulaX();
	afx_msg void OnBnClickedButtonTaHeight();

	afx_msg void OnBnClickedButtonFormulaFrom();
	afx_msg void OnBnClickedButtonFullImage();



protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	void Refresh( bool bSave = true );
	void OnSelchangeCombo(SvDef::ToolSizeAdjustEnum  mode);
	void OnBnClickedButtonFormula(SvDef::ToolSizeAdjustEnum  mode);
	DECLARE_MESSAGE_MAP()

public:
	
	// Dialog Data
	enum { IDD = IDD_TA_SIZE_DIALOG };

private:
	
	SvMc::SVEnumerateCombo m_ComboBox[SvDef::ToolSizeAdjustEnum::TSValuesCount];
	uint32_t m_ipId {SvDef::InvalidObjectId };
	uint32_t m_toolId {SvDef::InvalidObjectId}; ;
	CButton m_Button[SvDef::ToolSizeAdjustEnum::TSValuesCount];
	CEdit m_EditCtrl[SvDef::ToolSizeAdjustEnum::TSValuesCount];
	ToolSizeController m_ToolSizeHelper;
	ImageController m_ImageController;
	CButton m_Button_Full_Image;
	CButton m_Button_FormulaFrom;

	SvMc::AvailableObjectListComboBox<uint32_t> m_ComboBoxImages;


};
} //namespace SvOg