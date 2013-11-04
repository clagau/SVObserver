//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : HLB
//* .File Name       : $Workfile:   Hlb.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:13:32  $
//******************************************************************************


#ifndef HLB_H
#define HLB_H


typedef CArray< int, int > CIntArray;

/////////////////////////////////////////////////////////////////////////////
// CHorzListBox window

class CHorzListBox : public CListBox
{
// Construction
public:
	CHorzListBox();

// Attributes
protected:
	BOOL m_bLocked;
	CIntArray m_arrExtents;
	int m_nLongestExtent;
	int m_nTabStops;
	int* m_lpTabStops;

// Operations
public:
	void LockHExtentUpdate();
	void UnlockHExtentUpdate();
	void UpdateHExtent();

protected:
	void InsertNewExtent(int nItem, LPCTSTR lpszStr);
	void InsertNewExtent(int nItem, LPCTSTR lpszStr, CDC* pDC);
	void InitTabStops();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHorzListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHorzListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHorzListBox)
	//}}AFX_MSG
	afx_msg LRESULT OnAddString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInsertString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetTabStops(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\Hlb.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:13:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Nov 2010 16:09:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Apr 2003 15:51:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2001 22:07:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  213
 * SCR Title:  Upgrade Sapera Libraries from 3.00 to 3.11
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Used to diagnose issues with Coreco Acquisition process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
