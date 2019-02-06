//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Property Page: Position and Size Page for The ToolAdjustmen Dialog
//******************************************************************************

#pragma once
#pragma region Includes
#include "afxwin.h"
#include "SVMFCControls/SVEnumerateCombo.h"
#include "SVOGui\ISVPropertyPageDialog.h"
#include "Operators/ToolSizeAdjustTask.h"
#pragma endregion Includes

namespace SvOp
{
class EQAdjustSize;
}
namespace SvTo
{
class SVToolClass;
}

class SVToolAdjustmentDialogSheetClass;

//! \brief Property Page: Position and Size for The ToolAdjustmen Dialog
//!  has pointer to ToolSizeAdjustTask
class SVToolAdjustmentDialogSizePage : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
	DECLARE_DYNAMIC(SVToolAdjustmentDialogSizePage)

public:
	SVToolAdjustmentDialogSizePage(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* pParent);
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
	void OnSelchangeCombo(SvOp::ToolSizeAdjustTask::TSValues  mode);
	void OnBnClickedButtonFormula(SvOp::ToolSizeAdjustTask::TSValues  mode);
	DECLARE_MESSAGE_MAP()

public:
	
	// Dialog Data
	enum { IDD = IDD_TA_SIZE_DIALOG };

protected:
	
	//************************************
	//! Return pointer to the  4 SVEquationClass objects positions width and height 
	//! \param typ [in]
	//! \returns EQAdjustSize*
	//************************************
	SvOp::EQAdjustSize* GetEvaluateObject(SvOp::ToolSizeAdjustTask::TSValues typ  ) ;
	
	SvMc::SVEnumerateComboClass m_ComboBox[SvOp::ToolSizeAdjustTask::TSValuesCount];

	SVToolAdjustmentDialogSheetClass* m_pParentDialog;
	SvTo::SVToolClass* m_pTool;
	SvOp::ToolSizeAdjustTask* m_pToolSizeAdjustTask;
	SvOp::EQAdjustSize*  m_pEQAdjustSize[SvOp::ToolSizeAdjustTask::TSValuesCount];
	CButton m_Button[SvOp::ToolSizeAdjustTask::TSValuesCount];
	CEdit m_EditCtrl[SvOp::ToolSizeAdjustTask::TSValuesCount];
	
};
