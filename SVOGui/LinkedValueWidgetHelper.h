//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file LinkedValueWidgetHelper.h
/// Help to control the LinkedValue-widgets.
//*****************************************************************************

#pragma once

#include "StdAfx.h"
#include "LinkedValueSelectorDialog.h"

namespace SvOg
{
	class LinkedValueWidgetHelper
	{
		/// helpful for handling Linked Values that are to be modified in a MFC dialog
		/// holds GUI elements and IDs required for linked Values that are to be modified using editboxes
	public:

		LinkedValueWidgetHelper(CEdit& rValueEdit, CButton& rSelectButton, uint32_t inspectionId, uint32_t taskId, SvPb::EmbeddedIdEnum embeddedId, SvOg::ValueController* pValueController, ValidCheckCallback validCallback = nullptr, ConvertValueCallback convertCallback = nullptr);

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
		bool m_isWidgetEnabled{ true };
		ValidCheckCallback m_validCheckCallback;
		ConvertValueCallback m_convertValueCallback;
	};


	class WidgetHelperSource
	{
		/// creates unique pointers to LinkedValueWidgetHelpers that refer to the same inspection, task and ValueController
	private:
		uint32_t m_inspectionId;
		uint32_t m_taskId;
		ValueController* m_pValueController;

	public:
		WidgetHelperSource(uint32_t inspectionId, uint32_t taskId, ValueController* pValueController) :
			m_inspectionId(inspectionId),
			m_taskId(taskId),
			m_pValueController(pValueController)
		{}

		auto produce(CEdit& rValueEdit, CButton& rSelectButton, SvPb::EmbeddedIdEnum embeddedId, ValidCheckCallback validCallback = nullptr, ConvertValueCallback convertCallback = nullptr)
		{
			return std::make_unique<SvOg::LinkedValueWidgetHelper>(rValueEdit, rSelectButton, m_inspectionId, m_taskId, embeddedId, m_pValueController, validCallback, convertCallback);
		}
	};

}  //namespace SvOg
