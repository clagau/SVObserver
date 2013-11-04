//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilityArgumentDialogClass
//* .File Name       : $Workfile:   SVUtilityArgumentDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:57:50  $
//******************************************************************************

#include "stdafx.h"
#include "SVUtilityArgumentDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVUtilityArgumentDialogClass dialog

SVUtilityArgumentDialogClass::SVUtilityArgumentDialogClass(CWnd* pParent /*=NULL*/)
	: CDialog(SVUtilityArgumentDialogClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVUtilityArgumentDialogClass)
	mszArguments = _T("");
	//}}AFX_DATA_INIT
}

void SVUtilityArgumentDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVUtilityArgumentDialogClass)
	DDX_Text(pDX, IDC_UTILITY_ARGUMENTS, mszArguments);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVUtilityArgumentDialogClass, CDialog)
	//{{AFX_MSG_MAP(SVUtilityArgumentDialogClass)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVUtilityArgumentDialog.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:57:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 17:30:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */