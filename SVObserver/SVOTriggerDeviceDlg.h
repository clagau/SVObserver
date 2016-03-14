//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOTriggerDeviceDlg
//* .File Name       : $Workfile:   SVOTriggerDeviceDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:12:50  $
//******************************************************************************
#ifndef SVOTRIGGERDEVICEDLG_H
#define SVOTRIGGERDEVICEDLG_H


#include "SVTriggerSelectionDlg.h"

class CSVOConfigAssistantDlg;

class CSVOTriggerDeviceDlg : public CPropertyPage
{
// Construction
public:
	CSVOTriggerDeviceDlg(CWnd* pParent = NULL);   // standard constructor
    void SetupList();

// Dialog Data
	//{{AFX_DATA(CSVOTriggerDeviceDlg)
	enum { IDD = IDD_DLG_PROPPAGE_AA_TRIGGER };
	CButton	m_btnPropTrigger;
	CButton	m_btnNewTrigger;
	CButton	m_btnDeleteTrigger;
	CButton	m_btnAdvanced;
	CListBox	m_ctlTriggerList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVOTriggerDeviceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSVOTriggerDeviceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnPropTrig();
	afx_msg void OnBtnNewTrig();
	afx_msg void OnBtnDeleteTrig();
	afx_msg void OnBtnAdvanced();
	afx_msg void OnDblclkLstTriggers();
	afx_msg void OnSelchangeLstTriggers();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
    CSVOConfigAssistantDlg *m_pParent;
	void EnablePropertyEdit(int iSelection);
	void EnablePropertyButton(bool bEnable);
	void EnableAdvancedPropertyButton(bool bEnable);
	void DisablePropertyEdit();
	void GetNextAvailableTriggerList(SVTriggerNameIdList& rList) const;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOTRIGGERDEVICEDLG_H__2EA120CE_BE74_45B5_9774_E88E9903DCA9__INCLUDED_)

