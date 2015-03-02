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
#include "afxwin.h"
#include "sveditablelistbox.h"


typedef std::vector<CString> CStringVec;

class SVGroupDef
{
public:
	SVGroupDef(){};
	SVGroupDef(CString p_strGroupName, CString p_strPPQName )
		:m_strName( p_strGroupName )
		,m_strPPQ( p_strPPQName)
	{};
		
	CString m_strPPQ;
	CString m_strName;
};

typedef std::vector<std::pair<CString, CString>> CStringPairVect;

typedef std::vector<SVGroupDef> SVGroupDefVect;

class SVRemoteOutputGroupAddRemoveDlg : public CDialog
{
	DECLARE_DYNAMIC(SVRemoteOutputGroupAddRemoveDlg)

public:
	SVRemoteOutputGroupAddRemoveDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVRemoteOutputGroupAddRemoveDlg();

// Dialog Data
	enum { IDD = IDD_ADD_REMOVE_REMOTE_OUTPUT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	int StringPosition( CStringVec l_astrArray, CString l_strValue );
	void UpdateUsedList();

	DECLARE_MESSAGE_MAP()
public:
	HRESULT GetNewItems( CStringVec& OutputList );
	HRESULT GetRemoved( CStringVec& OutputList );
	HRESULT GetRenamed( CStringPairVect& OutputList);
	HRESULT AddValue( CString GroupName, CString PPQName );

	CStringVec m_astrAvailablePPQs;
	CStringVec m_astrSetupGroups;
	CStringVec m_astrOriginalGroups;
	CStringPairVect m_aRenamedGroups;
	SVGroupDefVect m_SetupGroup;

	CListBox m_AvailableList;

	afx_msg void OnBnClickedAddBtn();
	afx_msg void OnBnClickedRemoveBtn();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
private:
	int m_lOutputGroupCount;
	CString NextAvailableGroupName();

public:
	CString m_strGroupNameEdit;
	afx_msg LRESULT OnUsedListEditFinished(WPARAM wPar, LPARAM lPar);
protected:
	CButton m_AddButton;
public:
	afx_msg void OnLbnDblclkUsedList();
	SVEditableListBox m_UsedList;
	CButton m_RemoveBtn;
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVRemoteOutputGroupAddRemoveDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 14:52:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Aug 2012 15:25:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added code to disable add and remove buttons when they do not make sense.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Aug 2012 12:52:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified used list to use editable listbox.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */
