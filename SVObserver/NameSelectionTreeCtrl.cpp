//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : NameSelectionTreeCtrl
//* .File Name       : $Workfile:   NameSelectionTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.8  $
//* .Check In Date   : $Date:   15 Aug 2014 15:30:02  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "NameSelectionTreeCtrl.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const long LeafNodeIndicator = 0x80000000;

static UINT TokenizeName(const CString& strCompleteName, CStringArray& astrNames)
{
	CString csTemp = strCompleteName;
	astrNames.RemoveAll();

	bool bArraySubscript = false;
	
	// Limit iterations to 30 tokens for safety...
	for (int i = 0;i < 30;i++)
	{
		CString csToken;
		int nIndex = csTemp.Find(_T('.'),0);
		if ( nIndex == -1 )
		{
			// add support for array subscript, e.g. Tool.Name[1]
			// parse as separate token for the purposes
			// of this tree
			nIndex = csTemp.Find(_T('['),0);
			if (nIndex == -1)
			{
				csToken = csTemp;
			}
			else
			{
				if (bArraySubscript)
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

		if (nIndex < 0)
		{
			break;
		}

		// Set up for next token extraction iteration.
		// no dot for array subscript
		csTemp = csTemp.Right( csTemp.GetLength()-nIndex - (bArraySubscript ? 0 : 1) );
	}// end for ( int i = 0; i < 30; i++ )

	if (bArraySubscript)
	{
		// the last token should be Name[1], not [1]
		int iCount = static_cast<int>(astrNames.GetSize());
		if (iCount > 1)
			astrNames[iCount-1] = astrNames[iCount-2] + astrNames[iCount-1];
	}
	return static_cast<int>(astrNames.GetSize());
}

IMPLEMENT_DYNCREATE(NameSelectionTreeCtrl, CTreeCtrl)

BEGIN_MESSAGE_MAP(NameSelectionTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(NameSelectionTreeCtrl)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

NameSelectionTreeCtrl::NameSelectionTreeCtrl()
: CTreeCtrl()
, m_hLastChecked(NULL)
, m_bSingleSelect(false)
, m_bAllowWholeArray(false)
, m_bAllowBranchCheck(false)
{
}

NameSelectionTreeCtrl::~NameSelectionTreeCtrl()
{
}

void NameSelectionTreeCtrl::SetSingleSelect(bool bSingle)
{
	m_bSingleSelect = bSingle;
}

void NameSelectionTreeCtrl::SetAllowWholeArray()
{
	m_bAllowWholeArray = true;
}

void NameSelectionTreeCtrl::SetCanSelectObjectCallback(CanSelectObjectCallbackFn fn)
{
	m_fnCanSelectObject = fn;
}

void NameSelectionTreeCtrl::AllowNodeItemChecks(bool p_bAllow)
{
	m_bAllowBranchCheck = p_bAllow;
}

void NameSelectionTreeCtrl::InitTreeCtrl()
{
	m_StateImageList.Create(16, 16, true, 5, 5);
	
	// State image index 0 is not used
	m_StateImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_NOTEXIST));

	// State image index 1 is 'Not Checked'
	m_StateImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_NOTEXIST));  

	// State image index 2 is 'Checked'
	m_StateImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_EXIST));

	// State image index 3 is 'Some children Checked'
	m_StateImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_EXIST1));

	SetImageList(&m_StateImageList, TVSIL_STATE);
}

// SEJ - unwind this mess...
void NameSelectionTreeCtrl::Populate(const NameSelectionList& list)
{
	HTREEITEM hRoot = NULL;       

	m_mapIndexes.clear();
	DeleteAllItems();

	for (NameSelectionList::const_iterator it = list.begin();it != list.end();++it)
	{
		CString csCompleteName = it->first.c_str();
		bool bSelected = it->second;
		// 'Tokenize' the dotted name using the '.' as the delimiter
		CStringArray arrayCSNames;
		UINT uiTokens = TokenizeName(csCompleteName, arrayCSNames);

		CString csNameLeaf = csCompleteName;
		if (arrayCSNames.GetCount() > 1)
		{
			csNameLeaf = arrayCSNames[arrayCSNames.GetCount() - 1];
		}
		// Get items from the tree and compare the ItemData to the
		// dotted name tokens.  Assume a maximum 30 level tree.
		HTREEITEM hChild = hRoot;

		int nCountTokens = static_cast<int>(arrayCSNames.GetSize());
		for (int nLevel = 0; nLevel < nCountTokens; nLevel++)
		{
			// Check for next child. 
			// If hChild == NULL then we are at the root.
			if (nLevel == 0)
			{
				hChild = hRoot;    // Force the root node.
			}
			else
			{
				hChild = GetChildItem(hChild);
			}

			// Iterate the siblings, i.e. tree items at the same 'level'
			// looking for tree item text that match the current dotted
			// name's current level token in the tokenized array at nLevel.
			bool bDone = false;
			while (!bDone)
			{
				// Get text assigned to this tree item.
				CString csChild;
				if (hChild)
				{
					csChild = GetItemText(hChild);
				}
				// if the text in the tree item matches the token derived
				// from the 'dotted' name then go to next token.
				if (csChild == arrayCSNames.GetAt(nLevel))
				{
					goto next_level;
					// next tree level going toward the 'leaf' 
				}
				// At this same level see if there are any more tree 'siblings'.
				HTREEITEM hSibling = GetNextSiblingItem(hChild);
					
				if (hSibling == NULL)
				{
				   bDone = true;
				   continue;
				}
				hChild = hSibling;
			}// end while(!bDone)

			// We have a branch that is not already in the tree...
			// So fill out the branch to the 'leaf' starting at the current
			// 'nLevel' in the token name array.
			{// begin block
				HTREEITEM hNewChild = GetParentItem(hChild);
				for (int nNewLevel = nLevel; nNewLevel < nCountTokens; nNewLevel++)
				{
					CString csNewChild = arrayCSNames.GetAt(nNewLevel);
					if (hNewChild == NULL)
					{
						if (m_bAllowBranchCheck)
						{
							hRoot = InsertItem(TVIF_TEXT | TVIF_STATE | TVIF_PARAM,      // UINT nMask, 
										(LPCTSTR)csNewChild,             // LPCTSTR lpszItem, 
										0,                               // int nImage, 
										0,                               // int nSelectedImage, 
										INDEXTOSTATEIMAGEMASK(1),		 // UINT nState, Not Checked = 1
										TVIS_STATEIMAGEMASK,             // UINT nStateMask, 
										0,                               // LPARAM lParam,  See SetItemData() below
										TVI_ROOT,                        // HTREEITEM hParent, 
										TVI_LAST                         // HTREEITEM hInsertAfter 
									);
						}
						else
						{
							hRoot = InsertItem((LPCTSTR)csNewChild, TVI_ROOT, TVI_LAST);
						}
						ASSERT(hRoot);
						hNewChild = hRoot;
					}
					else
					{
						int nIndex = 0;
						if (m_bAllowBranchCheck || csNewChild == csNameLeaf)
						{
							nIndex = 1;    // Check box 'Not checked'

							hNewChild = InsertItem(TVIF_TEXT | TVIF_STATE | TVIF_PARAM,      // UINT nMask, 
										(LPCTSTR)csNewChild,             // LPCTSTR lpszItem, 
										0,                               // int nImage, 
										0,                               // int nSelectedImage, 
										INDEXTOSTATEIMAGEMASK(nIndex),	 // UINT nState, 
										TVIS_STATEIMAGEMASK,             // UINT nStateMask, 
										0,                               // LPARAM lParam,  See SetItemData() below
										hNewChild,                       // HTREEITEM hParent, 
										TVI_LAST                         // HTREEITEM hInsertAfter 
									);
						}
						else
						{
							hNewChild = InsertItem(TVIF_TEXT | TVIF_PARAM, 
										(LPCTSTR)csNewChild, 
										0,                 // Image index for little icon
										0,                 // Selected image 
										0,                 // UINT nState, 
										0,                 // UINT nStateMask, 
										0,                 // LPARAM lParam,  See SetItemData() below
										hNewChild,         // HTREEITEM hParent, 
										TVI_LAST           // HTREEITEM hInsertAfter 
									);
						}
						// Make sure this item is expanded and visible
						EnsureVisible(hNewChild);
					}
					ASSERT (hNewChild);

					// Are we at the tree 'leaf' yet?
					if (csNewChild == csNameLeaf)
					{
						int dist = static_cast<int>(std::distance(list.begin(), it));
						m_mapIndexes[dist] = hNewChild;
						SetItemData(hNewChild, dist | LeafNodeIndicator);
						// check if it should be selected
						if (bSelected)
						{
							// Make sure this item is expanded and visible
							EnsureVisible(hNewChild);
							//SetCheckState(hNewChild, true, false);
							Click(hNewChild);
						}
					}
					else
					{
						// Make sure this item is expanded and visible
						EnsureVisible(hNewChild);
					}
				}// end for (int nNewLevel = nLevel;nNewLevel <= nCountTokens;nNewLevel++)
				goto next_i;
			}// end block
			next_level:;
		}// end for (int nLevel = 0;nLevel < nCountTokens;nLevel++)
		next_i:;
	}// end for (int i = 0;i < nCount;i++)

	// Expand root to show next level to start with...
	// Will show the Tools in the Tool Set.
	if (GetCount() > 0)
	{
		Expand(hRoot, TVE_EXPAND);
		if (m_bAllowBranchCheck)
		{
			long l_lCheckState = 0;
			UpdateNodeStateColor(hRoot, l_lCheckState); 
		}
	}
	//Set the display to top of the tree
	SelectSetFirstVisible(GetRootItem());
}

bool NameSelectionTreeCtrl::SelectItemByIndex(int index)
{
	bool bRetVal = false;
	HTREEITEM hItem = m_mapIndexes[index];
	if (hItem)
	{
		bRetVal = Click(hItem);
	}
	return bRetVal;
}

void NameSelectionTreeCtrl::DeselectAll()
{
	HTREEITEM hItem = GetRootItem();
	SetBranchChecks(hItem, false);
}

bool NameSelectionTreeCtrl::SetCheckState(HTREEITEM hItem, bool bCheck, bool bSelect)
{
	TVITEM tvItem;
	
	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;
	
	//	Since state images are one-based, 1 in this macro turns the check off, and 2 turns it on.
	tvItem.state = INDEXTOSTATEIMAGEMASK((bCheck ? 2 : 1));

	if (bCheck)
	{
		if (m_bSingleSelect && m_hLastChecked != NULL) // deselect last on tree control
		{
			SetCheckState(m_hLastChecked, false);
		}
		m_hLastChecked = hItem;
	}

	BOOL bRet = SetItem(&tvItem);
	if (bSelect)
	{
		bRet = SelectItem(hItem) && bRet;
	}
	return bRet != 0;
}

bool NameSelectionTreeCtrl::GetCheckState(HTREEITEM hItem)
{
	TVITEM tvItem;
	
	// Prepare to receive the desired information.
	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;
	
	// Request the information.
	GetItem(&tvItem);
	
	// Return zero if it's not checked, or nonzero otherwise.
	return ((tvItem.state >> 12) - 1) != 0;
}

void NameSelectionTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// See if user 'clicked' on a tree item.
	UINT uFlags;
	HTREEITEM hItem = HitTest(point, &uFlags);

	// If it's a Tree Item and clicked on the Item Text
	if (hItem && (uFlags & TVHT_ONITEM))
	{
		Click(hItem);
	}
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void NameSelectionTreeCtrl::UpdateNodeStateColor(HTREEITEM hItem, long& p_rlFlags)
{
	HTREEITEM hItemLast = NULL;
	while (hItem)
	{
		hItemLast = hItem;

		hItem = GetChildItem(hItem);
		if (hItem) // has children...
		{
			long l_lFlags = 0;
			UpdateNodeStateColor(hItem, l_lFlags);
			p_rlFlags |= l_lFlags;
			switch (l_lFlags)
			{
				case 0:
				{
					break;
				}
				case 1:
				{
					// white none selected
					SetItemState(hItemLast, INDEXTOSTATEIMAGEMASK(1), TVIS_STATEIMAGEMASK);
					break;
				}
				case 2:
				{
					// Green all selected
					SetItemState(hItemLast, INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK);
					break;
				}
				case 3:
				{
					// Gray some selected
					SetItemState(hItemLast, INDEXTOSTATEIMAGEMASK(3), TVIS_STATEIMAGEMASK);
					break;
				}
			}
		}
		else
		{
			if (GetCheck(hItemLast))
			{
				p_rlFlags |= 2;
			}
			else
			{
				p_rlFlags |= 1;
			}
		}
		hItem = GetNextSiblingItem(hItemLast);
	}
}

bool NameSelectionTreeCtrl::Click(HTREEITEM hItem)
{
	DWORD_PTR dwItemData = GetItemData(hItem);
	if (dwItemData)
	{
		bool bCheckState = GetCheckState(hItem);

		bool bCanSelect = true;
		if (m_fnCanSelectObject)
		{
			int iIndex = static_cast<int>(dwItemData) & ~LeafNodeIndicator;
			bCanSelect = m_fnCanSelectObject(bCheckState, iIndex);
		}
		if (bCanSelect)
		{
			bool bNewCheckState = bCheckState ? false : true;
			
			SetCheckState(hItem, bNewCheckState);
			if (m_bAllowBranchCheck)
			{
				long l_lCheckState = 0;
				UpdateNodeStateColor(GetRootItem(), l_lCheckState); 
			}
			return true;
		}
	}
	else
	{
		if (m_bAllowBranchCheck)
		{
			bool l_bBranchState = GetCheckState(hItem);
			bool l_bNewBranchState = l_bBranchState ? false : true;
			SetBranchChecks( hItem, l_bNewBranchState);
			long l_lCheckState = 0;
			UpdateNodeStateColor(GetRootItem(), l_lCheckState); 
			return true;
		}
	}
	return false;
}

void NameSelectionTreeCtrl::SetBranchChecks(HTREEITEM hItem , bool p_bNewBranchState)
{
	SetCheckState(hItem, p_bNewBranchState, false);
	// **** Set Check on Everything below this node...
	HTREEITEM hChildItem = GetChildItem(hItem);
	while (hChildItem != NULL) // && l_bStillChecking )
	{
		DWORD_PTR dwItemData = GetItemData(hChildItem);
		if (!dwItemData)
		{
			SetBranchChecks(hChildItem, p_bNewBranchState);
		}
		else
		{
			bool l_bThisState = GetCheckState(hChildItem);

			if (p_bNewBranchState != l_bThisState)
			{
				bool bCanSelect = true;
				if (m_fnCanSelectObject)
				{
					int iIndex = static_cast<int>(dwItemData) & ~LeafNodeIndicator;
					bCanSelect = m_fnCanSelectObject(l_bThisState, iIndex);
				}
				if (bCanSelect)
				{
					SetCheckState(hChildItem, p_bNewBranchState);
				}
			}
		}
		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\NameSelectionTreeCtrl.cpp_v  $
 * 
 *    Rev 1.8   15 Aug 2014 15:30:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed DeselectItems.
 * Revised DeselectAll to call SetBranchChecks
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   15 Aug 2014 13:27:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added DeselectAll method.
 * Added DeselectItems method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Jun 2014 11:07:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed issue with branch selection
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 May 2014 17:51:22   sjones
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
 *    Rev 1.4   21 May 2014 12:19:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised the Populate method to use unsigned user data assigned to the lParam of the TreeItem.
 * Revised the Click method to correct a casting issue with GetItemData.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 May 2014 10:11:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised Populate to use size_t for assignment from std::distance
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   23 Apr 2014 14:29:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed Populate method to show the first item in the tree
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Apr 2014 10:43:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed issue to make sure all items are visible when the tree is loaded and fixed an issue with branch selection
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Apr 2014 16:25:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
