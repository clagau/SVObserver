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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDottedNameTreeCtrl.cpp_v  $
 * 
 *    Rev 1.3   21 May 2014 17:51:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised to remove unnessary casting.
 * Revised to correct casting warnings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 May 2014 12:21:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised AddItem and GetItemValue methods to use DWORD_PTR instead of DWORD to correct an issue in 64Bit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 14:12:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:20:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 09:09:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Nov 2002 11:58:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed COM support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Feb 2000 23:55:36   robert
 * Project:  SVObserver
 * Change Request (SCR) nbr:  93
 * SCR Title:  Integrate Multiple Camera Image Input per IPD
 * Checked in by:  Robert;  Robert Massinger
 * Change Description:  
 *   Initial Check In.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/