// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : PropertyItemEdit.h
// * .File Name       : $Workfile:   SVPropertyItemEdit.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:46:50  $
// ******************************************************************************

#if !defined(AFX_PROPERTYITEMEDIT_H__3717E417_70F3_4A34_A541_A13BA22FFD22__INCLUDED_)
#define AFX_PROPERTYITEMEDIT_H__3717E417_70F3_4A34_A541_A13BA22FFD22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVPropertyItem.h"

class SVPropertyItemEdit : public SVPropertyItem
{
	SVPropertyItemEdit(const SVPropertyItemEdit& d);
	SVPropertyItemEdit& operator=(const SVPropertyItemEdit& d);

protected:
	DECLARE_SERIAL(SVPropertyItemEdit)

public:
	SVPropertyItemEdit();
	SVPropertyItemEdit(LPCTSTR lpszName, LPCTSTR lpszText, DWORD dwStyle=0);
	virtual void Serialize(CArchive& ar);
	virtual ~SVPropertyItemEdit();

	// Attributes
	void SetData(LPCTSTR lpszText, DWORD dwStyle=0);
	BOOL ModifyStyle(DWORD dwRemove, DWORD dwAdd);
	
	// Operations
	inline CString GetData(void) { return m_strText; }

protected:
	// Operations
	virtual void Draw(CDC* pDC, CRect& rect);
	virtual BOOL Edit(CWnd* pParent, int nItem, CRect rect, CPoint point, UINT nID, UINT nChar, BOOL bExitOnArrows=TRUE);
	virtual void EndEdit(void);
	virtual void OnEndEdit(LPLVITEM plvItem);

private:
	CString	m_strText;
	DWORD		m_dwStyle;
};

#endif	// #define AFX_PROPERTYITEMEDIT_H__3717E417_70F3_4A34_A541_A13BA22FFD22__INCLUDED_

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVPropertyItemEdit.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:46:50   bWalter
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