// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOFileConfigDlg
// * .File Name       : $Workfile:   SVOFileConfigDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:32:54  $
// ******************************************************************************

#ifndef SVOFILECONFIGDLG_H
#define SVOFILECONFIGDLG_H

#include <map>
#include <vector>

#include "SVXMLLibrary/SVXMLCTreeCtrl.h"

class SVOFileConfigDlg : public CDialog
{
public:
	CString m_strLastDirectory;
	bool GetDirectoryRemoveLevel( CString& p_strFile, int p_level );

	SVOFileConfigDlg(CWnd* pParent = NULL);	// standard constructor

	virtual ~SVOFileConfigDlg();

	SVXMLCTreeCtrl m_XMLCTree;

	//{{AFX_DATA(SVOFileConfigDlg)
	enum { IDD = IDD_SVOFILECONFIG_DIALOG };
	CListBox	mList;
	CTreeCtrl	mTree;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(SVOFileConfigDlg)
	public:
		virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void CheckConfiguration();
	void GetIOChildren(HTREEITEM p_Item);
	void GetInspections(HTREEITEM p_Item);
	void GetPPQs(HTREEITEM p_Item);
	bool ParseInspection( CString p_strName, CString& p_rstrOut);
	bool ParsePPQ( CString p_strName, CString& p_rstrOut);
	void FixConfiguration();
	void ReIndexIOEntries(HTREEITEM p_Item);

	unsigned long m_ulCurrentVersion;

	std::vector<CString> m_Inspections;
	typedef std::pair<HTREEITEM, CString> HTIStringPair;
	typedef std::vector<HTIStringPair> HTIStringPairs;
	std::vector< CString > m_PPQs;
	HTIStringPairs m_IOEntryPairs;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(SVOFileConfigDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonLoad();
	afx_msg void OnButtonSave();
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonloadSvx();
	afx_msg void OnButtonsaveSvx();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOFILECONFIGDLG_H__BC7C3A47_497B_4C81_B529_7827DD5F422A__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOFileConfig\SVOFileConfigDlg.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 11:32:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Dec 2012 11:00:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  791
 * SCR Title:  Fix bug in SVOFileConfig utility program
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   - Added code to detect and remove IO Entries with duplicate names.  
 * - Added Code to remove IO Entries with un-used PPQs.
 * - Added code to break out of endless loop  to prevent lock-up..
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Sep 2012 18:06:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Feb 2012 11:41:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  739
 * SCR Title:  Upgrade SVObserver for Devario Release
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified SVOFileConfig test app to correct invalid IOEntries
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:01:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
