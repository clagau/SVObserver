// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemFont.cpp
// * .File Name       : $Workfile:   SVPropertyItemFont.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:47:04  $
// ******************************************************************************

#include "stdafx.h"
#include "SVPropertyItemFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVPropertyItemFont

IMPLEMENT_SERIAL(SVPropertyItemFont, SVPropertyItem, 1)


SVPropertyItemFont::SVPropertyItemFont()
	: SVPropertyItem()
{
	m_dwFlags = CF_EFFECTS | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
	m_rgb     = RGB(0, 0, 0);
}

SVPropertyItemFont::SVPropertyItemFont(LPCTSTR lpszName, int nPointSize, LPCTSTR lpszFaceName, CDC* pDC/*=NULL*/)
	: SVPropertyItem(lpszName)
{
	SetData(nPointSize, lpszFaceName, pDC);
	m_dwFlags = CF_EFFECTS | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
}

SVPropertyItemFont::SVPropertyItemFont(LPCTSTR lpszName, const LPLOGFONT lpLogFont)
	: SVPropertyItem(lpszName)
{
	SetData(lpLogFont);
	m_dwFlags = CF_EFFECTS | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
}


void SVPropertyItemFont::SetData(int nPointSize, LPCTSTR lpszFaceName, CDC* pDC/*=NULL*/)
{
	CFont	font;

	font.CreatePointFont(nPointSize*10, lpszFaceName, pDC);
	font.GetLogFont(&m_lf);
	m_rgb = RGB(0, 0, 0);
	m_dwFlags = CF_EFFECTS | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
}


void SVPropertyItemFont::SetData(LPLOGFONT lpLogFont)
{
	m_lf  = *lpLogFont;
	m_rgb = RGB(0, 0, 0);
}


void SVPropertyItemFont::SetData(	int nHeight,
																	int nWidth,
																	int nEscapement,
																	int nOrientation,
																	int nWeight,
																	BYTE bItalic,
																	BYTE bUnderline,
																	BYTE cStrikeOut,
																	BYTE nCharSet,
																	BYTE nOutPrecision,
																	BYTE nClipPrecision,
																	BYTE nQuality,
																	BYTE nPitchAndFamily,
																	LPCTSTR lpszFacename)
{
	CFont	font;
	font.CreateFont(nHeight,
									nWidth,
									nEscapement,
									nOrientation,
									nWeight,
									bItalic,
									bUnderline,
									cStrikeOut,
									nCharSet,
									nOutPrecision,
									nClipPrecision,
									nQuality,
									nPitchAndFamily,
									lpszFacename);
	font.GetLogFont(&m_lf);
	m_rgb = RGB(0, 0, 0);
}


BOOL SVPropertyItemFont::Edit(CWnd* pParent, int iItem, CRect rect, CPoint point, UINT nID, UINT nChar, BOOL bExitOnArrows/*=TRUE*/)
{
	LOGFONT	lf = m_lf;

	CFontDialog FontDlg(&lf, m_dwFlags, NULL, pParent);
	FontDlg.m_cf.rgbColors = m_rgb;
	if (FontDlg.DoModal() == IDOK)
	{
		m_lf = lf;
		m_rgb = FontDlg.GetColor();
		pParent->Invalidate();
	}
	return TRUE;
}


void SVPropertyItemFont::Draw(CDC* pDC, CRect& rect)
{
	CString	strText;
	CFont		font;
	LOGFONT	lf;

	pDC->GetCurrentFont()->GetLogFont(&lf);
	lf.lfWeight    = m_lf.lfWeight;
	lf.lfItalic    = m_lf.lfItalic;
	lf.lfStrikeOut = m_lf.lfStrikeOut;
	lf.lfUnderline = m_lf.lfUnderline;

	font.CreateFontIndirect(&lf);
	CFont *pfont = pDC->SelectObject(&font);
	
	int nPointSize = MulDiv(abs(m_lf.lfHeight), 72, pDC->GetDeviceCaps(LOGPIXELSY));
	strText.Format(_T("%s, %dpt"), m_lf.lfFaceName, nPointSize);

	COLORREF crOldTextColor = pDC->SetTextColor(m_rgb);
	pDC->DrawText(strText, -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
	pDC->SelectObject(pfont);
	pDC->SetTextColor(crOldTextColor);
}


void SVPropertyItemFont::Serialize(CArchive& ar) 
{
	SVPropertyItem::Serialize(ar);
	if (ar.IsStoring())
	{	// storing code
		CString str = LogFontToString();

		ar << str;
		ar << m_dwFlags;
	}
	else
	{	// loading code
		CString str;
		
		ar >> str;
		ar >> m_dwFlags;
		StringToLogFont(str);
	}
}


CString SVPropertyItemFont::LogFontToString()
{
	CString str, color;

	str.Format(_T("%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%s,%i"),
							m_lf.lfHeight,
							m_lf.lfWidth,
							m_lf.lfEscapement,
							m_lf.lfOrientation,
							m_lf.lfWeight,
							m_lf.lfItalic,
							m_lf.lfUnderline,
							m_lf.lfStrikeOut,
							m_lf.lfCharSet,
							m_lf.lfOutPrecision,
							m_lf.lfClipPrecision,
							m_lf.lfQuality,
							m_lf.lfPitchAndFamily,
							m_lf.lfFaceName,
							m_rgb);
	return str;
}


void SVPropertyItemFont::StringToLogFont(CString& str)
{
	m_lf.lfHeight=_ttol((LPCTSTR)GetToken(str, _T(",")));
	m_lf.lfWidth=_ttol((LPCTSTR)GetToken(str, _T(",")));
	m_lf.lfEscapement=_ttol((LPCTSTR)GetToken(str, _T(",")));
	m_lf.lfOrientation=_ttol((LPCTSTR)GetToken(str, _T(",")));
	m_lf.lfWeight=_ttol((LPCTSTR)GetToken(str, _T(",")));
	m_lf.lfItalic=_ttoi((LPCTSTR)GetToken(str, _T(",")));
	m_lf.lfUnderline=_ttoi((LPCTSTR)GetToken(str, _T(",")));
	m_lf.lfStrikeOut=_ttoi((LPCTSTR)GetToken(str, _T(",")));
	m_lf.lfCharSet=_ttoi((LPCTSTR)GetToken(str, _T(",")));
	m_lf.lfOutPrecision=_ttoi((LPCTSTR)GetToken(str, _T(",")));
	m_lf.lfClipPrecision=_ttoi((LPCTSTR)GetToken(str, _T(",")));
	m_lf.lfQuality=_ttoi((LPCTSTR)GetToken(str, _T(",")));
	m_lf.lfPitchAndFamily=_ttoi((LPCTSTR)GetToken(str, _T(",")));
	_tcscpy(m_lf.lfFaceName, (LPCTSTR)GetToken(str, _T(",")));
	m_rgb = _ttol(str);
}


CString SVPropertyItemFont::GetToken(CString& str, LPCTSTR c)
{
	int pos;
	CString token;

	pos=str.Find(c);
	token=str.Left(pos);
	str=str.Mid(pos+1);
	return token;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVPropertyItemFont.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:47:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Mar 2001 13:56:54   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  116
 * SCR Title:  Enhancements to SVListCtrl class and SVLibrary.Lib
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Corrections made for portability and better utilization by SVFocus. Also used by the SVReport project for SmithKline Beecham.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Dec 2000 13:24:14   cSchmittinger
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
 *    Rev 1.0   05 Dec 2000 09:27:40   cSchmittinger
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