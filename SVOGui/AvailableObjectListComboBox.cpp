//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the AvailableObjectListComboBox class for use by the GUI.
//******************************************************************************

#pragma region Includes
#include "Stdafx.h"
#include <algorithm>
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

		m_List = rList;
	
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
			int nIndex = SelectString(0, rSelectedItem.c_str());
			if (LB_ERR == nIndex)
			{//if string not found, take first item
				SetCurSel(0);
			}
		}
	}

	SVGUID AvailableObjectListComboBox::getSelectedGUID() const
	{
		SVGUID retValue = SVInvalidGUID;
		int index = GetCurSel();
		
		if (CB_ERR != index)
		{
			CString name;
			GetLBText(index, name);
			SvUl::NameGuidList::const_iterator iter = std::find_if(m_List.begin(), m_List.end(), [&](const SvUl::NameGuidPair& rVal)->bool 
			{ 
				return (!rVal.first.empty() && 0 == rVal.first.Compare(name)); 
			} );

			if (m_List.cend() != iter)
			{
				retValue = iter->second;
			}
		}

		return retValue;
	}
} /* namespace SVOGui */ } /* namespace Seidenader */