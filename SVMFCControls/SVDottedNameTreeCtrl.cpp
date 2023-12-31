//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDottedNameTreeCtrl
//* .File Name       : $Workfile:   SVDottedNameTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   21 May 2014 17:51:24  $
//******************************************************************************

#include "stdafx.h"
#include "SVDottedNameTreeCtrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvMc
{
	IMPLEMENT_DYNCREATE(SVDottedNameTreeCtrl, CTreeCtrl)

	BEGIN_MESSAGE_MAP(SVDottedNameTreeCtrl, CTreeCtrl)
		//{{AFX_MSG_MAP(SVDottedNameTreeCtrl)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	SVDottedNameTreeCtrl::SVDottedNameTreeCtrl()
	{
	}

	SVDottedNameTreeCtrl::~SVDottedNameTreeCtrl()
	{
	}


	////////////////////////////////////////////////////////////////////////////////
	// .Title       : AddItem
	// -----------------------------------------------------------------------------
	// .Description : Add Item which has a dotted name and apply a Value to him 
	//				: ( Value could also be a pointer, e.g. SVObjectClass* )
	//				: Expand Item if BExpand is TRUE ( default )
	////////////////////////////////////////////////////////////////////////////////
	BOOL SVDottedNameTreeCtrl::AddItem( LPCTSTR DottedItemName, DWORD_PTR DwItemValue, BOOL BExpand )
	{
		std::string dottedName {DottedItemName};
		if( false == dottedName.empty())
		{
			HTREEITEM hRoot = nullptr;

			std::vector<std::string> nameList;
			nameList.reserve(50);
			std::stringstream stringStream(dottedName);
			std::string name;
			while (std::getline(stringStream, name, '.'))
			{
				nameList.emplace_back(std::move(name));
			}

			for(auto iter = nameList.cbegin(); nameList.cend() != iter; ++iter)
			{
				// Note:
				// If there is no next token ( nullptr == tStrNextToken ), 
				// we assume the current token ( tStrToken )
				// is the object itself and DwItemValue is set for him !!!

				// Try to find token in current tree level...
				HTREEITEM hSibling = GetChildItem( hRoot );
				do
				{
					CString strItem = GetItemText( hSibling );
					if( strItem.CompareNoCase(iter->c_str()) == 0 )
						break;
				} while( NULL != (hSibling = GetNextItem( hSibling, TVGN_NEXT )));

				if( nullptr == hSibling )
				{
					// There were no leaf which was refered by the token at this
					// tree level yet.
					auto nextIter {iter};
					++nextIter;

					// Put in a new leaf...
					if(nameList.cend() == nextIter)
					{
						// We stick the object itself inside...
						hSibling = InsertItem( TVIF_TEXT | TVIF_PARAM, // UINT nMask, 
							iter->c_str(),  // LPCTSTR lpszItem, 
							0,                     // int nImage, 
							0,                     // int nSelectedImage, 
							0, //INDEXTOSTATEIMAGEMASK( nIndex ),	// UINT nState, 
							0, //TVIS_STATEIMAGEMASK,        // UINT nStateMask, 
							DwItemValue,            // LPARAM lParam,  See SetItemData() below
							hRoot,					// HTREEITEM hParent, 
							TVI_LAST				//HTREEITEM hInsertAfter 
							);


						if( BExpand )
							Expand( hRoot, TVE_EXPAND );

						return( nullptr != hSibling );
					}

					// We build just another sibling...
					hSibling = InsertItem( TVIF_TEXT,				// UINT nMask, 
						iter->c_str(),  // LPCTSTR lpszItem, 
						0,                     // int nImage, 
						0,                     // int nSelectedImage, 
						0, //INDEXTOSTATEIMAGEMASK( nIndex ),	// UINT nState, 
						0, //TVIS_STATEIMAGEMASK,        // UINT nStateMask, 
						0,			            // LPARAM lParam,  See SetItemData() below
						hRoot,					// HTREEITEM hParent, 
						TVI_LAST				//HTREEITEM hInsertAfter 
						);

				}

				if( nullptr != hSibling )
				{
					if( BExpand )
						Expand( hRoot, TVE_EXPAND );

					// We walk down the root...
					hRoot = hSibling;
					continue;
				}

				break;
			}	
		}

		return FALSE;
	}

	////////////////////////////////////////////////////////////////////////////////
	// .Title       : GetItem
	// -----------------------------------------------------------------------------
	// .Description : Get Item using dotted name. Starts always at the root.
	//				: Returns nullptr, if Item not exists.
	////////////////////////////////////////////////////////////////////////////////
	HTREEITEM SVDottedNameTreeCtrl::GetItem( LPCTSTR DottedItemName )
	{
		std::string dottedName {DottedItemName};
		if (false == dottedName.empty())
		{
			HTREEITEM hRoot = nullptr;

			std::vector<std::string> nameList;
			nameList.reserve(50);
			std::stringstream stringStream(dottedName);
			std::string name;
			while (std::getline(stringStream, name, '.'))
			{
				nameList.emplace_back(std::move(name));
			}

			for (auto iter = nameList.begin(); nameList.end() != iter; ++iter)
			{
				// Note:
				// If there is no next token ( nullptr == tStrNextToken ), 
				// we assume the current token ( tStrToken )
				// is the object itself !!!

				// Try to find token in current tree level...
				HTREEITEM hSibling = GetChildItem( hRoot );
				do
				{
					CString strItem = GetItemText( hSibling );
					if( strItem.CompareNoCase(iter->c_str()) == 0 )
						break;
				} while(NULL != (hSibling = GetNextItem( hSibling, TVGN_NEXT )));

				if( nullptr != hSibling )
				{
					auto nextIter {iter};
					++nextIter;

					if (nameList.cend() == nextIter)
					{
						return hSibling;
					}

					// We walk down the root...
					hRoot = hSibling;
					continue;
				}

				break;
			}	
		}

		return nullptr;
	}


	////////////////////////////////////////////////////////////////////////////////
	// .Title       : GetSelectedItemValue
	// -----------------------------------------------------------------------------
	// .Description : Returns applied Value of the currently selected Item.
	//				: Returns 0, if nothing is selected.
	////////////////////////////////////////////////////////////////////////////////
	DWORD_PTR SVDottedNameTreeCtrl::GetSelectedItemValue()
	{
		HTREEITEM hItem = GetSelectedItem();
		if( nullptr != hItem )
			return GetItemData( hItem );

		return 0;
	}
} //namespace SvMc
