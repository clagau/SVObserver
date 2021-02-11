//*****************************************************************************
// \copyright COPYRIGHT (c) 2015,2015 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file ValueEditWidgetHelper.cpp
/// contains the class ValueEditWidgetHelper
//*****************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "ValueEditWidgetHelper.h"
#pragma endregion Includes

namespace SvOg
{
	CBitmap ValueEditWidgetHelper::ms_downArrowBitmap;
	bool ValueEditWidgetHelper::ms_downArrowBitmapWasLoaded = false;

	void ValueEditWidgetHelper::EnsureDownArrowBitmapIsLoaded()
	{
		if (!ms_downArrowBitmapWasLoaded)
		{
			ms_downArrowBitmap.LoadOEMBitmap(OBM_DNARROW); //AB this can only be done once!
			ms_downArrowBitmapWasLoaded = true;
		}
	}

	void ValueEditWidgetHelper::EditboxToValue()
	{
		CString temp;

		m_rValueEdit.GetWindowText(temp);

		m_rValueController.Set<CString>(m_embeddedId, temp);
	}

	void ValueEditWidgetHelper::EnableGuiElements(BOOL enable)
	{
		m_rValueEdit.EnableWindow(enable);
		if (m_pDottedNameSelectButton) //i.e. this is a linked value
		{
			m_pDottedNameSelectButton->EnableWindow(enable);
		}
	}

	std::string ValueEditWidgetHelper::GetValueString()
	{
		std::string temp;

		if (SvPb::NoEmbeddedId != m_embeddedLinkId) //is this a linked value?
		{
			temp = m_rValueController.Get<CString>(m_embeddedLinkId); //See whether we can get a dotted name
		}

		if (temp.empty()) // this either isn't a linked value or its link is currently empty
		{
			temp = m_rValueController.Get<CString>(m_embeddedId);
		}

		return temp;
	}

	void ValueEditWidgetHelper::ValueToEditbox()
	{
		auto temp = GetValueString();

		m_rValueEdit.SetWindowText(temp.c_str());

		if (m_pDottedNameSelectButton) //We have an associated button that should contain a down arrow
		{
			m_pDottedNameSelectButton->SetBitmap(static_cast<HBITMAP> (ms_downArrowBitmap)); // Put the Down Arrow on the button
		}
	}

}  //namespace SvOg
 