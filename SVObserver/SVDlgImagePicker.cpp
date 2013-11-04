//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgImagePicker
//* .File Name       : $Workfile:   SVDlgImagePicker.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:17:50  $
//******************************************************************************

#include "stdafx.h"
#include "SVDlgImagePicker.h"
#include "SVObserver.h"	// for TheSVObserverApp
#include "SVToolSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVDlgImagePicker, CDialog)
	//{{AFX_MSG_MAP(SVDlgImagePicker)
	//}}AFX_MSG_MAP
//	ON_BN_CLICKED(IDC_SELECT_OBJECT_FILTER, OnSelectObjectFilter)
END_MESSAGE_MAP()


SVDlgImagePicker::SVDlgImagePicker(CWnd* pParent /*=NULL*/)
	: CDialog(SVDlgImagePicker::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVDlgImagePicker)
	//}}AFX_DATA_INIT

	m_uAttributesDesired = SV_PUBLISH_RESULT_IMAGE;
	m_pToolSet = NULL;
}


void SVDlgImagePicker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVDlgImagePicker)
	DDX_Control(pDX, IDC_TREE_RESULTS, m_treeImageList);
	//}}AFX_DATA_MAP
}

void SVDlgImagePicker::SetCaptionTitle( LPCTSTR pszCaptionTitle )
{
	csCaptionTitle = pszCaptionTitle;       // Dialog bar text
}

BOOL SVDlgImagePicker::OnInitDialog() 
{
	ASSERT( m_pToolSet != NULL );	// caller needs to set this
	CWaitCursor wait;

	CDialog::OnInitDialog();

	//
	// User specified Dialog caption text if any...
	//
	if(csCaptionTitle.GetLength() > 0)
	{
		SetWindowText( (LPCTSTR)csCaptionTitle );
	}

	m_treeImageList.InitOutputListTreeCtrl();

	m_pToolSet->GetImageList( m_ImageList );

	//
	// Build the output list and set the object 'attributes' to use.
	//
	m_treeImageList.BuildImageListTree( &m_ImageList, m_uAttributesDesired, SVNotSetObjectType );

//#ifdef _DEBUG
	if ( TheSVObserverApp.GetProfileInt(_T("Debug"), _T("SelectObjectFilter"), FALSE ) != FALSE )
	{
		CWnd* pWnd = GetDlgItem( IDC_SELECT_OBJECT_FILTER );
		if ( pWnd )
		{
			pWnd->ShowWindow( SW_SHOW );
		}
	}
//#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVDlgImagePicker::OnOK() 
{
	//
	// Make sure the tree 'states' are converted into 
	// selected 'attributes'.
	//
	m_treeImageList.UpdateAttributesInOutputList();

	CDialog::OnOK();
}

void SVDlgImagePicker::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDlgImagePicker.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:17:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Nov 2005 14:46:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration - allow user to select image like result picker
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/