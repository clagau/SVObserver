//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgCharLabel
//* .File Name       : $Workfile:   SVDlgCharLabel.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 09:25:32  $
//******************************************************************************

#include "stdafx.h"
#include "SVOCRGrayFontTrain.h"
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOCRGrayFontTrain\SVDlgCharLabel.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 09:25:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 15:06:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/