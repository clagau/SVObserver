//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOLicenseMgrModelessDlg
//* .File Name       : $Workfile:   SVLicenseMgrModelessDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   06 May 2013 14:38:20  $
//******************************************************************************

#ifndef LICMGRMODELESSDLG_H_
#define LICMGRMODELESSDLG_H_

#include "SVOLicenseManager\SVOLicenseManager.h"
#include "SVOResource\resource.h"

class SVLicenseMgrModelessDlg;

// the following is from Windows Developer's Journal Feb 1998 p 54
// code by James M. Curran; JamesCurram@CompuServe.com
class DialogUnits
{
public:
	DialogUnits()
	{
		DWORD dw = GetDialogBaseUnits();
		m_duXx4 = LOWORD(dw);
		m_duYx8 = HIWORD(dw);
	}

	~DialogUnits() {};

	int X(int x) {return ((x * m_duXx4) / 4);}
	int Y(int y) {return ((y * m_duYx8) / 8);}

private:
	WORD m_duXx4;	// dialog units on the X axis times 4
	WORD m_duYx8;	// dialog units on the Y axis times 8
};

class SVLicenseMgrModelessDlg : public CDialog
{
// Construction
public:
	SVLicenseMgrModelessDlg();
	virtual ~SVLicenseMgrModelessDlg();
	static void Show(const CString& sMsg, const SVLicenseErrorListSet& p_sList, HANDLE p_hEvent);
	static void Destroy();


protected:
	CString m_sMsg;
	HANDLE m_hEvent;
	SVLicenseErrorListSet m_sList;

private:
	void Init(const CString& sMsg, const SVLicenseErrorListSet& p_sList, HANDLE p_hEvent);
	static SVLicenseMgrModelessDlg& Instance();
	void CleanUp();


public:
// Dialog Data
	//{{AFX_DATA(SVLicenseMgrModelessDlg)
	enum { IDD = IDD_DLG_LICMGR_TOOLERRORS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVLicenseMgrModelessDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVLicenseMgrModelessDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CString m_sMessageTitle;
	CListBox m_ctlErrorList;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVLicenseMgrModelessDlg.h_v  $
 * 
 *    Rev 1.1   06 May 2013 14:38:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Apr 2013 15:56:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the ErrorList from a deque to a set
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:46:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Mar 2013 12:33:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/