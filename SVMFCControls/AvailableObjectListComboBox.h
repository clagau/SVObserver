//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the AvailableObjectListComboBox class for use by the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVUtilityLibrary\NameGuidList.h"
#pragma endregion Includes

namespace SvMc
{
	class AvailableObjectListComboBox : public CComboBox
	{
	public:
		AvailableObjectListComboBox();
		virtual ~AvailableObjectListComboBox();

		//************************************
		/// Initialized Combobox
		/// \param List [in] Name-guid list to add to the combobox
		/// \param rSelectedItem [in] Name of selected Item, if "" no selection.
		/// \param rEmptyListText [in] Text in the combobox if list is empty.
		/// \param rFirstListText [in] First entry in the combobox for an non selection (e.g. No Analyzer), if "" non selection will added.
		//************************************
		void Init(const SvUl::NameGuidList& List, const std::string& rSelectedItem, const std::string& rEmptyListText, const std::string& rFirstListText = _T(""));

		//************************************
		/// Return the guid of the selected item. If no item selected or an error it return GUID_NULL.
		/// \returns SVGUID
		//************************************
		SVGUID getSelectedGUID() const;

		void remove(const std::string& rItemName);

	protected:
		//{{AFX_MSG(AvailableObjectListComboBox)
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()

		//{{AFX_VIRTUAL(AvailableObjectListComboBox)
		//}}AFX_VIRTUAL

	private:
		SvUl::NameGuidList m_List;
	};
} //namespace SvMc
