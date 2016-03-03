//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the AvailableObjectListComboBox class for use by the GUI.
//******************************************************************************
#pragma once

#pragma region Includes
#include <afxwin.h>
#include <map>
#include <Guiddef.h>
#include "SVUtilityLibrary\SVString.h"
#include "SVUtilityLibrary\NameGuidList.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVMFCControls
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
			void Init(const SvUl::NameGuidList& List, const SVString& rSelectedItem, const SVString& rEmptyListText, const SVString& rFirstListText = _T(""));

			//************************************
			/// Return the guid of the selected item. If no item selected or an error it return SVInvalidGUID.
			/// \returns SVGUID
			//************************************
			SVGUID getSelectedGUID() const;

			void remove(const SVString& rItemName);

		protected:
			//{{AFX_MSG(AvailableObjectListComboBox)
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()

			//{{AFX_VIRTUAL(AvailableObjectListComboBox)
			//}}AFX_VIRTUAL

		private:
			SvUl::NameGuidList m_List;
		};
	}
}

namespace SvMc = Seidenader::SVMFCControls;