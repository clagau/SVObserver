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
	namespace SVOGui
	{
		class AvailableObjectListComboBox : public CComboBox
		{
		public:
			AvailableObjectListComboBox();
			virtual ~AvailableObjectListComboBox();

			void Init(const SvUl::NameGuidList& List, const SVString& rSelectedItem, const SVString& rEmptyListText);

			//************************************
			/// Return the guid of the selected item. If no item selected or an error it return SVInvalidGUID.
			/// \returns SVGUID
			//************************************
			SVGUID getSelectedGUID() const;

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

namespace SvOg = Seidenader::SVOGui;