// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVInPlaceList.h
// * .File Name       : $Workfile:   SVInPlaceList.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:37:12  $
// ******************************************************************************

#if !defined(SVINPLACELIST_INCLUDED_)
#define SVINPLACELIST_INCLUDED_

class SVInPlaceList : public CComboBox
{
    friend class CComboEdit;

// Construction
public:
	SVInPlaceList(CWnd* pParent,        // parent
               CRect& rect,           // dimensions & location
               DWORD dwStyle,         // window/combobox style
               UINT nID,              // control ID
               int nRow, int nColumn, // row and column
							 CStringArray& Items,   // Items in list
               CString sInitText,     // initial selection
							 UINT nFirstChar,       // first character to pass to control
							 BOOL bExitOnArrows=FALSE);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVInPlaceList)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~SVInPlaceList();

	void EndEdit();

protected:
	int GetCorrectDropWidth();

// Generated message map functions
protected:
	//{{AFX_MSG(SVInPlaceList)
	afx_msg void OnDropdown();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnKillfocus();
	afx_msg void OnSelendcancel();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	//afx_msg void OnSelendOK();

	DECLARE_MESSAGE_MAP()

private:
	int			m_nNumLines;
	CString	m_sInitText;
	int			m_nRow;
	int			m_nCol;
 	UINT		m_nLastChar; 
	BOOL		m_bExitOnArrows; 
	BOOL		m_bEscape;
};

#endif // #define SVINPLACELIST_INCLUDED_

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVInPlaceList.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:37:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/