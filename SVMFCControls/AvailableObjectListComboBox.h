//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the AvailableObjectListComboBox class for use by the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#pragma endregion Includes

namespace SvMc
{
	template <class value>
	class AvailableObjectListComboBox : public CComboBox
	{
	public:
		using PairClass = std::pair<std::string, typename value>;
		using Container = std::vector<PairClass>;
		AvailableObjectListComboBox() = default;
		virtual ~AvailableObjectListComboBox() = default;

		//************************************
		/// Initialized Combobox
		/// \param List [in] Name-value list to add to the combobox
		/// \param rSelectedItem [in] Name of selected Item, if "" no selection.
		/// \param rEmptyListText [in] Text in the combobox if list is empty.
		/// \param rFirstListText [in] First entry in the combobox for an non selection (e.g. No Analyzer), if "" non selection will added.
		//************************************
		void Init(const Container& rList, const std::string& rSelectedItem, const std::string& rEmptyListText, const std::string& rFirstListText = _T(""));

		//************************************
		/// Return the value of the selected item. If no item selected or an error it return the default value e.g. for uint32_t(objectId) = SvDef::InvalidObjectId and for an enum value 0.
		/// \returns value
		//************************************
		value getSelectedValue() const;

		void remove(const std::string& rItemName);

	protected:

	private:
		Container m_List;
		bool m_isFirstListText = false;
	};
} //namespace SvMc

#include "AvailableObjectListComboBox.inl"


