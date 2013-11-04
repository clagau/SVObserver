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
#ifndef INCL_SVTRIGGERSELECTIONDLG_H
#define INCL_SVTRIGGERSELECTIONDLG_H

#include <map>
#include "SVUtilityLibrary/SVString.h"
#include "resource.h"
#include "afxwin.h"

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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTriggerSelectionDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:52:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Jan 2013 11:03:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/