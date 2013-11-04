//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatResultDlgClass
//* .File Name       : $Workfile:   SVPatResultDlgClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:19:36  $
//******************************************************************************

#include "stdafx.h"
#include "SVPatResultDlgClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVPatResultDlgClass dialog

SVPatResultDlgClass::SVPatResultDlgClass(CWnd* pParent /*=NULL*/)
	: CDialog(SVPatResultDlgClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVPatResultDlgClass)
	m_strAngle = _T("0.0");
	m_strScore = _T("0.0");
	m_strXPos = _T("0.0");
	m_strYPos = _T("0.0");
	//}}AFX_DATA_INIT
}

void SVPatResultDlgClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPatResultDlgClass)
	DDX_Text(pDX, IDC_PAT_M_ANGLE, m_strAngle);
	DDX_Text(pDX, IDC_PAT_M_SCORE, m_strScore);
	DDX_Text(pDX, IDC_PAT_X_POSITION, m_strXPos);
	DDX_Text(pDX, IDC_PAT_Y_POSITION, m_strYPos);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVPatResultDlgClass, CDialog)
	//{{AFX_MSG_MAP(SVPatResultDlgClass)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPatResultDlgClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:19:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   31 Dec 2008 11:17:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  644
 * SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised for Pattern Analyzer fixes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:37:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/