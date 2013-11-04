// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemFile.cpp
// * .File Name       : $Workfile:   SVPropertyItemFile.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:46:54  $
// ******************************************************************************

#include "stdafx.h"
#include "SVPropertyItemFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR path[_MAX_PATH];

IMPLEMENT_SERIAL(SVPropertyItemFile, SVPropertyItem, 1)

SVPropertyItemFile::SVPropertyItemFile()
	: SVPropertyItem()
{
	m_strPath = _T("");
	m_dwStyle = FES_FOLDER;
}


SVPropertyItemFile::SVPropertyItemFile(LPCTSTR lpszName, LPCTSTR lpszText, DWORD dwStyle/*=FES_FOLDER*/, LPCTSTR lpszFilter/*=NULL*/)
	: SVPropertyItem(lpszName)
{
	m_strPath = lpszText;
	m_dwStyle = dwStyle;
	SetFilter(lpszFilter);
}

void SVPropertyItemFile::SetData(LPCTSTR lpszText, DWORD dwStyle/*=FES_FOLDER*/, LPCTSTR lpszFilter/*=NULL*/)
{
	m_strPath   = lpszText;
	m_dwStyle   = dwStyle;
	m_strFilter = lpszFilter;
}

BOOL SVPropertyItemFile::Edit(CWnd* pParent, int iItem, CRect rect, CPoint point, UINT nID, UINT nChar, BOOL bExitOnArrows/*=TRUE*/)
{
	m_bEditing = TRUE;
	m_pEditWnd = new SVFileEditCtrl(pParent, rect, m_dwStyle, nID, iItem, 0, m_strPath, nChar, bExitOnArrows);

	if (m_pEditWnd)
	{
		if (!(m_dwStyle & FES_FOLDER))
		{
			OPENFILENAME* pofn = ((SVFileEditCtrl*)m_pEditWnd)->GetOpenFileName();
			pofn->lpstrFilter = m_strFilter;
		}
	}
	
	return m_pEditWnd ? TRUE : FALSE;
}

void SVPropertyItemFile::EndEdit(void)
{
	if (m_pEditWnd)
		((SVFileEditCtrl*)m_pEditWnd)->EndEdit();
}


void SVPropertyItemFile::OnEndEdit(LPLVITEM plvItem)
{
	m_strPath  = plvItem->pszText;
	m_bEditing = FALSE;
	m_pEditWnd = NULL;
}


void SVPropertyItemFile::SetFilter(LPCTSTR lpszFilter)
{
	m_strFilter = lpszFilter;
	
	if (!m_strFilter.IsEmpty())
	{
		int	pos;
		
		while ((pos = m_strFilter.ReverseFind('|')) != -1)
			m_strFilter.SetAt(pos, '\0');
	}
}

void SVPropertyItemFile::Draw(CDC *pDC, CRect &rect)
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
		CRect button  = rect;
		button.left   = rect.right - sizeScroll.cx;
		button.bottom = rect.top + sizeScroll.cy;
		
		// Do the draw 
		// create a CBrush for the background colour of the button
		CBrush theBrush(GetSysColor(COLOR_3DFACE));
		CBrush *poldBrush = pDC->SelectObject(&theBrush);
		CPen *poldPen, thePen;
		
		// draw the outside border
		thePen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DFACE));
		poldPen = pDC->SelectObject(&thePen);
		pDC->Rectangle(&button);
		pDC->SelectObject(poldPen);
		thePen.DeleteObject();
		
		// draw the top and left sides
		thePen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
		poldPen = pDC->SelectObject(&thePen);
		pDC->MoveTo(button.left + 1, button.bottom - 1);
		pDC->LineTo(button.left + 1, button.top + 1);
		pDC->LineTo(button.right - 2, button.top + 1);
		pDC->SelectObject(poldPen);
		thePen.DeleteObject();
		
		// draw the bottom and right sides (top edge)
		thePen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		poldPen = pDC->SelectObject(&thePen);
		pDC->LineTo(button.right - 2, button.bottom - 2);
		pDC->LineTo(button.left + 1, button.bottom - 2);
		pDC->SelectObject(poldPen);
		thePen.DeleteObject();
		
		// draw the bottom and right sides (bottom edge)
		thePen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DDKSHADOW));
		poldPen = pDC->SelectObject(&thePen);
		pDC->MoveTo(button.right - 1, button.top);
		pDC->LineTo(button.right - 1, button.bottom - 1);
		pDC->LineTo(button.left, button.bottom - 1);
		
		// draw the dots
		int x = button.left + (button.Width() / 2 - 3);
		int y = button.top + (button.Height() / 2 - 1);
		COLORREF dots = GetSysColor(COLOR_BTNTEXT);
		pDC->SetPixel(x, y, dots);
		x += 3;
		pDC->SetPixel(x, y, dots);
		x += 3;
		pDC->SetPixel(x, y, dots);
		pDC->SelectObject(poldBrush);
		pDC->SelectObject(poldPen);
		
		thePen.DeleteObject();
		theBrush.DeleteObject();
		
		// Adjust the remaining space in the cell
		rect.right = button.left;
	}
*/	
	// drop through and complete the cell drawing using the base class' method
	pDC->DrawText(m_strPath, -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
}


BOOL SVPropertyItemFile::HitTest(CRect rect, CPoint point)
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


void SVPropertyItemFile::Serialize(CArchive& ar) 
{
	SVPropertyItem::Serialize(ar);
	if (ar.IsStoring())
	{	// storing code
		ar << m_strPath;
		ar << m_dwStyle;
		ar << m_strFilter;
	}
	else
	{	// loading code
		ar >> m_strPath;
		ar >> m_dwStyle;
		ar >> m_strFilter;
	}
}


LPCTSTR SVPropertyItemFile::GetPathName(void)
{
	_tcsncpy(path, m_strPath, _MAX_PATH);
	return path;
}


LPCTSTR SVPropertyItemFile::GetFolder(void)
{
	TCHAR	drive[_MAX_DRIVE];
	TCHAR	dir[_MAX_DIR];
	TCHAR	fname[_MAX_FNAME];
	TCHAR	ext[_MAX_EXT];

	CString strPath = m_strPath;
	if (m_dwStyle & FES_FOLDER)
		if (strPath.GetAt(strPath.GetLength()-1) != _T('\\'))
			strPath += _T('\\');

	_tsplitpath(strPath, drive, dir, fname, ext);
	_tmakepath(path, drive, dir, NULL, NULL);
	return path;
}


LPCTSTR SVPropertyItemFile::GetFilename(void)
{
	TCHAR	drive[_MAX_DRIVE];
	TCHAR	dir[_MAX_DIR];
	TCHAR	fname[_MAX_FNAME];
	TCHAR	ext[_MAX_EXT];

	if (m_dwStyle & FES_FOLDER)
		return NULL;

	_tsplitpath(m_strPath, drive, dir, fname, ext);
	_tmakepath(path, NULL, NULL, fname, ext);
	return path;
}


LPCTSTR SVPropertyItemFile::GetExtension(void)
{
	TCHAR	drive[_MAX_DRIVE];
	TCHAR	dir[_MAX_DIR];
	TCHAR	fname[_MAX_FNAME];
	TCHAR	ext[_MAX_EXT];

	if (m_dwStyle & FES_FOLDER)
		return NULL;

	_tsplitpath(m_strPath, drive, dir, fname, ext);
	_tmakepath(path, NULL, NULL, NULL, ext);
	return path;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVPropertyItemFile.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:46:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3.1.0   22 Oct 2010 08:10:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Mar 2001 13:56:56   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  116
 * SCR Title:  Enhancements to SVListCtrl class and SVLibrary.Lib
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Corrections made for portability and better utilization by SVFocus. Also used by the SVReport project for SmithKline Beecham.
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
 *    Rev 1.1   06 Dec 2000 11:57:34   mike
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
