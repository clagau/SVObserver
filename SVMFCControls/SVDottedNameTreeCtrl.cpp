//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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

namespace Seidenader
{
	namespace SVMFCControls
	{
		IMPLEMENT_DYNCREATE(SVDottedNameTreeCtrlClass, CTreeCtrl)

			SVDottedNameTreeCtrlClass::SVDottedNameTreeCtrlClass()
		{
		}

		SVDottedNameTreeCtrlClass::~SVDottedNameTreeCtrlClass()
		{
		}


		////////////////////////////////////////////////////////////////////////////////
		// .Title       : AddItem
		// -----------------------------------------------------------------------------
		// .Description : Add Item which has a dotted name and apply a Value to him 
		//				: ( Value could also be a pointer, e.g. SVObjectClass* )
		//				: Expand Item if BExpand is TRUE ( default )
		////////////////////////////////////////////////////////////////////////////////
		// .History
		//	 Date		Author		Comment                                       
		//  :26.01.2000 RO 			First Implementation
		////////////////////////////////////////////////////////////////////////////////
		BOOL SVDottedNameTreeCtrlClass::AddItem( CString StrDottedItemName, DWORD_PTR DwItemValue, BOOL BExpand )
		{
			LPTSTR tStrSource = _tcsdup( StrDottedItemName );
			if( tStrSource )
			{
				HTREEITEM hRoot = NULL;

				TCHAR sep[] = _T( "." );
				LPTSTR tStrToken = _tcstok( tStrSource, sep );
				LPTSTR tStrNextToken = _tcstok( NULL, sep );

				while( tStrToken != NULL )   
				{
					// Note:
					// If there is no next token ( tStrNextToken == NULL ), 
					// we assume the current token ( tStrToken )
					// is the object itself and DwItemValue is set for him !!!

					// Try to find token in current tree level...
					HTREEITEM hSibling = GetChildItem( hRoot );
					do
					{
						CString strItem = GetItemText( hSibling );
						if( strItem.CompareNoCase( tStrToken ) == 0 )
							break;
					} while( hSibling = GetNextItem( hSibling, TVGN_NEXT ) );

					if( hSibling == NULL )
					{
						// There were no leaf which was refered by the token at this
						// tree level yet.

						// Put in a new leaf...
						if( tStrNextToken == NULL )
						{
							// We stick the object itself inside...
							hSibling = InsertItem( TVIF_TEXT | TVIF_PARAM, // UINT nMask, 
								( LPCTSTR ) tStrToken,  // LPCTSTR lpszItem, 
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

							free( tStrSource );
							return( hSibling != NULL );
						}

						// We build just another sibling...
						hSibling = InsertItem( TVIF_TEXT,				// UINT nMask, 
							( LPCTSTR ) tStrToken,  // LPCTSTR lpszItem, 
							0,                     // int nImage, 
							0,                     // int nSelectedImage, 
							0, //INDEXTOSTATEIMAGEMASK( nIndex ),	// UINT nState, 
							0, //TVIS_STATEIMAGEMASK,        // UINT nStateMask, 
							0,			            // LPARAM lParam,  See SetItemData() below
							hRoot,					// HTREEITEM hParent, 
							TVI_LAST				//HTREEITEM hInsertAfter 
							);

					}

					if( hSibling != NULL )
					{
						if( BExpand )
							Expand( hRoot, TVE_EXPAND );

						// We walk down the root...
						hRoot = hSibling;

						// And get the next token...
						tStrToken = tStrNextToken;
						tStrNextToken = _tcstok( NULL, sep );

						continue;
					}

					break;
				}	

				free( tStrSource );
			}

			return FALSE;
		}

		////////////////////////////////////////////////////////////////////////////////
		// .Title       : GetItem
		// -----------------------------------------------------------------------------
		// .Description : Get Item using dotted name. Starts always at the root.
		//				: Returns NULL, if Item not exists.
		//				: 
		////////////////////////////////////////////////////////////////////////////////
		// .History
		//	 Date		Author		Comment                                       
		//  :26.01.2000 RO 			First Implementation
		////////////////////////////////////////////////////////////////////////////////
		HTREEITEM SVDottedNameTreeCtrlClass::GetItem( CString StrDottedItemName )
		{
			LPTSTR tStrSource = _tcsdup( StrDottedItemName );
			if( tStrSource )
			{
				HTREEITEM hRoot = NULL;

				TCHAR sep[] = _T( "." );
				LPTSTR tStrToken = _tcstok( tStrSource, sep );
				LPTSTR tStrNextToken = _tcstok( NULL, sep );

				while( tStrToken != NULL )   
				{
					// Note:
					// If there is no next token ( tStrNextToken == NULL ), 
					// we assume the current token ( tStrToken )
					// is the object itself !!!

					// Try to find token in current tree level...
					HTREEITEM hSibling = GetChildItem( hRoot );
					do
					{
						CString strItem = GetItemText( hSibling );
						if( strItem.CompareNoCase( tStrToken ) == 0 )
							break;
					} while( hSibling = GetNextItem( hSibling, TVGN_NEXT ) );

					if( hSibling != NULL )
					{
						if( tStrNextToken == NULL )
						{
							// We found the item...
							free( tStrSource );
							return hSibling;
						}

						// We walk down the root...
						hRoot = hSibling;

						// And get the next token...
						tStrToken = tStrNextToken;
						tStrNextToken = _tcstok( NULL, sep );

						continue;
					}

					break;
				}	

				free( tStrSource );
			}

			return NULL;
		}


		////////////////////////////////////////////////////////////////////////////////
		// .Title       : GetSelectedItemValue
		// -----------------------------------------------------------------------------
		// .Description : Returns applied Value of the currently selected Item.
		//				: Returns NULL, if nothing is selected.
		////////////////////////////////////////////////////////////////////////////////
		// .History
		//	 Date		Author		Comment                                       
		//  :26.01.2000 RO 			First Implementation
		////////////////////////////////////////////////////////////////////////////////
		DWORD_PTR SVDottedNameTreeCtrlClass::GetSelectedItemValue()
		{
			HTREEITEM hItem = GetSelectedItem();
			if( hItem != NULL )
				return GetItemData( hItem );

			return NULL;
		}

		BEGIN_MESSAGE_MAP(SVDottedNameTreeCtrlClass, CTreeCtrl)
			//{{AFX_MSG_MAP(SVDottedNameTreeCtrlClass)
			// NOTE - the ClassWizard will add and remove mapping macros here.
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()
	} //SVMFCControls
} //Seidenader

