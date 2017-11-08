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
#pragma once

#pragma region Includes
#include "SVOLicenseManager.h"
#include "SVOResource\resource.h"

#pragma endregion Includes

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
	static void Show(const std::string& sMsg, const SVGuidSet& rList, HANDLE hEvent);
	static void Destroy();

private:
	void Init(const std::string& rMsg, const SVGuidSet& rList, HANDLE hEvent);
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
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual void PostNcDestroy() override;
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVLicenseMgrModelessDlg)
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	std::string m_Msg;
	HANDLE m_hEvent;
	SVGuidSet m_sList;

	CString m_sMessageTitle;
	CListBox m_ctlErrorList;
};

