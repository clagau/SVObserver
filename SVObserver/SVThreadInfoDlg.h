//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThreadInfoDlg
//* .File Name       : $Workfile:   SVThreadInfoDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Dec 2014 07:08:52  $
//******************************************************************************
#pragma once
#pragma region Includes
#include "SVRPropertyTree/SVRPropTree.h"
#include "SVSystemLibrary/SVThreadManager.h"
#pragma endregion

class SVRPropertyItemCombo;

class SVThreadInfoDlg : public CDialog
{
	typedef std::list<int> AffinityBitList;
#pragma region Public
public:
	SVThreadInfoDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVThreadInfoDlg();
	virtual BOOL OnInitDialog() override;
	void UpdateThreadInfo( SVThreadAttribute eAttribute);


	// Dialog Data
	enum { IDD = IDD_THREAD_INFO };

#pragma endregion

#pragma region Protected
protected:
#pragma region Message Map
	DECLARE_MESSAGE_MAP()
    afx_msg void OnItemButtonClick(NMHDR* pNotifyStruct, LRESULT* plResult);
	afx_msg void OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
	afx_msg void OnBnClickedSave();
	afx_msg void OnEnableCheck();
#pragma endregion
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
#pragma endregion


#pragma region Private
private:
	void InsertComboThreadItem( SVRPropertyItem* pRoot, LPCTSTR Name, unsigned int Affinity, unsigned int nID );
	void InsertThreadItem( SVRPropertyItem* pRoot, LPCTSTR Name, int Affinity, unsigned int nID );
	void AddComboAffinitys( SVRPropertyItemCombo* pCombo, AffinityBitList& affinitys );
	bool FindName( SVRPropertyItem* pRoot, LPCTSTR Name );

	SVRPropTree m_ThreadList;
	int m_IdIndex;
	int m_DisplayState;
#pragma endregion
public:
	BOOL m_bManagerEnable;
};

