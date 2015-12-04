//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the AvailableObjectListComboBox class for use by the GUI.
//******************************************************************************

#pragma region Includes
#include "Stdafx.h"
#include "AvailableObjectListComboBox.h"
#pragma endregion Includes

namespace Seidenader { namespace SVOGui
{
	BEGIN_MESSAGE_MAP(AvailableObjectListComboBox, CComboBox)
		//{{AFX_MSG_MAP(AvailableObjectListComboBox)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	AvailableObjectListComboBox::AvailableObjectListComboBox()
	{
	}

	AvailableObjectListComboBox::~AvailableObjectListComboBox()
	{
	}

	void AvailableObjectListComboBox::Init(const SvUl::NameGuidList& rList, const SVString& rSelectedItem, const SVString& rEmptyListText)
	{
		// Clear list...
		ResetContent();
	
		// Generate new list...
		for (SvUl::NameGuidList::const_iterator it = rList.begin(); it != rList.end();++it)
		{
			if (!it->first.empty())
			{
				AddString(it->first.c_str());
			}
		}
		
		// Empty List in Combobox...
		if (!GetCount())
		{
			AddString(rEmptyListText.c_str());
			SetCurSel(-1);
		}
		else
		{
			SelectString(0, rSelectedItem.c_str());
		}
	}
} /* namespace SVOGui */ } /* namespace Seidenader */