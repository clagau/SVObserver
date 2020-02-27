//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the AvailableObjectListComboBox class for use by the GUI.
//******************************************************************************

namespace SvMc
{
	template <class value>
	void AvailableObjectListComboBox<value>::Init(const Container& rList, const std::string& rSelectedItem, const std::string& rEmptyListText, const std::string& rFirstListText)
	{
		// Clear list...
		ResetContent();

		m_List = rList;

		// Generate new list...
		for (auto it = rList.begin(); it != rList.end(); ++it)
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
			SetCurSel(0);
		}
		else
		{
			if (!rFirstListText.empty())
			{
				InsertString(0, rFirstListText.c_str());
			}

			if (!rSelectedItem.empty())
			{
				SelectString(0, rSelectedItem.c_str());
			}
		}
	}

	template <class value>
	value AvailableObjectListComboBox<value>::getSelectedValue() const
	{
		value Result {};
		int index = GetCurSel();

		if (CB_ERR != index)
		{
			CString name;
			GetLBText(index, name);
			Container::const_iterator iter = std::find_if(m_List.begin(), m_List.end(), [&](const auto& rVal)->bool
			{
				return (!rVal.first.empty() && 0 == rVal.first.compare(name));
			});

			if (m_List.cend() != iter)
			{
				Result = iter->second;
			}
		}

		return Result;
	}

	template <class value>
	void AvailableObjectListComboBox<value>::remove(const std::string& rItemName)
	{
		int iIndex = FindString(0, rItemName.c_str());

		if (iIndex != LB_ERR)
		{
			DeleteString(iIndex);
			auto iter = std::find_if(m_List.begin(), m_List.end(), [&](const auto& rVal)->bool
			{
				return (!rVal.first.empty() && 0 == rVal.first.compare(rItemName));
			});

			if (m_List.cend() != iter)
			{
				m_List.erase(iter);
			}
		}
	}
} //namespace SvMc