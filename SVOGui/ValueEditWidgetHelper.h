//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
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
		ValueEditWidgetHelper(CEdit& valueEdit, SvPb::EmbeddedIdEnum embeddedId, SvOg::ValueController& rValueController) :
			m_rValueEdit(valueEdit),
			m_embeddedId(embeddedId),
			m_rValueController(rValueController) {m_rValueController.Init();}

		/// reads a string from an editbox and sets the appropriate ValueObject accordingly.
		void EditboxToValue();
		/// reads a string from a ValueObject and sets the appropriate editbox accordingly.
		void ValueToEditbox();
		void EnableGuiElements(BOOL enable);

	private:
			
		CEdit& m_rValueEdit;
		SvPb::EmbeddedIdEnum m_embeddedId;
		SvOg::ValueController& m_rValueController;
	};


}  //namespace SvOg
