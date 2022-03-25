//******************************************************************************
//* COPYRIGHT (c) 2016 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
// This is the ObjectsListBox class for use by the GUI. It is a list box which is initialized by a name/objectId list.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "ObjectsListBox.h"
#pragma endregion Includes

namespace SvMc
{
	BEGIN_MESSAGE_MAP(ObjectsListBox, CListBox)
		//{{AFX_MSG_MAP(ObjectsListBox)
		ON_WM_CREATE()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	ObjectsListBox::ObjectsListBox()
	{
	}

	ObjectsListBox::~ObjectsListBox()
	{
	}

	void ObjectsListBox::init(const SvUl::NameObjectIdList& rList, const std::string& rEmptyListText)
	{
		int index;

		ResetContent();

		m_List = rList;

		// Generate new list...
		for (SvUl::NameObjectIdList::const_iterator it = rList.begin(); it != rList.end(); ++it)
		{
			if (!it->first.empty())
			{
				AddString(it->first.c_str());
			}
		}

		if (GetCount() <= 0)
		{
			SetItemData(AddString(rEmptyListText.c_str()), 0);
			index = 0;
		}
		else
		{
			index = AddString(_T("--------------------------------------------"));
		}

		if (index >= GetCount())
		{
			index = GetCount() - 1;
		}

		if (LB_ERR == index) // First init, but existing entry
		{
			index = 0;
		}

		SetCurSel(index);
	}

	uint32_t ObjectsListBox::getObjectId(int index) const
	{
		uint32_t Result = SvDef::InvalidObjectId;
		if (0 <= index && m_List.size() > index)
		{
			Result = m_List[index].second;
		}
		return Result;
	}

	BOOL ObjectsListBox::PreCreateWindow(CREATESTRUCT& cs)
	{
		cs.style ^= LBS_SORT;
		return CListBox::PreCreateWindow(cs);
	}

	int ObjectsListBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CListBox::OnCreate(lpCreateStruct) == -1)
			return -1;

		lpCreateStruct->style ^= LBS_SORT;


		return 0;
	}
} //namespace SvMc
