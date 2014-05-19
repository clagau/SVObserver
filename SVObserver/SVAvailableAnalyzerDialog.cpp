//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAvailableAnalyzerDialog
//* .File Name       : $Workfile:   SVAvailableAnalyzerDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 May 2014 10:14:16  $
//******************************************************************************

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// General Include File(s)
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

////////////////////////////////////////////////////////////////////////////////
// Other Necessary Include File(s) - Module Link(s)
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Prototyping
////////////////////////////////////////////////////////////////////////////////

#include "SVAvailableAnalyzerDialog.h"

//******************************************************************************
//* DEFINITIONS OF MODULE-LOCAL VARIABLES:
//******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//******************************************************************************
//* CLASS METHOD IMPLEMENTATION(S):
//******************************************************************************



//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/
//* Class Name : SVAvailableAnalyzerDialogClass
//* Note(s)    : Dialog
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/


SVAvailableAnalyzerDialogClass::SVAvailableAnalyzerDialogClass( CWnd* pParent /*=NULL*/ )
							   :CDialog( SVAvailableAnalyzerDialogClass::IDD, pParent )
{
	//{{AFX_DATA_INIT(SVAvailableAnalyzerDialogClass)
		// NOTE - the ClassWizard will add and remove here.
	//}}AFX_DATA_INIT

	PAvailableAnalyzerList = NULL;
}

void SVAvailableAnalyzerDialogClass::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVAvailableAnalyzerDialogClass)
	DDX_Control(pDX, IDC_AVAILABLE_ANALYZER_LIST, AvailableAnalyzerListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP( SVAvailableAnalyzerDialogClass, CDialog )
	//{{AFX_MSG_MAP(SVAvailableAnalyzerDialogClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//******************************************************************************
// Message Handler
//******************************************************************************

BOOL SVAvailableAnalyzerDialogClass::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if( PAvailableAnalyzerList )
	{
		AvailableAnalyzerListCtrl.ModifyStyle( 0, LVS_REPORT );
		// Set column witdh...
		CRect rect;
		AvailableAnalyzerListCtrl.GetClientRect( &rect );
		AvailableAnalyzerListCtrl.InsertColumn( 0, _T( "" ), LVCFMT_LEFT, rect.Width(), -1 );

		// Populate List Ctrl...
		for( int i = 0; i < PAvailableAnalyzerList->GetSize(); ++ i )
		{
			SVClassInfoStruct& rClassInfo = PAvailableAnalyzerList->ElementAt( i );
			int item = AvailableAnalyzerListCtrl.InsertItem( 0, rClassInfo.ClassName );
			if( item >= 0 )
				AvailableAnalyzerListCtrl.SetItemData( item, static_cast<DWORD_PTR>(i) );
		}
		
		UpdateData( FALSE ); // set data to dialog
	}
	else
		// Not valid call...
		SendMessage( WM_CLOSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVAvailableAnalyzerDialogClass::OnCancel() 
{
	CDialog::OnCancel();
}

void SVAvailableAnalyzerDialogClass::OnOK() 
{
	if( PAvailableAnalyzerList )
	{
		int item = -1;
		int listIndex = 0;
		// Get all selected items...
		while( ( item = AvailableAnalyzerListCtrl.GetNextItem( item, LVNI_ALL | LVNI_SELECTED ) ) >= 0 )
		{
			listIndex = ( int ) AvailableAnalyzerListCtrl.GetItemData( item );
			if( listIndex >= 0 && listIndex < PAvailableAnalyzerList->GetSize() )
			{
				// It´s a valid index ...
				// Store the analyzer in the selection list...
				SelectedAnalyzerList.Add( PAvailableAnalyzerList->GetAt( listIndex ) );
			}
		}
	}
	CDialog::OnOK();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVAvailableAnalyzerDialog.cpp_v  $
 * 
 *    Rev 1.1   15 May 2014 10:14:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised SetItemData to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:40:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   17 Apr 2003 16:45:14   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   19 Nov 2002 10:30:56   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Removed COM support
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 11:00:28   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   Nov 10 1999 12:32:08   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  57
   SCR Title:  Version 3.00 Beta 18 Versioning
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Document changed flag set if on Dialog OK button.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.2   25 Aug 1999 12:27:56   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  21
   SCR Title:  Port Gage Tool code from program version 2.42.
   Checked in by:  Robert Massinger
   Change Description:  
     Ported to 3.0. Used by TA Multiple Analyzer Page.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/