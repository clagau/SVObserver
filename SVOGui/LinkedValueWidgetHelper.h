//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file LinkedValueWidgetHelper.h
/// Help to control the LinkedValue-widgets.
//*****************************************************************************

#pragma once

#include "StdAfx.h"
#include "LinkedValueSelectorDialog.h"

namespace SvOg
{
	// holds GUI elements and IDs required for linked Values that are to be modified using an MFC dialog
	class LinkedValueWidgetHelper
	{
	public:
		LinkedValueWidgetHelper(CEdit& rValueEdit, CButton& rSelectButton, uint32_t inspectionId, uint32_t taskId, SvPb::EmbeddedIdEnum embeddedId, SvOg::ValueController* pValueController, const ObjectSelectorData& selectorData = {}, ValidCheckCallback validCallback = nullptr, ConvertValueCallback convertCallback = nullptr);

		void setValueController(SvOg::ValueController* pValueController);

		void EnableGuiElements(bool enable);
		void OnButton();
		void update();

		/// reads a string from an editbox and sets the appropriate ValueObject accordingly.
		/// \param ignoreEmptyString [in] if this flag set, it ignore the editbox-text if it is empty.
		void EditboxToValue(bool ignoreEmptyString = false);

		static ValidCheckCallback getCheckValueFunction(int minValue, int maxValue);

	private:
		static void EnsureDownArrowBitmapIsLoaded();

		/// reads a string from a ValueObject and sets the appropriate editbox accordingly.
		void ValueToEditbox();

	private:
		static CBitmap ms_downArrowBitmap;
		static bool ms_downArrowBitmapWasLoaded;

		CEdit& m_rValueEdit;
		CButton& m_rSelectButton;
		uint32_t m_inspectionId;
		uint32_t m_taskId; 
		SvPb::EmbeddedIdEnum m_embeddedId;
		SvOg::ValueController* m_pValueController;
		bool m_isControllsEnable{ true };
		ObjectSelectorData m_selectorData;
		ValidCheckCallback m_validCheckCallback;
		ConvertValueCallback m_convertValueCallback;
	};


}  //namespace SvOg
