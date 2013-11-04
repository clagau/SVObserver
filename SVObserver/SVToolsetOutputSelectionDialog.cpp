//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : <SVToolsetOutputSelectionDialog>
//* .File Name       : $Workfile:   SVToolsetOutputSelectionDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:44:22  $
//******************************************************************************

#include "stdafx.h"
#include "SVToolsetOutputSelectionDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVToolsetOutputSelectionDialog dialog

BEGIN_MESSAGE_MAP(SVToolsetOutputSelectionDialog, SVDlgResultPicker)
	//{{AFX_MSG_MAP(SVToolsetOutputSelectionDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVToolsetOutputSelectionDialog::SVToolsetOutputSelectionDialog(CWnd* pParent /*=NULL*/)
	: SVDlgResultPicker(pParent)
{
	//{{AFX_DATA_INIT(SVToolsetOutputSelectionDialog)
	//}}AFX_DATA_INIT
	m_bUpdateObjectAttributes = false;
}

/////////////////////////////////////////////////////////////////////////////
// SVToolsetOutputSelectionDialog message handlers

BOOL SVToolsetOutputSelectionDialog::OnInitDialog() 
{

	csCaptionTitle.LoadString ( IDS_SELECT_TOOLSET_OUTPUT );

	SVDlgResultPicker::OnInitDialog();

	m_treeOutputList.SetSingleSelect(true);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVToolsetOutputSelectionDialog::OnOK() 
{
	if ( m_bUpdateObjectAttributes )
	{
		m_treeOutputList.UpdateAttributesInOutputList();
	}

	// Get Selected Item
	SVObjectReference ref = m_treeOutputList.GetCheckedObject();
	m_refSelectedObject = ref;
	if( ref.Object() != NULL )
	{
		m_sSelectedOutputName = ref.GetCompleteOneBasedObjectNameToObjectType( NULL, SVToolSetObjectType );
	}

	CDialog::OnOK();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVToolsetOutputSelectionDialog.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:44:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Nov 2012 08:59:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  800
 * SCR Title:  Fix Blob Array Result index string
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Used OneBased name function in OnOk function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   28 Jul 2005 08:16:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added capability for SVToolsetOutputSelectionDialog to update object attributes when done selecting
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Jul 2005 16:04:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to make available the SVObjectReference that was selected
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Jun 2005 15:12:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to use SVObjectReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Jul 2004 12:32:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  429
 * SCR Title:  Fix Select Object tree control
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modified to work with the new tree control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 16:56:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   31 Jan 2003 09:32:40   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Feb 2000 14:17:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  106
 * SCR Title:  Math Tool - List processing
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn. New Class for Selecting a single toolset
 * output via a tree control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/