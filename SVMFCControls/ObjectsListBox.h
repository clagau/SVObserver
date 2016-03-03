//******************************************************************************
//* COPYRIGHT (c) 2016 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
 // This is the ObjectsListBox class for use by the GUI. It is a list box which is initialized by a name/GUID list.
//******************************************************************************

#pragma once
#include "SVUtilityLibrary\NameGuidList.h"

namespace Seidenader
{
	namespace SVMFCControls
	{
		class ObjectsListBox : public CListBox
		{
		public:
			ObjectsListBox();
			virtual ~ObjectsListBox();

			void init( const SvUl::NameGuidList& rList, const SVString& rEmptyListText );

			//************************************
			/// Return the guid of an entry. If invalid index of the selected item has on GUID it return SVInvalidGUID.
			/// \param index [in] Index of the entry
			/// \returns SVGUID
			//************************************
			SVGUID getGUID(int index) const;

			//{{AFX_VIRTUAL(ObjectsListBox)
		protected:
			virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
			//}}AFX_VIRTUAL
		protected:
			//{{AFX_MSG(ObjectsListBox)
			afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
			//}}AFX_MSG

			DECLARE_MESSAGE_MAP()

		private:
			SvUl::NameGuidList m_List;
		};
	}  //end namespace SVOGUI
}  //end namespace Seidenader

namespace SvMc = Seidenader::SVMFCControls;

