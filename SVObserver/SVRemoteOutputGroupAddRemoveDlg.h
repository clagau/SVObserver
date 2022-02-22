//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteOutputGroupAddRemoveDlg
//* .File Name       : $Workfile:   SVRemoteOutputGroupAddRemoveDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:52:16  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVMFCControls/SVEditableListBox.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

class SVGroupDef
{
public:
	SVGroupDef(){};
	SVGroupDef(const std::string& rGroupName, const std::string& rPPQName ) :
	  m_Name( rGroupName )
	, m_PPQName( rPPQName)
	{};
		
	std::string m_PPQName;
	std::string m_Name;
};

typedef std::vector<SVGroupDef> SVGroupDefVect;

class SVRemoteOutputGroupAddRemoveDlg : public CDialog
{
public:
	SVRemoteOutputGroupAddRemoveDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVRemoteOutputGroupAddRemoveDlg();

// Dialog Data
	enum { IDD = IDD_ADD_REMOVE_REMOTE_OUTPUT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	int StringPosition( const SvDef::StringVector& rList, const std::string& rValue );
	void UpdateUsedList();

	DECLARE_MESSAGE_MAP()
public:
	HRESULT GetNewItems( SvDef::StringVector& rOutputList );
	HRESULT GetRemoved(SvDef::StringVector& rOutputList );
	HRESULT GetRenamed(SvDef::StringPairVector& rRenamedList);

	SvDef::StringVector m_AvailablePPQs;
	SvDef::StringVector m_SetupGroups;
	SvDef::StringVector m_OriginalGroups;
	SvDef::StringPairVector m_RenamedGroups;
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
	std::string NextAvailableGroupName();
	std::string m_GroupNameEdit;
	CButton m_AddButton;
	SvMc::SVEditableListBox m_UsedList;
	CButton m_RemoveBtn;
};

