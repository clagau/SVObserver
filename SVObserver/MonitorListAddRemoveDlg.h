//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListAddRemoveDlg
//* .File Name       : $Workfile:   MonitorListAddRemoveDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   18 Aug 2014 16:09:06  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "afxwin.h"
#include "RemoteMonitorList.h"
#pragma endregion Includes

class MonitorListAddRemoveDlg : public CDialog
{
	DECLARE_DYNAMIC(MonitorListAddRemoveDlg)

#pragma region Public Methods
public:
	MonitorListAddRemoveDlg(const RemoteMonitorList& rMonitorList, const PPQNameListNames& rList, CWnd* pParent = NULL);   // standard constructor
	virtual ~MonitorListAddRemoveDlg();

	const RemoteMonitorList& GetRemoteMonitorList() const;

	afx_msg void OnDblClickUsedList();
	afx_msg void OnBnClickedAddBtn();
	afx_msg void OnBnClickedRemoveBtn();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnProperties();

// Dialog Data
	enum { IDD = IDD_DLG_ADD_REMOVE_MONITOR_LIST };
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void UpdateUsedList(const CString& PPQName, const NameDepthPairList& rList);

	DECLARE_MESSAGE_MAP()
#pragma endregion Protected Methods

#pragma region Private Members
private:
	RemoteMonitorList m_MonitorList;
	PPQNameListNames m_SetupList;
	CListBox m_AvailableList;
	CListBox m_UsedList;
	CButton m_RemoveButton;
	CButton m_AddButton;
	CButton m_PropertiesButton;

	void ReplaceList(const CString& oldName, const CString& newName);
	CString BuildListDisplayName(const CString& PPQName, const CString& name) const;
	CString GetListNameFromDisplayName(const CString& name) const;
	CString GetPPQName(const CString& name) const;
	CString NextAvailableListName() const;
#pragma region Private Members
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\MonitorListAddRemoveDlg.h_v  $
 * 
 *    Rev 1.1   18 Aug 2014 16:09:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed OnUsedListEditFinished method.
 * Removed IsValidName method.
 * Changed m_UsedList from SVEditableListBox to CListBox.
 * Renamed OnLbnDblclkUsedList to OnDblClickUsedList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Apr 2014 16:22:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */
