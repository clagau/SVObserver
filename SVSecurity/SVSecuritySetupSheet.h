//******************************************************************************
//* COPYRIGHT (c) 2008 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecuritySetupSheet
//* .File Name       : $Workfile:   SVSecuritySetupSheet.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 10:00:52  $
//******************************************************************************

#pragma once

//	item - A pointer to another branch off the tree (nullptr if none).
//	name - The string the TreeCtrl displays for that node.
//	func - A function pointer to the function to execute upon double-clicking the node.
//	image - An integer specifying which image in the image list that the node uses.
//	simage - An integer specifying which image in the image list that the node uses when selected.


class SVAccessClass;

class SVSecuritySetupSheet : public CPropertySheet
{
// Construction
public:
	
	SVSecuritySetupSheet();
	
	explicit SVSecuritySetupSheet(UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	explicit SVSecuritySetupSheet(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

	SVAccessClass* m_pAccess {nullptr};

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVSecuritySetupSheet)
	public:
	
	virtual BOOL OnInitDialog() override;
	
	virtual INT_PTR DoModal() override;
	protected:
	
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	//}}AFX_VIRTUAL

// Implementation
public:
	
	virtual ~SVSecuritySetupSheet();
	
	void InitializeGroupList(CListBox& list);

	// Generated message map functions
protected:
	//{{AFX_MSG(SVSecuritySetupSheet)
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	
	friend BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LPARAM ptr);
	
	friend BOOL CALLBACK EnumApplicationNames(CString &szKey, void* ptr);
	friend class SVSecuritySetupPage;

private:
};

/////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK EnumResNameProc(HANDLE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG lParam);

