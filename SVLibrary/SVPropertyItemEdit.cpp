// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemEdit.cpp
// * .File Name       : $Workfile:   SVPropertyItemEdit.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:46:46  $
// ******************************************************************************

#include "stdafx.h"
#include "SVPropertyItemEdit.h"
#include "SVInPlaceEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(SVPropertyItemEdit, SVPropertyItem, 1)

SVPropertyItemEdit::SVPropertyItemEdit()
	: SVPropertyItem()
{
	SetData(_T(""));
}


SVPropertyItemEdit::SVPropertyItemEdit(LPCTSTR lpszName, LPCTSTR lpszText, DWORD dwStyle/*=0*/)
	: SVPropertyItem(lpszName)
{
	SetData(lpszText, dwStyle);
}

SVPropertyItemEdit::~SVPropertyItemEdit()
{
}

void SVPropertyItemEdit::SetData(LPCTSTR lpszText, DWORD dwStyle/*=0*/)
{
	m_strText = lpszText;
	m_dwStyle = dwStyle;
}

BOOL SVPropertyItemEdit::Edit(CWnd* pParent, int nItem, CRect rect, CPoint point, UINT nID, UINT nChar, BOOL bExitOnArrows/*=TRUE*/)
{
	m_bEditing = TRUE;
	m_pEditWnd = new SVInPlaceEdit(pParent, rect, m_dwStyle, nID, nItem, 0, m_strText, nChar, bExitOnArrows);
   
	return m_pEditWnd ? TRUE : FALSE;
}

void SVPropertyItemEdit::EndEdit()
{
	if (m_pEditWnd)
		((SVInPlaceEdit*)m_pEditWnd)->EndEdit();
}


void SVPropertyItemEdit::OnEndEdit(LPLVITEM plvItem)
{
	m_strText  = plvItem->pszText;
	m_bEditing = FALSE;
	m_pEditWnd = NULL;
}


void SVPropertyItemEdit::Serialize(CArchive& ar) 
{
	SVPropertyItem::Serialize(ar);
	if (ar.IsStoring())
	{	// storing code
		ar << m_strText;
		ar << m_dwStyle;
	}
	else
	{	// loading code
		ar >> m_strText;
		ar >> m_dwStyle;
	}
}


void SVPropertyItemEdit::Draw(CDC *pDC, CRect &rect)
{
	DWORD dwStyle = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;

	if (m_dwStyle & ES_RIGHT)
		dwStyle |= DT_RIGHT;
	else if (m_dwStyle & ES_CENTER)
		dwStyle |= DT_CENTER;
	else
		dwStyle |= DT_LEFT;

	pDC->DrawText(m_strText, -1, &rect, dwStyle);
}


BOOL SVPropertyItemEdit::ModifyStyle(DWORD dwRemove, DWORD dwAdd)
{
	DWORD dwNewStyle = (m_dwStyle & ~dwRemove) | dwAdd;
	m_dwStyle        = dwNewStyle;

	return TRUE;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVPropertyItemEdit.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:46:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Mar 2001 13:52:52   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  116
 * SCR Title:  Enhancements to SVListCtrl class and SVLibrary.Lib
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Corrections made for portability and better utilization by SVFocus.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Dec 2000 13:24:12   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Redefined SVPropertyItem class and Added methods to all inherited classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Dec 2000 11:57:32   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Split out several classes into their own files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Dec 2000 09:27:38   cSchmittinger
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