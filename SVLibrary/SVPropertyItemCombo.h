// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemCombo.h
// * .File Name       : $Workfile:   SVPropertyItemCombo.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:46:40  $
// ******************************************************************************

#if !defined(AFX_PROPERTYITEMCOMBO_H__3717E417_70F3_4A34_A541_A13BA22FFD22__INCLUDED_)
#define AFX_PROPERTYITEMCOMBO_H__3717E417_70F3_4A34_A541_A13BA22FFD22__INCLUDED_

#include "SVPropertyItem.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SVInPlaceList;

/////////////////////////////////////////////////////////////////////////////
// SVPropertyItemCombo

class SVPropertyItemCombo : public SVPropertyItem
{
	SVPropertyItemCombo(const SVPropertyItemCombo& d);
	SVPropertyItemCombo& operator=(const SVPropertyItemCombo& d);

protected:
	DECLARE_SERIAL(SVPropertyItemCombo)

public:
	SVPropertyItemCombo();
	SVPropertyItemCombo(LPCTSTR lpszName, LPCTSTR lpszItems,     int nItem,        DWORD dwStyle=CBS_DROPDOWNLIST);
	SVPropertyItemCombo(LPCTSTR lpszName, CStringArray& Strings, int nItem,        DWORD dwStyle=CBS_DROPDOWNLIST);
	SVPropertyItemCombo(LPCTSTR lpszName, LPCTSTR lpszItems,     LPCTSTR lpszText, DWORD dwStyle=CBS_DROPDOWN);
	SVPropertyItemCombo(LPCTSTR lpszName, CStringArray& Strings, LPCTSTR lpszText, DWORD dwStyle=CBS_DROPDOWN);
	virtual ~SVPropertyItemCombo();
	virtual void Serialize(CArchive& ar);

	// Attributes
	void SetData(LPCTSTR lpszText, DWORD dwStyle=CBS_DROPDOWNLIST);
	void SetOptions(CStringArray& ar, int nItem=-1);
	void SetOptions(CString str, int nItem=-1);

	// Operations
	inline CString GetData(void) const { return m_strText; }
	int GetDataIndex(void) const;

protected:
	// Operations
	virtual void Draw(CDC* pDC, CRect& rect);
	virtual BOOL Edit(CWnd* pParent, int nItem, CRect rect, CPoint point, UINT nID, UINT nChar, BOOL bExitOnArrows=TRUE);
	virtual void EndEdit(void);
	virtual void OnEndEdit(LPLVITEM plvItem);
	virtual BOOL HitTest(CRect rect, CPoint point);

	// Data
private:
	CStringArray	m_Strings;
	CString				m_strText;
	int						m_nItem;
	DWORD					m_dwStyle;
};

#endif	// #define AFX_PROPERTYITEMCOMBO_H__3717E417_70F3_4A34_A541_A13BA22FFD22__INCLUDED_

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVPropertyItemCombo.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:46:40   bWalter
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