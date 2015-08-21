//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOFileConfigDlg
//* .File Name       : $Workfile:   SVOFileConfig.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 11:31:38  $
//******************************************************************************

#ifndef SVOFILECONFIG_H
#define SVOFILECONFIG_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "SVXMLLibrary/SVXMLCTreeCtrl.h"

//Note this declaration is also in SVObserver\TextDevinesSvo.h
const TCHAR* const ErrorUsedPreviouslyForSec = _T("This was previously used for .sec files.");

class SVOFileConfigDlg;

class SVOFileConfigApp : public CWinApp
{
	COleTemplateServer m_server;

protected:
	DECLARE_SERIAL( SVOFileConfigApp )

public:
	SVOFileConfigDlg *pdlg;
	SVXMLCTreeCtrl::SVBranchHandle m_Inspection;
	unsigned long ulVersion;
	virtual void LoadIODoc( LPCTSTR szFileName );
	virtual void LoadIPDoc( LPCTSTR szFileName );
	SVOFileConfigApp();
	virtual ~SVOFileConfigApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOFileConfigApp)
	public:
	virtual BOOL InitInstance();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(SVOFileConfigApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern SVOFileConfigApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOFileConfig\SVOFileConfig.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 11:31:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Sep 2012 18:06:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:01:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
