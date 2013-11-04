//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVDialog
//* .File Name       : $Workfile:   SVOCVDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:58:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCVDialog.h"
#include "SVIPDoc.h"
#include "SVTool.h"
#include "SVUnaryImageOperatorList.h"
#include "SVOCVAnalyzerResult.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVOCVDialogClass::SVOCVDialogClass(CWnd* pParent /*=NULL*/)
	: CPropertySheet(IDS_OCV_PARAMETERS, pParent)
{

    pOCVAnalyzerResult  = NULL;
	pTool				= NULL;
	pDocument			= NULL;

	AddPageDialogs ();
	//{{AFX_DATA_INIT(SVOCVDialogClass)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVOCVDialogClass::~SVOCVDialogClass()
{
	RemovePage (&m_GeneralParamsDlg);
	RemovePage (&m_MatchStringParamsDlg);
}

void SVOCVDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertySheet::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVOCVDialogClass)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVOCVDialogClass, CPropertySheet)
	//{{AFX_MSG_MAP(SVOCVDialogClass)
	//}}AFX_MSG_MAP
	ON_COMMAND( IDOK, OnOK)
	ON_COMMAND( IDCANCEL, OnCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//
//
//
BOOL SVOCVDialogClass::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	ASSERT(pOCVAnalyzerResult);

	if( pTool && pDocument && pOCVAnalyzerResult )
	{
	}
	
	return bResult;
}

void SVOCVDialogClass::AddPageDialogs ()
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;

	AddPage (&m_GeneralParamsDlg);
	AddPage (&m_MatchStringParamsDlg);
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCVDialogClass::OnOK()
{
	m_GeneralParamsDlg.UpdateOCVParameters ();
	m_MatchStringParamsDlg.UpdateOCVParameters ();

    pDocument->SetModifiedFlag(TRUE);       // Set document 'changed'.

	CPropertySheet::EndDialog(IDOK);
}

/////////////////////////////////////////////////////////////////////////////
//
//
//
void SVOCVDialogClass::OnCancel()
{
	CPropertySheet::EndDialog(IDCANCEL);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCVDialog.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:58:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Feb 2011 15:05:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 12:15:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Sep 2003 13:07:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
