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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVAboutDialogClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:23:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Oct 2011 09:07:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  726
 * SCR Title:  Upgrade SVObserver to 5.41
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed scroll in Information box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Mar 2006 14:44:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  545
 * SCR Title:  Display version information for rabbit board component
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added a List box to display additional version information on the SVObserver about box.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Apr 2003 16:14:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Jun 2001 10:50:00   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Revised debug pragma.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   20 Apr 2001 16:05:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   This change was required due to changes in SVObserver.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/