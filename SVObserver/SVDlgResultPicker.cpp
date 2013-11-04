//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgResultPicker
//* .File Name       : $Workfile:   SVDlgResultPicker.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:18:06  $
//******************************************************************************

#include "stdafx.h"
#include "SVDlgResultPicker.h"
#include "SVObserver.h"	// for TheSVObserverApp

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVDlgResultPicker::SVDlgResultPicker(CWnd* pParent /*=NULL*/)
: CDialog(SVDlgResultPicker::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVDlgResultPicker)
	//}}AFX_DATA_INIT

	PTaskObjectList = NULL;	
	uAttributesDesired = SV_SELECTABLE_FOR_EQUATION;
}

void SVDlgResultPicker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVDlgResultPicker)
	DDX_Control(pDX, IDC_TREE_RESULTS, m_treeOutputList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVDlgResultPicker, CDialog)
	//{{AFX_MSG_MAP(SVDlgResultPicker)
	ON_BN_CLICKED(IDC_SELECT_OBJECT_FILTER, OnSelectObjectFilter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Set text for the dialog caption bar.
void SVDlgResultPicker::SetCaptionTitle( LPCTSTR pszCaptionTitle )
{
	csCaptionTitle = pszCaptionTitle;       // Dialog bar text
}

/////////////////////////////////////////////////////////////////////////////
// SVDlgResultPicker message handlers
BOOL SVDlgResultPicker::OnInitDialog() 
{
    CWaitCursor wait;

	CDialog::OnInitDialog();

    //
	// User specified Dialog caption text if any...
	//
	if(csCaptionTitle.GetLength() > 0)
	{
		SetWindowText( (LPCTSTR)csCaptionTitle );
	}

	m_treeOutputList.InitOutputListTreeCtrl();

	//
	// Build the output list and set the object 'attributes' to use.
	//
	m_treeOutputList.BuildOutputList(
		PTaskObjectList,      // SVTaskObjectListClass
		uAttributesDesired    // UINT Attributes we desire
	);

#ifdef _DEBUG
	if ( TheSVObserverApp.GetProfileInt(_T("Debug"), _T("SelectObjectFilter"), FALSE ) != FALSE )
	{
		CWnd* pWnd = GetDlgItem( IDC_SELECT_OBJECT_FILTER );
		if ( pWnd )
		{
			pWnd->ShowWindow( SW_SHOW );
		}
	}
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVDlgResultPicker::OnOK() 
{
	//
	// Make sure the tree 'states' are converted into 
	// selected 'attributes'.
	//
	m_treeOutputList.UpdateAttributesInOutputList();

	CDialog::OnOK();
}

void SVDlgResultPicker::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void SVDlgResultPicker::OnSelectObjectFilter() 
{
	SVSelectObjectFilterDlg dlg;
	dlg.m_pTree = &m_treeOutputList;
	dlg.DoModal();
}

BEGIN_MESSAGE_MAP(SVSelectObjectFilterDlg, CDialog)
	//{{AFX_MSG_MAP(SVSelectObjectFilterDlg)
	ON_BN_CLICKED(IDC_SELECT, OnSelect)
	ON_BN_CLICKED(IDC_DESELECT, OnDeselect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVSelectObjectFilterDlg::SVSelectObjectFilterDlg(CWnd* pParent /*=NULL*/)
: CDialog(SVSelectObjectFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVSelectObjectFilterDlg)
	m_strTextFilter = _T("");
	//}}AFX_DATA_INIT
}

void SVSelectObjectFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVSelectObjectFilterDlg)
	DDX_Control(pDX, IDC_ATTRIBUTE_FILTER, m_lbFilter);
	DDX_Text(pDX, IDC_TEXT_FILTER, m_strTextFilter);
	//}}AFX_DATA_MAP
}

BOOL SVSelectObjectFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_lbFilter.SetItemData( m_lbFilter.AddString(_T("All")), ~0 );
	m_lbFilter.SetItemData( m_lbFilter.AddString(_T("Remotely Settable")), SV_REMOTELY_SETABLE );
	m_lbFilter.SetItemData( m_lbFilter.AddString(_T("Online Settable")), SV_SETABLE_ONLINE );
	//m_lbFilter.SetItemData( m_lbFilter.AddString(_T("Archivable")), SV_ARCHIVABLE );
	//m_lbFilter.SetItemData( m_lbFilter.AddString(_T("Archivable Image")), SV_ARCHIVABLE_IMAGE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVSelectObjectFilterDlg::OnSelect() 
{
	UpdateData();
	SVSelectObjectFilterInfoStruct info;
	info.bOnOff = true;
	info.uAttributeFilter = GetSelectedAttributes();
	info.strTextFilter = m_strTextFilter;
	m_pTree->SelectObjectFilter( info );
}

void SVSelectObjectFilterDlg::OnDeselect() 
{
	UpdateData();
	SVSelectObjectFilterInfoStruct info;
	info.bOnOff = false;
	info.uAttributeFilter = GetSelectedAttributes();
	info.strTextFilter = m_strTextFilter;
	m_pTree->SelectObjectFilter( info );
}

UINT SVSelectObjectFilterDlg::GetSelectedAttributes()
{
	UINT uAttributes = 0;
	int iCount = m_lbFilter.GetSelCount();
	std::vector<int> aSelected(iCount);
	m_lbFilter.GetSelItems(iCount, &(aSelected[0]) );
	for ( int i = 0; i < iCount; i++ )
	{
		uAttributes |= m_lbFilter.GetItemData( aSelected[i] );
	}
	return uAttributes;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVDlgResultPicker.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:18:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   08 Aug 2012 15:59:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code and resources to fix issues with missing linkages for dialogs associated with resources.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   25 Aug 2005 13:46:28   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     added text filter capabilities
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   18 Feb 2005 11:01:08   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  463
   SCR Title:  Add new Linear Measurement Tool to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated the result picker to allow in debug mode to select items by a criteria.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   22 Apr 2003 09:09:14   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   29 Jan 2003 15:27:30   joe
   Project:  SVObserver
   Change Request (SCR) nbr:  301
   SCR Title:  Implement Result Image Circle Buffer
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated include information.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
