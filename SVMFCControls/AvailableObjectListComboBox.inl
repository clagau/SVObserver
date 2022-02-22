//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Körber Pharma Inspection GmbH. All Rights Reserved
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
				m_isFirstListText = true;
			}

			if (!rSelectedItem.empty())
			{
				SelectString(m_isFirstListText ? 0 : -1, rSelectedItem.c_str());
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
			const auto iter = std::find_if(m_List.cbegin(), m_List.cend(), [&](const auto& rVal)->bool
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
		int iIndex = FindString(m_isFirstListText ? 0 : -1, rItemName.c_str());

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
