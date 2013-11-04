// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemFont.h
// * .File Name       : $Workfile:   SVPropertyItemFont.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:47:08  $
// ******************************************************************************

#ifndef PROPERTYITEMFONT_H
#define PROPERTYITEMFONT_H

#include "SVPropertyItem.h"

class SVPropertyItemFont : public SVPropertyItem
{
	SVPropertyItemFont(const SVPropertyItemFont& d);
	SVPropertyItemFont& operator=(const SVPropertyItemFont& d);

protected:
	DECLARE_SERIAL(SVPropertyItemFont)

public:
	SVPropertyItemFont();
	SVPropertyItemFont(LPCTSTR lpszName, int nPointSize, LPCTSTR lpszFaceName, CDC* pDC=NULL);
	SVPropertyItemFont(LPCTSTR lpszName, const LPLOGFONT lpLogFont);
	virtual void Serialize(CArchive& ar);

	void SetData(int nPointSize, LPCTSTR lpszFaceName, CDC* pDC=NULL);
	void SetData(LPLOGFONT lpLogFont);
	void SetData(	int nHeight, int nWidth, int nEscapement, int nOrientation,
								int nWeight, BYTE bItalic, BYTE bUnderline, BYTE cStrikeOut,
								BYTE nCharSet, BYTE nOutPrecision, BYTE nClipPrecision,
								BYTE nQuality, BYTE nPitchAndFamily, LPCTSTR lpszFacename);

	inline const LOGFONT* GetData(void) const { return &m_lf; }
	inline COLORREF GetColor(void) const {return m_rgb; }

protected:
	virtual void Draw(CDC* pDC, CRect& rect);
	virtual BOOL Edit(CWnd* pParent, int nItem, CRect rect, CPoint point, UINT nID, UINT nChar, BOOL bExitOnArrows=TRUE);

	CString LogFontToString();
	void StringToLogFont(CString& str);
	CString GetToken(CString& str, LPCTSTR c);

private:
	LOGFONT		m_lf;
	COLORREF	m_rgb;
	DWORD			m_dwFlags;

};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVPropertyItemFont.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:47:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3.1.0   22 Oct 2010 08:10:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
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
 *    Rev 1.1   06 Dec 2000 11:57:36   mike
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
