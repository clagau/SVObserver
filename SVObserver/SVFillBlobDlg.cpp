// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVFillBlobDlg.cpp
// * .File Name       : $Workfile:   SVFillBlobDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   12 Nov 2014 07:05:36  $
// ******************************************************************************

#include "stdafx.h"
#include "svobserver.h"
#include "SVFillBlobDlg.h"

#include "SVTaskObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVFillBlobDlg dialog


SVFillBlobDlg::SVFillBlobDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVFillBlobDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVFillBlobDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

HRESULT SVFillBlobDlg::SetInspectionData()
{
	HRESULT l_hrOk = S_OK;

	bool l_bRequestAdded = false;

	int iCurSel = m_ctlBlobColor.GetCurSel();
	if( iCurSel >= 0 )
	{
		long lValue = ( long ) m_ctlBlobColor.GetItemData( iCurSel );

		if( AddInputRequest( m_pvoBlobFillColor, lValue ) == S_OK )
		{
			l_bRequestAdded = true;
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	iCurSel = m_ctlBlobType.GetCurSel();
	if( iCurSel >= 0 )
	{
		long lValue = ( long ) m_ctlBlobType.GetItemData( iCurSel );

		if( AddInputRequest( m_pvoBlobType, lValue ) == S_OK )
		{
			l_bRequestAdded = true;
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}

	if( l_hrOk == S_OK && l_bRequestAdded )
	{
		l_hrOk = AddInputRequestMarker();

		if( l_hrOk == S_OK )
		{
			l_hrOk = RunOnce( m_pvoBlobType->GetTool() );
		}
	}

	return l_hrOk;
}


void SVFillBlobDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVFillBlobDlg)
	DDX_Control(pDX, IDC_COMBO_BLOB_TYPE, m_ctlBlobType);
	DDX_Control(pDX, IDC_COMBO_BLOB_COLOR, m_ctlBlobColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVFillBlobDlg, CDialog)
	//{{AFX_MSG_MAP(SVFillBlobDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_BLOB_COLOR, OnSelchangeComboBlobColor)
	ON_CBN_SELCHANGE(IDC_COMBO_BLOB_TYPE, OnSelchangeComboBlobType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVFillBlobDlg message handlers

BOOL SVFillBlobDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetTaskObject( dynamic_cast<SVTaskObjectClass *>( m_pvoBlobType->GetOwner() ) );

	FillCombos();
	// TODO: Add extra initialization here

	//use the value object pointers to fill in the combo box and current values
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void SVFillBlobDlg::FillCombos()
{
	CString l_sBlobTypeStr;
	CString l_sBlobColorStr;

	m_pvoBlobType->GetEnumTypes(l_sBlobTypeStr);
	m_ctlBlobType.SetEnumTypes(l_sBlobTypeStr);

	CString l_sCurrentType;
	m_pvoBlobType->GetValue(l_sCurrentType);
	m_ctlBlobType.SelectString(-1,l_sCurrentType);



	CString l_sCurrentColor;
	m_pvoBlobFillColor->GetEnumTypes(l_sBlobColorStr);
	m_ctlBlobColor.SetEnumTypes(l_sBlobColorStr);
	m_pvoBlobFillColor->GetValue(l_sCurrentColor);
	m_ctlBlobColor.SelectString(-1,l_sCurrentColor);

	UpdateData( FALSE );
	
}


void SVFillBlobDlg::OnSelchangeComboBlobColor() 
{
	SetInspectionData();
}

void SVFillBlobDlg::OnSelchangeComboBlobType() 
{
	SetInspectionData();
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVFillBlobDlg.cpp_v  $
 * 
 *    Rev 1.1   12 Nov 2014 07:05:36   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  938
 * SCR Title:  Add Black Blob Mode to Blob Analyzer (SVO-336)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   rename m_pvoBlobFillColor instead m_pvoBlobColor to clarify this parameter
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:38:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   11 Aug 2005 13:18:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added UpdateData( FALSE ) to OnInitDialog method to make sure the inspection data was pushed to the dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Aug 2005 15:59:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Nov 2003 07:40:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the layout of the dialog and changed logic based on if fill blobs is turned on
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Oct 2003 14:05:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  392
 * SCR Title:  Blob Analyzer have option for filling in the blobs
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/