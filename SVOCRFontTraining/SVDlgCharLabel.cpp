//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgCharLabel
//* .File Name       : $Workfile:   SVDlgCharLabel.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 16:35:52  $
//******************************************************************************

#include "stdafx.h"
#include "svocrfonttraining.h"
#include "SVDlgCharLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVDlgCharLabel dialog

SVDlgCharLabel::SVDlgCharLabel(CWnd* pParent /*=NULL*/)
	: CDialog(SVDlgCharLabel::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVDlgCharLabel)
	m_csCharLabel = _T("");
	//}}AFX_DATA_INIT
}

void SVDlgCharLabel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVDlgCharLabel)
	DDX_Text(pDX, IDC_EDIT_CHAR_LABEL, m_csCharLabel);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVDlgCharLabel, CDialog)
	//{{AFX_MSG_MAP(SVDlgCharLabel)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOCRFontTraining\SVDlgCharLabel.cpp_v  $
 * 
 *    Rev 1.0   24 Apr 2013 16:35:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/