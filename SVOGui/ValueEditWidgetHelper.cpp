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
	void ValueEditWidgetHelper::EditboxToValue()
	{
		CString temp;

		m_rValueEdit.GetWindowText(temp);

		m_rValueController.Set<CString>(m_embeddedId, temp);
	}

	void ValueEditWidgetHelper::EnableGuiElements(BOOL enable)
	{
		m_rValueEdit.EnableWindow(enable);
	}

	void ValueEditWidgetHelper::ValueToEditbox()
	{
		m_rValueEdit.SetWindowText(m_rValueController.Get<CString>(m_embeddedId));
	}

}  //namespace SvOg
 