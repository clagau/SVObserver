// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : CModelessMsgBox
// * .File Name       : $Workfile:   ModelessMessageBox.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 09:12:36  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class CModelessMsgBox;
#define ModelessMessageBox(s,e)	{CModelessMsgBox* pBox = new CModelessMsgBox((s),(e)); pBox->Create(CModelessMsgBox::IDD, nullptr);}

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

/////////////////////////////////////////////////////////////////////////////
// CModelessMsgBox dialog

class CModelessMsgBox : public CDialog
{
// Construction
public:
	CModelessMsgBox(const SVString& rMsg, HANDLE p_hEvent);   // standard constructor
	virtual ~CModelessMsgBox();


protected:
	SVString m_Msg;
	HANDLE m_hEvent;

public:
// Dialog Data
	//{{AFX_DATA(CModelessMsgBox)
	enum { IDD = IDD_MODELESS_MESSAGE_BOX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelessMsgBox)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual void PostNcDestroy() override;
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModelessMsgBox)
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	virtual void OnCancel() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

