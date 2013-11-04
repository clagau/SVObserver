// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPropertyList.h
// * .File Name       : $Workfile:   SVPropertyListBox.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:47:18  $
// ******************************************************************************

#if !defined(AFX_PROPERTYLIST_H__74205380_1B56_11D4_BC48_00105AA2186F__INCLUDED_)
#define AFX_PROPERTYLIST_H__74205380_1B56_11D4_BC48_00105AA2186F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define IDC_INPLACE_CONTROL   8                  // ID of inplace edit controls

class SVPropertyItem;
/////////////////////////////////////////////////////////////////////////////
// SVPropertyListBox window

class SVPropertyListBox : public CListBox
{
	SVPropertyListBox(const SVPropertyListBox& d);
	SVPropertyListBox& operator=(const SVPropertyListBox& d);

public:
	SVPropertyListBox();
	virtual ~SVPropertyListBox();

// Attributes
	SVPropertyItem* GetPropertyItem(int nItem) const;

// Operations
	int AddPropertyItem(SVPropertyItem* pItem);
	void ResetContent(BOOL bRemoveItems=FALSE);
	void AutoEdit(BOOL bAutoEdit=TRUE) { m_bAutoEdit = bAutoEdit; }
	COLORREF SetBkColor(COLORREF rgb);
	COLORREF SetTextColor(COLORREF rgb);
	COLORREF GetBkColor(void);
	COLORREF GetTextColor(void);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyList)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(SVPropertyListBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchange();
	//}}AFX_MSG
	afx_msg void OnBeginInPlaceCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndInPlaceCtrl(NMHDR* pNMHDR, LRESULT* pResult);

protected:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void GetItemValueRect(CRect& rect);
	BOOL LookupPropertyItem(char nStartChar);
	int FindPropertyItem(char nStartChar, int nFromIndex, int nCount);
	void InvertLine(CDC* pDC,CPoint ptFrom,CPoint ptTo);

	// Editing
	virtual void  OnEditItem(int nItem, CPoint point, UINT nChar);
	virtual void  OnEndEditItem(LPLVITEM plvItem);

	DECLARE_MESSAGE_MAP()

protected:
	int				m_nDivider;
	int				m_nDivTop;
	int				m_nDivBtm;
	int				m_nOldDivX;
	BOOL			m_bTracking;
	BOOL			m_bAutoEdit;
	HCURSOR		m_hCursorArrow;
	HCURSOR		m_hCursorSize;
	COLORREF	m_crBackground;
	COLORREF	m_crText;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYLIST_H__74205380_1B56_11D4_BC48_00105AA2186F__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVPropertyListBox.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:47:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Mar 2001 14:11:06   cSchmittinger
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  116
 * SCR Title:  Enhancements to SVListCtrl class and SVLibrary.Lib
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Corrections made for portability and better utilization by SVFocus. This is also used by the SVReport project for SmithKline Beechem.
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