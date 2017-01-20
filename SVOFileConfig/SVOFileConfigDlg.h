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

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVOFileConfigDlg : public CDialog
{
public:
	bool GetDirectoryRemoveLevel( SVString& rFileName, int p_level );

	SVOFileConfigDlg(CWnd* pParent = nullptr);	// standard constructor

	virtual ~SVOFileConfigDlg();

	SvXml::SVXMLCTreeCtrl m_XMLCTree;

	//{{AFX_DATA(SVOFileConfigDlg)
	enum { IDD = IDD_SVOFILECONFIG_DIALOG };
	CListBox	mList;
	CTreeCtrl	mTree;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(SVOFileConfigDlg)
	public:
		virtual BOOL DestroyWindow() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void CheckConfiguration();
	void GetIOChildren(HTREEITEM p_Item);
	void GetInspections(HTREEITEM p_Item);
	void GetPPQs(HTREEITEM p_Item);
	bool ParseInspection( const SVString& rName, SVString& rOut);
	bool ParsePPQ( const SVString& rName, SVString& rOut);
	void FixConfiguration();
	void ReIndexIOEntries(HTREEITEM p_Item);

	unsigned long m_ulCurrentVersion;

	std::vector<SVString> m_Inspections;
	typedef std::pair<HTREEITEM, SVString> HTIStringPair;
	typedef std::vector<HTIStringPair> HTIStringPairs;
	std::vector< SVString > m_PPQs;
	HTIStringPairs m_IOEntryPairs;
	HICON m_hIcon;
	SVString m_strLastDirectory;

	// Generated message map functions
	//{{AFX_MSG(SVOFileConfigDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonloadSvx();
	afx_msg void OnButtonsaveSvx();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

