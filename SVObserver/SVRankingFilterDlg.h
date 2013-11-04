//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRankingFilterDlg
//* .File Name       : $Workfile:   SVRankingFilterDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:39:24  $
//******************************************************************************

#if !defined(AFX_SVRANKINGFILTERDLG_H__INCLUDED_)
#define AFX_SVRANKINGFILTERDLG_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVTaskObjectInterfaceClass.h"

class SVRankingFilterClass;

/////////////////////////////////////////////////////////////////////////////
// SVRankingFilterDlg dialog

class SVRankingFilterDlg : public CDialog, public SVTaskObjectInterfaceClass
{
	// Construction
public:
	SVRankingFilterDlg();   // standard constructor

	SVRankingFilterClass *GetRankingFilter();
	
	virtual HRESULT SetInspectionData();

	void EnableCells();

	long m_lRankingCells[49];
		
	// Dialog Data
	//{{AFX_DATA(SVRankingFilterDlg)
	public:
	enum { IDD = IDD_RANKING_FILTER };
	CComboBox	m_ctlRankingWidth;
	CComboBox	m_ctlRankingHeight;
	CComboBox	m_ctlRankingRank;
	long	m_lRankingWidth;
	long	m_lRankingHeight;	
	long	m_lRankingRank;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVRankingFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Generated message map functions
	//{{AFX_MSG(SVRankingFilterDlg)
	protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeRankingWidth();
	afx_msg void OnSelchangeRankingHeight();
	afx_msg void OnSelchangeRankingRank();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVRANKINGFILTERDLG_H__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRankingFilterDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:39:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Aug 2005 08:09:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   04 Nov 2003 14:40:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  394
 * SCR Title:  Implement new RankingFilter for WindowTool that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new ranking filter class that has a 1-7x1-7 customizable neighborhood.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/