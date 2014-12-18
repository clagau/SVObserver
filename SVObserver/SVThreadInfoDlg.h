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
#include "afxcmn.h"
#include "PropertyTree\PROPTREE.H"
#include "SVSystemLibrary\SVThreadManager.h"
#pragma endregion

// SVThreadInfoDlg dialog

class SVThreadInfoDlg : public CDialog
{
	typedef std::list<int> AffinityBitList;
#pragma region Public
public:
	SVThreadInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVThreadInfoDlg();
	virtual BOOL OnInitDialog();
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
#pragma endregion


#pragma region Private
private:
	void InsertComboThreadItem( SVRPropertyItem* pRoot, CString name, unsigned int Affinity, unsigned int nID );
	void InsertThreadItem( SVRPropertyItem* pRoot, CString name, int Affinity, unsigned int nID );
	void AddComboAffinitys( SVRPropertyItemCombo* pCombo, AffinityBitList& affinitys );
	bool FindName( SVRPropertyItem* pRoot, LPCTSTR Name );

	DECLARE_DYNAMIC(SVThreadInfoDlg)
	SVRPropTree m_ThreadList;
	int m_IdIndex;
	int m_DisplayState;
#pragma endregion
public:
	BOOL m_bManagerEnable;
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVThreadInfoDlg.h_v  $
 * 
 *    Rev 1.2   17 Dec 2014 07:08:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  941
 * SCR Title:  Update SVObserver Version Number for the 7.10 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added Thread Manager Enable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Dec 2014 09:53:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Include SVThreadManager.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Dec 2014 13:11:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial check in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */
