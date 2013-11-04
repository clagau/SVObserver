//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCAddRemoveDlg
//* .File Name       : $Workfile:   SVPLCAddRemoveDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:23:32  $
//******************************************************************************

#pragma once
#include "afxwin.h"

typedef std::vector<CString> CStringVec;

class SVPLCAddRemoveDlg : public CDialog
{
	DECLARE_DYNAMIC(SVPLCAddRemoveDlg)

public:
	SVPLCAddRemoveDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVPLCAddRemoveDlg();

// Dialog Data
	enum { IDD = IDD_ADD_REMOVE_PLC_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


	DECLARE_MESSAGE_MAP()
public:
	HRESULT GetNewItems( CStringVec& OutputList );
	HRESULT GetRemoved( CStringVec& OutputList );
	CStringVec m_astrAvailablePPQs;
	CStringVec m_astrSetupPPQs;
	CStringVec m_astrOriginalPPQs;

	CListBox m_AvailableList;
	CListBox m_UsedList;

	afx_msg void OnBnClickedAddBtn();
	afx_msg void OnBnClickedRemoveBtn();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCAddRemoveDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:23:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/