//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskEditorPpg
//* .File Name       : $Workfile:   SVMaskEditorPpg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Apr 2013 14:08:22  $
//******************************************************************************

#if !defined(AFX_SVMASKEDITORPPG_H__C87C9B84_E6DC_11D2_A77B_00106F000C7D__INCLUDED_)
#define AFX_SVMASKEDITORPPG_H__C87C9B84_E6DC_11D2_A77B_00106F000C7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SVMaskEditorPpg.h : Declaration of the SVMaskEditorPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// SVMaskEditorPropPage : See SVMaskEditorPpg.cpp.cpp for implementation.

class SVMaskEditorPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(SVMaskEditorPropPage)
	DECLARE_OLECREATE_EX(SVMaskEditorPropPage)

// Constructor
public:
	SVMaskEditorPropPage();

// Dialog Data
	//{{AFX_DATA(SVMaskEditorPropPage)
	enum { IDD = IDD_PROPPAGE_SVMASKEDITOR };
	CStatic	image;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(SVMaskEditorPropPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVMASKEDITORPPG_H__C87C9B84_E6DC_11D2_A77B_00106F000C7D__INCLUDED)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVMaskEditor\SVMaskEditorPpg.h_v  $
 * 
 *    Rev 1.0   26 Apr 2013 14:08:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
