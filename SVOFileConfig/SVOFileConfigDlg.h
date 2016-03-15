// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOFileConfigDlg
// * .File Name       : $Workfile:   SVOFileConfigDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   03 Feb 2014 17:07:36  $
// ******************************************************************************

#ifndef SVOFILECONFIGDLG_H
#define SVOFILECONFIGDLG_H

#include <map>
#include <vector>



class SVOFileConfigDlg : public CDialog
{
public:
	CString m_strLastDirectory;
	bool GetDirectoryRemoveLevel( CString& p_strFile, int p_level );

	SVOFileConfigDlg(CWnd* pParent = NULL);	// standard constructor

	virtual ~SVOFileConfigDlg();

	SvXml::SVXMLCTreeCtrl m_XMLCTree;

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
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonloadSvx();
	afx_msg void OnButtonsaveSvx();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOFILECONFIGDLG_H__BC7C3A47_497B_4C81_B529_7827DD5F422A__INCLUDED_)

