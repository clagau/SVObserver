//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file LinkedValueWidgetHelper.cpp
/// contains the class LinkedValueWidgetHelper
//*****************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "DataController.h"
#include "LinkedValueWidgetHelper.h"
#include "LinkedValueSelectorDialog.h"
#include "Definitions/GlobalConst.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace
{
bool checkValue(int minValue, int maxValue, const variant_t& rValue, SvStl::MessageContainer& rMessageContainer)
{
	bool isOk {true};
	try
	{
		int value = (int)rValue;
		isOk = (minValue <= value && maxValue >= value);
	}
	catch (...)
	{
		isOk = false;
	}
	if (false == isOk)
	{
		SvDef::StringVector msgList;
		msgList.push_back(std::format(_T("{}"), minValue));
		msgList.push_back(std::format(_T("{}"), maxValue));
		variant_t val;
		HRESULT hr = ::VariantChangeTypeEx(&val, &rValue, SvDef::LCID_USA, VARIANT_ALPHABOOL, VT_BSTR);	// use United States locale
		if (S_OK == hr)
		{
			msgList.push_back(SvUl::createStdString(val));
		}
		else
		{
			msgList.push_back("<undef>");
		}
		rMessageContainer.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Value_Invalid, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	return isOk;
}
}

namespace SvOgu
{
	CBitmap LinkedValueWidgetHelper::ms_downArrowBitmap;
	bool LinkedValueWidgetHelper::ms_downArrowBitmapWasLoaded = false;

	LinkedValueWidgetHelper::LinkedValueWidgetHelper(CEdit& rValueEdit, CButton& rSelectButton, uint32_t inspectionId, uint32_t taskId, SvPb::EmbeddedIdEnum embeddedId, ValueController* pValueController, ValidCheckCallback validCallback/* = nullptr*/, ConvertValueCallback convertCallback/* = nullptr*/)
		: m_rValueEdit(rValueEdit)
		, m_rSelectButton(rSelectButton)
		, m_inspectionId(inspectionId)
		, m_taskId(taskId)
		, m_embeddedId(embeddedId)
		, m_pValueController(pValueController)
		, m_validCheckCallback(validCallback)
		, m_convertValueCallback(convertCallback)
	{
		EnsureDownArrowBitmapIsLoaded();
		m_rSelectButton.SetBitmap(static_cast<HBITMAP> (ms_downArrowBitmap)); // add the "down arrow" to the button

		update();
	}

	void LinkedValueWidgetHelper::setValueController(ValueController* pValueController)
	{
		m_pValueController = pValueController;
		update();
	}

	void LinkedValueWidgetHelper::EnableGuiElements(bool enable)
	{
		m_isWidgetEnabled = enable;
		update();
	}

	void LinkedValueWidgetHelper::OnButton()
	{
		assert(m_pValueController);
		if (m_pValueController)
		{
			auto data = m_pValueController->Get<LinkedValueData>(m_embeddedId);
			LinkedValueSelectorDialog dlg(m_inspectionId, m_pValueController->GetObjectID(m_embeddedId), m_pValueController->GetName(m_embeddedId), data, data.m_defaultValue.vt, m_validCheckCallback);
			if (IDOK == dlg.DoModal())
			{
				data = dlg.getData();
				m_pValueController->Set<LinkedValueData>(m_embeddedId, data);
				ValueToEditbox();
			}
		}
	}

	void LinkedValueWidgetHelper::update()
	{
		if (m_pValueController)
		{
			m_rSelectButton.ShowWindow(SW_SHOW);
			m_rValueEdit.ShowWindow(SW_SHOW);
			m_rSelectButton.EnableWindow(m_isWidgetEnabled);
			ValueToEditbox();
		}
		else
		{
			m_rSelectButton.ShowWindow(SW_HIDE);
			m_rValueEdit.ShowWindow(SW_HIDE);
		}
	}

	void LinkedValueWidgetHelper::EditboxToValue(bool ignoreEmptyString)
	{
		assert(m_pValueController);
		if (m_pValueController)
		{
			auto data = m_pValueController->Get<LinkedValueData>(m_embeddedId);
			if (SvPb::LinkedSelectedOption::DirectValue == data.m_selectedOption)
			{
				CString tmpStr;
				m_rValueEdit.GetWindowText(tmpStr);
				if (ignoreEmptyString && tmpStr.IsEmpty())
				{
					return;
				}

				SvStl::MessageContainer msgContainer;
				bool isValid {true};
				if (0 == (data.m_defaultValue.vt & VT_ARRAY))
				{
					variant_t tmp {tmpStr};
					isValid = (S_OK == ::VariantChangeTypeEx(&data.m_directValue, &tmp, SvDef::LCID_USA, VARIANT_ALPHABOOL, data.m_defaultValue.vt));
				}
				else
				{
					SvUl::StringToSafeArray<double>(std::string{tmpStr}, data.m_directValue);
				}

				if (false == isValid)
				{
					SvDef::StringVector msgList;
					msgList.push_back(m_pValueController->GetName(m_embeddedId));
					msgContainer.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinkedValue_ValidateStringFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
				}
				else if (m_validCheckCallback)
				{
					isValid = m_validCheckCallback(data.m_directValue, msgContainer);
				}

				if (isValid)
				{
					data.m_Value = data.m_directValue;
					m_pValueController->Set<LinkedValueData>(m_embeddedId, data);
					ValueToEditbox();
				}
				else
				{
					SvStl::MessageManager Msg(SvStl::MsgType::Display);
					Msg.setMessage(msgContainer.getMessage());

					m_rValueEdit.SetFocus();
				}
			}
		}
	}

	ValidCheckCallback LinkedValueWidgetHelper::getCheckValueFunction(int minValue, int maxValue)
	{
		return std::bind(checkValue, minValue, maxValue, std::placeholders::_1, std::placeholders::_2);
	}

	void LinkedValueWidgetHelper::EnsureDownArrowBitmapIsLoaded()
	{
		if (!ms_downArrowBitmapWasLoaded)
		{
			ms_downArrowBitmap.LoadOEMBitmap(OBM_DNARROW); //this can only be done once!
			ms_downArrowBitmapWasLoaded = true;
		}
	}

	void LinkedValueWidgetHelper::ValueToEditbox()
	{
		assert(m_pValueController);
		if (m_pValueController)
		{
			auto data = m_pValueController->Get<LinkedValueData>(m_embeddedId);
			bool enableFlag {false};
			switch (data.m_selectedOption)
			{
				case SvPb::LinkedSelectedOption::DirectValue:
					enableFlag = m_isWidgetEnabled;
					break;
				case SvPb::LinkedSelectedOption::IndirectValue:
				case SvPb::LinkedSelectedOption::Formula:
					break;
				default:
					m_rValueEdit.EnableWindow(false);
					assert(false);
					return;
			}
			m_rValueEdit.EnableWindow(enableFlag);
			CString currentStr;
			m_rValueEdit.GetWindowText(currentStr);
			CString newStr;
			if (m_convertValueCallback)
			{
				newStr = m_convertValueCallback(data.m_Value);
			}
			else
			{
				newStr = SvUl::VariantToString(data.m_Value).c_str();
			}
			if (currentStr != newStr)
			{
				m_rValueEdit.SetWindowText(newStr);
			}
		}
	}
}  //namespace SvOgu
