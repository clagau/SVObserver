//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputGroupAddRemoveDlg
//* .File Name       : $Workfile:   SVRemoteOutputGroupAddRemoveDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:52:16  $
//******************************************************************************
#pragma once
#include "SVMFCControls/SVEditableListBox.h"
#include "SVUtilityLibrary/SVString.h"

class SVGroupDef
{
public:
	SVGroupDef(){};
	SVGroupDef(const SVString& rGroupName, const SVString& rPPQName ) :
	  m_Name( rGroupName )
	, m_PPQName( rPPQName)
	{};
		
	SVString m_PPQName;
	SVString m_Name;
};

typedef std::vector<SVStringPair> SVStringPairVector;

typedef std::vector<SVGroupDef> SVGroupDefVect;

class SVRemoteOutputGroupAddRemoveDlg : public CDialog
{
	DECLARE_DYNAMIC(SVRemoteOutputGroupAddRemoveDlg)

public:
	SVRemoteOutputGroupAddRemoveDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVRemoteOutputGroupAddRemoveDlg();

// Dialog Data
	enum { IDD = IDD_ADD_REMOVE_REMOTE_OUTPUT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	int StringPosition( const SVStringVector& rList, const SVString& rValue );
	void UpdateUsedList();

	DECLARE_MESSAGE_MAP()
public:
	HRESULT GetNewItems( SVStringVector& rOutputList );
	HRESULT GetRemoved( SVStringVector& rOutputList );
	HRESULT GetRenamed( SVStringPairVector& rRenamedList);

	SVStringVector m_AvailablePPQs;
	SVStringVector m_SetupGroups;
	SVStringVector m_OriginalGroups;
	SVStringPairVector m_RenamedGroups;
	SVGroupDefVect m_SetupGroup;

	CListBox m_AvailableList;

	afx_msg void OnBnClickedAddBtn();
	afx_msg void OnBnClickedRemoveBtn();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLbnDblclkUsedList();
	afx_msg LRESULT OnUsedListEditFinished(WPARAM wPar, LPARAM lPar);
	virtual BOOL OnInitDialog() override;
private:
	int m_lOutputGroupCount;
	SVString NextAvailableGroupName();
	SVString m_GroupNameEdit;
	CButton m_AddButton;
	SvMc::SVEditableListBox m_UsedList;
	CButton m_RemoveBtn;
};

