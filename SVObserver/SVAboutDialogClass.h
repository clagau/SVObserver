//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAboutDialogClass
//* .File Name       : $Workfile:   SVAboutDialogClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:23:22  $
//******************************************************************************

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#ifndef _SVABOUTDIALOGCLASS_H
#define _SVABOUTDIALOGCLASS_H

#ifdef SV_SHOW_INCLUDES
	#pragma message( "Including " __FILE__ )
#endif

//******************************************************************************
//* OBJECT DEFINITION
//******************************************************************************
class SVAboutDialogClass : public CDialog
{
protected:
	//{{AFX_MSG(SVAboutDialogClass)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	//{{AFX_VIRTUAL(SVAboutDialogClass)
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_DATA(SVAboutDialogClass)
	enum { IDD = IDD_ABOUTBOX };
	CListCtrl m_LCVersion;
	//}}AFX_DATA

	DECLARE_MESSAGE_MAP()

//******************************************************************************
//* Operator(s):
//******************************************************************************
public:

	SVAboutDialogClass();
	~SVAboutDialogClass();

protected:
	void LoadOEMInfo();
	void LoadVersionList();
	void HideSVRText();
	void SetOEMText(LPCTSTR text);

//******************************************************************************
//* Attributes(s):
//******************************************************************************
protected:
	HBITMAP logoBmp;
};

#endif   // _SVABOUTDIALOGCLASS_H

