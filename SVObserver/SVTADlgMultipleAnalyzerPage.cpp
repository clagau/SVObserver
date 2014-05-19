//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogMultipleAnalyzerPage
//* .File Name       : $Workfile:   SVTADlgMultipleAnalyzerPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 12:50:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVTADlgMultipleAnalyzerPage.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVAvailableAnalyzerDialog.h"
#include "SVChildrenSetupDialog.h"
#include "SVGetObjectDequeByTypeVisitor.h"
#include "SVGlobal.h"
#include "SVIPDoc.h"
#include "SVInspectionProcess.h"
#include "SVLineAnalyzer.h"
#include "SVResult.h"
#include "SVSetupDialogManager.h"
#include "SVTool.h"
#include "SVToolAdjustmentDialogSheetClass.h"

////////////////////////////////////////////////////////////////////////////////
// Prototyping
////////////////////////////////////////////////////////////////////////////////


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
//* Class Name : SVToolAdjustmentDialogMultipleAnalyzerPageClass
//* Note(s)    : Property Page
//*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/*\*/


////////////////////////////////////////////////////////////////////////////////
// .Title       : SVToolAdjustmentDialogMultipleAnalyzerPageClass
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :01.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVToolAdjustmentDialogMultipleAnalyzerPageClass::SVToolAdjustmentDialogMultipleAnalyzerPageClass( SVToolAdjustmentDialogSheetClass* PParent ) : CPropertyPage( SVToolAdjustmentDialogMultipleAnalyzerPageClass::IDD )
{
	//{{AFX_DATA_INIT(SVToolAdjustmentDialogMultipleAnalyzerPageClass)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT

	if( pSheet = PParent )
	{
		pTool = pSheet->GetTool();

		if( pTool )
		{
			SVObjectTypeInfoStruct info;
			info.ObjectType = SVAnalyzerObjectType;

			::SVSendMessage( pTool, SVM_GETAVAILABLE_OBJECTS, reinterpret_cast<DWORD_PTR>(&availableAnalyzers), reinterpret_cast<DWORD_PTR>(&info) );

			pCurrentAnalyzer = reinterpret_cast<SVAnalyzerClass *>( ::SVSendMessage( pTool, SVM_GETFIRST_OBJECT, NULL, reinterpret_cast<DWORD_PTR>(&info) ) );
		}
	}
	else
	{
		pTool = NULL;
		pCurrentAnalyzer = NULL;
	}
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : ~SVToolAdjustmentDialogMultipleAnalyzerPageClass
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :01.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
SVToolAdjustmentDialogMultipleAnalyzerPageClass::~SVToolAdjustmentDialogMultipleAnalyzerPageClass()
{
}

HRESULT SVToolAdjustmentDialogMultipleAnalyzerPageClass::SetInspectionData()
{
	HRESULT l_hrOk = S_FALSE;

	UpdateData( TRUE ); // get data from dialog

	l_hrOk = RunOnce( pTool );

	UpdateData( FALSE );

	return l_hrOk;
}


////////////////////////////////////////////////////////////////////////////////
// .Title       : redrawList
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :11.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVToolAdjustmentDialogMultipleAnalyzerPageClass::redrawList()
{
	CWnd* pWnd = NULL;

	// Clean up list anyway...
	multipleAnalyzerListCtrl.DeleteAllItems();

	if( pTool )
	{
		// Enable List Ctrl...
		if( pWnd = GetDlgItem( IDC_MULTIPLE_ANALYZER_LIST ) )
			pWnd->EnableWindow( TRUE );

		// Enable/Disable Insert Analyzer Button...
		if( pWnd = GetDlgItem( IDC_INSERT_ANALYZER_BUTTON ) )
			pWnd->EnableWindow( availableAnalyzers.GetSize() > 0 );

		SVObjectTypeInfoStruct analyzerInfo;
		analyzerInfo.ObjectType = SVAnalyzerObjectType;

		SVObjectTypeInfoStruct resultInfo;
		resultInfo.ObjectType = SVResultObjectType;



		int i = 0;
		int item;

		SVGetObjectDequeByTypeVisitor l_AnalyzerVisitor( analyzerInfo );

		SVObjectManagerClass::Instance().VisitElements( l_AnalyzerVisitor, pTool->GetUniqueObjectID() );

		SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

		if( l_AnalyzerVisitor.GetObjects().empty() )
		{
			// No Analyzer selected...
			multipleAnalyzerListCtrl.SetItemData( multipleAnalyzerListCtrl.InsertItem( 0, _T( "(No Analyzer)" ) ), static_cast<DWORD_PTR>(NULL) );

			// Disable buttons...
			if( pWnd = GetDlgItem( IDC_RESULT_BUTTON ) )
			pWnd->EnableWindow( FALSE );

			if( pWnd = GetDlgItem( IDC_DETAILS_BUTTON ) )
				pWnd->EnableWindow( FALSE );

			// Set data to dialog...
			UpdateData( FALSE ); 

			return;
		}

		for( l_Iter = l_AnalyzerVisitor.GetObjects().begin(); l_Iter != l_AnalyzerVisitor.GetObjects().end(); ++l_Iter )
		{
			SVAnalyzerClass* pAnalyzer = dynamic_cast< SVAnalyzerClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

			if( pAnalyzer != NULL )
			{
				CString strText;

				// Insert Item...
				item = multipleAnalyzerListCtrl.InsertItem( i , pAnalyzer->GetName() );
				multipleAnalyzerListCtrl.SetItemData( item, reinterpret_cast<DWORD_PTR>(pAnalyzer) );
				multipleAnalyzerListCtrl.SetItemText( item, 1, pAnalyzer->GetObjectName() );

				SVGetObjectDequeByTypeVisitor l_Visitor( resultInfo );

				SVObjectManagerClass::Instance().VisitElements( l_Visitor, pAnalyzer->GetUniqueObjectID() );

				SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

				for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
				{
					SVResultClass* pResult = dynamic_cast< SVResultClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

					if( pResult != NULL )
					{
						strText += pResult->GetName();
						strText += _T( ", " );
					}
				}


				multipleAnalyzerListCtrl.SetItemText( item, 2, strText );

				// Set current selected...
				if( pAnalyzer == pCurrentAnalyzer )
				{
					multipleAnalyzerListCtrl.SetItemState( item, LVIS_SELECTED | multipleAnalyzerListCtrl.GetItemState( item, 0xFFFF ), LVIS_SELECTED );
				}
			}
		}

		SetInspectionData();
	}
	else
	{
		// Disable List Ctrl...
		if( pWnd = GetDlgItem( IDC_MULTIPLE_ANALYZER_LIST ) )
			pWnd->EnableWindow( FALSE );

		// Disable all special buttons...
		if( pWnd = GetDlgItem( IDC_INSERT_ANALYZER_BUTTON ) )
			pWnd->EnableWindow( FALSE );

		if( pWnd = GetDlgItem( IDC_RESULT_BUTTON ) )
			pWnd->EnableWindow( FALSE );

		if( pWnd = GetDlgItem( IDC_DETAILS_BUTTON ) )
			pWnd->EnableWindow( FALSE );

		// Set data to dialog...
		UpdateData( FALSE ); 
	}
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : DoDataExchange
// -----------------------------------------------------------------------------
// .Description : ...
//              :
// -----------------------------------------------------------------------------
// .Input(s)
//	 Type				Name				Description
//	: 
//  :
// .Output(s)
//	:
//  :
// .Return Value
//	: 
// -----------------------------------------------------------------------------
// .Import Function Reference(s)
//	:
// -----------------------------------------------------------------------------
// .Import Variable Reference(s)
//	:
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :01.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVToolAdjustmentDialogMultipleAnalyzerPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVToolAdjustmentDialogMultipleAnalyzerPageClass)
	DDX_Control(pDX, IDC_MULTIPLE_ANALYZER_LIST, multipleAnalyzerListCtrl);
	//}}AFX_DATA_MAP
}

//******************************************************************************
// Message Map Entries
//******************************************************************************

BEGIN_MESSAGE_MAP(SVToolAdjustmentDialogMultipleAnalyzerPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVToolAdjustmentDialogMultipleAnalyzerPageClass)
	ON_BN_CLICKED(IDC_DETAILS_BUTTON, OnButtonDetails)
	ON_BN_CLICKED(IDC_INSERT_ANALYZER_BUTTON, OnInsertAnalyzerButton)
	ON_BN_CLICKED(IDC_RESULT_BUTTON, OnResultButton)
	ON_NOTIFY(LVN_KEYDOWN, IDC_MULTIPLE_ANALYZER_LIST, OnKeyDownMultipleAnalyzerList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MULTIPLE_ANALYZER_LIST, OnItemChangedMultipleAnalyzerList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//******************************************************************************
// Message Handler
//******************************************************************************

BOOL SVToolAdjustmentDialogMultipleAnalyzerPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	SetTaskObject( pTool );

	if( pTool )
	{
		multipleAnalyzerListCtrl.ModifyStyle( 0, LVS_REPORT );
		multipleAnalyzerListCtrl.InsertColumn( 0, _T( "ID" ), LVCFMT_LEFT, -1, -1 );
		multipleAnalyzerListCtrl.InsertColumn( 1, _T( "Analyzer" ), LVCFMT_LEFT, -1, -1 );
		multipleAnalyzerListCtrl.InsertColumn( 2, _T( "Result" ), LVCFMT_LEFT, -1, -1 );

		// Set column width...
		CRect rect;
		multipleAnalyzerListCtrl.GetClientRect( &rect );
		int normalizedColumnWidth = rect.Width() / 6;
		multipleAnalyzerListCtrl.SetColumnWidth( 0, 2 * normalizedColumnWidth );
		multipleAnalyzerListCtrl.SetColumnWidth( 1, 2 * normalizedColumnWidth );
		multipleAnalyzerListCtrl.SetColumnWidth( 2, 2 * normalizedColumnWidth );

	/*
		CImageList* pImageList = new CImageList;
		pImageList->Create( 16, 16, TRUE, 5, 5 );
		pImageList->Add( AfxGetApp()->LoadIcon( IDI_IOITEM_ICON ) );
		pImageList->Add( AfxGetApp()->LoadIcon( IDI_NOIOITEM_ICON ) );
	
		CImageList* pStateImageList = new CImageList;
		pStateImageList->Create( 16, 16, TRUE, 2, 2 );
		pStateImageList->Add( AfxGetApp()->LoadIcon( IDI_INPUT_ICON ) );
		pStateImageList->Add( AfxGetApp()->LoadIcon( IDI_OUTPUT_ICON ) );


		GetListCtrl().SetImageList( pStateImageList, LVSIL_STATE );
		GetListCtrl().SetImageList( pImageList, LVSIL_NORMAL );
		GetListCtrl().SetImageList( pImageList, LVSIL_SMALL );
	*/

		redrawList();

		return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
	}

	// Not valid call...try to close!
	if( GetParent() )
		GetParent()->SendMessage( WM_CLOSE );
	else
		SendMessage( WM_CLOSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVToolAdjustmentDialogMultipleAnalyzerPageClass::OnButtonDetails() 
{
	if( pCurrentAnalyzer )
	{
		SVSetupDialogManager::Instance().SetupDialog( pCurrentAnalyzer->GetClassID(), pCurrentAnalyzer->GetUniqueObjectID(), this );

		// And now redraw the analyzers...
		redrawList();
	}
}



void SVToolAdjustmentDialogMultipleAnalyzerPageClass::OnInsertAnalyzerButton() 
{
	CWaitCursor l_cwcMouse;

	if( pTool )
	{
		// Prepare available analyzer dialog...
		SVAvailableAnalyzerDialogClass dlg;
		dlg.PAvailableAnalyzerList = &availableAnalyzers;

		dlg.DoModal();

		SVAnalyzerClass* pAnalyzer;
		for( int i = 0; i < dlg.SelectedAnalyzerList.GetSize(); ++ i )
		{
			// and now Instantiate a new Object
			SVObjectManagerClass::Instance().ConstructObject( dlg.SelectedAnalyzerList[ i ].ClassId, pAnalyzer );

			if( pAnalyzer )
			{
				pTool->Add( pAnalyzer );

				// Ensure this Object's inputs get connected
				::SVSendMessage( pAnalyzer, SVM_CONNECT_ALL_INPUTS, NULL, NULL );

				// And last - Create (initialize) it

				if( ! pAnalyzer->IsCreated() )
				{
					// And finally try to create the child object...
					if( ::SVSendMessage( pTool, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pAnalyzer), SVMFSetDefaultInputs | SVMFResetInspection ) != SVMR_SUCCESS )
					{
						AfxMessageBox( _T( "Creation of Analyzer Failed" ) );
						
						// Remove it from the Tool TaskObjectList ( Destruct it )
						GUID objectID = pAnalyzer->GetUniqueObjectID();
						if( objectID != SVInvalidGUID )
							pTool->Delete( objectID );
						else
							delete pAnalyzer;

						pAnalyzer = NULL;
					}
				}
			}
			else
			{
				AfxMessageBox( _T( "Instantiation of Analyzer Failed" ) );
			}
		}

		// Refresh...
		redrawList();
	}
}

void SVToolAdjustmentDialogMultipleAnalyzerPageClass::OnItemChangedMultipleAnalyzerList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	int item = multipleAnalyzerListCtrl.GetSelectionMark();
	if( item >= 0 )
	{
		pCurrentAnalyzer = ( SVAnalyzerClass* )	multipleAnalyzerListCtrl.GetItemData( item );
	}
	else
		pCurrentAnalyzer = NULL;

	CWnd* pWnd;
	if( pWnd = GetDlgItem( IDC_RESULT_BUTTON ) )
		pWnd->EnableWindow( pCurrentAnalyzer != NULL );

	if( pWnd = GetDlgItem( IDC_DETAILS_BUTTON ) )
		pWnd->EnableWindow( pCurrentAnalyzer != NULL );

	// Set data to dialog...
	UpdateData( FALSE ); 

	*pResult = 0;
}


void SVToolAdjustmentDialogMultipleAnalyzerPageClass::OnResultButton() 
{
	if( pCurrentAnalyzer )
	{
		// Get Available Results...
		SVClassInfoStructListClass	availableResults;
		SVObjectTypeInfoStruct		resultTypeInfo;
		resultTypeInfo.ObjectType = SVResultObjectType;
		::SVSendMessage( pCurrentAnalyzer, SVM_GETAVAILABLE_OBJECTS, reinterpret_cast<DWORD_PTR>(&availableResults), reinterpret_cast<DWORD_PTR>(&resultTypeInfo) );

		// Get Dialog Title...
		CString strTitle;
		strTitle.LoadString( IDS_RESULT_ADJUSTMENT_DIALOG_TITLE );
		// Get Complete Name up to the tool level...
		strTitle = pCurrentAnalyzer->GetCompleteObjectNameToObjectType( NULL, SVToolSetObjectType ) + SV_TSTR_SPACE + strTitle;

		SVIPDoc* l_pIPDoc = NULL;

		if( pCurrentAnalyzer->GetInspection() != NULL )
		{
			l_pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( pCurrentAnalyzer->GetInspection()->GetUniqueObjectID() );
		}

		// Prepare result adjustment dialog...
		SVChildrenSetupDialogClass dlg;
		dlg.PDocument = l_pIPDoc;
		dlg.BAllowMultipleChildrenInstances = FALSE;
		dlg.PParentObject					= pCurrentAnalyzer;
		dlg.PAvailableChildrenList			= &availableResults;
		dlg.StrTitle						= strTitle;

		// Call dialog...
		dlg.DoModal();

		// Use, if dialog was cancled parsed analyzer pointer...
		pCurrentAnalyzer = ( SVAnalyzerClass* ) dlg.PParentObject;

		// And now refresh...
		redrawList();
	}
}


void SVToolAdjustmentDialogMultipleAnalyzerPageClass::OnCancel() 
{
	CPropertyPage::OnCancel();
}


void SVToolAdjustmentDialogMultipleAnalyzerPageClass::OnOK() 
{
	CPropertyPage::OnOK();
}


void SVToolAdjustmentDialogMultipleAnalyzerPageClass::OnKeyDownMultipleAnalyzerList( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;
	if( SV_IS_KIND_OF( pLVKeyDown, LV_KEYDOWN ) )
	{
		switch( pLVKeyDown->wVKey )
		{
			case VK_DELETE:
			{
				if( pCurrentAnalyzer )
				{
					// Close, Disconnect and Delete the Object
					::SVSendMessage( pTool, SVM_DESTROY_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(pCurrentAnalyzer), SVMFSetDefaultInputs );

					pCurrentAnalyzer = NULL;

					// Refresh...
					redrawList();
				}
				return;
			}	// case VK_DELETE

		}	// switch
	}
	*pResult = 0;
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgMultipleAnalyzerPage.cpp_v  $
 * 
 *    Rev 1.2   15 May 2014 12:50:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SendMessage to use proper type cast of DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2014 12:14:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:15:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   30 Jul 2012 13:05:38   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated the parameter change for the visitor fucntionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   27 Jul 2012 09:05:10   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fixed include information and updated overlay collection functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   02 Jul 2012 17:40:16   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   27 Jan 2011 12:02:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   08 Dec 2010 13:39:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   01 Jun 2010 11:10:04   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   09 Aug 2005 08:18:16   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  500
   SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated objects and dialogs to use the ActiveX Interface to update inspection data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   17 Feb 2005 16:04:32   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Modifications to support new reset / create methodology.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   22 Apr 2003 16:12:22   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   20 Nov 2002 13:46:18   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  226
   SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     #include "SVObserver.h"
    TheSVClassRegister
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
