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

#pragma endregion Includes

typedef std::map<std::string, int> SVTriggerNameIdList;

class SVTriggerSelectionDlg : public CDialog
{
	DECLARE_DYNAMIC(SVTriggerSelectionDlg)

public:
	SVTriggerSelectionDlg(const SVTriggerNameIdList& triggerList, CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVTriggerSelectionDlg();

// Dialog Data
	enum { IDD = IDD_TRIGGER_SELECTION_DLG };

	void GetSelectedTrigger(std::string& rName, int& rID) const;

	virtual BOOL OnInitDialog() override;
	afx_msg void OnBnClickedOk();

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
	SVTriggerNameIdList m_triggerCallbackMap;
	CListBox m_selectionListBox;
	CString m_selectedTriggerName;
	int m_selectedTriggerID;

	void PopulateSelectionList();
public:
	afx_msg void OnLbnDblclkTriggerList();
};
