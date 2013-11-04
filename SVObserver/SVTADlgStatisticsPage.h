//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogStatisticsPageClass
//* .File Name       : $Workfile:   SVTADlgStatisticsPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:22:48  $
//******************************************************************************

#ifndef SVTADLGSTATISTICSPAGE_H
#define SVTADLGSTATISTCISPAGE_H

class SVStatisticsToolClass;
class SVToolClass;
class SVToolSetClass;

class SVToolAdjustmentDialogStatisticsPageClass : public CPropertyPage
{
public:
	SVToolAdjustmentDialogStatisticsPageClass( SVToolClass* PTool );
	~SVToolAdjustmentDialogStatisticsPageClass();

	void UpdateStatisticsParameters();
	void OnOK();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();

protected:
	void getParameters();
	//void initVariablesComboBox();
	//int getComboBoxStringExtent( CComboBox& RComboBox, CString& RStr );
	void initListBox(CListBox* pListBox, TCHAR atcTestChar);

protected:
	//{{AFX_MSG(SVToolAdjustmentDialogStatisticsPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	afx_msg void OnSetRange();
	afx_msg void OnPublishButton();
	afx_msg void OnBtnObjectPicker();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(SVToolAdjustmentDialogStatisticsPageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

public:
	//{{AFX_DATA(SVToolAdjustmentDialogStatisticsPageClass)
	enum { IDD = IDD_TA_STATISTICS_PARAMETERS_DIALOG };
	CListBox    m_lbSelectedList;
	CListBox    m_lbAvailableList;
	CString     m_strTestValue;
	CString     m_strVariableToMonitor;
	//}}AFX_DATA

protected:
	SVStatisticsToolClass*  m_pTool;
	SVToolSetClass*         m_pToolSet;
	CString                 m_strFullNameOfVariable;
};

//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

#endif	//	SVTADLGSTATISTICSPAGE_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTADlgStatisticsPage.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 11:22:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   27 Jul 2005 16:05:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated StatisticsTool to use SVObjectReference (array value objects) instead of GUIDs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 16:21:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:43:12   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Oct 1999 20:02:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added OnOk method and UpdateStatisticsParameters
 * method.
 * Revised m_testValue from type double to type CString.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Oct 1999 16:36:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Logic to setup results.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Oct 1999 13:06:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  39
 * SCR Title:  Integrate Statistics Tool
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin. Dialog to setup SVStatisticsToolClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
