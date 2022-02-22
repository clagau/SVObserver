//******************************************************************************
//* COPYRIGHT (c) 2016 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
 // This is the ObjectsListBox class for use by the GUI. It is a list box which is initialized by a name/objectId list.
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary\NameObjectIdList.h"

#pragma endregion Includes

namespace SvMc
{
	class ObjectsListBox : public CListBox
	{
	public:
		ObjectsListBox();
		virtual ~ObjectsListBox();

		void init( const SvUl::NameObjectIdList& rList, const std::string& rEmptyListText );

		//************************************
		/// Return the id of an entry. If invalid index of the selected item has on ID it return SvDef::InvalidObjectId.
		/// \param index [in] Index of the entry
		/// \returns uint32_t
		//************************************
		uint32_t getObjectId(int index) const;

		//{{AFX_VIRTUAL(ObjectsListBox)
	protected:
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
		//}}AFX_VIRTUAL
	protected:
		//{{AFX_MSG(ObjectsListBox)
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()

	private:
		SvUl::NameObjectIdList m_List;
	};
} //namespace SvMc
