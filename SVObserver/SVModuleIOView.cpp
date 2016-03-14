//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVModuleIOView.cpp
//* .File Name       : $Workfile:   SVModuleIOView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:36:46  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"

#include "SVIODoc.h"
#include "SVIOAdjustDialog.h"
#include "SVInputObjectList.h"
#include "SVDDEInputObject.h"
#include "SVRemoteInputObject.h"
#include "SVDigitalInputObject1.h"

#include "SVModuleIOView.h"
#include "SVInfoStructs.h"
#include "SVConfigurationObject.h"

#include "SVSVIMStateClass.h"
#include "SVMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVModuleIOView

IMPLEMENT_DYNCREATE(SVModuleIOView, CListView)

BEGIN_MESSAGE_MAP(SVModuleIOView, CListView)
	//{{AFX_MSG_MAP(SVModuleIOView)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY( )
	//}}AFX_MSG_MAP
	// Standard-Druckbefehle
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVModuleIOView Konstruktion/Destruktion

SVModuleIOView::SVModuleIOView()
: CListView()
{
}

SVModuleIOView::~SVModuleIOView()
{
}

BOOL SVModuleIOView::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// SVModuleIOView Zeichnen

void SVModuleIOView::OnDraw( CDC* pDC )
{
	SVIODoc* pDoc = GetDocument();
	ASSERT_VALID( pDoc );
}

void SVModuleIOView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();
}

/////////////////////////////////////////////////////////////////////////////
// SVModuleIOView Drucken

/////////////////////////////////////////////////////////////////////////////
// SVModuleIOView Diagnose

#ifdef _DEBUG
void SVModuleIOView::AssertValid() const
{
	CListView::AssertValid();
}

void SVModuleIOView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

SVIODoc* SVModuleIOView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(SVIODoc)));
	return (SVIODoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// SVModuleIOView Nachrichten-Handler

BOOL SVModuleIOView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	dwStyle |= LVS_REPORT; //LVS_ICON; //LVS_REPORT;

	BOOL RetVal = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	ImageList.Create( 16, 16, TRUE, 5, 5 );                      // 20 Apr 1999 - frb.
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_IOITEM_ICON ) );
	ImageList.Add( AfxGetApp()->LoadIcon( IDI_NOIOITEM_ICON ) );

	StateImageList.Create( 16, 16, TRUE, 2, 2 );                 // 20 Apr 1999 - frb.
	StateImageList.Add( AfxGetApp()->LoadIcon( IDI_INPUT_ICON ) );
	StateImageList.Add( AfxGetApp()->LoadIcon( IDI_OUTPUT_ICON ) );

	GetListCtrl().SetImageList( &StateImageList, LVSIL_STATE );
	GetListCtrl().SetImageList( &ImageList, LVSIL_NORMAL );
	GetListCtrl().SetImageList( &ImageList, LVSIL_SMALL );

	GetListCtrl().InsertColumn( 0, _T( "Inputs" ), LVCFMT_LEFT, -1, -1 );
	GetListCtrl().InsertColumn( 1, _T( "Description" ), LVCFMT_LEFT, -1, -1 );
	GetListCtrl().InsertColumn( 2, _T( "Forced" ), LVCFMT_LEFT, -1, -1 );
	GetListCtrl().InsertColumn( 3, _T( "Inverted" ), LVCFMT_LEFT, -1, -1 );

	GetListCtrl().SetColumnWidth( 0, 125 );
	GetListCtrl().SetColumnWidth( 1, 500 );
	GetListCtrl().SetColumnWidth( 2,  50 );
	GetListCtrl().SetColumnWidth( 3,  55 );

	return RetVal;
}

void SVModuleIOView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
	SVIODoc* pIODoc = dynamic_cast< SVIODoc* >( GetDocument() );
	long lCount;
	long l;
	SVIOEntryStruct *pTempIO;

	if( pIODoc && ::IsWindow( m_hWnd ) )
	{
		// Delete all existing items
		lCount = GetListCtrl().GetItemCount();
		for( l = 0; l < lCount; l++ )
		{
			pTempIO = (SVIOEntryStruct*) GetListCtrl().GetItemData( l );
			delete pTempIO;
		}// end for

		// First clean up list view
		GetListCtrl().DeleteAllItems();

		CString strItem;
		long lSize;
		long lChannel;
		int h;
		int j;
		int i;
		SVInputObjectList *pInputList;
		SVDigitalInputObject *pDigInput;
		SVDDEInputObject *pDdeInput;
		SVRemoteInputObject *pRemInput;
		SVIOEntryStruct **ppIOEntries;
		SVIOEntryStruct *pIOEntry;

		// Get list of available inputs
		if( !TheSVObserverApp.m_pConfiguration->GetInputObjectList( &pInputList ) )
			DebugBreak();

		// Check if the list is up yet
		if( pInputList  == NULL )
			return;
		
		if( !pInputList->FillInputs( lSize, ppIOEntries ) )
			DebugBreak();

		// Module Inputs
		DWORD maxInput = 0;
		TheSVObserverApp.m_svIOConfig.GetDigitalInputCount( maxInput );
		for( i = 0; i < (long) maxInput; ++i )
		{
			// First column: Module I/O
			strItem.Format( _T( "Digital Input %d" ), i + 1 );
			GetListCtrl().InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE,
									  i, strItem,
									  INDEXTOSTATEIMAGEMASK( 1 ),	// state
									  LVIS_STATEIMAGEMASK,			// stateMask
									  1, 0 );						// Set item data to NULL

			// Find each digital input
			for( j = 0; j < lSize; j++ )
			{
				pIOEntry = ppIOEntries[j];
				if( pIOEntry->eObjectType != IO_DIGITAL_INPUT )
					continue;

				pDigInput = (SVDigitalInputObject*) pIOEntry->pIOObject;

				if( !pDigInput )
					continue;

				if( !pDigInput->GetChannel( lChannel ) )
					DebugBreak();

				if( i == lChannel )
				{
					GetListCtrl().SetItem( i, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0 );
					GetListCtrl().SetItemData( i, (DWORD) pIOEntry );

					// Column: Description
					GetListCtrl().SetItemText( i, 1, pDigInput->GetName() );

					// Column: Force
					if( pDigInput->IsForced() )
					{
						strItem.Format( _T( "%d" ), pDigInput->GetForcedValue() ? 1 : 0 );
						GetListCtrl().SetItemText( i, 2, strItem );
					}// end if

					// Column: Inverted
					strItem.Format( _T( "%s" ), pDigInput->IsInverted() ? _T( "1" ) : _T( "" ) );
					GetListCtrl().SetItemText( i, 3, strItem );

					break;
				}// end if

			}// end for

		}// end for

		h = 0;
		// Find each DDE input
		for( j = 0; j < lSize; j++ )
		{
			pIOEntry = ppIOEntries[j];
			if( pIOEntry->eObjectType != IO_DDE_INPUT )
				continue;

			pDdeInput = (SVDDEInputObject*) pIOEntry->pIOObject;

			if( !pDdeInput )
				continue;

			// First column: Result I/O
			strItem.Format( _T( "DDE Input %d" ), h + 1 );
			GetListCtrl().InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE,
				h + i, strItem,
				INDEXTOSTATEIMAGEMASK( 1 ),	// state
				LVIS_STATEIMAGEMASK,		// stateMask
				1, 0 );						// Set item data to NULL

			GetListCtrl().SetItem( h + i, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0 );
			GetListCtrl().SetItemData( h + i, (DWORD) pIOEntry );

			// Column: Description
			CString strDde;
			CString strItem;
			CString strTemp;

			pDdeInput->GetItemName( strItem );
			pDdeInput->GetDDEName( strDde );
			strTemp.Format( "%s - (%s)", strDde, strItem );
			GetListCtrl().SetItemText( h + i, 1, strTemp );

			// Increment the number of dde inputs
			h++;

		}// end for

		i = i + h;
		h = 0;
		// Find each remote input
		for( j = 0; j < lSize; j++ )
		{
			pIOEntry = ppIOEntries[j];
			if( pIOEntry->eObjectType != IO_REMOTE_INPUT )
				continue;

			pRemInput = (SVRemoteInputObject*) pIOEntry->pIOObject;

			if( !pRemInput )
				continue;

			// First column: Result I/O
			strItem.Format( _T( "Remote Input %d" ), h + 1 );
			GetListCtrl().InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE,
				h + i, strItem,
				INDEXTOSTATEIMAGEMASK( 1 ),	// state
				LVIS_STATEIMAGEMASK,		// stateMask
				1, 0 );						// Set item data to NULL

			GetListCtrl().SetItem( h + i, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0 );
			GetListCtrl().SetItemData( h + i, (DWORD) pIOEntry );

			// Column: Description
			GetListCtrl().SetItemText( h + i, 1, pRemInput->GetName() );

			// Increment the number of remote outputs
			h++;

		}// end for

		delete [] ppIOEntries;

	}// end if

}// end OnUpdate

void SVModuleIOView::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	SVSVIMStateClass l_svState;
	SVInputObjectList *pInputList;
	SVDigitalInputObject *pDigInput;
	SVInputObject *pInput;
	SVIOEntryStruct **ppIOEntries;
	SVIOEntryStruct *pIOEntry;
	UINT flags;
	long lSize;

	int item = GetListCtrl().HitTest( point, &flags );
	SVIODoc* pIODoc = dynamic_cast< SVIODoc* >( GetDocument() );

	if ( ! l_svState.CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) &&
		TheSVObserverApp.OkToEdit() &&
	     pIODoc )
	{
		if( item >= 0 && item < GetListCtrl().GetItemCount() &&
			( flags & ( LVHT_ONITEMSTATEICON | LVHT_ONITEMICON | LVHT_ONITEMLABEL ) ) )
		{
			SVIOAdjustDialogClass dlg;

			// Get list of available inputs
			if( !TheSVObserverApp.m_pConfiguration->GetInputObjectList( &pInputList ) )
				DebugBreak();

			if( !pInputList->FillInputs( lSize, ppIOEntries ) )
				DebugBreak();

			// Search for In or Out
			pIOEntry = (SVIOEntryStruct*) GetListCtrl().GetItemData( item );
			pInput = (SVInputObject*) pIOEntry->pIOObject;
			if( SV_IS_KIND_OF( pInput, SVDDEInputObject ) )
			{
				GetDocument()->OnExtrasEditDDEInputs();
			}// end if
			else if( SV_IS_KIND_OF( pInput, SVRemoteInputObject ) )
			{
				GetDocument()->OnExtrasEditRemoteInputs();
			}// end else if
			else if( SV_IS_KIND_OF( pInput, SVDigitalInputObject ) )
			{
				pDigInput = (SVDigitalInputObject*) pInput;
				if( pDigInput )
				{
					dlg.StrIOName = _T( "Module " ) + GetListCtrl().GetItemText( item, 0 );
					dlg.StrIOName += _T( ", " ) + GetListCtrl().GetItemText( item, 1 );
					dlg.StrIOValue.Format( "%d", pDigInput->GetValue() ? 1 : 0 );
					dlg.m_pDigInput = pDigInput;
					dlg.m_pIOEntry  = pIOEntry;
					dlg.m_bInputMode = TRUE;
					SVSVIMStateClass l_svState;

					l_svState.AddState( SV_STATE_EDITING );
					int nResult = dlg.DoModal();

					switch(nResult)
					{
					case IDOK:
						l_svState.AddState( SV_STATE_MODIFIED );
						break;

					case IDCANCEL:
					default:
						break;
					}// end switch

					OnUpdate( NULL, NULL, NULL );

					l_svState.RemoveState( SV_STATE_EDITING );
				}// end if

			}// end else if

		}// end if

	}// end if

}// end OnLButtonDblClk

void SVModuleIOView::OnDestroy()
{
	SVIOEntryStruct *pIOEntry;
	long lSize;
	long l;

	lSize = GetListCtrl().GetItemCount();
	for( l = 0; l < lSize; l++ )
	{
		pIOEntry = (SVIOEntryStruct*) GetListCtrl().GetItemData( l );
		delete pIOEntry;
	}// end for

	CListView::OnDestroy();
}// end OnDestroy

