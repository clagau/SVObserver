// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemCombo.cpp
// * .File Name       : $Workfile:   SVPropertyItemCombo.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   23 Jan 2014 07:37:26  $
// ******************************************************************************

#include "stdafx.h"
#include "SVPropertyItemCombo.h"
#include "SVInPlaceList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(SVPropertyItemCombo, SVPropertyItem, 1)


SVPropertyItemCombo::SVPropertyItemCombo()
	: SVPropertyItem()
{
	SetData(_T(""), CBS_DROPDOWNLIST);
}


SVPropertyItemCombo::SVPropertyItemCombo(LPCTSTR lpszName, LPCTSTR lpszItems, int nItem, DWORD dwStyle/*=CBS_DROPDOWNLIST*/)
	: SVPropertyItem(lpszName)
{
	SetData(_T(""), dwStyle);
	SetOptions(lpszItems, nItem);
}


SVPropertyItemCombo::SVPropertyItemCombo(LPCTSTR lpszName, CStringArray& Strings, int nItem, DWORD dwStyle/*=CBS_DROPDOWNLIST*/)
	: SVPropertyItem(lpszName)
{
	SetData(_T(""), dwStyle);
	SetOptions(Strings, nItem);
}


SVPropertyItemCombo::SVPropertyItemCombo(LPCTSTR lpszName, LPCTSTR lpszItems, LPCTSTR lpszText, DWORD dwStyle/*=CBS_DROPDOWN*/)
	: SVPropertyItem(lpszName)
{
	SetData(lpszText, dwStyle);
	SetOptions(lpszItems);
	m_nItem = GetDataIndex();
}


SVPropertyItemCombo::SVPropertyItemCombo(LPCTSTR lpszName, CStringArray& Strings, LPCTSTR lpszText, DWORD dwStyle/*=CBS_DROPDOWN*/)
	: SVPropertyItem(lpszName)
{
	SetData(lpszText, dwStyle);
	SetOptions(Strings);
	m_nItem = GetDataIndex();
}


SVPropertyItemCombo::~SVPropertyItemCombo()
{
}


void SVPropertyItemCombo::SetData(LPCTSTR lpszText, DWORD dwStyle/*=CBS_DROPDOWNLIST*/)
{
	m_strText = lpszText;
	m_dwStyle = dwStyle;
	m_nItem   = -1;
}


void SVPropertyItemCombo::Draw(CDC* pDC, CRect& rect)
{
/*
	// Cell selected?
	// Get the size of the scroll box
	CSize sizeScroll(	GetSystemMetrics(SM_CXVSCROLL), 
										GetSystemMetrics(SM_CYHSCROLL));
	
	// enough room to draw?
	if (sizeScroll.cy < rect.Width() && sizeScroll.cy < rect.Height())
	{
		// Draw control at RHS of cell
		CRect ScrollRect  = rect;
		ScrollRect.left   = rect.right - sizeScroll.cx;
		ScrollRect.bottom = rect.top + sizeScroll.cy;
		
		// Do the draw 
		pDC->DrawFrameControl(ScrollRect, DFC_SCROLL, DFCS_SCROLLDOWN);
		
		// Adjust the remaining space in the cell
		rect.right = ScrollRect.left;
	}
*/	
	pDC->DrawText(m_strText, -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}


BOOL SVPropertyItemCombo::Edit(CWnd* pParent, int nItem, CRect rect, CPoint point, UINT nID, UINT nChar, BOOL bExitOnArrows/*=TRUE*/)
{
	m_bEditing = TRUE;
    
	if (m_nItem > -1 && m_nItem < m_Strings.GetSize())
		m_strText = m_Strings.GetAt(m_nItem);

	// CInPlaceList auto-deletes itself
	m_pEditWnd = new SVInPlaceList(pParent,
																rect,
																m_dwStyle,
																nID,
																nItem, 0, 
																m_Strings,
																m_strText,
																nChar,
																FALSE);
		
	return m_pEditWnd ? TRUE : FALSE;
}


void SVPropertyItemCombo::EndEdit()
{
	if (m_pEditWnd)
		((SVInPlaceList*)m_pEditWnd)->EndEdit();
}


void SVPropertyItemCombo::OnEndEdit(LPLVITEM plvItem)
{
	m_strText  = plvItem->pszText;
	m_nItem    = ((CComboBox*)m_pEditWnd)->FindStringExact(-1, m_strText);
	m_bEditing = FALSE;
	m_pEditWnd = NULL;
}

// For setting the strings that will be displayed in the drop list
void SVPropertyItemCombo::SetOptions(CStringArray& ar, int nItem/*=-1*/)
{ 
	m_Strings.RemoveAll();
	for (int i = 0; i < ar.GetSize(); i++)
		m_Strings.Add(ar[i]);

	m_nItem = nItem;
	if (m_nItem > -1 && m_nItem < m_Strings.GetSize())
		m_strText = m_Strings.GetAt(m_nItem);
}


void SVPropertyItemCombo::SetOptions(CString str, int nItem/*=-1*/)
{
	m_Strings.RemoveAll();

	CString strWork = str;
	CString strTemp;
	int			pos = 0;

	// while the string is not empty
	while (!strWork.IsEmpty())
	{
		// find the position of the next delimiter
		pos = strWork.Find(_T('|'));
		// if the delimiter is found
		if (pos >= 0)
		{
			// store the field
			strTemp = strWork.Left(pos);
			// delete the field and the found delimiter
			strWork = strWork.Mid(pos+1);
		}
		// otherwise if a delimiter isn't found
		else
		{
			// the rest of the string is a field
			strTemp = strWork;
			// and make the string empty
			strWork.Empty();
		}
		// add the field to the CStringArray
		m_Strings.Add(strTemp);
	}

	m_nItem = nItem;
	if (m_nItem > -1 && m_nItem < m_Strings.GetSize())
		m_strText = m_Strings.GetAt(m_nItem);
}


BOOL SVPropertyItemCombo::HitTest(CRect rect, CPoint point)
{
	CSize sizeScroll(	GetSystemMetrics(SM_CXVSCROLL), 
		GetSystemMetrics(SM_CYHSCROLL));
	
	// enough room to draw?
	if (sizeScroll.cy < rect.Width() && sizeScroll.cy < rect.Height())
	{
		CRect ScrollRect = rect;
		ScrollRect.left   = rect.right - sizeScroll.cx;
		ScrollRect.bottom = rect.top + sizeScroll.cy;
		
		return ScrollRect.PtInRect(point);
	}
	return FALSE;
}


void SVPropertyItemCombo::Serialize(CArchive& ar) 
{
	CString	str;
	__int32	size;

	SVPropertyItem::Serialize(ar);
	if (ar.IsStoring())
	{	// storing code
		ar << m_strText;
		ar << m_dwStyle;
		size = m_Strings.GetSize();
		ar << size;
		for (__int32 i=0; i<size; i++)
		{
			str = m_Strings.GetAt(i);
			ar << str;
		}
	}
	else
	{	// loading code
		ar >> m_strText;
		ar >> m_dwStyle;
		ar >> size;
		m_Strings.RemoveAll();
		for (__int32 i=0; i<size; i++)
		{
			ar >> str;
			m_Strings.Add(str);
		}
	}
}


int SVPropertyItemCombo::GetDataIndex(void) const
{
	int	index = -1;

	INT_PTR x = m_Strings.GetSize();
	for (INT_PTR i=0; i<x; i++)
	{
		if (m_Strings.GetAt(i) == m_strText)
		{
			index = static_cast<int>(i);
			break;
		}
	}

	return index;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVPropertyItemCombo.cpp_v  $
 * 
 *    Rev 1.2   23 Jan 2014 07:37:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed type used for size to __int32 in serialize.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 10:02:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 13:46:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Mar 2001 13:51:18   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  116
 * SCR Title:  Enhancements to SVListCtrl class and SVLibrary.Lib
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Corrections made for portability and better utilization by SVFocus.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Dec 2000 13:24:10   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Redefined SVPropertyItem class and Added methods to all inherited classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Dec 2000 11:57:30   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Split out several classes into their own files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Dec 2000 09:27:36   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   SVPropertyListBox and associated item classes.
 * SVPropertyItem
 * SVPropertyItemCombo
 * SVPropertyItemEdit
 * SVPropertyItemFile
 * SVPropertyItemFont
 * 
 * Initial Release
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/