// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemFile.cpp
// * .File Name       : $Workfile:   PropTreeItemFile.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   18 Sep 2014 13:33:28  $
// ******************************************************************************

// PropertyItemFile.cpp : implementation file
//
//  Copyright (C) 1998-2001 Scott Ramsay
//	sramsay@gonavi.com
//	http://www.gonavi.com
//
//  This material is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
// 
//  Permission to use or copy this software for any purpose is hereby granted 
//  without fee, provided the above notices are retained on all copies.
//  Permission to modify the code and to distribute modified code is granted,
//  provided the above notices are retained, and a notice that the code was
//  modified is included with the above copyright notice.
// 
//	If you use this code, drop me an email.  I'd like to know if you find the code
//	useful.

#include "stdafx.h"
#include "proptree.h"
#include "PropTreeItemFile.h"
#include "SVMFCControls/SVDlgFolder.h"
#include "SVMFCControls/SVFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Button states

enum
{
	BTN_UP        = 0,
	BTN_DOWN      = 1,
	BTN_DISABLED  = 2,
};

void SVRPropertyItemFile::Initialize()
{
	// Initialize all variables
	m_bButtonLeft      = false;
	m_bCreatingControl = true;
	m_bFindFolder      = false;
	m_bMouseCaptured   = false;
	m_bTextChanged     = true;
	m_bTrailingSlash   = false;
	m_nButtonState     = BTN_UP;
	m_rcButtonRect.SetRectEmpty();
}

void SVRPropertyItemFile::ResetControl()
{
	m_bCreatingControl = true;
}

/////////////////////////////////////////////////////////////////////////////
// SVRPropertyItemFile

SVRPropertyItemFile::SVRPropertyItemFile(bool bFullAccess, DWORD dwFlags, LPCTSTR lpszVal, LPCTSTR sInitialDir, BOOL bSetDir) 
: m_sAttribute(_T(""))
, m_sFilter(_T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"))
, m_bFullAccess(bFullAccess)
{
    m_sInitialDir = sInitialDir;
    m_bInitialDirSet = bSetDir;
    Initialize();
    SetItemType(dwFlags,lpszVal);
}

SVRPropertyItemFile::~SVRPropertyItemFile()
{
	ResetControl();
}

BEGIN_MESSAGE_MAP(SVRPropertyItemFile, CEdit)
	//{{AFX_MSG_MAP(SVRPropertyItemFile)
	ON_CONTROL_REFLECT_EX(EN_CHANGE, OnChange)
	ON_WM_DESTROY()
	ON_WM_ENABLE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCPAINT()
	ON_WM_SIZE()
	ON_WM_KILLFOCUS()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVRPropertyItemFile message handlers

void SVRPropertyItemFile::DrawAttribute(CDC* pDC, const RECT& rc)
{
	ASSERT(m_pProp!=NULL);
	CFont*	l_pOldFont;

	l_pOldFont = pDC->SelectObject(IsReadOnly() ? m_pProp->GetNormalFont() : m_pProp->GetBoldFont());
	pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkMode(TRANSPARENT);

	CRect r = rc;
	pDC->DrawText(m_sAttribute, r, DT_SINGLELINE|DT_VCENTER|DT_PATH_ELLIPSIS);
	
//JAB110508	
	pDC->SelectObject(l_pOldFont);
}

void SVRPropertyItemFile::OnMove()
{
	if (IsWindow(m_hWnd))
	{
		SetWindowPos(NULL, m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height(), SWP_NOZORDER|SWP_NOACTIVATE);
	}
}

void SVRPropertyItemFile::OnRefresh()
{
	if (IsWindow(m_hWnd))
	{
		SetWindowText(m_sAttribute);
	}
}

void SVRPropertyItemFile::OnCommit()
{
	// hide edit control
	ShowWindow(SW_HIDE);

	GetWindowText(m_sAttribute);
}

void SVRPropertyItemFile::OnActivate()
{
	CRect r = m_rc;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|WS_TABSTOP;

	if (!IsWindow(m_hWnd))
	{
		if (CreateEx(0, _T("EDIT"), _T(""), dwStyle, r, m_pProp->GetCtrlParent(), GetCtrlID()))
		{
			// Force a call to SVRPropertyItemFile::OnNcCalcSize() to calculate button size
			SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
		SendMessage(WM_SETFONT, (WPARAM)m_pProp->GetNormalFont()->m_hObject);
	}

	OnRefresh();
	SetSel(0, -1);

	SetFocus();
	SetWindowPos(NULL, r.left, r.top, r.Width(), r.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);
	SendMessage(EM_LIMITTEXT, _MAX_PATH, 0);
}

void SVRPropertyItemFile::ButtonClicked()
{
	bool bResult = false;
	if (m_bFindFolder)
	{
		bResult = SVRBrowseForFolder();
	}
	else
	{
		bResult = SVROpenFile();
	}
	if (bResult)
	{
		m_bCommitOnce = false;
		CommitChanges();
	}
}

void SVRPropertyItemFile::DrawButton(int nButtonState)
{
	if (!IsWindow(m_hWnd) || !IsWindowVisible())
		return;
	
	// if the control is disabled, ensure the button is drawn disabled
	if (GetStyle() & WS_DISABLED)
		nButtonState = BTN_DISABLED;
	
	// Draw the button in the specified state (Up, Down, or Disabled)
	CWindowDC DC(this);		// get the DC for drawing
	
	CBrush theBrush(GetSysColor(COLOR_3DFACE));		// the colour of the button background
	CBrush *pOldBrush = DC.SelectObject(&theBrush);
	
	if (nButtonState == BTN_DOWN)	// Draw button as down
	{
		// draw the border
		CPen thePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		CPen *pOldPen = DC.SelectObject(&thePen);
		DC.Rectangle(&m_rcButtonRect);
		DC.SelectObject(pOldPen);
		thePen.DeleteObject();
		
		// draw the dots
		DrawDots(&DC, GetSysColor(COLOR_BTNTEXT), 1);
	}
	else	// draw button as up
	{
		CPen thePen(PS_SOLID, 1, GetSysColor(COLOR_3DFACE));
		CPen *pOldPen = DC.SelectObject(&thePen);
		DC.Rectangle(&m_rcButtonRect);
		DC.SelectObject(pOldPen);
		thePen.DeleteObject();
		
		// draw the border
		DC.DrawEdge(&m_rcButtonRect, EDGE_RAISED, BF_RECT);

//-		JAB110508 - The following statement obviously does not do anything 
//-		with whatever font object DrawEdge assigned.
		DC.SelectObject ((CFont*) NULL);

		// draw the dots
		if (nButtonState == BTN_DISABLED)
		{
			DrawDots(&DC, GetSysColor(COLOR_3DHILIGHT), 1);
			DrawDots(&DC, GetSysColor(COLOR_GRAYTEXT));
		}
		else if (nButtonState == BTN_UP)
		{
//-			JAB110508 - It appears that the following function assigns a 
//-			Font to the DC.  This probably doesn't really matter, but 
//-			BoundsChecker doesn't like it.
			DrawDots(&DC, GetSysColor(COLOR_BTNTEXT));

			DC.SelectObject ((CFont*) NULL);
		}
		else
			ASSERT(FALSE);	// Invalid nButtonState
	}
	DC.SelectObject(pOldBrush);
	theBrush.DeleteObject();
	
	// update m_nButtonState
	m_nButtonState = nButtonState;
}

void SVRPropertyItemFile::DrawDots(CDC *pDC, COLORREF CR, int nOffset /* = 0 */)
{
	// draw the dots on the button
	int width = m_rcButtonRect.Width();			// width of the button
	div_t divt = div(width, 4);
	int delta = divt.quot;						// space between dots
	int left = m_rcButtonRect.left + width / 2 - delta - (divt.rem ? 0 : 1); // left side of first dot
	width = width / 10;							// width and height of one dot
	int top = m_rcButtonRect.Height() / 2 - width / 2 + 1;	// top of dots
	left += nOffset;							// draw dots shifted? ( for button pressed )
	top += nOffset;
	// draw the dots
	if (width < 2)
	{
		pDC->SetPixel(left, top, CR);
		left += delta;
		pDC->SetPixel(left, top, CR);
		left += delta;
		pDC->SetPixel(left, top, CR);
	}
	else
	{
		CPen thePen(PS_SOLID, 1, CR);			// set the dot colour
		CPen *pOldPen = pDC->SelectObject(&thePen);
		CBrush theBrush(CR);
		CBrush *pOldBrush = pDC->SelectObject(&theBrush);
		pDC->Ellipse(left, top, left + width, top + width);
		left += delta;
		pDC->Ellipse(left, top, left + width, top + width);
		left += delta;
		pDC->Ellipse(left, top, left + width, top + width);
		pDC->SelectObject(pOldBrush);			// reset the DC
		theBrush.DeleteObject();
		pDC->SelectObject(pOldPen);
		thePen.DeleteObject();
	}
}

bool SVRPropertyItemFile::SVRBrowseForFolder()
{
	bool bReturnValue = false;
	
	CString szPath = m_sAttribute;
	// remove the trailing slash if present (SHBrowseForFolder() does not like it)
	int len = szPath.GetLength() - 1;
	if (len != 2 && szPath[len] == _T('\\'))
		szPath.Delete(len);

	SvMc::SVDlgFolder dlg(m_bFullAccess, szPath);
	dlg.InitDlgFolder(_T("OK"), _T("Select Folder"));

	INT_PTR rc = dlg.DoModal();
	if (IDOK == rc)
	{
		CString path = dlg.GetPathName();
		if (m_bTrailingSlash)				// add a trailing slash if it is not already there
		{
			int len = path.GetLength();
			if (path[len - 1] != _T('\\'))
				path += _T("\\");
		}
		SetWindowText(path);		// update edit control
		bReturnValue = true;
	}
	
	m_pProp->SetFocusedItem(this);
	SetFocus();									// ensure focus returns to this control

	return bReturnValue;
}

bool SVRPropertyItemFile::SVROpenFile()
{
	bool bReturnValue = false;
	bool bDirectory   = true;			// assume user of this class has set the initial directory

	SvMc::SVFileDialog dlg(true, m_bFullAccess, nullptr, nullptr, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, m_sFilter, this);

	dlg.m_ofn.lpstrTitle = _T("Select File");
	TCHAR lpstrDirectory[_MAX_PATH] = _T("");
	if (dlg.m_ofn.lpstrInitialDir == nullptr)
	{
		bDirectory = false;				// directory in edit control
		_tcscpy(lpstrDirectory, GetPathName());
		dlg.m_ofn.lpstrInitialDir = lpstrDirectory;
	}
    if (m_bInitialDirSet)
    {
        dlg.m_ofn.lpstrInitialDir = m_sInitialDir;
    }

	if (IDOK == dlg.DoModal())			// Start the FileDialog
	{									// user clicked OK, enter files selected into edit control
		CString szFileSeperator;
		szFileSeperator = _T(";");
		ASSERT(szFileSeperator.GetLength() == 1);	// must be one character only
		szFileSeperator += _T(" ");
		CString szPath = dlg.GetPathName();
		SetWindowText(szPath);
		bReturnValue = true;
	}
	
	m_pProp->SetFocusedItem(this);
	SetFocus();									// ensure focus returns to this control
	
	return bReturnValue;
}

CString SVRPropertyItemFile::GetPathName(void)
{
	TCHAR lpstrReturnString[_MAX_PATH];
	CString str = m_sAttribute;

	int pos = str.ReverseFind(_T('\\'));
	if (pos > 1)
	{
		if (str.GetAt(pos-1) == _T(':'))
			str = str.Left(pos+1);
		else
			str = str.Left(pos);
	}
	else
		str.Empty();

	_tfullpath(lpstrReturnString, str, _MAX_PATH);	// get absolute path from any relative paths
	return (CString)lpstrReturnString;
}

BOOL SVRPropertyItemFile::OnChange() 
{
	m_bTextChanged = true;
	return FALSE;		// Allow the parent window to also handle this message
}

void SVRPropertyItemFile::OnDestroy() 
{
}

void SVRPropertyItemFile::OnEnable(BOOL bEnable) 
{
	// enables/disables the control
	CEdit::OnEnable(bEnable);
	DrawButton(bEnable ? BTN_UP : BTN_DISABLED);
}

void SVRPropertyItemFile::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if (nChar == VK_ESCAPE)
			OnRefresh();
		CommitChanges();
		return;
	}
	
	// check for CTRL + 'period' keystroke, if found, simulate a mouse click on the button
	if ((nChar == 0xBE || nChar == VK_DECIMAL) && GetKeyState(VK_CONTROL) < 0)
	{
		ButtonClicked();
		return;
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void SVRPropertyItemFile::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CEdit::OnLButtonUp(nFlags, point);
	
	// Release the mouse capture and draw the button as normal. If the
	// cursor is over the button, simulate a click by carrying
	// out the required action.
	if (m_bMouseCaptured)
	{
		ReleaseCapture();
		m_bMouseCaptured = FALSE;
		if (m_nButtonState != BTN_UP)
			DrawButton(BTN_UP);
		ClientToScreen(&point);
		if (ScreenPointInButtonRect(point))
			ButtonClicked();
	}
}

void SVRPropertyItemFile::OnMouseMove(UINT nFlags, CPoint point) 
{
	CEdit::OnMouseMove(nFlags, point);
	
	if (m_bMouseCaptured)
	{
		ClientToScreen(&point);
		if (ScreenPointInButtonRect(point))
		{
			if (m_nButtonState != BTN_DOWN)
				DrawButton(BTN_DOWN);
		}
		else if (m_nButtonState != BTN_UP)
			DrawButton(BTN_UP);
	}
}

void SVRPropertyItemFile::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	// calculate the size of the client area and the button.
	CEdit::OnNcCalcSize(bCalcValidRects, lpncsp);
	// set button area equal to client area of edit control
	m_rcButtonRect = lpncsp->rgrc[0];
	if (m_bButtonLeft)		// draw the button on the left side of the control
	{
		// shrink left side of client area by 80% of the height of client area
		lpncsp->rgrc[0].left +=(lpncsp->rgrc[0].bottom - lpncsp->rgrc[0].top) * 8/10;
		// shrink the button so its right side is at the left side of client area
		m_rcButtonRect.right = lpncsp->rgrc[0].left;
	}
	else					// draw the button on the right side of the control
	{
		// shrink right side of client area by 80% of the height of client area
		lpncsp->rgrc[0].right -=(lpncsp->rgrc[0].bottom - lpncsp->rgrc[0].top) * 8/10;
		// shrink the button so its left side is at the right side of client area
		m_rcButtonRect.left = lpncsp->rgrc[0].right;
	}
	if (bCalcValidRects)
		// convert button coordinates from parent client coordinates to control window coordinates
		m_rcButtonRect.OffsetRect(-lpncsp->rgrc[1].left, -lpncsp->rgrc[1].top);
	m_rcButtonRect.NormalizeRect();
}

LRESULT SVRPropertyItemFile::OnNcHitTest(CPoint point) 
{
	LRESULT where = CEdit::OnNcHitTest(point);
	if (where == HTNOWHERE && ScreenPointInButtonRect(point))
		where = HTBORDER;
	return where;
}

void SVRPropertyItemFile::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	CEdit::OnNcLButtonDown(nHitTest, point);
	
	if (ScreenPointInButtonRect(point))
	{
		// Capture mouse input, set the focus to this control,
		// and draw the button as pressed
		SetCapture();
		m_bMouseCaptured = TRUE;
		SetFocus();
		DrawButton(BTN_DOWN);
	}
}

void SVRPropertyItemFile::OnNcPaint() 
{
	CEdit::OnNcPaint();				// draws the border around the control
	DrawButton(m_nButtonState);	// draw the button in its current state
}

void SVRPropertyItemFile::OnSize(UINT nType, int cx, int cy) 
{
	CEdit::OnSize(nType, cx, cy);
	// Force a recalculation of the button's size and position
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

BOOL SVRPropertyItemFile::ScreenPointInButtonRect(CPoint point)
{
	CRect ControlRect;
	GetWindowRect(&ControlRect);
	// convert point from screen coordinates to window coordinates
	point.Offset(-ControlRect.left, -ControlRect.top);
	return m_rcButtonRect.PtInRect(point);
}

void SVRPropertyItemFile::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
	//if (pNewWnd != NULL && pNewWnd != this) //CWnd::GetParent() == pNewWnd)
	{
		if (m_pProp->GetFocusedItem() == this)
		{
			m_pProp->SetFocusedItem(NULL);
		}
		CommitChanges();
	}
}

UINT SVRPropertyItemFile::OnGetDlgCode() 
{
	return CEdit::OnGetDlgCode() | DLGC_WANTALLKEYS;
}

bool SVRPropertyItemFile::SetItemType(DWORD dwFlags, LPCTSTR sFilter /*=NULL*/)
{
	ResetControl();

	m_bButtonLeft    = (dwFlags & SVR_BUTTONLEFT) ? true : false;
	m_bFindFolder    = (dwFlags & SVR_FOLDER) ? true : false;
	m_bTrailingSlash = (dwFlags & SVR_TRAILINGSLASH) ? true : false;
	m_sFilter        = _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||");

	if (sFilter)
		m_sFilter = sFilter;

	if (m_bFindFolder)
	{
		ASSERT(!(dwFlags & SVR_FILE));			// can not specify SVR_FILE with SVR_FOLDER
	}
	else
	{
		ASSERT(dwFlags & SVR_FILE);			// must specify either SVR_FILE or SVR_FOLDER
	}
	return true;
}

bool SVRPropertyItemFile::GetItemValue(CString& strVal)
{
	strVal = m_sAttribute;
	return true;
}

bool SVRPropertyItemFile::GetItemValue(VARIANT& vtVal)
{
	VariantClear(&vtVal);
    V_VT(&vtVal) = VT_BSTR;
    V_BSTR(&vtVal) = m_sAttribute.AllocSysString();
	return true;
}

bool SVRPropertyItemFile::SetItemValue(LPCTSTR lpszVal)
{
	m_sAttribute = lpszVal;
	return true;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\PropertyTree\PropTreeItemFile.cpp_v  $
 * 
 *    Rev 1.2   18 Sep 2014 13:33:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  944
 * SCR Title:  Fix Security for File and Folder Selection Dialog for 64 Bit
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed SVRFileDilaog.
 * Revised to use SVDialogFolder and SVFileDialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Sep 2013 14:07:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2013 16:42:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   14 Nov 2008 13:46:08   jbrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  641
 * SCR Title:  BoundsChecker results
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SVRPropertyItemFile::DrawAttribute ()
 * Deselected Font (GDI) object when finished with it.
 * 
 * SVRPropertyItemFile::DrawButton ()
 * Deselected Font (GDI) objects when finished with them.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Sep 2008 10:56:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Corrected Focus and Change Notification logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Jun 2007 08:42:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Jun 2007 12:20:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Jul 2003 08:44:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed #define to enum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 15:41:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of the files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/