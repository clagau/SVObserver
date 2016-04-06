//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerSelectionDlg
//* .File Name       : $Workfile:   SVTriggerSelectionDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:52:32  $
//******************************************************************************
#pragma once

#pragma region Includes

//Moved to precompiled header: #include <map>
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

typedef std::map<SVString, int> SVTriggerNameIdList;

class SVTriggerSelectionDlg : public CDialog
{
	DECLARE_DYNAMIC(SVTriggerSelectionDlg)

public:
	SVTriggerSelectionDlg(const SVTriggerNameIdList& triggerList, CWnd* pParent = NULL);   // standard constructor
	virtual ~SVTriggerSelectionDlg();

// Dialog Data
	enum { IDD = IDD_TRIGGER_SELECTION_DLG };

	void GetSelectedTrigger(CString& name, int& id) const;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
	SVTriggerNameIdList m_triggerList;
	CListBox m_selectionListBox;
	CString m_selectedTriggerName;
	int m_selectedTriggerID;

	void PopulateSelectionList();
public:
	afx_msg void OnLbnDblclkTriggerList();
};
