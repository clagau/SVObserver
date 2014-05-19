//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputInfoListTreeCtrl
//* .File Name       : $Workfile:   SVOutputInfoListTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:40:38  $
//******************************************************************************

#include "stdafx.h"
#include <iterator> // (VS2010)
#include "SVOutputInfoListTreeCtrl.h"
#include "SVTaskObjectList.h"
#include "SVImageListClass.h"
#include "SVImageClass.h"
#include "SVObjectLibrary/SVObjectListClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class SVCompareObjectReference
{
public:
	SVCompareObjectReference(const SVObjectReference& p_Object)
	: m_object(p_Object) {}

	bool operator()(const SVObjectReference& p_Object) const 
	{
		return (p_Object.Guid() == m_object.Guid()) ? true : false;
	}

private:
	SVObjectReference m_object;
};

/////////////////////////////////////////////////////////////////////////////
// SVOutputInfoListTreeCtrlClass

IMPLEMENT_DYNCREATE(SVOutputInfoListTreeCtrlClass, CTreeCtrl)

BEGIN_MESSAGE_MAP(SVOutputInfoListTreeCtrlClass, CTreeCtrl)
	//{{AFX_MSG_MAP(SVOutputInfoListTreeCtrlClass)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


SVOutputInfoListTreeCtrlClass::SVOutputInfoListTreeCtrlClass()
: CTreeCtrl()
{
	m_pTaskObjectList = NULL;
	m_pOutputInfoList = NULL;
	m_uiAttributesSetFilter = SV_SELECTABLE_FOR_EQUATION;
	m_uiAttributesAllowedFilter = SV_SELECTABLE_FOR_EQUATION;
	m_hLastChecked = NULL;
	m_bSingleSelect = false;
	m_bAllowWholeArray = false;
	m_bAllowBranchCheck = false;
}

SVOutputInfoListTreeCtrlClass::~SVOutputInfoListTreeCtrlClass()
{
}


/////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL SVOutputInfoListTreeCtrlClass::InitOutputListTreeCtrl()
{
	m_StateImageList.Create( 16, 16, TRUE, 5, 5 );
	//
	// State image index 0 is not used
	//
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_ICON_NOTEXIST ) );

	//
	// State image index 1 is 'Not Checked'
	//
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_ICON_NOTEXIST ) );  

	//
	// State image index 2 is 'Checked'
	//
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_ICON_EXIST ) );

	//
	// State image index 3 is 'Some children Checked'
	//
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_ICON_EXIST1 ) );

	SetImageList( &m_StateImageList, TVSIL_STATE );

	return TRUE;
}

void SVOutputInfoListTreeCtrlClass::SetSingleSelect(bool bSingle)
{
	m_bSingleSelect = bSingle;
}


void SVOutputInfoListTreeCtrlClass::SetSelection(const SVObjectReferenceVector& p_selectObjectList)
{
	m_selectObjectList = p_selectObjectList;
}

/////////////////////////////////////////////////////////////////////////////
//
//
// Build the output list and set the object 'attributes' to use.
// Also rebuild the tree.
//
void SVOutputInfoListTreeCtrlClass::BuildOutputList
	( SVTaskObjectListClass* pTaskObjectList, UINT uiAttributesSetFilter, UINT uiAttributesAllowedFilter/*=0*/ )
{
	//
	// Save the pointer to the object list so we can get the object type
	// later.
	//
	m_pTaskObjectList = pTaskObjectList;
	m_uiAttributesSetFilter = uiAttributesSetFilter;

	if ( uiAttributesAllowedFilter == 0 )
		m_uiAttributesAllowedFilter = uiAttributesSetFilter;
	else
		m_uiAttributesAllowedFilter = uiAttributesAllowedFilter;

	//
	// Get the Outputs list
	//
	if(!SV_IS_KIND_OF( pTaskObjectList, SVTaskObjectListClass ))
	{
		ASSERT(0);
		return;
	}

	//
	// Build a tree view from our list of outputs and
	// the 'attributes' desired.
	//

	SVOutputInfoListClass l_OutputList;

	m_pTaskObjectList->GetOutputList( l_OutputList );


	//
	// Build a list of objects.
	//
	SVObjectReferenceVector objectList;

	int nCount = l_OutputList.GetSize();
	for(int i=0; i<nCount; i++)
	{
		SVOutObjectInfoStruct* pInfoItem = NULL;

		try
		{
			pInfoItem = l_OutputList.GetAt(i);
		}
		catch( ... )
		{
			::AfxMessageBox( "EXCEPTION: Error in getting Output Object Information Structure" );
		}

		if( pInfoItem != NULL )
		{
			SVObjectReference object;

			try
			{
				object = pInfoItem->GetObjectReference();
			}
			catch( ... )
			{
				::AfxMessageBox( "EXCEPTION: Error in getting Object" );
			}

			SVObjectClass* pObject = object.Object();

			bool bTypeFilterInclude = pObject && m_setTypeFilterInclude.empty() || m_setTypeFilterInclude.find( typeid(*pObject) ) != m_setTypeFilterInclude.end();
			bool bTypeFilterExclude = pObject && m_setTypeFilterExclude.find( typeid(*pObject) ) != m_setTypeFilterExclude.end();
			bool bTypeFilter = bTypeFilterInclude && !bTypeFilterExclude;

			SVValueObjectClass* pValueObject = dynamic_cast <SVValueObjectClass*> (object.Object());
			if ( pValueObject != NULL	&& pValueObject->IsArray() )
			{
				bool bAttributesOK = (pValueObject->ObjectAttributesAllowed() & m_uiAttributesAllowedFilter) == m_uiAttributesAllowedFilter;
				bAttributesOK = bAttributesOK && ( (pValueObject->ObjectAttributesAllowed() & SV_HIDDEN) == 0 );
				if( bTypeFilter && bAttributesOK )
				{
					if ( m_bAllowWholeArray )
					{
						// stuff entire array specifier at the top of the list
						object.SetEntireArray();
						objectList.push_back(object);
					}

					// add array elements
					int iArraySize = pValueObject->GetArraySize();
					for ( int i=0; i < iArraySize; i++ )
					{
						object.SetArrayIndex(i);
						objectList.push_back(object);
					}
				}
			}// end if array value object
			else
			{
				//
				// Check for the required Output object attributes.
				//
				if( pValueObject )
				{
					bool bAttributesOK = (object->ObjectAttributesAllowed() & m_uiAttributesAllowedFilter) == m_uiAttributesAllowedFilter;
					bAttributesOK = bAttributesOK && ( (pValueObject->ObjectAttributesAllowed() & SV_HIDDEN) == 0 );
					if( bTypeFilter && bAttributesOK )
					{
						objectList.push_back(object);
					}
				}
			}
		}
		else
		{
			::AfxMessageBox( "ERROR: Error in getting Output Object Information Structure" );
		}
	}

	RebuildTree( objectList, m_pTaskObjectList->GetObjectType() );

	return;
}

/////////////////////////////////////////////////////////////////////////////
//
//
// A tree based on images for possible archivable.
//
void SVOutputInfoListTreeCtrlClass::BuildImageListTree(
	SVImageListClass* pImageList,
	UINT uiAttributesSetFilter,
	SVObjectTypeEnum objectTypeToInclude,
	UINT uiAttributesAllowedFilter
	)
{
	//
	// Build a list of objects from the image list.
	//
	SVObjectReferenceVector objectList;

	m_uiAttributesSetFilter = uiAttributesSetFilter;
	if ( uiAttributesAllowedFilter == 0 )
		m_uiAttributesAllowedFilter = uiAttributesSetFilter;
	else
		m_uiAttributesAllowedFilter = uiAttributesAllowedFilter;


	int nCount = pImageList->GetSize();
	for(int i=0; i<nCount; i++)
	{
		SVImageClass* pImage = pImageList->GetAt(i);
		
		if ( pImage )
		{
			//
			// Check for the required Output object attributes.
			//
			bool bAttributesOK = (pImage->ObjectAttributesAllowed() & m_uiAttributesAllowedFilter) != 0;	// if any filter bits are set
			bAttributesOK = bAttributesOK && ( (pImage->ObjectAttributesAllowed() & SV_HIDDEN) == 0 );
			if ( bAttributesOK )
			{
				objectList.push_back( pImage );
			}
		}
	}

	RebuildTree( objectList, objectTypeToInclude );

	return;
}

void SVOutputInfoListTreeCtrlClass::AllowNodeItemChecks( bool p_bAllow )
{
	m_bAllowBranchCheck = p_bAllow;
}



/////////////////////////////////////////////////////////////////////////////
//
//
//
//
void SVOutputInfoListTreeCtrlClass::RebuildTree
	(const SVObjectReferenceVector& p_rObjectVector, SVObjectTypeEnum objectTypeToInclude )
{
	BOOL bResult;
	HTREEITEM hRoot = NULL;       

	DeleteAllItems();
	m_ObjectList.clear();
	m_ObjectList.push_back( SVObjectReference() );	// add dummy NULL so index 0 will be "invalid"

	//
	// Build the tree from the 'dotted' names of each output object.
	//

	int nCount = static_cast<int>(p_rObjectVector.size());
	for ( int i = 0; i < nCount; i++ )
	{

		//SVObjectClass* pObject = pObjectList->GetAt(i);
		SVObjectReference object = p_rObjectVector.at(i);

		//
		// Check for the required Output object attributes.
		//
		//if((pObject->uObjectAttributesAllowed & m_uiAttributesSetFilter) == 
		//	m_uiAttributesSetFilter )
		{
			//
			// Get the 'dotted' name including the Object Type name selected
			// for the 'root'.
			//
			CString csCompleteName = object.GetCompleteOneBasedObjectNameToObjectType(
			                                    NULL, objectTypeToInclude );

			//
			// Get the 'leaf' name
			//
			CString csNameLeaf = object.GetOneBasedName();

			//
			// 'Tokenize' the dotted name using the '.' as the delimiter
			//
			CStringArray arrayCSNames;
			UINT uiTokens = TokenizeName(csCompleteName, arrayCSNames);
			ASSERT ( uiTokens > 0 );

			//
			// Get items from the tree and compare the ItemData to the
			// dotted name tokens.  Assume a maximum 30 level tree.
			//
			HTREEITEM hChild = hRoot;

			int nCountTokens = static_cast<int>(arrayCSNames.GetSize());
			for ( int nLevel = 0; nLevel < nCountTokens; nLevel++ )
			{
				//
				// Check for next child. 
				// If hChild == NULL then we are at the root.
				//
				if ( nLevel == 0 )
				{
					hChild = hRoot;    // Force the root node.
				}
				else
				{
					hChild = GetChildItem( hChild );
				}

				//
				// Iterate the siblings, i.e. tree items at the same 'level'
				// looking for tree item text that match the current dotted
				// name's current level token in the tokenized array at 
				// nLevel.
				//
				bool bDone = false;
				while ( !bDone )
				{
					//
					// Get text assigned to this tree item.
					//
					CString csChild;
					if ( hChild )
					{
						csChild = GetItemText( hChild );
					}

					//
					// if the text in the tree item matches the token derived
					// from the 'dotted' name then go to next token.
					//
					if ( csChild == arrayCSNames.GetAt(nLevel) )
					{
						goto next_level;
						   // next tree level going toward the 'leaf' 
					}

					//
					// At this same level see if there are any more 
					// tree 'siblings'.
					//
					HTREEITEM hSibling = GetNextSiblingItem(hChild);
					
					if ( hSibling == NULL )
					{
					   bDone = true;
					   continue;
					}

					hChild = hSibling;
				}// end while(!bDone)

				//
				// We have a branch that is not already in the tree...
				// So fill out the branch to the 'leaf' starting at the current
				// 'nLevel' in the token name array.
				//
				{// begin block

					HTREEITEM hNewChild = GetParentItem(hChild);

					for( int nNewLevel = nLevel; nNewLevel <= nCountTokens; nNewLevel++ )
					{
						CString csNewChild = arrayCSNames.GetAt(nNewLevel);
						if ( hNewChild == NULL )
						{
							if( m_bAllowBranchCheck )
							{
								hRoot = InsertItem
									(
										TVIF_TEXT |       /* TVIF_IMAGE | TVIF_SELECTEDIMAGE |*/
											TVIF_STATE | TVIF_PARAM,      // UINT nMask, 
										(LPCTSTR)csNewChild,             // LPCTSTR lpszItem, 
										0,                               // int nImage, 
										0,                               // int nSelectedImage, 
										INDEXTOSTATEIMAGEMASK( 1 ),		// UINT nState, Not Checked = 1
										TVIS_STATEIMAGEMASK,             // UINT nStateMask, 
										0L,                              // LPARAM lParam,  See SetItemData() below
										TVI_ROOT,                       // HTREEITEM hParent, 
										TVI_LAST                         //HTREEITEM hInsertAfter 
									);
							}
							else
							{
								hRoot = InsertItem( (LPCTSTR)csNewChild, TVI_ROOT, TVI_LAST );
							}
							ASSERT(hRoot);

							hNewChild = hRoot;
						}
						else
						{
							int nIndex = 0;
							if ( m_bAllowBranchCheck || csNewChild == csNameLeaf )
							{
								nIndex = 1;    // Check box 'Not checked'

								hNewChild =InsertItem
									(
										TVIF_TEXT |       /* TVIF_IMAGE | TVIF_SELECTEDIMAGE |*/
											TVIF_STATE | TVIF_PARAM,      // UINT nMask, 
										(LPCTSTR)csNewChild,             // LPCTSTR lpszItem, 
										0,                               // int nImage, 
										0,                               // int nSelectedImage, 
										INDEXTOSTATEIMAGEMASK( nIndex ), // UINT nState, 
										TVIS_STATEIMAGEMASK,             // UINT nStateMask, 
										0L,                              // LPARAM lParam,  See SetItemData() below
										hNewChild,                       // HTREEITEM hParent, 
										TVI_LAST                         //HTREEITEM hInsertAfter 
									);
							}
							else
							{
								hNewChild = InsertItem
									(
										TVIF_TEXT | TVIF_PARAM, 
										(LPCTSTR)csNewChild, 
										0,                 // Image index for little icon
										0,                 // Selected image 
										0,                 // UINT nState, 
										0,                 // UINT nStateMask, 
										0L,                // LPARAM lParam,  See SetItemData() below
										hNewChild,         // HTREEITEM hParent, 
										TVI_LAST           // HTREEITEM hInsertAfter 
									);
							}
						}
						ASSERT ( hNewChild );

						//
						// Are we at the tree 'leaf' yet?
						//
						if ( csNewChild == csNameLeaf )
						{
							int iItemIndex = static_cast<int>(m_ObjectList.size());
							bResult = SetItemData ( hNewChild, iItemIndex );
							m_ObjectList.push_back(object);
							m_mapIndexes[iItemIndex] = hNewChild;
							//bResult = SetItemData ( hNewChild, reinterpret_cast<DWORD_PTR>(pObject) );

							if (m_selectObjectList.size() > 0)
							{
								// check if it should be selected
								if (std::find_if(m_selectObjectList.begin(), m_selectObjectList.end(), SVCompareObjectReference(object)) != m_selectObjectList.end())
								{
									//
									// Make sure this item is expanded and visible
									//
									EnsureVisible( hNewChild  ); // HTREEITEM hItem 
							
									m_setOrigSelected.insert( object );
									SetCheckState( hNewChild, true, false);
								}
							}
							else
							{
								//
								// Set the attributes back into the tree list item.
								//
								UINT uiAttributesSet = object.ObjectAttributesSet();
								bool bItemState = (( uiAttributesSet & m_uiAttributesSetFilter) == m_uiAttributesSetFilter);
								if ( bItemState )
								{
									//
									// Make sure this item is expanded and visible
									//
									BOOL bResult = EnsureVisible( hNewChild  ); // HTREEITEM hItem 
									//ASSERT(bResult);
								
									m_setOrigSelected.insert( object );
								}
							
								bool bResult = SetCheckState( hNewChild, bItemState, FALSE);
								//ASSERT ( bResult );
							}
							goto next_i;
						}
						else
						{
							bResult = SetItemData ( hNewChild, static_cast<DWORD_PTR>(NULL) );  
						}
					}// end for( int nNewLevel = nLevel; nNewLevel <= nCountTokens; nNewLevel++ )
				}// end block
			next_level:;
			}// end for( int nLevel = 0; nLevel < nCountTokens; nLevel++ )
		}// end block
	next_i:;
	}// end for(int i=0; i<nCount; i++)

	m_setNewSelected = m_setOrigSelected;

	//
	// Expand root to show next level to start with..
	// Will show the Tools in the Tool Set.
	//
	bResult = Expand( hRoot, TVE_EXPAND );
	ASSERT(bResult);

	if( this->m_bAllowBranchCheck )
	{
		long l_lCheckState = 0;
		UpdateNodeStateColor( hRoot, l_lCheckState ); 
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//
//
UINT SVOutputInfoListTreeCtrlClass::TokenizeName
	(CString& strCompleteName, CStringArray& astrNames )
{
	CString csTemp = strCompleteName;
	astrNames.RemoveAll();

	bool bArraySubscript = false;
	//
	// Limit iterations to 30 tokens for safety...
	//
	for ( int i = 0; i < 30; i++ )
	{
		CString csToken;
		int nIndex = csTemp.Find(_TCHAR('.'),0);
		if ( nIndex == -1 )
		{
			// add support for array subscript, e.g. Tool.Name[1]
			// parse as separate token for the purposes
			// of this tree
			nIndex = csTemp.Find(_TCHAR('['),0);
			if ( nIndex == -1 )
			{
				csToken = csTemp;
			}
			else
			{
				if ( bArraySubscript )
				{
					csToken = csTemp;
					nIndex = -1;
				}
				else
				{
					csToken = csTemp.Left(nIndex);
					bArraySubscript = true;
				}
			}
		}
		else
		{
			csToken = csTemp.Left(nIndex);
		}

		astrNames.Add(csToken);

		if ( nIndex < 0 )
		{
			break;
		}

		//
		// Set up for next token extraction iteration.
		// no dot for array subscript
		//
		csTemp = csTemp.Right( csTemp.GetLength()-nIndex - (bArraySubscript ? 0 : 1) );

	}// end for ( int i = 0; i < 30; i++ )

	if ( bArraySubscript )
	{
		// the last token should be Name[1], not [1]
		int iCount = static_cast<int>(astrNames.GetSize());
		if ( iCount > 1 )
			astrNames[iCount-1] = astrNames[iCount-2] + astrNames[iCount-1];
	}

	return static_cast<int>(astrNames.GetSize());
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
bool SVOutputInfoListTreeCtrlClass::SetCheckState( HTREEITEM hItem, bool bCheck, bool bSelect )
{
	TVITEM tvItem;
	
	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;
	
	/*
	Since state images are one-based, 1 in this macro turns the check off, and 
	2 turns it on.
	*/
	tvItem.state = INDEXTOSTATEIMAGEMASK((bCheck ? 2 : 1));

	// remember selection
	m_mapSelected[hItem] = bCheck;

	if ( bCheck )
		m_setNewSelected.insert( ObjectFromItem( hItem ) );
	else
		m_setNewSelected.erase( ObjectFromItem( hItem ) );

	if ( bCheck )
	{
		if ( m_bSingleSelect && m_hLastChecked != NULL )
		{// deselect last on tree control
			SetCheckState( m_hLastChecked, FALSE );
		}
		m_hLastChecked = hItem;
	}

	BOOL bRet = SetItem( &tvItem );
	if ( bSelect )
		bRet = SelectItem(hItem) && bRet;
	return bRet != 0;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
bool SVOutputInfoListTreeCtrlClass::GetCheckState( HTREEITEM hItem )
{
	TVITEM tvItem;
	
	// Prepare to receive the desired information.
	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;
	
	// Request the information.
	GetItem( &tvItem );
	
	// Return zero if it's not checked, or nonzero otherwise.
	return ((tvItem.state >> 12) - 1) != 0;
}

HTREEITEM SVOutputInfoListTreeCtrlClass::GetCheckedItem()
{
	MapSelectedType::iterator iter;
	for ( iter = m_mapSelected.begin() ; iter != m_mapSelected.end() ; ++iter )
	{
		if ( iter->second == true )
		{
			return iter->first;
		}
	}
	return NULL;
}

SVObjectReference SVOutputInfoListTreeCtrlClass::ObjectFromItem( HTREEITEM ht )
{
	if( ht )
	{
		int iIndex = static_cast<int>(GetItemData(ht));
		return m_ObjectList.at(iIndex);
	}
	else
	{
		return NULL;
	}
}

SVObjectReference SVOutputInfoListTreeCtrlClass::GetCheckedObject()
{
	HTREEITEM ht = GetCheckedItem();
	return ObjectFromItem(ht);
}

BOOL SVOutputInfoListTreeCtrlClass::GetCheckedObjects(SVObjectListClass* pObjectList)
{
	pObjectList->RemoveAll();
	MapSelectedType::iterator iter;
	for ( iter = m_mapSelected.begin() ; iter != m_mapSelected.end() ; ++iter )
	{
		if ( iter->second == true )
		{
			pObjectList->Add( ObjectFromItem(iter->first) );
		}
	}
	return TRUE;
}




/////////////////////////////////////////////////////////////////////////////
//
//
//
//

/////////////////////////////////////////////////////////////////////////////
// SVOutputInfoListTreeCtrlClass message handlers


/////////////////////////////////////////////////////////////////////////////
//
//
//
//
void SVOutputInfoListTreeCtrlClass::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	//
	// See if user 'clicked' on a tree item.
	//
	UINT uFlags;

	HTREEITEM hItem = HitTest( point, &uFlags );

	//
	// Is it a tree item and if so is a 'leaf' item...
	//
	if ( hItem && (uFlags & TVHT_ONITEM ) ) // Click On the Item
	{
		Click( hItem );
	}
	
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
//
// Move tree item states to 'attributes' in output list.
// Usually called from OnOK() in a dialog that contains this control. 
//
BOOL SVOutputInfoListTreeCtrlClass::UpdateAttributesInOutputList()
{
	//
	// Iterate the tree and get all items and process.
	//
	HTREEITEM hItem = GetRootItem();

	WalkTheTree( hItem, NULL, TreeCallbackFn(this, &SVOutputInfoListTreeCtrlClass::ProcessItemAttributes) );

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOutputInfoListTreeCtrlClass::WalkTheTree( HTREEITEM hItem, void* pData, TreeCallbackFn fnItemProc )
{
	HTREEITEM hItemLast = NULL;
	
	while ( hItem )
	{
		//
		// Process the attributes if any.
		//
		fnItemProc( hItem, pData );
		
		hItemLast = hItem;
		
		hItem = GetChildItem( hItem );
		if ( hItem )
		{
			WalkTheTree( hItem, pData, fnItemProc );
		}
		ASSERT ( hItemLast );

		hItem = GetNextSiblingItem( hItemLast );
	}
}

void SVOutputInfoListTreeCtrlClass::UpdateNodeStateColor( HTREEITEM hItem, long& p_rlFlags)
{
	HTREEITEM hItemLast = NULL;
	while( hItem )
	{
		hItemLast = hItem;

		hItem = GetChildItem( hItem );
		if ( hItem ) // has children...
		{
			long l_lFlags = 0;
			UpdateNodeStateColor( hItem, l_lFlags );
			p_rlFlags |= l_lFlags;
			switch( l_lFlags )
			{
				case 0:
				{
					break;
				}
				case 1:
				{
					// white none selected
					SetItemState( hItemLast, INDEXTOSTATEIMAGEMASK( 1 ), TVIS_STATEIMAGEMASK);
					break;
				}
				case 2:
				{
					// Green all selected
					SetItemState( hItemLast, INDEXTOSTATEIMAGEMASK( 2 ), TVIS_STATEIMAGEMASK);
					//SetCheck( hItemLast, 2 );
					break;
				}
				case 3:
				{
					// Gray some selected
					SetItemState( hItemLast, INDEXTOSTATEIMAGEMASK( 3 ), TVIS_STATEIMAGEMASK);
//					SetCheck( hItemLast, 3 );
					break;
				}
			}
		}
		else
		{
			if( GetCheck( hItemLast ))
				p_rlFlags |= 2;
			else
				p_rlFlags |= 1;
		}
		hItem = GetNextSiblingItem( hItemLast );
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOutputInfoListTreeCtrlClass::ProcessItemAttributes( HTREEITEM hItem, void* )
{
	DWORD dwItemData = static_cast<DWORD>(GetItemData(hItem));
	if( dwItemData )
	{
		bool bCheckState = GetCheckState( hItem );
		//
		// Now update the original output list item attributes.
		//
		//SVObjectClass* pObject = pInfoItem->PObject;
		//SVObjectClass* pObject = (SVObjectClass*) dwItemData;
		//ASSERT(pObject);
		int iIndex = dwItemData;
		SVObjectReference object = m_ObjectList.at(iIndex);

		if( bCheckState )
		{
			UINT uAttributesSet = object.ObjectAttributesSet();
			uAttributesSet |= m_uiAttributesSetFilter;
			object.ObjectAttributesSetRef() = uAttributesSet;
		}
		else
		{
			//
			// Clear the attributes bits.
			//
			UINT uAttributesSet = object.ObjectAttributesSet();
			uAttributesSet &= ~m_uiAttributesSetFilter;
			object.ObjectAttributesSetRef() = uAttributesSet;
		}
	}
}

HRESULT SVOutputInfoListTreeCtrlClass::SelectObjectFilter( SVSelectObjectFilterInfoStruct svFilter )
{
	HRESULT hr = S_OK;
	
	HTREEITEM hItem = GetRootItem();

	WalkTheTree( hItem, &svFilter, TreeCallbackFn(this, &SVOutputInfoListTreeCtrlClass::SelectObjectFilterItemProc) );

	return hr;
}


void SVOutputInfoListTreeCtrlClass::SelectObjectFilterItemProc( HTREEITEM hItem, void* pData )
{
	SVSelectObjectFilterInfoStruct* pInfo = static_cast<SVSelectObjectFilterInfoStruct*> (pData);
	ASSERT( pData );
	if ( pData )
	{
		int iIndex = static_cast<int>(GetItemData(hItem));
		SVObjectReference refObject = m_ObjectList.at(iIndex);
		if ( refObject.Object() )
		{
			bool bAttributes = (refObject.ObjectAttributesAllowed() & pInfo->uAttributeFilter) > 0;	// if any of the attributes in the filter are allowed (i.e. logical OR )
			bAttributes = bAttributes && ( (refObject.ObjectAttributesAllowed() & SV_HIDDEN) == 0 );
			bool bText = pInfo->strTextFilter.IsEmpty() ? false : refObject->GetCompleteObjectName().Find( pInfo->strTextFilter ) != -1;
			if ( bAttributes || bText )
				SetCheckState( hItem, pInfo->bOnOff ? TRUE : FALSE, FALSE );
		}
	}
}

void SVOutputInfoListTreeCtrlClass::SetCanSelectObjectCallback( SVObjectTreeCanSelectObjectCallbackFn fn )
{
	m_fnCanSelectObject = fn;
}

void SVOutputInfoListTreeCtrlClass::SetAllowWholeArray()
{
	m_bAllowWholeArray = true;
}

HRESULT SVOutputInfoListTreeCtrlClass::GetAddedObjects( std::set<SVObjectReference>& p_rAddedSet )
{
	p_rAddedSet.clear();
	std::set_difference( m_setNewSelected.begin(), m_setNewSelected.end(),
	                     m_setOrigSelected.begin(), m_setOrigSelected.end(),
	                     std::inserter( p_rAddedSet, p_rAddedSet.begin() ) );
	return S_OK;
}

HRESULT SVOutputInfoListTreeCtrlClass::GetRemovedObjects( std::set<SVObjectReference>& p_rRemovedSet )
{
	p_rRemovedSet.clear();
	std::set_difference( m_setOrigSelected.begin(), m_setOrigSelected.end(),
	                     m_setNewSelected.begin(), m_setNewSelected.end(),
	                     std::inserter( p_rRemovedSet, p_rRemovedSet.begin() ) );
	return S_OK;
}

HRESULT SVOutputInfoListTreeCtrlClass::GetChangedObjects( std::set<SVObjectReference>& p_rChangedSet )
{
	p_rChangedSet.clear();
	std::set_symmetric_difference( m_setOrigSelected.begin(), m_setOrigSelected.end(),
	                               m_setNewSelected.begin(), m_setNewSelected.end(),
	                               std::inserter( p_rChangedSet, p_rChangedSet.begin() ) );
	return S_OK;
}

HRESULT SVOutputInfoListTreeCtrlClass::GetSelectedObjects( SVObjectReferenceVector& p_rSelectedObjects )
{
	p_rSelectedObjects.clear();
	p_rSelectedObjects.reserve( m_setNewSelected.size() );
	std::copy( m_setNewSelected.begin(), m_setNewSelected.end(),
	           std::inserter( p_rSelectedObjects, p_rSelectedObjects.begin() ) );
	return S_OK;
}

HRESULT SVOutputInfoListTreeCtrlClass::SetTypeFilter( const std::set<Loki::TypeInfo>& p_setFilterInclude, const std::set<Loki::TypeInfo>& p_setFilterExclude )
{
	m_setTypeFilterInclude = p_setFilterInclude;
	m_setTypeFilterExclude = p_setFilterExclude;
	return S_OK;
}

HRESULT SVOutputInfoListTreeCtrlClass::GetSelectedObjectsInTreeOrder( SVObjectReferenceVector& p_rvecSelectedObjects, std::vector<int>& p_rvecIndexes )
{
	p_rvecSelectedObjects.clear();
	p_rvecIndexes.clear();
	for ( size_t i = 0; i < m_ObjectList.size(); ++i )
	{
		SVObjectReference& rObject = m_ObjectList[i];
		if ( m_setNewSelected.find( rObject ) != m_setNewSelected.end() )
		{
			p_rvecSelectedObjects.push_back( rObject );
			p_rvecIndexes.push_back( static_cast<int>(i) );
		}
	}
	return S_OK;
}

bool SVOutputInfoListTreeCtrlClass::SelectItemByIndex( int iIndex, bool bSelect )
{
	HTREEITEM hItem = m_mapIndexes[iIndex];
	return Click( hItem );
}

bool SVOutputInfoListTreeCtrlClass::Click( HTREEITEM hItem )
{
	DWORD dwItemData = static_cast<DWORD>(GetItemData(hItem));
	if ( dwItemData )
	{
		bool bCheckState = GetCheckState( hItem );

		bool bCanSelect = true;
		if ( m_fnCanSelectObject )
		{
			int iIndex = static_cast<int>(GetItemData(hItem));
			SVObjectReference refObject = m_ObjectList.at(iIndex);
			bCanSelect = m_fnCanSelectObject(refObject, bCheckState, iIndex);
		}

		if ( bCanSelect )
		{
			bool bNewCheckState = bCheckState ? false : true;
			
			SetCheckState( hItem, bNewCheckState );
			if( m_bAllowBranchCheck )
			{
				long l_lCheckState = 0;
				UpdateNodeStateColor( GetRootItem(), l_lCheckState ); 
			}
			return true;
		}
	}
	else
	{
		if( m_bAllowBranchCheck )
		{
			bool l_bBranchState = GetCheckState( hItem );
			bool l_bNewBranchState = l_bBranchState ? false : true;
			SetBranchChecks( hItem, l_bNewBranchState );
			long l_lCheckState = 0;
			UpdateNodeStateColor( GetRootItem(), l_lCheckState ); 
			return true;
		}
	}
	return false;
}

void SVOutputInfoListTreeCtrlClass::SetBranchChecks( HTREEITEM hItem , bool p_bNewBranchState )
{
	
	SetCheckState( hItem, p_bNewBranchState, false );
	// **** Set Check on Everything below this node...
	HTREEITEM l_hItem = GetChildItem( hItem );
	while( l_hItem != NULL ) // && l_bStillChecking )
	{
		if( GetItemData(l_hItem) == 0 )
		{
			SetBranchChecks(l_hItem, p_bNewBranchState );
		}
		else
		{

			bool l_bThisState = GetCheckState( l_hItem );

			if( p_bNewBranchState != l_bThisState )
			{
				bool bCanSelect = true;
				if ( m_fnCanSelectObject )
				{
					int iIndex = static_cast<int>(GetItemData(l_hItem));
					SVObjectReference refObject = m_ObjectList.at(iIndex);
					bCanSelect = m_fnCanSelectObject(refObject, l_bThisState, iIndex);
				}

				if ( bCanSelect )
				{
					SetCheckState( l_hItem, p_bNewBranchState );
				}
			}
		}
		l_hItem = GetNextItem(l_hItem, TVGN_NEXT);
	}
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVOutputInfoListTreeCtrl.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 11:40:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SetItemData to cast values to DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 07:01:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:15:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   16 Jan 2013 16:43:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  802
   SCR Title:  Add new product type GD1A
   Checked in by:  bWalter;  Ben Walter
   Change Description:  
     Migrating branch code into primary code base.  
   
   Updated Ouput fucntionality assoicated with the PPQ.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   30 Nov 2012 08:49:42   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  800
   SCR Title:  Fix Blob Array Result index string
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added OneBased name function to rebuild tree.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   04 Oct 2012 10:49:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Changed from post incrementor to pre incrementor.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   19 Jun 2012 13:32:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Separated dotted name parsing and management to its own class.  This allows for the use of this functionality without have to involve the Reference Class.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   21 Feb 2012 13:40:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  747
   SCR Title:  Add New Remote Input Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated selection functionality to fix a multi/single select issue.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   28 Oct 2010 14:08:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.33 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   01 Jun 2010 14:57:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to improve ability to track performance.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   01 Jun 2010 10:59:28   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   16 Dec 2009 14:05:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   16 Jun 2008 09:25:22   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  616
   SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Made hit test so the user must click on the item if they wish to check the item.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   07 Mar 2008 12:56:12   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  616
   SCR Title:  Add Server-side Configuration Data Def Lists to the SVObserver program
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Changes to add branch node check boxes
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   21 Jun 2007 14:34:04   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   23 Jun 2006 09:22:14   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  562
   SCR Title:  Upgrade SVObserver to the Dione Release
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     removed compiler warning
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   30 Jan 2006 09:44:28   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  529
   SCR Title:  Add Conditional Product History
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     made changed to allow the Set attributes and Allowed attributes to be specified independently
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   25 Jan 2006 15:00:14   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  534
   SCR Title:  Add Image Select for Auxiliary Extent Source
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     check the SV_HIDDEN flag
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   25 Jan 2006 12:39:12   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  529
   SCR Title:  Add Conditional Product History
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added GetSelectedObjectsInTreeOrder, SelectItemByIndex, Click
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   17 Jan 2006 09:56:42   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  529
   SCR Title:  Add Conditional Product History
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added GetSelectedObjects, SetTypeFilter
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   02 Nov 2005 09:10:50   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  512
   SCR Title:  Add Published Result Images functionality
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added functions GetAddedObjects, GetRemovedObjects, GetChangedObjects
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   21 Sep 2005 07:42:44   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated objects to handle invalid items on the input and output lists.  Methods now verify object validity before use.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   25 Aug 2005 13:46:26   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     added text filter capabilities
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   18 Aug 2005 08:15:36   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     updated filter callback to use SVObjectReference
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   16 Aug 2005 11:20:00   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  492
   SCR Title:  Upgrade SVObserver to version 4.60
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     removed warnings
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   10 Aug 2005 15:59:52   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     added AllowWholeArray
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   08 Aug 2005 14:05:56   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     removed unnecessary array processing (all attributes allowed are the same)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   08 Aug 2005 14:04:30   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  450
   SCR Title:  Add asynchronous functionality to the archive tool
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented "can select" callback
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   30 Jun 2005 15:12:24   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     updated to use SVObjectReference
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   27 Jun 2005 14:56:18   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     use SVObjectReference
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   22 Jun 2005 08:57:00   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented interface for array value objects
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   17 Feb 2005 15:09:00   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   12 Jul 2004 12:31:06   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  429
   SCR Title:  Fix Select Object tree control
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     fixed selection bug
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   22 Apr 2003 13:38:02   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   30 Jan 2003 15:30:14   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated header information.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   20 Nov 2002 10:31:44   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed COM support
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
