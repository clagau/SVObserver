//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file LinkedValueSelectorDialog.cpp
/// Dialog to set the LinkedValue settings.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "LinkedValueSelectorDialog.h"
#include "Definitions/StringTypeDef.h"
#include "ObjectSelectorLibrary/ObjectTreeGenerator.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "Definitions/GlobalConst.h"
#include "FormulaController.h"
#include "SVFormulaEditorPage.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvOgu
{
	// Constant types
	enum
	{
		SV_FORMULA_DECIMAL_CONSTANT_TYPE = 0,
		SV_FORMULA_HEXADECIMAL_CONSTANT_TYPE = 1,
		SV_FORMULA_BINARY_CONSTANT_TYPE = 2,

		SV_EQUATION_BUFFER_SIZE = 4096, //This enumeration defines the Equation Buffer Size.
	};

	const int c_offsetXForCursorToolbar = 340;
	constexpr const TCHAR* cQuote = _T("\"");

	BEGIN_MESSAGE_MAP(LinkedValueSelectorDialog, CDialog)
		//{{AFX_MSG_MAP(LinkedValueSelectorDialog)
		ON_BN_CLICKED(IDC_VALUE, OnValue)
		ON_BN_CLICKED(IDC_LINKED, OnLinked)
		ON_BN_CLICKED(IDC_FORMULA, OnFormula)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	LinkedValueSelectorDialog::LinkedValueSelectorDialog(uint32_t inspectionId, uint32_t objectId, const SvOgu::GroupInputResultData& resultdata)
		: CDialog(LinkedValueSelectorDialog::IDD)
		, m_inspectionId(inspectionId)
		, m_objectId(objectId)
		, m_data(resultdata.m_data )
		, m_vtType(resultdata.m_data.m_Value.vt)
		, m_ObjectName(resultdata.m_name)
		, m_validCheckCallback(nullptr)
		, m_LinkedValueType(resultdata.m_type)
	{
		m_type = m_data.m_selectedOption;
		if (VT_EMPTY == m_vtType)
		{
			m_possibleTypes = LinkedValueSelectorTypesEnum::Indirect;
		}
		else if (VT_ARRAY == (m_vtType & VT_ARRAY) || VT_BSTR == m_vtType || SvDef::InvalidObjectId == m_data.m_equationId)
		{
			m_possibleTypes = LinkedValueSelectorTypesEnum::DirectIndirect;
		}
		else
		{
			m_possibleTypes = LinkedValueSelectorTypesEnum::All;
			m_FormulaController = std::make_shared<FormulaController>(m_inspectionId, m_objectId, m_data.m_equationId);
		}
	}

	LinkedValueSelectorDialog::LinkedValueSelectorDialog(uint32_t inspectionId, uint32_t objectId, const std::string& rName, const LinkedValueData& data, VARTYPE vtType, ValidCheckCallback validCallback /*= nullptr*/)
		: CDialog(LinkedValueSelectorDialog::IDD)
		, m_inspectionId(inspectionId)
		, m_objectId(objectId)
		, m_data(data)
		, m_vtType(vtType)
		, m_ObjectName(rName)
		, m_validCheckCallback(validCallback)
	{
		m_type = m_data.m_selectedOption;
		if (VT_EMPTY == m_vtType)
		{
			m_possibleTypes = LinkedValueSelectorTypesEnum::Indirect;
		}
		else if (VT_ARRAY == (m_vtType & VT_ARRAY) || VT_BSTR == m_vtType || SvDef::InvalidObjectId == m_data.m_equationId)
		{
			m_possibleTypes = LinkedValueSelectorTypesEnum::DirectIndirect;
		}
		else
		{
			m_possibleTypes = LinkedValueSelectorTypesEnum::All;
			m_FormulaController = std::make_shared<FormulaController>(m_inspectionId, m_objectId, m_data.m_equationId);
		}
	}

	LinkedValueSelectorDialog::~LinkedValueSelectorDialog()
	{
	}

	bool LinkedValueSelectorDialog::createVariantFromString(VARTYPE vtType, const std::string& rStrValue, variant_t& rVariantValue)
	{
		bool isOk;
		switch (vtType)
		{
			case VT_ARRAY | VT_R8:
				isOk = (SvUl::StringToSafeArray<double>(rStrValue, rVariantValue) >= 0);
				break;
			case VT_ARRAY | VT_I4:
				isOk = (SvUl::StringToSafeArray<long>(rStrValue, rVariantValue) >= 0);
				break;
			default:
				variant_t tmp {rStrValue.c_str()};
				isOk = (S_OK == ::VariantChangeTypeEx(&rVariantValue, &tmp, SvDef::LCID_USA, VARIANT_ALPHABOOL, vtType));
				break;
		}
		return isOk;
	}

	void LinkedValueSelectorDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(LinkedValueSelectorDialog)
		DDX_Text(pDX, IDC_VALUE_EDIT, m_directValue);
		//}}AFX_DATA_MAP
	}

	BOOL LinkedValueSelectorDialog::OnInitDialog()
	{
		CDialog::OnInitDialog();

		switch (m_possibleTypes)
		{
		case LinkedValueSelectorTypesEnum::All:
			break;
		case LinkedValueSelectorTypesEnum::DirectIndirect:
			GetDlgItem(IDC_FORMULA)->ShowWindow(SW_HIDE);
			break;
		case LinkedValueSelectorTypesEnum::Indirect:
			m_type = SvPb::LinkedSelectedOption::IndirectValue;
			GetDlgItem(IDC_VALUE)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_FORMULA)->ShowWindow(SW_HIDE);
			break;
		default:
			Log_Assert(false);
			return false;
		}
		
		if(m_LinkedValueType == SvPb::LinkedValueTypeEnum::TypeStates)
			GetDlgItem(IDC_FORMULA)->ShowWindow(SW_HIDE);

		//Value
		m_directValue = SvUl::VariantToString(m_data.m_directValue).c_str();
		CString textStr;
		GetWindowText(textStr);
		textStr = textStr + ":" + m_ObjectName.c_str();
		SetWindowText(textStr);

		createObjectPage();
		Log_Assert(m_pDlgLinkedPage);
		if (nullptr == m_pDlgLinkedPage)
		{
			return false;
		}
		m_dlgLinkedSheet.AddPage(m_pDlgLinkedPage.get());

		if (LinkedValueSelectorTypesEnum::All == m_possibleTypes)
		{
			createFormulaPage();
			Log_Assert(m_pDlgFormulaPage);
			if (nullptr == m_pDlgFormulaPage)
			{
				return false;
			}
			m_dlgLinkedSheet.AddPage(m_pDlgFormulaPage.get());
		}
		
		m_dlgLinkedSheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
		CRect rcSheet;
		GetDlgItem(IDC_PROPERTY_SHEET)->GetWindowRect(&rcSheet);
		ScreenToClient(&rcSheet);
		m_dlgLinkedSheet.SetWindowPos(nullptr, rcSheet.left - 7, rcSheet.top, 0, 0,
			SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
		m_dlgLinkedSheet.GetTabControl()->ShowWindow(false);
		m_dlgLinkedSheet.ModifyStyleEx(0, WS_EX_CONTROLPARENT);
		m_dlgLinkedSheet.ModifyStyle(0, WS_TABSTOP);
		m_dlgLinkedSheet.SetActivePage(0);

		CWnd* pWnd = GetDlgItem(IDC_PROPERTY_SHEET);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_HIDE);
		}

		updateShownControls();
		UpdateData(FALSE);

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void LinkedValueSelectorDialog::OnOK()
	{
		UpdateData();
		m_data.m_selectedOption = m_type;

		//check and set directValue
		if (false == checkAndSetDirectValue() && SvPb::LinkedSelectedOption::DirectValue == m_data.m_selectedOption)
		{
			return;
		}

		//check and set indirect value
		m_linkedTreeGenerator.checkModifiedItems();
		m_data.m_indirectIdName = m_linkedTreeGenerator.getSingleObjectResult();
		if (m_data.m_indirectIdName.empty() && SvPb::LinkedSelectedOption::IndirectValue == m_data.m_selectedOption)
		{
			SvDef::StringVector msgList;
			msgList.push_back(m_ObjectName);
			SvStl::MessageManager Msg(SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
			return;
		}

		//formula check
		if (LinkedValueSelectorTypesEnum::All == m_possibleTypes && m_FormulaController 
			&& m_pDlgFormulaPage && m_pDlgFormulaPage->GetSafeHwnd()) //If m_pDlgFormulaPage is not initialized, not try to set it, because getEquationText return a empty string. 
		{
			auto equationText = m_pDlgFormulaPage->getEquationText();
			double value{ 0 };
			SvStl::MessageContainerVector ErrorMessages;
			const int result = m_FormulaController->ValidateEquation(equationText, value, false, ErrorMessages);
			
			if (SvOi::IFormulaController::validateSuccessful == result || SvOi::IFormulaController::resetFailed == result)
			{
				m_data.m_formula = equationText;
			}
			else
			{
				if (SvPb::LinkedSelectedOption::Formula == m_data.m_selectedOption)
				{
					for (const auto& rMessage : ErrorMessages)
					{
						SvStl::MessageManager message(SvStl::MsgType::Log | SvStl::MsgType::Display);
						message.setMessage(rMessage.getMessage());
					}
					return;
				}
			}
		}

		//get current value and set it to data
		SvPb::InspectionCmdRequest requestCmd;
		auto* pRequest = requestCmd.mutable_validatelinkedvaluerequest();
		auto* pData = pRequest->mutable_data();
		pData->set_objectid(m_objectId);
		*pData->mutable_linkedvalue() = convertLinkedValue(m_data);
		SvPb::InspectionCmdResponse responseCmd;
		auto hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);
		if (S_OK == hr && responseCmd.has_validatelinkedvalueresponse())
		{
			SvPb::ConvertProtobufToVariant(responseCmd.validatelinkedvalueresponse().value(), m_data.m_Value);
			CDialog::OnOK();
		}
		else
		{
			if (responseCmd.has_validatelinkedvalueresponse())
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Display);
				Msg.setMessage(SvPb::convertProtobufToMessage(responseCmd.validatelinkedvalueresponse().errormessage()).getMessage());
			}
			else
			{
				SvDef::StringVector msgList;
				msgList.push_back(m_ObjectName);
				SvStl::MessageManager Msg(SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
				Log_Assert(false);
			}
		}
	}

	void LinkedValueSelectorDialog::OnValue()
	{
		changeType(SvPb::DirectValue);
	}

	void LinkedValueSelectorDialog::OnLinked()
	{
		changeType(SvPb::IndirectValue);
	}

	void LinkedValueSelectorDialog::OnFormula()
	{
		changeType(SvPb::Formula);
	}

	void LinkedValueSelectorDialog::changeType(SvPb::LinkedSelectedOption newType)
	{
		UpdateData();
		if (SvPb::LinkedSelectedOption::Formula == m_type)
		{
			auto equationText = m_pDlgFormulaPage->getEquationText();
			double value{ 0 };
			SvStl::MessageContainerVector ErrorMessages;
			const int result = m_FormulaController->ValidateEquation(equationText, value, false, ErrorMessages);

			if (SvOi::IFormulaController::validateSuccessful != result && SvOi::IFormulaController::resetFailed != result)
			{
				for (const auto& rMessage : ErrorMessages)
				{
					SvStl::MessageManager message(SvStl::MsgType::Log | SvStl::MsgType::Display);
					message.setMessage(rMessage.getMessage());
				}
				return;
			}
		}
		m_type = newType;

		updateShownControls();
	}


	void LinkedValueSelectorDialog::createObjectPage()
	{
		SvPb::InspectionCmdRequest requestCmd;
		requestCmd.mutable_getobjectselectoritems2request()->set_objectid(m_objectId);
		SvPb::InspectionCmdResponse responseCmd;
		SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);

		m_linkedTreeGenerator.setSelectorType(SvOsl::ObjectTreeGenerator::TypeSingleObject);
		if (responseCmd.has_getobjectselectoritemsresponse())
		{
			m_linkedTreeGenerator.insertTreeObjects(responseCmd.getobjectselectoritemsresponse().tree());
		}

		if (!m_data.m_indirectIdName.empty())
		{
			SvDef::StringSet nameSet;
			nameSet.insert(m_data.m_indirectIdName);
			m_linkedTreeGenerator.setCheckItems(nameSet);
		}

		m_linkedTreeGenerator.getTreeContainer().setNodeCheckedStates();
		m_linkedTreeGenerator.getTreeContainer().synchronizeCheckedStates();

		m_pDlgLinkedPage = std::make_unique<SvOsl::ObjectSelectorPpg>(m_linkedTreeGenerator.getTreeContainer(), "Link");
		
	}

	void LinkedValueSelectorDialog::createFormulaPage()
	{
		m_pDlgFormulaPage = std::make_unique<SVFormulaEditorPageClass>(m_FormulaController, m_data.m_formula);
	}

	void LinkedValueSelectorDialog::updateShownControls()
	{
		CWnd* pWnd = GetDlgItem(IDC_VALUE_EDIT);
		if (pWnd)
		{
			pWnd->ShowWindow(SvPb::DirectValue == m_type ? SW_SHOW : SW_HIDE);
		}

		pWnd = GetDlgItem(IDC_VALUE);
		if (pWnd)
		{
			pWnd->EnableWindow(SvPb::DirectValue != m_type);
		}

		pWnd = GetDlgItem(IDC_LINKED);
		if (pWnd)
		{
			pWnd->EnableWindow(SvPb::IndirectValue != m_type);
		}

		pWnd = GetDlgItem(IDC_FORMULA);
		if (pWnd)
		{
			pWnd->EnableWindow(SvPb::Formula != m_type);
		}

		bool isSheet = false;
		if (SvPb::LinkedSelectedOption::IndirectValue == m_type)
		{
			m_dlgLinkedSheet.SetActivePage(0);
			isSheet = true;
		}

		if (LinkedValueSelectorTypesEnum::All == m_possibleTypes && SvPb::LinkedSelectedOption::Formula == m_type)
		{
			m_dlgLinkedSheet.SetActivePage(1);
			isSheet = true;
		}
		
		m_dlgLinkedSheet.ShowWindow(isSheet ? SW_SHOW : SW_HIDE);
	}

	bool LinkedValueSelectorDialog::checkAndSetDirectValue()
	{
		if (LinkedValueSelectorTypesEnum::All == m_possibleTypes || LinkedValueSelectorTypesEnum::DirectIndirect == m_possibleTypes)
		{
			bool isOk = createVariantFromString(m_vtType, std::string {m_directValue}, m_data.m_directValue);
				
			if (false == isOk)
			{
				if (SvPb::LinkedSelectedOption::DirectValue == m_data.m_selectedOption)
				{
					SvDef::StringVector msgList;
					msgList.push_back(m_ObjectName);
					SvStl::MessageManager Msg(SvStl::MsgType::Display);
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
				}
				//empty value if variant can not be converted in the right type
				::VariantClear(&m_data.m_directValue);
				return false;
			}
			
			if (SvPb::LinkedSelectedOption::DirectValue == m_data.m_selectedOption && m_validCheckCallback)
			{
				SvStl::MessageContainer messageContainer;
				bool isValid = m_validCheckCallback(m_data.m_directValue, messageContainer);
				if (false == isValid)
				{
					SvStl::MessageManager Exception(SvStl::MsgType::Display);
					Exception.setMessage(messageContainer.getMessage());
					return false;
				}
			}
		}
		return true;
	}
} //namespace SvOgu

