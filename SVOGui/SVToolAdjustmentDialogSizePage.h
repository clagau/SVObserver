//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Property Page: Position and Size Page for The ToolAdjustmen Dialog
//******************************************************************************

#pragma once
#pragma region Includes
#include "Definitions/SVExtentEnums.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#include "ISVPropertyPageDialog.h"
#include "DataController.h"
#pragma endregion Includes

namespace SvOg
{
//! \brief Property Page: Position and Size for The ToolAdjustmen Dialog
class SVToolAdjustmentDialogSizePage : public CPropertyPage, public ISVPropertyPageDialog
{
	DECLARE_DYNAMIC(SVToolAdjustmentDialogSizePage)

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
	SvMc::SVEnumerateComboClass m_ComboBox[SvDef::ToolSizeAdjustEnum::TSValuesCount];
	typedef DataController<ValuesAccessor, ValuesAccessor::value_type> Controller;
	std::unique_ptr<ValueController> m_pTaskValueController;

	uint32_t m_ipId;
	uint32_t m_toolId;
	uint32_t m_taskId = SvDef::InvalidObjectId;
	struct EQAdjustStruct
	{
		uint32_t m_Id = SvDef::InvalidObjectId;
		SvPb::SVObjectSubTypeEnum m_subType = SvPb::SVObjectSubTypeEnum::SVNotSetSubObjectType;
		SvPb::EmbeddedIdEnum m_inputModeEmbeddedId = SvPb::NoEmbeddedId;
		SvPb::SVExtentPropertyEnum m_extentProp;
		std::string m_name;
	};
	EQAdjustStruct m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSValuesCount];
	CButton m_Button[SvDef::ToolSizeAdjustEnum::TSValuesCount];
	CEdit m_EditCtrl[SvDef::ToolSizeAdjustEnum::TSValuesCount];
	
	bool m_isFullSizeAllowed = false;
	bool m_isAdjustSizeAllowed = false;
	bool m_isAdjustPositionAllowed = false;
	SvPb::EAutoSize m_autoSizeEnabled = SvPb::EAutoSize::EnableNone;
};
} //namespace SvOg