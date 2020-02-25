//******************************************************************************
/// COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Property Page: Position and Size Page for The ToolAdjustmen Dialog
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Definitions/TextDefineSvDef.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "DataController.h"
#include "FormulaController.h"
#include "SVFormulaEditorSheet.h"
#include "ValuesAccessor.h"
#include "SVToolAdjustmentDialogSizePage.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvOg
{
IMPLEMENT_DYNAMIC(SVToolAdjustmentDialogSizePage, CPropertyPage)

SVToolAdjustmentDialogSizePage::SVToolAdjustmentDialogSizePage(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID)
: CPropertyPage(SVToolAdjustmentDialogSizePage::IDD)
, m_ipId(rInspectionID)
, m_toolId(rTaskObjectID)
{
}

SVToolAdjustmentDialogSizePage::~SVToolAdjustmentDialogSizePage()
{
}

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
END_MESSAGE_MAP()

// SVToolAdjustmentDialogSizePage message handlers
BOOL SVToolAdjustmentDialogSizePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	bool isOk = true;

	// Get the Evaluate Objects..
	SvDef::SVObjectTypeInfoStruct evaluateObjectInfo;
	evaluateObjectInfo.m_ObjectType = SvPb::SVEquationObjectType;
	evaluateObjectInfo.m_SubType = SvPb::EQSizePositionXType;
	SvPb::InspectionCmdMsgs requestMessage, responseMessage;
	auto* pRequest = requestMessage.mutable_getobjectidrequest()->mutable_info();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_ownerid(), m_toolId);
	SvCmd::setTypeInfos(evaluateObjectInfo, *pRequest->mutable_infostruct());
	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
	if (S_OK == hr && responseMessage.has_getobjectidresponse())
	{
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSPositionX].m_Id = SvPb::GetGuidFromProtoBytes(responseMessage.getobjectidresponse().objectid());
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSPositionX].m_name = SvUl::LoadStdString(IDS_CLASSNAME_EQ_ADJUSTSIZE_POSITION_X);
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSPositionX].m_subType = SvPb::EQSizePositionXType;
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSPositionX].m_extentProp = SvPb::SVExtentPropertyPositionPointX;
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSPositionX].m_inputModeEmbeddedId = SvPb::ToolSizeAdjustSizePositionXModeEId;
	}
	else
	{
		isOk = false;
		assert(false);
	}

	evaluateObjectInfo.m_SubType = SvPb::EQSizePositionYType;
	SvPb::SetGuidInProtoBytes(pRequest->mutable_ownerid(), m_toolId);
	SvCmd::setTypeInfos(evaluateObjectInfo, *pRequest->mutable_infostruct());
	hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
	if (S_OK == hr && responseMessage.has_getobjectidresponse())
	{
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSPositionY].m_Id = SvPb::GetGuidFromProtoBytes(responseMessage.getobjectidresponse().objectid());
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSPositionY].m_name = SvUl::LoadStdString(IDS_CLASSNAME_EQ_ADJUSTSIZE_POSITION_Y);
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSPositionY].m_subType = SvPb::EQSizePositionYType;
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSPositionY].m_extentProp = SvPb::SVExtentPropertyPositionPointY;
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSPositionY].m_inputModeEmbeddedId = SvPb::ToolSizeAdjustSizePositionYModeEId;
	}
	else
	{
		isOk = false;
		assert(false);
	}

	evaluateObjectInfo.m_SubType = SvPb::EQSizeWidthType;
	SvPb::SetGuidInProtoBytes(pRequest->mutable_ownerid(), m_toolId);
	SvCmd::setTypeInfos(evaluateObjectInfo, *pRequest->mutable_infostruct());
	hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
	if (S_OK == hr && responseMessage.has_getobjectidresponse())
	{
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSWidth].m_Id = SvPb::GetGuidFromProtoBytes(responseMessage.getobjectidresponse().objectid());
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSWidth].m_name = SvUl::LoadStdString(IDS_CLASSNAME_EQ_ADJUSTSIZE_WIDTH);
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSWidth].m_subType = SvPb::EQSizeWidthType;
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSWidth].m_extentProp = SvPb::SVExtentPropertyWidth;
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSWidth].m_inputModeEmbeddedId = SvPb::ToolSizeAdjustSizeWidthModeEId;
	}
	else
	{
		isOk = false;
		assert(false);
	}

	evaluateObjectInfo.m_SubType = SvPb::EQSizeHeightType;
	SvPb::SetGuidInProtoBytes(pRequest->mutable_ownerid(), m_toolId);
	SvCmd::setTypeInfos(evaluateObjectInfo, *pRequest->mutable_infostruct());
	hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
	if (S_OK == hr && responseMessage.has_getobjectidresponse())
	{
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSHeight].m_Id = SvPb::GetGuidFromProtoBytes(responseMessage.getobjectidresponse().objectid());
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSHeight].m_name = SvUl::LoadStdString(IDS_CLASSNAME_EQ_ADJUSTSIZE_HEIGHT);
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSHeight].m_subType = SvPb::EQSizeHeightType;
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSHeight].m_extentProp = SvPb::SVExtentPropertyHeight;
		m_EQAdjustStruct[SvDef::ToolSizeAdjustEnum::TSHeight].m_inputModeEmbeddedId = SvPb::ToolSizeAdjustSizeHeightModeEId;
	}
	else
	{
		isOk = false;
		assert(false);
	}

	// Get ToolSizeAdjustTask 
	SvDef::SVObjectTypeInfoStruct ToolSizeAdjustTaskInfo;
	ToolSizeAdjustTaskInfo.m_ObjectType = SvPb::SVToolSizeAdjustTaskType;
	SvCmd::setTypeInfos(ToolSizeAdjustTaskInfo, *pRequest->mutable_infostruct());
	hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
	if (S_OK == hr && responseMessage.has_getobjectidresponse())
	{
		m_taskId = SvPb::GetGuidFromProtoBytes(responseMessage.getobjectidresponse().objectid());
		m_pTaskValueController = std::make_unique<Controller>(BoundValues {m_ipId, m_taskId});
		if (nullptr != m_pTaskValueController)
		{
			m_pTaskValueController->Init();
		}
		else
		{
			assert(false);
			isOk = false;
		}
	}
	else
	{
		assert(false);
		isOk = false;
	}

	if (isOk)
	{
		auto* pParamRequest = requestMessage.mutable_gettoolsizeadjustparameterrequest();
		SvPb::SetGuidInProtoBytes(pParamRequest->mutable_objectid(), m_taskId);
		hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
		if (S_OK == hr && responseMessage.has_gettoolsizeadjustparameterresponse())
		{
			auto responseData = responseMessage.gettoolsizeadjustparameterresponse();
			m_isFullSizeAllowed = responseData.isfullsizeallowed();
			m_isAdjustSizeAllowed = responseData.isadjustsizeallowed();
			m_isAdjustPositionAllowed = responseData.isadjustpositionallowed();
			m_autoSizeEnabled = responseData.enableautosize();
		}

		for (int vType = SvDef::ToolSizeAdjustEnum::TSPositionX; vType < SvDef::ToolSizeAdjustEnum::TSValuesCount; ++vType)
		{
			m_ComboBox[vType].SetEnumTypes(m_pTaskValueController->GetEnumTypes(m_EQAdjustStruct[vType].m_inputModeEmbeddedId));
			if (!m_isFullSizeAllowed)
			{
				///Remove Fullsize from combobox
				int index = m_ComboBox[vType].FindString(-1, SvDef::SizeAdjustTextFullSize);
				if (index >= 0)
				{
					m_ComboBox[vType].DeleteString(index);
				}
			}
		}
	}

	if (isOk)
	{
		Refresh(false);
		UpdateData(FALSE);
	}
	else
	{
		// Not valid call...
		if( GetParent())
		{
			GetParent()->SendMessage( WM_CLOSE );
		}
		else
		{
			SendMessage( WM_CLOSE );
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}

HRESULT SVToolAdjustmentDialogSizePage::SetInspectionData()
{
	HRESULT hresult = S_OK;
	UpdateData(true); // get data from dialog

	for (int vType = SvDef::ToolSizeAdjustEnum::TSPositionX; vType < SvDef::ToolSizeAdjustEnum::TSValuesCount; ++vType)
	{
		int sel = m_ComboBox[vType].GetCurSel();
		if (sel >= 0)
		{
			long Value = (long)m_ComboBox[vType].GetItemData(sel);

			if ((vType == SvDef::ToolSizeAdjustEnum::TSPositionX || vType == SvDef::ToolSizeAdjustEnum::TSPositionY))
			{
				if ((m_autoSizeEnabled & SvPb::EnablePosition) == 0)
				{
					Value = SvDef::TSNone;
				}
			}
			else if ((m_autoSizeEnabled & SvPb::EnableSize) == 0)
			{
				Value = SvDef::TSNone;
			}

			m_pTaskValueController->Set<long>(m_EQAdjustStruct[vType].m_inputModeEmbeddedId, Value);
			bool bEnabled = (Value == SvDef::TSFormula);
			Controller EquationValues {BoundValues{ m_ipId, m_EQAdjustStruct[vType].m_Id }};
			EquationValues.Init();
			EquationValues.Set<bool>(SvPb::EquationEnabledEId, bEnabled);
			EquationValues.Commit(PostAction::doNothing);
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

	m_pTaskValueController->Commit();
	return hresult;
}

void SVToolAdjustmentDialogSizePage::Refresh( bool bSave /*= true*/ )
{
	if (bSave)
	{
		SetInspectionData();
	}

	///Hide not allowed Controls
	int show = (!m_isAdjustSizeAllowed) ? SW_HIDE : SW_SHOW;

	bool bEnable = (!m_isAdjustSizeAllowed) ? false : true;
	bEnable = (m_autoSizeEnabled & SvPb::EnableSize) == SvPb::EnableSize ? bEnable : false;
	m_ComboBox[SvDef::ToolSizeAdjustEnum::TSWidth].ShowWindow(show);
	m_EditCtrl[SvDef::ToolSizeAdjustEnum::TSWidth].ShowWindow(show);
	m_ComboBox[SvDef::ToolSizeAdjustEnum::TSHeight].ShowWindow(show);
	m_EditCtrl[SvDef::ToolSizeAdjustEnum::TSHeight].ShowWindow(show);

	m_ComboBox[SvDef::ToolSizeAdjustEnum::TSWidth].EnableWindow(bEnable);
	m_ComboBox[SvDef::ToolSizeAdjustEnum::TSHeight].EnableWindow(bEnable);

	show = (!m_isAdjustPositionAllowed) ? SW_HIDE : SW_SHOW;
	bEnable = (!m_isAdjustPositionAllowed) ? false : true;
	bEnable = (m_autoSizeEnabled & SvPb::EnablePosition) == SvPb::EnablePosition ? bEnable : false;

	m_ComboBox[SvDef::ToolSizeAdjustEnum::TSPositionX].ShowWindow(show);
	m_EditCtrl[SvDef::ToolSizeAdjustEnum::TSPositionX].ShowWindow(show);
	m_ComboBox[SvDef::ToolSizeAdjustEnum::TSPositionY].ShowWindow(show);
	m_EditCtrl[SvDef::ToolSizeAdjustEnum::TSPositionY].ShowWindow(show);

	m_ComboBox[SvDef::ToolSizeAdjustEnum::TSPositionX].EnableWindow(bEnable);
	m_ComboBox[SvDef::ToolSizeAdjustEnum::TSPositionY].EnableWindow(bEnable);

	::google::protobuf::RepeatedPtrField< ::SvPb::ExtentParameter > extentParameter, parentExtentParameter;
	SvPb::InspectionCmdMsgs requestMessage, responseMessage;
	auto* pRequest = requestMessage.mutable_getextentparameterrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_objectid(), m_toolId);
	HRESULT hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
	if (S_OK == hr && responseMessage.has_getextentparameterresponse())
	{
		extentParameter = responseMessage.getextentparameterresponse().parameters();
	}
	responseMessage = SvPb::InspectionCmdMsgs(); //reset response
	pRequest->set_shouldfromparent(true);
	hr = SvCmd::InspectionCommands(m_ipId, requestMessage, &responseMessage);
	if (S_OK == hr && responseMessage.has_getextentparameterresponse())
	{
		parentExtentParameter = responseMessage.getextentparameterresponse().parameters();
	}

	// refresh  combo settings...
	for (int vType = SvDef::ToolSizeAdjustEnum::TSPositionX; vType < SvDef::ToolSizeAdjustEnum::TSValuesCount; ++vType)
	{
		long nShow(SW_HIDE);
		CString csResult;
		long SelMode = m_pTaskValueController->Get<long>(m_EQAdjustStruct[vType].m_inputModeEmbeddedId);
		if (SvDef::ToolSizeAdjustEnum::TSPositionX == vType || SvDef::ToolSizeAdjustEnum::TSPositionY == vType)
		{
			if (0 == (m_autoSizeEnabled & SvPb::EnablePosition))
			{
				SelMode = SvDef::TSNone;
			}
		}
		else if (0 == (m_autoSizeEnabled & SvPb::EnableSize))
		{
			SelMode = SvDef::TSNone;
		}

		m_ComboBox[vType].SetCurSelItemData(SelMode);

		if (SvDef::TSFormula == SelMode)
		{
			nShow = SW_SHOW;
			FormulaController formula(m_ipId, m_toolId, m_EQAdjustStruct[vType].m_Id);
			bool isOwnerEnabled, isEquationEnabled = false;
			formula.IsOwnerAndEquationEnabled(isOwnerEnabled, isEquationEnabled);
			std::string text = formula.GetEquationText();
			if (isEquationEnabled && !text.empty())
			{
				double result = 0;
				SvStl::MessageContainerVector errorMessage;
				int errorPos = formula.ValidateEquation(text, result, false, errorMessage);
				if (FormulaController::validateSuccessful == errorPos)
				{
					csResult.Format(_T("%ld"), static_cast<long>(result));
				}
			}
		}
		else if (SvDef::TSNone == SelMode)
		{
			auto propEnum = m_EQAdjustStruct[vType].m_extentProp;
			auto valuePair = std::find_if(extentParameter.begin(), extentParameter.end(), [propEnum](const auto value) { return value.type() == propEnum; });
			if (extentParameter.end() != valuePair)
			{
				csResult.Format(_T("%ld"), static_cast<long>(valuePair->value()));
			}
		}
		else if (SelMode == SvDef::TSFullSize)
		{
			if (SvDef::ToolSizeAdjustEnum::TSHeight == SvDef::ToolSizeAdjustEnum(vType) || SvDef::ToolSizeAdjustEnum::TSWidth == SvDef::ToolSizeAdjustEnum(vType))
			{
				auto propEnum = m_EQAdjustStruct[vType].m_extentProp;
				auto valuePair = std::find_if(parentExtentParameter.begin(), parentExtentParameter.end(), [propEnum](const auto value) { return value.type() == propEnum; });
				if (parentExtentParameter.end() != valuePair)
				{
					csResult.Format(_T("%ld"), static_cast<long>(valuePair->value()));
				}
			}
			else
			{
				csResult.Format(_T("%ld"), 0l);
			}
		}
		m_EditCtrl[vType].SetWindowText(csResult);
		m_Button[vType].ShowWindow(nShow);
	}
	UpdateData( false ); // set data to dialog
}

void SVToolAdjustmentDialogSizePage::OnBnClickedButtonFormula(SvDef::ToolSizeAdjustEnum mode)
{
	if (mode < 0 || mode >= SvDef::ToolSizeAdjustEnum::TSValuesCount)
	{
		ASSERT(false);
		return;
	}
	
	CString strCaption = m_EQAdjustStruct[mode].m_name.c_str();
	strCaption += _T(" Formula");

	SvDef::SVObjectTypeInfoStruct info(SvPb::SVEquationObjectType, m_EQAdjustStruct[mode].m_subType);
	SVFormulaEditorSheetClass dlg(m_ipId, m_toolId, info, strCaption);
	dlg.DoModal();
	Refresh(true);
}

void SVToolAdjustmentDialogSizePage::OnSelchangeCombo(SvDef::ToolSizeAdjustEnum  mode)
{
	/// full size must be selected in both or none of the combo boxes 
	DWORD_PTR selMode = m_ComboBox[mode].GetCurSelItemData();
	if (SvDef::TSFullSize == selMode)
	{
		///set all other to full size 
		for( int vType = SvDef::ToolSizeAdjustEnum::TSPositionX; vType < SvDef::ToolSizeAdjustEnum::TSValuesCount; ++vType)
		{
			if (vType == mode)
			{
				continue;	
			}
			m_ComboBox[vType].SetCurSelItemData(SvDef::TSFullSize);
		}
	}
	else 
	{
		///Then all must be not be full sized
		for( int vType = SvDef::ToolSizeAdjustEnum::TSPositionX; vType < SvDef::ToolSizeAdjustEnum::TSValuesCount; ++vType)
		{
			if (vType == mode)
			{
				continue;	
			}
			if (SvDef::TSFullSize == m_ComboBox[vType].GetCurSelItemData() )
			{
				m_ComboBox[vType].SetCurSelItemData(SvDef::TSNone);
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

bool SVToolAdjustmentDialogSizePage::QueryAllowExit()
{
	/// no empty formulas 
	for( int vType  = SvDef::ToolSizeAdjustEnum::TSPositionX; vType < SvDef::ToolSizeAdjustEnum::TSValuesCount; ++vType)
	{
		int sel = m_ComboBox[vType].GetCurSel();
		if( sel >= 0 )
		{
			long Value = static_cast<long>(m_ComboBox[vType].GetItemData( sel ));
			if (SvDef::TSFormula == Value)
			{
				FormulaController formula(m_ipId, m_toolId, m_EQAdjustStruct[vType].m_Id);
				if(formula.GetEquationText().empty() )
				{
					SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display );
					Exception.setMessage( SVMSG_SVO_64_EMPTY_FORMULAS_ARE_NOT_ALLOWED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16038_EmptyFormula );
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
	SvPb::InspectionCmdMsgs Request, Response;
	SvPb::ResetObjectRequest* pResetObjectRequest = Request.mutable_resetobjectrequest();
	SvPb::SetGuidInProtoBytes(pResetObjectRequest->mutable_objectid(), m_toolId);
	hr = SvCmd::InspectionCommands(m_ipId, Request, &Response);
	if (S_OK == hr)
	{
		SvCmd::RunOnceSynchronous(m_ipId);
	}
	return CPropertyPage::OnKillActive();
}
} //namespace SvOg