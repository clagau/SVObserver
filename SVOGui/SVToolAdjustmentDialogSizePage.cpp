//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Property Page: Position and Size Page for The ToolAdjustmen Dialog
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "FormulaController.h"
#include "SVFormulaEditorSheet.h"
#include "DataController.h"
#include "SVToolAdjustmentDialogSizePage.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace
{

static LPCTSTR NoImageTag = _T("(No Image Available)");
static LPCTSTR NoToolTag = _T("(No Tool Available)");
}
namespace SvOg
{

SVToolAdjustmentDialogSizePage::SVToolAdjustmentDialogSizePage(uint32_t inspectionId, uint32_t taskObjectId)
	: CPropertyPage(SVToolAdjustmentDialogSizePage::IDD)
	, m_ipId(inspectionId)
	, m_toolId(taskObjectId)
	, m_ToolSizeHelper(inspectionId, taskObjectId)
	, m_ImageController {inspectionId, taskObjectId, SvPb::SVNotSetSubObjectType,false}
{}

SVToolAdjustmentDialogSizePage::~SVToolAdjustmentDialogSizePage()
{}

void SVToolAdjustmentDialogSizePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_TA_SIZE_MODE_WIDTH, m_ComboBox[SvDef::ToolSizeAdjustEnum::TSWidth]);
	DDX_Control(pDX, IDC_COMBO_TA_SIZE_MODE_HEIGHT, m_ComboBox[SvDef::ToolSizeAdjustEnum::TSHeight]);
	DDX_Control(pDX, IDC_COMBO_TA_SIZE_MODE_POSITIONX, m_ComboBox[SvDef::ToolSizeAdjustEnum::TSPositionX]);
	DDX_Control(pDX, IDC_COMBO_TA_SIZE_MODE_POSITIONY, m_ComboBox[SvDef::ToolSizeAdjustEnum::TSPositionY]);

	DDX_Control(pDX, IDC_BUTTON_TA_WIDTH_FORMULA, m_Button[SvDef::ToolSizeAdjustEnum::TSWidth]);
	DDX_Control(pDX, IDC_BUTTON_TA_HEIGHT, m_Button[SvDef::ToolSizeAdjustEnum::TSHeight]);
	DDX_Control(pDX, IDC_BUTTON_POSITION_FORMULA_X, m_Button[SvDef::ToolSizeAdjustEnum::TSPositionX]);
	DDX_Control(pDX, IDC_BUTTON_POSITION_Y, m_Button[SvDef::ToolSizeAdjustEnum::TSPositionY]);

	DDX_Control(pDX, IDC_WIDTH_EDIT, m_EditCtrl[SvDef::ToolSizeAdjustEnum::TSWidth]);
	DDX_Control(pDX, IDC_HEIGHT_EDIT, m_EditCtrl[SvDef::ToolSizeAdjustEnum::TSHeight]);
	DDX_Control(pDX, IDC_TRANSLATIONX_EDIT, m_EditCtrl[SvDef::ToolSizeAdjustEnum::TSPositionX]);
	DDX_Control(pDX, IDC_TRANSLATIONY_EDIT, m_EditCtrl[SvDef::ToolSizeAdjustEnum::TSPositionY]);

	DDX_Control(pDX, IDC_BUTTON_FULL_IMAGE, m_Button_Full_Image);
	DDX_Control(pDX, IDC_BUTTON_FORMULA_FROM, m_Button_FormulaFrom);
	DDX_Control(pDX, IDC_COMBO_IMAGE_LIST, m_ComboBoxImages);


}

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogSizePage, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_TA_SIZE_MODE_POSITIONY, &SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboPositionY)
	ON_CBN_SELCHANGE(IDC_COMBO_TA_SIZE_MODE_POSITIONX, &SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboPositionX)
	ON_CBN_SELCHANGE(IDC_COMBO_TA_SIZE_MODE_WIDTH, &SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboWidthMode)
	ON_CBN_SELCHANGE(IDC_COMBO_TA_SIZE_MODE_HEIGHT, &SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboHeightMode)
	ON_BN_CLICKED(IDC_BUTTON_TA_WIDTH_FORMULA, &SVToolAdjustmentDialogSizePage::OnBnClickedButtonTaWidthFormula)
	ON_BN_CLICKED(IDC_BUTTON_POSITION_Y, &SVToolAdjustmentDialogSizePage::OnBnClickedButtonPositionY)
	ON_BN_CLICKED(IDC_BUTTON_POSITION_FORMULA_X, &SVToolAdjustmentDialogSizePage::OnBnClickedButtonPositionFormulaX)
	ON_BN_CLICKED(IDC_BUTTON_TA_HEIGHT, &SVToolAdjustmentDialogSizePage::OnBnClickedButtonTaHeight)
	ON_BN_CLICKED(IDC_BUTTON_FULL_IMAGE, &SVToolAdjustmentDialogSizePage::OnBnClickedButtonFullImage)
	ON_BN_CLICKED(IDC_BUTTON_FORMULA_FROM, &SVToolAdjustmentDialogSizePage::OnBnClickedButtonFormulaFrom)
END_MESSAGE_MAP()

// SVToolAdjustmentDialogSizePage message handlers
BOOL SVToolAdjustmentDialogSizePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	m_ImageController.Init();
	m_ToolSizeHelper.InitValues();

	for (const auto type : SvDef::AllToolSizeAdjustEnum)
	{
		m_ComboBox[type].SetEnumTypes(m_ToolSizeHelper.GetEnumTypes(type));

	}

	bool isOk = m_ToolSizeHelper.InitToolList();
	const SvUl::NameObjectIdList& rAvailableToolList = m_ToolSizeHelper.GetAvailableToolList();

	std::string selectedToolName;
	if (rAvailableToolList.size())
	{

		selectedToolName = rAvailableToolList.begin()->first;
	}

	m_ComboBoxImages.Init(rAvailableToolList, selectedToolName, NoToolTag);

	if (isOk)
	{
		Refresh(false);
		UpdateData(FALSE);
	}
	else
	{
		// Not valid call...
		if (GetParent())
		{
			GetParent()->SendMessage(WM_CLOSE);
		}
		else
		{
			SendMessage(WM_CLOSE);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}

HRESULT SVToolAdjustmentDialogSizePage::SetInspectionData()
{
	HRESULT hresult = S_OK;
	UpdateData(true); // get data from dialog
	SizeModes modes;

	for (const auto vType : SvDef::AllToolSizeAdjustEnum)
	{
		int sel = m_ComboBox[vType].GetCurSel();
		if (sel >= 0)
		{
			long Value = (long)m_ComboBox[vType].GetItemData(sel);

			modes[vType] = (SvDef::ToolSizeModes)Value;

		}
		else
		{
			hresult = SvStl::Err_16029_InvalidSelection;
		}
		if (S_OK != hresult)
		{
			break;
		}
	}

	if (hresult == S_OK)
	{
		hresult = m_ToolSizeHelper.SetToolSizeMode(modes, false);
	}
	return hresult;
}

void SVToolAdjustmentDialogSizePage::Refresh(bool bSave /*= true*/)
{
	if (bSave)
	{
		SetInspectionData();
	}
	m_ToolSizeHelper.InitValues();
	m_ToolSizeHelper.SetFormulas(true, false);

	///Hide not allowed Controls
	int show = m_ToolSizeHelper.IsAdjustSizeAllowed() ? SW_SHOW : SW_HIDE;
	for (auto ad : SvDef::AllToolSizeAdjustEnum)
	{
		m_ComboBox[ad].ShowWindow(show);
		m_EditCtrl[ad].ShowWindow(show);
	}
	bool bEnable = m_ToolSizeHelper.IsAdjustSizeAllowed() && ((m_ToolSizeHelper.GetAutoSizeEnabled() & SvPb::EnableSize) == SvPb::EnableSize);

	m_ComboBox[SvDef::ToolSizeAdjustEnum::TSWidth].EnableWindow(bEnable);
	m_ComboBox[SvDef::ToolSizeAdjustEnum::TSHeight].EnableWindow(bEnable);

	bEnable = m_ToolSizeHelper.IsAdjustSizeAllowed() && ((m_ToolSizeHelper.GetAutoSizeEnabled() & SvPb::EnablePosition) == SvPb::EnablePosition);

	m_ComboBox[SvDef::ToolSizeAdjustEnum::TSPositionX].EnableWindow(bEnable);
	m_ComboBox[SvDef::ToolSizeAdjustEnum::TSPositionY].EnableWindow(bEnable);


	SizeModes Modes;
	SizeValues Values;
	m_ToolSizeHelper.GetToolSizeMode(true, Modes, Values);
	for (const auto vType : SvDef::AllToolSizeAdjustEnum)
	{
		long nShow = SvDef::TSFormula == Modes[vType] ? SW_SHOW : SW_HIDE;

		CString csResult = Values[vType].c_str();
		m_ComboBox[vType].SetCurSelItemData(Modes[vType]);
		m_EditCtrl[vType].SetWindowText(csResult);
		m_Button[vType].ShowWindow(nShow);
	}
	m_Button_Full_Image.ShowWindow(m_ToolSizeHelper.CanResizeToParent() ? SW_SHOW : SW_HIDE);
	UpdateData(false); // set data to dialog
}

void SVToolAdjustmentDialogSizePage::OnBnClickedButtonFormula(SvDef::ToolSizeAdjustEnum mode)
{
	if (mode < 0 || mode >= SvDef::ToolSizeAdjustEnum::TSValuesCount)
	{
		assert(false);
		return;
	}

	ToolSizeController::EQAdjustStruct  EqStruct = m_ToolSizeHelper.GetEquationStruct(mode);
	CString strCaption = EqStruct.m_name.c_str();
	strCaption += _T(" Formula");

	SvDef::SVObjectTypeInfoStruct info(SvPb::SVEquationObjectType, EqStruct.m_subType);
	SVFormulaEditorSheetClass dlg(m_ipId, m_toolId, info, strCaption);
	dlg.DoModal();
	Refresh(true);
}

void SVToolAdjustmentDialogSizePage::OnSelchangeCombo(SvDef::ToolSizeAdjustEnum  mode)
{
	/// full size must be selected in both or none of the combo boxes 
	DWORD_PTR selMode = m_ComboBox[mode].GetCurSelItemData();
	if (SvDef::TSAutoFit == selMode)
	{
		///set all other to full size 
		for (const auto type : SvDef::AllToolSizeAdjustEnum)
		{
			if (type == mode)
			{
				continue;
			}
			m_ComboBox[type].SetCurSelItemData(SvDef::TSAutoFit);
		}

	}
	else
	{
		///Then all must be not be auto fit 
		for (const auto type : SvDef::AllToolSizeAdjustEnum)
		{
			if (type == mode)
			{
				continue;
			}
			if (SvDef::TSAutoFit == m_ComboBox[type].GetCurSelItemData())
			{
				m_ComboBox[type].SetCurSelItemData(SvDef::TSNone);
			}
		}
	}


	Refresh(true);
}

void SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboWidthMode()
{
	OnSelchangeCombo(SvDef::ToolSizeAdjustEnum::TSWidth);
}

void SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboHeightMode()
{
	OnSelchangeCombo(SvDef::ToolSizeAdjustEnum::TSHeight);
}

void SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboPositionX()
{
	OnSelchangeCombo(SvDef::ToolSizeAdjustEnum::TSPositionX);
}

void SVToolAdjustmentDialogSizePage::OnCbnSelchangeComboPositionY()
{
	OnSelchangeCombo(SvDef::ToolSizeAdjustEnum::TSPositionY);
}

void SVToolAdjustmentDialogSizePage::OnBnClickedButtonTaWidthFormula()
{
	return OnBnClickedButtonFormula(SvDef::ToolSizeAdjustEnum::TSWidth);
}

void SVToolAdjustmentDialogSizePage::OnBnClickedButtonPositionY()
{
	return OnBnClickedButtonFormula(SvDef::ToolSizeAdjustEnum::TSPositionY);
}

void SVToolAdjustmentDialogSizePage::OnBnClickedButtonPositionFormulaX()
{
	return OnBnClickedButtonFormula(SvDef::ToolSizeAdjustEnum::TSPositionX);
}

void SVToolAdjustmentDialogSizePage::OnBnClickedButtonTaHeight()
{
	return OnBnClickedButtonFormula(SvDef::ToolSizeAdjustEnum::TSHeight);
}


void SVToolAdjustmentDialogSizePage::OnBnClickedButtonFormulaFrom()
{
	bool ready {false};

	UpdateData(TRUE); // get data from dialog
	int index = m_ComboBoxImages.GetCurSel();
	if (LB_ERR != index)
	{

		CString toolName;
		m_ComboBoxImages.GetLBText(index, toolName);
		if (!toolName.IsEmpty() && toolName != NoToolTag)
		{
			std::string stoolName(toolName);

			SizeValues values;
			for (auto en : SvDef::AllToolSizeAdjustEnum)
			{
				values[en] = "\"" + stoolName + "." +
					SvUl::LoadStdString(m_ToolSizeHelper.IDS_Objectnames[en])
					+ "\"";
			}
			m_ToolSizeHelper.SetAllToolSizeMode(SvDef::TSFormula, false);
			ready = m_ToolSizeHelper.SetFormulas(true, true, values);
			
		}
	}
	if (!ready)
	{
		m_ToolSizeHelper.SetAllToolSizeMode(SvDef::TSFormula, false);
		m_ToolSizeHelper.SetFormulas(true, true);
	}
	Refresh(false);
}

void SVToolAdjustmentDialogSizePage::OnBnClickedButtonFullImage()
{
	m_ToolSizeHelper.SetAllToolSizeMode(SvDef::TSNone, false);
	m_ToolSizeHelper.SetExtentToParent();
	Refresh(false);
}



bool SVToolAdjustmentDialogSizePage::QueryAllowExit()
{
	/// no empty formulas 

	for (const auto vType : SvDef::AllToolSizeAdjustEnum)
	{
		int sel = m_ComboBox[vType].GetCurSel();
		if (sel >= 0)
		{
			long Value = static_cast<long>(m_ComboBox[vType].GetItemData(sel));
			if (SvDef::TSFormula == Value)
			{
				DWORD eqId = m_ToolSizeHelper.GetEquationStruct(vType).m_Id;
				FormulaController formula(m_ipId, m_toolId, eqId);
				if (formula.GetEquationText().empty())
				{
					SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
					Exception.setMessage(SVMSG_SVO_64_EMPTY_FORMULAS_ARE_NOT_ALLOWED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16038_EmptyFormula);
					return false;
				}
			}
		}
	}
	return true;
}

BOOL SVToolAdjustmentDialogSizePage::OnSetActive()
{
	Refresh(false);
	return CPropertyPage::OnSetActive();
}

BOOL SVToolAdjustmentDialogSizePage::OnKillActive()
{
	HRESULT hr = E_FAIL;
	if (false == QueryAllowExit())
	{
		return false;
	}
	SvPb::InspectionCmdRequest requestCmd;
	auto* pRequest = requestCmd.mutable_resetobjectrequest();
	pRequest->set_objectid(m_toolId);

	hr = SvCmd::InspectionCommands(m_ipId, requestCmd, nullptr);
	if (S_OK == hr)
	{
		SvCmd::RunOnceSynchronous(m_ipId);
	}
	return CPropertyPage::OnKillActive();
}
} //namespace SvOg