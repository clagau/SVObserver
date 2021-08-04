//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file LinkedValueWidgetHelper.cpp
/// contains the class LinkedValueWidgetHelper
//*****************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "LinkedValueWidgetHelper.h"
#include "LinkedValueSelectorDialog.h"
#include "Definitions/GlobalConst.h"
#pragma endregion Includes

namespace SvOg
{
	CBitmap LinkedValueWidgetHelper::ms_downArrowBitmap;
	bool LinkedValueWidgetHelper::ms_downArrowBitmapWasLoaded = false;

	LinkedValueWidgetHelper::LinkedValueWidgetHelper(CEdit& rValueEdit, CButton& rSelectButton, uint32_t inspectionId, uint32_t taskId, SvPb::EmbeddedIdEnum embeddedId, SvOg::ValueController* pValueController, const ObjectSelectorData& selectorData/* = {}*/, ValidCheckCallback validCallback/* = nullptr*/, ConvertValueCallback convertCallback/* = nullptr*/)
		: m_rValueEdit(rValueEdit)
		, m_rSelectButton(rSelectButton)
		, m_inspectionId(inspectionId)
		, m_taskId(taskId)
		, m_embeddedId(embeddedId)
		, m_pValueController(pValueController)
		, m_selectorData(selectorData)
		, m_validCheckCallback(validCallback)
		, m_convertValueCallback(convertCallback)
	{
		EnsureDownArrowBitmapIsLoaded();
		m_rSelectButton.SetBitmap(static_cast<HBITMAP> (ms_downArrowBitmap)); // Put the Down Arrow on the button

		update();
	}

	void LinkedValueWidgetHelper::setValueController(SvOg::ValueController* pValueController)
	{
		m_pValueController = pValueController;
		update();
	}

	void LinkedValueWidgetHelper::EnableGuiElements(bool enable)
	{
		m_isControllsEnable = enable;
		update();
	}

	void LinkedValueWidgetHelper::OnButton()
	{
		assert(m_pValueController);
		if (m_pValueController)
		{
			auto data = m_pValueController->Get<LinkedValueData>(m_embeddedId);
			LinkedValueSelectorDialog dlg(m_inspectionId, m_pValueController->GetObjectID(m_embeddedId), m_pValueController->GetName(m_embeddedId), data, data.m_defaultValue.vt, m_selectorData, m_validCheckCallback);
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
			m_rSelectButton.EnableWindow(m_isControllsEnable);
			ValueToEditbox();
		}
		else
		{
			m_rSelectButton.ShowWindow(SW_HIDE);
			m_rValueEdit.ShowWindow(SW_HIDE);
		}
	}

	void LinkedValueWidgetHelper::EditboxToValue()
	{
		assert(m_pValueController);
		if (m_pValueController)
		{
			auto data = m_pValueController->Get<LinkedValueData>(m_embeddedId);
			if (SvPb::LinkedSelectedType::DirectValue == data.m_type)
			{
				CString tmpStr;
				m_rValueEdit.GetWindowText(tmpStr);
				variant_t tmp {tmpStr};
				SvStl::MessageContainer msgContainer;
				bool isValid = (S_OK == ::VariantChangeTypeEx(&data.m_directValue, &tmp, SvDef::LCID_USA, VARIANT_ALPHABOOL, data.m_defaultValue.vt));
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

	void LinkedValueWidgetHelper::EnsureDownArrowBitmapIsLoaded()
	{
		if (!ms_downArrowBitmapWasLoaded)
		{
			ms_downArrowBitmap.LoadOEMBitmap(OBM_DNARROW); //AB this can only be done once!
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
			switch (data.m_type)
			{
				case SvPb::LinkedSelectedType::DirectValue:
					enableFlag = m_isControllsEnable;
					break;
				case SvPb::LinkedSelectedType::IndirectValue:
				case SvPb::LinkedSelectedType::Formula:
					break;
				default:
					m_rValueEdit.EnableWindow(false);
					assert(false);
					return;
			}
			m_rValueEdit.EnableWindow(enableFlag);
			if (m_convertValueCallback)
			{
				m_rValueEdit.SetWindowText(m_convertValueCallback(data.m_Value));
			}
			else
			{
				m_rValueEdit.SetWindowText(static_cast<CString>(data.m_Value));
			}
		}
	}
}  //namespace SvOg
