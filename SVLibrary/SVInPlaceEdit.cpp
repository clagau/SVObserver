// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemEdit.cpp
// * .File Name       : $Workfile:   SVInPlaceEdit.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   09 Dec 2013 11:37:54  $
// ******************************************************************************

#include "stdafx.h"
#include "SVInPlaceEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVInPlaceEdit::SVInPlaceEdit(CWnd* pParent,
														 CRect& rect,
														 DWORD dwStyle,
														 UINT nID,
														 int nRow, int nCol,
														 CString sInitText,
														 UINT nFirstChar,
														 BOOL bExitOnArrows/*=TRUE*/)
{
	m_sInitText     = sInitText;
	m_nRow          = nRow;
	m_nCol          = nCol;
	m_nLastChar     = 0; 
	m_bExitOnArrows = bExitOnArrows;
    
	DWORD dwEditStyle = WS_CHILD | WS_VISIBLE | dwStyle;
	if (!Create(dwEditStyle, rect, pParent, nID))
		return;
    
	if (dwEditStyle & ES_NUMBER)
	{
		m_bEditNumber = TRUE;
		ModifyStyle(ES_NUMBER, 0);
	}
	else
		m_bEditNumber = FALSE;
	
	SetFont(pParent->GetFont());
	SetWindowText(sInitText);
	SetFocus();
    
	switch (nFirstChar)
	{
		case VK_RETURN:
			SetSel((int)_tcslen(m_sInitText), -1);
			return;
		case VK_BACK:
			SetSel((int)_tcslen(m_sInitText), -1);
			break;
		case VK_TAB:
		case VK_DOWN: 
		case VK_UP:   
		case VK_RIGHT:
		case VK_LEFT:  
		case VK_NEXT:  
		case VK_PRIOR: 
		case VK_HOME:
		case VK_SPACE:
		case VK_END:
			SetSel(0,-1);
			return;
		default:
			SetSel(0,-1);
	}
    
	SendMessage(WM_CHAR, nFirstChar);
}


SVInPlaceEdit::~SVInPlaceEdit()
{
}


BEGIN_MESSAGE_MAP(SVInPlaceEdit, CEdit)
	//{{AFX_MSG_MAP(SVInPlaceEdit)
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL SVInPlaceEdit::PreTranslateMessage(MSG* pMsg) 
{
	// Make sure that the keystrokes continue to the appropriate handlers
	if( pMsg->message == WM_KEYDOWN )
	{
		UINT nChar = static_cast<UINT>(pMsg->wParam);

		if ((nChar == VK_PRIOR || nChar == VK_NEXT ||
				nChar == VK_DOWN  || nChar == VK_UP   /*|| 
				// Do not capture right and left messages, let the base edit use these.
				nChar == VK_RIGHT || nChar == VK_LEFT*/) &&
				(m_bExitOnArrows || GetKeyState(VK_CONTROL) < 0))
		{
			m_nLastChar = nChar;
			GetParent()->SetFocus();
			return TRUE;
		}

		if (nChar == VK_ESCAPE || nChar == VK_RETURN || nChar == VK_TAB)
		{
			if (nChar == VK_ESCAPE)
				SetWindowText(m_sInitText);
			m_nLastChar = static_cast<UINT>(pMsg->wParam);
			GetParent()->SetFocus();
			return TRUE;
		}
	}
	
	// Catch the Alt key so we don't choke if focus is going to an owner drawn button
	if (pMsg->message == WM_SYSCHAR)
		return TRUE;

	return CWnd::PreTranslateMessage(pMsg);
}


void SVInPlaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (m_bEditNumber)
	{
		if ((nChar >= _T('0') && nChar <= _T('9')) ||
				(!m_bSign && (nChar == _T('+') || nChar == _T('-'))) ||
				(!m_bDecimal && nChar == _T('.')) ||
				nChar == VK_BACK)
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			return;
		}
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}


void SVInPlaceEdit::PostNcDestroy() 
{
	CEdit::PostNcDestroy();
    
	delete this;	
}


void SVInPlaceEdit::EndEdit()
{
	static BOOL bAlreadyEnding = FALSE;
	CString str;


	if (bAlreadyEnding)
		return;

	bAlreadyEnding = TRUE;
	GetWindowText(str);

	// Send Notification to parent
	LV_DISPINFO dispinfo;
	
	dispinfo.hdr.hwndFrom    = GetSafeHwnd();
	dispinfo.hdr.idFrom      = GetDlgCtrlID();
	dispinfo.hdr.code        = LVN_ENDLABELEDIT;
	
	dispinfo.item.mask       = LVIF_TEXT | LVIF_PARAM;
	dispinfo.item.iItem      = m_nRow;
	dispinfo.item.iSubItem   = m_nCol;
	dispinfo.item.pszText    = LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	dispinfo.item.lParam     = (LPARAM)m_nLastChar; 

	CWnd* pOwner = GetOwner();
	if (pOwner)
		pOwner->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo );

	// Close this window (PostNcDestroy will delete this)
	if (IsWindow(GetSafeHwnd()))
		SendMessage(WM_CLOSE, 0, 0);
	bAlreadyEnding = FALSE;
}


void SVInPlaceEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
	EndEdit();	
}


void SVInPlaceEdit::OnUpdate() 
{
	if (m_bEditNumber)
	{
		CString	str;
		GetWindowText(str);

		m_bSign    = str.FindOneOf(_T("+-")) != -1 ? TRUE : FALSE;
		m_bSign    = !str.IsEmpty();
		m_bDecimal = str.FindOneOf(_T("."))  != -1 ? TRUE : FALSE;
	}
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVInPlaceEdit.cpp_v  $
 * 
 *    Rev 1.2   09 Dec 2013 11:37:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  829
 * SCR Title:  Fix tool adjust dialogs to validate all math equations before exiting.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modify edit to allow use of right and left arrows to allow navigation while inside the edit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 09:57:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 13:36:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Mar 2001 13:40:52   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  116
 * SCR Title:  Enhancements to SVListCtrl class and SVLibrary.Lib
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Replacement classes for the following:
 * SVFEEdit h/cpp
 * SVLVEdit h/cpp
 * SVComboEdit h/cpp
 * Modifications make the preceeding files obsolete.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Dec 2000 13:24:06   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Redefined SVPropertyItem class and Added methods to all inherited classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Dec 2000 11:54:58   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  75
 * SCR Title:  New Library Classes
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Split existing classes into their own file
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