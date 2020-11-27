//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file ValueEditWidgetHelper.h
/// contains the class ValueEditWidgetHelper
//*****************************************************************************

#pragma once

#include "StdAfx.h"
#include "DataController.h"

namespace SvOg
{
	// holds GUI elements and IDs required for (possibly linked) Values that are to be modified using an MFC dialog
	class ValueEditWidgetHelper
	{
	public:
		ValueEditWidgetHelper(CEdit& valueEdit, SvPb::EmbeddedIdEnum embeddedId, CButton* pDottedNameSelectButton, SvPb::EmbeddedIdEnum embeddedLinkId, SvOg::ValueController& rValueController) :
			m_rValueEdit(valueEdit),
			m_embeddedId(embeddedId),
			m_pDottedNameSelectButton(pDottedNameSelectButton), // can be: nullptr
			m_embeddedLinkId(embeddedLinkId), // can be: SvPb::NoEmbeddedId
			m_rValueController(rValueController) {m_rValueController.Init();}

		static void EnsureDownArrowBitmapIsLoaded();

		void EditboxToValue();
		void ValueToEditbox();
		void EnableGuiElements(BOOL enable);

	private:
		static CBitmap ms_downArrowBitmap;
		static bool ms_downArrowBitmapWasLoaded;

		CEdit& m_rValueEdit;
		SvPb::EmbeddedIdEnum m_embeddedId;
		CButton* m_pDottedNameSelectButton;
		SvPb::EmbeddedIdEnum m_embeddedLinkId;
		SvOg::ValueController& m_rValueController;
	};


}  //namespace SvOg
