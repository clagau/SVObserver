//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : <SVPQVariableSelectionDialog>
//* .File Name       : $Workfile:   SVPQVariableSelectionDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   27 Aug 2014 01:32:28  $
//******************************************************************************

#include "stdafx.h"
#include "SVPQVariableSelectionDialog.h"
#include "SVInfoStructs.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVValueObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVPQVariableSelectionDialog::SVPQVariableSelectionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD,pParent)
{
	//{{AFX_DATA_INIT(SVPQVariableSelectionDialog)
	//}}AFX_DATA_INIT

	csAvailableText.LoadString( IDS_AVAILABLE_DATA ); 

}

void SVPQVariableSelectionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPQVariableSelectionDialog)
	DDX_Control(pDX, IDC_TREE_RESULTS, m_treeCtrl);
	//}}AFX_DATA_MAP
}

void SVPQVariableSelectionDialog::SetCaptionTitle( LPCTSTR pszCaptionTitle )
{
	m_csCaptionTitle = pszCaptionTitle;       // Dialog bar text
}

BEGIN_MESSAGE_MAP(SVPQVariableSelectionDialog, CDialog)
	//{{AFX_MSG_MAP(SVPQVariableSelectionDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SVPQVariableSelectionDialog message handlers

BOOL SVPQVariableSelectionDialog::OnInitDialog() 
{
	SetWindowText( m_csCaptionTitle );

	SetDlgItemText( IDC_AVAILABLE_TEXT, csAvailableText );

	CDialog::OnInitDialog();

	m_treeCtrl.uRequiredAttributes = SV_VIEWABLE;
	m_treeCtrl.InitTreeCtrl();

	size_t l;
	SVIOEntryHostStructPtr pIOEntry;
	HTREEITEM hParent = NULL;       
	HTREEITEM hChild = NULL;

	// Loop through the inputs
	for( l = 0; l < m_ppPPQInputs.size(); l++ )
	{
		pIOEntry = m_ppPPQInputs[l];

		hChild = m_treeCtrl.InsertItem( TVIF_TEXT | TVIF_STATE | TVIF_PARAM,  
										 (LPCTSTR) pIOEntry->m_pValueObject->GetName(),
 										 0, 0, INDEXTOSTATEIMAGEMASK( 1 ),	
										 TVIS_STATEIMAGEMASK, 0, hParent, TVI_LAST );

		// Attach index into the List
		m_treeCtrl.SetItemData( hChild, reinterpret_cast<DWORD_PTR>(pIOEntry->m_pValueObject) );  

		// if selected previously
		// Check the attributes in the base class (SVObject) of the SVPQDataObjectClass
		if( pIOEntry->m_pValueObject->ObjectAttributesSet() & SV_VIEWABLE )
		{
			// Make sure this item is expanded and visible
			m_treeCtrl.EnsureVisible( hChild );
			m_treeCtrl.SetCheckState( hChild, TRUE );
		}// end if

	}// end for

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
//
//
void SVPQVariableSelectionDialog::OnOK() 
{
	m_treeCtrl.UpdateAttributes();

	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
//
//
void SVPQVariableSelectionDialog::OnCancel() 
{
	CDialog::OnCancel();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPQVariableSelectionDialog.cpp_v  $
 * 
 *    Rev 1.4   27 Aug 2014 01:32:28   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fixed input variable sort problem
 * Changed methods: OnInitDialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 May 2014 17:53:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised InsertItem to use proper type for the lParam
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 May 2014 12:32:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SetItemData to cast values to DWORD_PTR.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Oct 2013 13:31:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   64 bit platform types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:33:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   08 Aug 2012 15:59:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code and resources to fix issues with missing linkages for dialogs associated with resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   12 Mar 2012 10:02:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  757
 * SCR Title:  Fix results view to sort PPQ object selected for viewing
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified OnInitDialog to sort the treeCtrl list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   19 Apr 2011 16:22:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   18 Apr 2011 10:59:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   08 Dec 2010 13:29:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Dec 2009 12:29:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jun 2005 08:27:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Feb 2005 15:04:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated header files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 14:07:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Nov 2002 11:01:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed to accommodate new architecture
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 May 2000 17:38:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  141
 * SCR Title:  View DDE Inputs/Outputs on Results View
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SVPQObjectCLass* variable.
 * Removed SelectEntries method.
 * Added GetSelectedEntries method.
 * Revised OnInitDialog to initialize the Tree control.
 * Revised OnOk to update the object attributes and get the selected entries.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 May 2000 09:34:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  141
 * SCR Title:  View DDE Inputs/Outputs on Results View
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * Implements wraper dialog for PPQ data selection tree control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
