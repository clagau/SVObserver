//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultView
//* .File Name       : $Workfile:   SVResultView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   17 Jul 2014 20:21:28  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObserver.h"
#include "SVResultView.h"
#include "SVIPDoc.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVResultsWrapperClass.h"
#include "SVSVIMStateClass.h"
#include "SVToolSet.h"
#include "SVInspectionProcess.h"
#include "SVXMLLibrary/SVNavigateTreeClass.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
#include "SVIPChildFrm.h"
#include "EnvironmentObject.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum { SV_NUMBER_RESULTVIEW_COLUMNS = 4 };
#pragma endregion Declarations

IMPLEMENT_DYNCREATE( SVResultViewClass, CListView )

BEGIN_MESSAGE_MAP( SVResultViewClass, CListView )
	//{{AFX_MSG_MAP(SVResultViewClass)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

SVResultViewClass::SVResultViewClass()
: CListView()
{
	CRect r( 0, 0, 0, 0 );
	rect = r;
	oldValue = 0.0;
	oldEndTime = 0.0;

	columnWidthSet = FALSE;
}

SVResultViewClass::~SVResultViewClass()
{
}

SVIPDoc* SVResultViewClass::GetIPDoc() const
{
	return dynamic_cast< SVIPDoc* >( GetDocument() );
}

BOOL SVResultViewClass::OnEraseBkgnd( CDC* p_pDC )
{
	BOOL l_bOk = TRUE;
	
	//if( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) )
	{
		l_bOk = CListView::OnEraseBkgnd( p_pDC );
	}

	return l_bOk;
}

void SVResultViewClass::OnTimer( UINT_PTR nIDEvent ) 
{
	CListView::OnTimer(nIDEvent);
}

void SVResultViewClass::OnInitialUpdate() 
{
	if( GetIPDoc() == NULL )
	{
		AfxMessageBox( "Failed to init inspection result view!" );
		return;
	}

	// Add Column headings
	addColumnHeadings();

	CListView::OnInitialUpdate();
}

void SVResultViewClass::OnDestroy() 
{
	CListView::OnDestroy();
}

void SVResultViewClass::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) 
{
	SVIPDoc* l_pIPDoc = GetIPDoc();

	BOOL Update = TRUE;
	EnvironmentObject::getEnvironmentValue( ::EnvironmentResultUpdate, Update );
	Update = Update || !SVSVIMStateClass::CheckState( SV_STATE_RUNNING );

	if( !Update )
	{
		return;
	}

	if( l_pIPDoc != NULL )
	{
		CListCtrl& listCtrl = GetListCtrl();

		listCtrl.SetRedraw( false );

		l_pIPDoc->GetResultData( m_ResultData );

		if( l_pIPDoc->IsResultDefinitionsUpdated() == S_OK )
		{
			listCtrl.DeleteAllItems();

			l_pIPDoc->GetResultDefinitions( m_ResultDefinitions );
		}

		int i = 0;
		DWORD l_Color = 0x00FFFFFF;

		for( i = 0; i < static_cast< int >( m_ResultDefinitions.size() ); ++i )
		{
			CString l_Name;
			CString l_NameToType;
			CString l_ItemIndex;
			CString l_Value;

			SVIPResultItemDefinition& l_rDef = m_ResultDefinitions[ i ];

			SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( l_rDef.GetObjectID() );

			if( l_pObject != NULL )
			{
				if( l_rDef.GetIndexPresent())
				{
					l_Name.Format( "%s[%d]", l_pObject->GetName(), atol( l_rDef.GetIndex().c_str())+1 );
				}
				else
				{
					l_Name = l_pObject->GetName();
				}
				l_NameToType = l_pObject->GetCompleteObjectNameToObjectType( NULL, SVToolObjectType );
			}

			l_ItemIndex.Format( _T( "%d" ),i );

			SVIPResultData::SVResultDataMap::const_iterator l_Iter = m_ResultData.m_ResultData.find( l_rDef );

			if( l_Iter != m_ResultData.m_ResultData.end() )
			{
				l_Value = l_Iter->second.GetValue().c_str();
				l_Color = l_Iter->second.GetColor();

				if( l_Iter->second.IsIOTypePresent() )
				{
					l_ItemIndex.Empty();

					if( l_Iter->second.GetIOType() == IO_DIGITAL_INPUT )
					{
						l_NameToType.LoadString( IDS_OBJECTNAME_DIGITAL_INPUT );
					}
					else if( l_Iter->second.GetIOType() == IO_REMOTE_INPUT )
					{
						l_NameToType.LoadString( IDS_OBJECTNAME_REMOTE_INPUT );
					}
				}
			}

			if( listCtrl.GetItemCount() <= i )
			{
				listCtrl.InsertItem( i, l_Name );
				listCtrl.SetItemText( i, 2, l_NameToType );
				listCtrl.SetItemText( i, 3, l_ItemIndex );
			}
			else
			{
				CString l_TempName = listCtrl.GetItemText( i, 0 );

				if( l_TempName != l_Name )
				{
					listCtrl.SetItemText( i, 0, l_Name );
					listCtrl.SetItemText( i, 2, l_NameToType );
				}
			}

			listCtrl.SetItemText( i, 1, l_Value );
			listCtrl.SetItemData( i, l_Color );
		}

		CString strTemp;

		if( listCtrl.GetItemCount() <= i )
		{
			// Add the Toolset Time
			strTemp = _T( "Toolset Time" );
			listCtrl.InsertItem( i, strTemp );
		}

		if( listCtrl.GetItemCount() <= ( i + 1 ) )
		{
			// Add the Toolset Complete Process per Second
			strTemp = _T( "Complete Processes per Second" );
			listCtrl.InsertItem( i + 1, strTemp );
		}

		// Get ToolSet Time
		if( m_ResultData.m_ToolSetAvgTime < 0.0 && m_ResultData.m_ToolSetEndTime < 0.0 )
		{
			strTemp.Empty();
		}
		else
		{
			strTemp.Format( _T( "%.3f ms ( %.3f ms )" ), m_ResultData.m_ToolSetEndTime * 1000, m_ResultData.m_ToolSetAvgTime * 1000 );
		}
		listCtrl.SetItemText( i, 1, strTemp );

		// Get ToolSet complete processes per second
		strTemp = CalcProcessesPerSecond( m_ResultData.m_TriggerDistance );
		listCtrl.SetItemText( i + 1, 1, strTemp );
			
		l_Color = 0x00ffffff;
			
		listCtrl.SetItemData( i, l_Color );     // toolset time
		listCtrl.SetItemData( i + 1, l_Color ); // processes/sec

		strTemp = l_pIPDoc->GetCompleteToolSetName();
		listCtrl.SetItemText( i, 2, strTemp );     // toolset time
		listCtrl.SetItemText( i + 1, 2, strTemp ); // processes/sec

		listCtrl.SetRedraw( true );
	}

	//CListView::OnUpdate( pSender, lHint, pHint );   // This call causes flicker
}

CString SVResultViewClass::CalcProcessesPerSecond( double p_LastTriggerDistance )
{
	CString timeStr;
	double dTime;

	// complete processes per second
	dTime = p_LastTriggerDistance / 1000.0;

	if ( dTime != 0.0 )
	{
		timeStr.Format( _T( "%.3f / sec (%.3f / min)" ), 1.0 / dTime, 1.0 / dTime * 60.0 );
	}// end if
	
	return timeStr;
}// end CalcProcessesPerSecond

BOOL SVResultViewClass::Create( LPCTSTR LPSZClassName, LPCTSTR LPSZWindowName, DWORD DwStyle, const RECT& Rect, CWnd* PParentWnd, UINT NID, CCreateContext* PContext ) 
{
	DWORD viewStyle = LVS_REPORT | LVS_NOSORTHEADER | LVS_SINGLESEL | LVS_OWNERDRAWFIXED;
	BOOL rc = CWnd::Create( LPSZClassName, _T( "Result Page" ), DwStyle |viewStyle, Rect, PParentWnd, NID, PContext );
	
	return rc;
}

void SVResultViewClass::addColumnHeadings()
{
	CString columnName;
	CListCtrl& listCtrl = GetListCtrl();
		
	// load the Column names
	for( int i = 0; i < SV_NUMBER_RESULTVIEW_COLUMNS; i++ )
	{
		columnName.LoadString(IDS_RESULTVIEW_COLUMN_NAME0 + i );
		listCtrl.InsertColumn( i, columnName, LVCFMT_LEFT, LVSCW_AUTOSIZE, i );
	}
	columnWidthSet = FALSE;
}

void SVResultViewClass::setColumnWidths()
{
	CListCtrl& listCtrl = GetListCtrl();
	
	if (!columnWidthSet )
	{
		CRect viewRect;
		listCtrl.GetClientRect( viewRect );

		//the last column is only used so that we can use the LVSCW_AUTOSIZE_USEHEADER style
		//on autisizing the 2nd last column.

		//calc the width of the first three columns
		int columnWidth = ( viewRect.Width() - 50 ) / ( SV_NUMBER_RESULTVIEW_COLUMNS - 1 );

		//set the width of the first three columns
		for( int i = 0; i < SV_NUMBER_RESULTVIEW_COLUMNS - 1; i++ )
		{
			listCtrl.SetColumnWidth( i, columnWidth );
		}

		listCtrl.SetColumnWidth(SV_NUMBER_RESULTVIEW_COLUMNS - 1,LVSCW_AUTOSIZE_USEHEADER);
		
		columnWidthSet = TRUE;
	}
}

int SVResultViewClass::addResults( SVResultListClass* pResultList, SVResultsWrapperClass* pSVRWC )
{
	CString szCol1, szCol2, szCol3, szCol4;
	DWORD dwColor = 0;
	size_t i = 0;
	int iNumberOfItems = 0;
	int nType;

	BOOL bRefreshComplete = ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST );

	SVIPDoc* pCurrentDocument = dynamic_cast <SVIPDoc*> (GetDocument());

	for( i = 0; i < pResultList->m_vecObjects.size(); ++i )
	{
		SVObjectReference ref = pResultList->m_vecObjects.at(i);
		if( ref.Object() )
		{
			// Insert all items that are picked for viewing
			szCol1 = ref.GetName();

			dwColor = 0;

			SVValueObjectReference voref(ref);  // try to assign to value object
			if( voref.Object() )                // if successful
			{
				if( voref->GetObjectType() == SVStringValueObjectType)
				{
					CString l_strQuotes;
					CString l_strValue;
					voref.GetValue( l_strValue );
					// Wrap string in Quotes...
					l_strQuotes.Format(_T("\042%s\042"),l_strValue);
					szCol2 = l_strQuotes;
				}
				else
				{
					HRESULT hr = voref.GetValue( szCol2 );
					if ( hr == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE )
						szCol2 = _T("< ") + szCol2 + _T(" >");
					else if ( hr != S_OK )
						szCol2 = _T( "<Not Valid>" );
				}

				// Set Color...
				dwColor = ref.Object()->GetObjectColor();
				if( ref.Object()->GetOwner() )
					dwColor = ref.Object()->GetOwner()->GetObjectColor();

				nType = ref.Object()->GetObjectType();
			}

			szCol3 = ref.GetCompleteObjectNameToObjectType( NULL, SVToolObjectType );
			szCol4.Format(_T("%d"),iNumberOfItems);

			CString szIPD = pCurrentDocument->GetTitle();
			szIPD += _T( ".ipd" );
			pSVRWC->AddData( iNumberOfItems, szIPD, szCol1, szCol2, szCol3, szCol4, dwColor, nType);
			
			++iNumberOfItems;
		}
	}

	return iNumberOfItems;
}// end addResults

/*
void SVResultViewClass::addItems()
{
	DWORD dwColor;

	BOOL bRefreshComplete = ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST );
	CListCtrl& listCtrl = GetListCtrl();

	listCtrl.SetRedraw( FALSE );       // 10 Nov 1999 - frb.

	CRect l_rectTopItem;
	int l_iTopIndex = listCtrl.GetTopIndex();
	BOOL l_bGetItemPosition = listCtrl.GetItemRect( l_iTopIndex, &l_rectTopItem, LVIR_BOUNDS );
	

	if( bRefreshComplete )
	{
		listCtrl.DeleteAllItems();
	}// end if
	
	SVIPDoc* pCurrentDocument = dynamic_cast< SVIPDoc* >( GetDocument() );

	if( pCurrentDocument )
	{
		// Get the ToolSet
		SVToolSetClass* pToolSet;
		pToolSet = pCurrentDocument->GetToolSet();
		if( pToolSet )
		{
			size_t l;
			int iNumberOfItems = 0;
			CString strTemp;

			// Get the OutputList
			SVResultListClass* pResultList = pToolSet->GetResultList();
			if( pResultList )
			{
				SVResultsListCtrlWrapper SVRLCW(&listCtrl);
				iNumberOfItems = addResults( pResultList, &SVRLCW);
			}// end if

			dwColor = 0x00ffffff;
			// Insert all PPQ input items that are picked for viewing
			for( l = 0; pCurrentDocument->GetInspectionProcess() && l < pCurrentDocument->GetInspectionProcess()->m_PPQInputs.size(); l++ )
			{
				SVIOEntryStruct pIOEntry;
				SVValueObjectClass* pObject;			
				pIOEntry = pCurrentDocument->GetInspectionProcess()->m_PPQInputs[l];
				pObject = dynamic_cast <SVValueObjectClass*> (pIOEntry.m_IOEntryPtr->m_pValueObject);
				if ( pObject )
				{
					if( !( pObject->ObjectAttributesSet() & SV_VIEWABLE ) )
						continue;

					if( bRefreshComplete )
					{
						strTemp = pObject->GetName();
						listCtrl.InsertItem( iNumberOfItems, strTemp );
					}// end if

					if( m_lLastResultDataIndex > -1)
					{
						// Get the Data
						pObject->GetValue( m_lLastResultDataIndex, strTemp );
						listCtrl.SetItemText( iNumberOfItems, 1, strTemp );

						// Set Color...
						listCtrl.SetItemData( iNumberOfItems, dwColor );
					}// end if

					if( bRefreshComplete )
					{
						if( pIOEntry.m_IOEntryPtr->m_ObjectType == IO_DIGITAL_INPUT )
						{
							strTemp.LoadString( IDS_OBJECTNAME_DIGITAL_INPUT );
						}
						else if( pIOEntry.m_IOEntryPtr->m_ObjectType == IO_DDE_INPUT )
						{
							strTemp.LoadString( IDS_OBJECTNAME_DDE_INPUT );
						}
						else if( pIOEntry.m_IOEntryPtr->m_ObjectType == IO_REMOTE_INPUT )
						{
							strTemp.LoadString( IDS_OBJECTNAME_REMOTE_INPUT );
						}

						listCtrl.SetItemText( iNumberOfItems, 2, strTemp );
					}// end if

					iNumberOfItems++;

				}// end if ( pObject )

			}

			if( bRefreshComplete )
			{
				// Add the Toolset Time
				strTemp = _T( "Toolset Time" );
				listCtrl.InsertItem( iNumberOfItems, strTemp );

				// Add the Toolset Complete Process per Second
				strTemp = _T( "Complete Processes per Second" );
				listCtrl.InsertItem( iNumberOfItems + 1, strTemp );
			}

			// Get ToolSet Time
			strTemp = CalcToolSetTime( pToolSet );
			listCtrl.SetItemText( iNumberOfItems, 1, strTemp );

			// Get ToolSet complete processes per second
			strTemp = CalcProcessesPerSecond();
			listCtrl.SetItemText( iNumberOfItems + 1, 1, strTemp );
			
			dwColor = 0x00ffffff;
			
			listCtrl.SetItemData( iNumberOfItems, dwColor );     // toolset time
			listCtrl.SetItemData( iNumberOfItems + 1, dwColor ); // processes/sec

			if( bRefreshComplete )
			{
				strTemp = pToolSet->GetCompleteObjectNameToObjectType( NULL, SVToolObjectType ); //pToolSet->GetName();
				listCtrl.SetItemText( iNumberOfItems, 2, strTemp );     // toolset time
				listCtrl.SetItemText( iNumberOfItems + 1, 2, strTemp ); // processes/sec

			}// end if
		}// end if( pToolSet )
	}// end if( pCurrentDocument )

	listCtrl.SetRedraw(TRUE);        // 10 Nov 1999 - frb.
	if ( bRefreshComplete && l_bGetItemPosition )
	{
		listCtrl.Scroll(CSize(0, l_rectTopItem.Height() * l_iTopIndex));
	}
}// end addItems
*/

void SVResultViewClass::OnSize(UINT nType, int cx, int cy)
{
	CListView::OnSize(nType, cx, cy);
}

void SVResultViewClass::SetViewSize(CSize &Size)
{
	CWnd *l_pcWnd = this;
	SVIPSplitterFrame *l_pSplitterFrame = NULL;

	do
	{
		l_pcWnd = l_pcWnd->GetParent();

		if ( l_pcWnd != NULL )
		{
			l_pSplitterFrame = dynamic_cast<SVIPSplitterFrame *>( l_pcWnd );
		}
	}
	while ( l_pSplitterFrame == NULL && l_pcWnd != NULL );

	if ( l_pSplitterFrame != NULL )
	{
		l_pSplitterFrame->SetViewSize( this, Size );
	}
}


void SVResultViewClass::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	CListCtrl& listCtrl = GetListCtrl();

	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	CRect rcItem( lpDrawItemStruct->rcItem );	
	int nItem = lpDrawItemStruct->itemID;
	CImageList* pImageList;
	
	// Save dc state	
	int nSavedDC = pDC->SaveDC();

	// Get item image and state info	
	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE;	lvi.iItem = nItem;	
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;		
	
	// get all state flags	
	listCtrl.GetItem(&lvi);
	
	// Should the item be highlighted
	BOOL bHighlight =( ( lvi.state & LVIS_DROPHILITED ) ||
					   ( ( lvi.state & LVIS_SELECTED ) && ( ( GetFocus() == this ) ||
						 ( GetStyle() & LVS_SHOWSELALWAYS) ) ) );

	// Get rectangles for drawing	
	CRect rcBounds, rcLabel, rcIcon;
	listCtrl.GetItemRect( nItem, rcBounds, LVIR_BOUNDS );
	listCtrl.GetItemRect( nItem, rcLabel, LVIR_LABEL );	
	listCtrl.GetItemRect( nItem, rcIcon, LVIR_ICON );
	CRect rcCol( rcBounds ); 	
	CString sLabel = listCtrl.GetItemText( nItem, 0 );
	DWORD_PTR dwColor = listCtrl.GetItemData( nItem );

	// Labels are offset by a certain amount  
	// This offset is related to the width of a space character
	int offset = pDC->GetTextExtent( _T( " " ), 1 ).cx * 2;	
	CRect rcHighlight;
	CRect rcWnd;	
	int nExt;
	
	nExt = pDC->GetOutputTextExtent( sLabel ).cx + offset;		
			rcHighlight = rcLabel;
		
	if( rcLabel.left + nExt < rcLabel.right )
		rcHighlight.right = rcLabel.left + nExt;


	GetClientRect( &rcWnd );
	rcHighlight = rcBounds;
	rcHighlight.left = rcLabel.left;
	rcHighlight.right = rcWnd.right-1;

	/*
	switch( m_nHighlight )	
	{	
		case 0: 
			nExt = pDC->GetOutputTextExtent( sLabel ).cx + offset;		
			rcHighlight = rcLabel;
		
			if( rcLabel.left + nExt < rcLabel.right )
				rcHighlight.right = rcLabel.left + nExt;		
			break;	
		
		case 1:
			rcHighlight = rcBounds;		
			rcHighlight.left = rcLabel.left;		
			break;	
		
		case 2:
			GetClientRect( &rcWnd );		
			rcHighlight = rcBounds;
			rcHighlight.left = rcLabel.left;		
			rcHighlight.right = rcWnd.right;		
			break;
	
		default:		
			rcHighlight = rcLabel;	
	}
	*/

	// Draw the background color
	
	/*
	if( bHighlight )	
	{		
		pDC->SetTextColor( ::GetSysColor(COLOR_HIGHLIGHTTEXT ) );
		pDC->SetBkColor( dwColor );
		//pDC->FillRect( rcHighlight, &CBrush( ::GetSysColor( COLOR_HIGHLIGHT ) ) );	
		pDC->FillSolidRect( rcHighlight, ::GetSysColor( COLOR_HIGHLIGHT ) );	
	}	
	else
	{
		pDC->SetTextColor( dwColor );
		//pDC->FillRect( rcHighlight, &CBrush( ::GetSysColor( COLOR_WINDOW ) ) );	
		pDC->FillSolidRect( rcHighlight,::GetSysColor( COLOR_WINDOW ) );	
	}
	*/
	
	
	pDC->FillSolidRect( rcHighlight,::GetSysColor( COLOR_WINDOW ) );	
	pDC->SetTextColor( RGB( 0, 0, 0 ) );
	

	// Set clip region	
	rcCol.right = rcCol.left + listCtrl.GetColumnWidth( 0 );	
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcCol );	
	pDC->SelectClipRgn( &rgn );
	rgn.DeleteObject();
	
	// Draw state icon	
	if( lvi.state & LVIS_STATEIMAGEMASK )	
	{
		int nImage = ( ( lvi.state & LVIS_STATEIMAGEMASK ) >> 12 ) - 1;
		pImageList = listCtrl.GetImageList( LVSIL_STATE );		
		if( pImageList )		
		{
			pImageList->Draw( pDC, nImage,
				CPoint( rcCol.left, rcCol.top ), ILD_TRANSPARENT );		
		}	
	}	
	// Draw normal and overlay icon	
	pImageList = listCtrl.GetImageList( LVSIL_SMALL );
	if( pImageList )	
	{		
		UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;
		pImageList->Draw( pDC, lvi.iImage, CPoint( rcIcon.left, rcIcon.top ),
			( bHighlight ? ILD_BLEND50 : 0 ) | ILD_TRANSPARENT | nOvlImageMask );	
	}		
	// Draw item label - Column 0	
	rcLabel.left += offset/2;
	rcLabel.right -= offset;
	pDC->DrawText( sLabel, -1, rcLabel, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP 
					| DT_VCENTER | DT_END_ELLIPSIS);
	
	// Draw labels for remaining columns
	LV_COLUMN lvc;	lvc.mask = LVCF_FMT | LVCF_WIDTH;
	
	rcBounds.right = rcHighlight.right > rcBounds.right ? rcHighlight.right :
							rcBounds.right;	rgn.CreateRectRgnIndirect( &rcBounds );
	pDC->SelectClipRgn( &rgn );				   
	for(int nColumn = 1; listCtrl.GetColumn( nColumn, &lvc ); nColumn++)	
	{
		rcCol.left = rcCol.right;		
		rcCol.right += lvc.cx;

		sLabel = listCtrl.GetItemText( nItem, nColumn );		
		if( sLabel.GetLength() == 0 )
			continue;
		
		// Get the text justification		
		UINT nJustify = DT_LEFT;
		switch(lvc.fmt & LVCFMT_JUSTIFYMASK)		
		{		
			case LVCFMT_RIGHT:
				nJustify = DT_RIGHT;			
				break;
				
			case LVCFMT_CENTER:			
				nJustify = DT_CENTER;
				break;		

			default:			
				break;		
		}		
		rcLabel = rcCol;		
		rcLabel.left += offset;
		rcLabel.right -= offset;
		
		
		//if( bHighlight && nColumn == 1 )
		if( nColumn == 1 )
		{
			int h = rcLabel.Height();
			CRect rcState( rcLabel.left, rcLabel.top + 2, rcLabel.left + h, rcLabel.bottom - 2 );
			
			pDC->FillSolidRect( rcState, static_cast<COLORREF>(dwColor ));
			rcLabel.left += (h + 4);
		}
		
		pDC->DrawText(sLabel, -1, rcLabel, nJustify | DT_SINGLELINE | 
					DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);	
	}
	// Draw focus rectangle if item has focus
	if ( ( lvi.state & LVIS_FOCUSED ) && ( GetFocus() == this ) )
		pDC->DrawFocusRect(rcHighlight);
	
	// Restore dc	
	pDC->RestoreDC( nSavedDC );
}// end DrawItem


void SVResultViewClass::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CListView ::OnWindowPosChanged(lpwndpos);

	
	setColumnWidths();
}

#ifdef _DEBUG
void SVResultViewClass::AssertValid() const
{
	CListView::AssertValid();
}

void SVResultViewClass::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG


void SVResultViewClass::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CListView ::OnRButtonUp(nFlags, point);
}

void SVResultViewClass::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CMenu ResultsMenu;

	if( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_READY ) &&
		TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET ) )
	{
		if( ResultsMenu.LoadMenu( IDR_RESULTS_CONTEXT_MENU ) )
		{
			CMenu* pPopup = ResultsMenu.GetSubMenu( 0 );
			if( nullptr != pPopup )
			{
				CPoint Point = point;
				ClientToScreen( &Point );
				pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, Point.x, Point.y, this );
			}
		}
	}
}

BOOL SVResultViewClass::GetParameters(SVObjectWriter& rWriter)
{
	BOOL bOk = TRUE;

	CRect l_cRect;
	GetWindowRect( l_cRect );

	CString strFreezeScript;
	_variant_t svVariant;

	svVariant = l_cRect.Height();
	rWriter.WriteAttribute(CTAG_CELL_HEIGHT, svVariant);
	svVariant.Clear();

	svVariant = l_cRect.Width();
	rWriter.WriteAttribute(CTAG_CELL_WIDTH, svVariant);
	svVariant.Clear();

	return bOk;
}

BOOL SVResultViewClass::SetParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	BOOL bOk = FALSE;

	CSize l_Size;

	_variant_t svVariant;

	bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_CELL_HEIGHT, htiParent, svVariant );
	if ( bOk )
	{
		l_Size.cy = svVariant;
	}

	if ( bOk )
	{
		bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_CELL_WIDTH, htiParent, svVariant );
		if ( bOk )
		{
			l_Size.cx = svVariant;
		}
	}

	if ( bOk )
	{
		SetViewSize( l_Size );
	}

	return bOk;
}// end SetParameters

BOOL SVResultViewClass::CheckParameters( SVTreeType& rTree, SVTreeType::SVBranchHandle htiParent )
{
	BOOL bOk = FALSE;

	CSize l_Size;

	_variant_t svVariant;

	CRect l_cRect;

	GetWindowRect( l_cRect );

	bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_CELL_HEIGHT, htiParent, svVariant );
	if ( bOk )
	{
		l_Size.cy = svVariant;
	}

	if ( bOk )
	{
		bOk = SVNavigateTreeClass::GetItem( rTree, CTAG_CELL_WIDTH, htiParent, svVariant );
		if ( bOk )
		{
			l_Size.cx = svVariant;
		}
	}

	if ( bOk )
	{
		if( l_Size.cx != l_cRect.Width() || l_Size.cy != l_cRect.Height() )
		{
			SetViewSize( l_Size );

			GetWindowRect( l_cRect );
		}
	}

	return bOk;
}// end CheckParameters

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVResultView.cpp_v  $
 * 
 *    Rev 1.7   17 Jul 2014 20:21:28   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   10 Jul 2014 08:28:34   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  859
 * SCR Title:  Add Results Picker Access via Right-Click (Context Menu) in the Results Area
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Check security settings when calling the context menu for the result picker in the view
 * Method changed OnRButtonDown
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Jun 2014 10:20:12   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Checks the Result display update flag to determine if the view is to be updated.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Oct 2013 12:05:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   23 Sep 2013 11:58:34   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  859
 * SCR Title:  Add Results Picker Access via Right-Click (Context Menu) in the Results Area
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added OnRButtonDown.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 May 2013 16:43:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.52   29 Apr 2013 14:28:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  830
   SCR Title:  Consolidate all Time Stamp and Clock Functionality
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.1   06 May 2013 20:12:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.51   16 Apr 2013 15:35:48   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  822
   SCR Title:  Remove DDE Input and Output functionality from SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed DDE functionality from the source code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 14:58:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.50   10 Jan 2013 16:19:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  800
   SCR Title:  Fix Blob Array Result index string
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added missing log entries.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.49   03 Dec 2012 09:53:22   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  800
   SCR Title:  Fix Blob Array Result index string
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified OnUpdate to display one based array indexes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.48   11 Oct 2012 10:59:38   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated include information based on the navigate tree class moving to the XML library.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.47   28 Sep 2012 14:49:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated saving operation to use the object writer instead of the XML tree.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.46   25 Sep 2012 15:27:00   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated to use new tree type.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.45   18 Sep 2012 18:32:24   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.44   04 Sep 2012 15:37:54   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  602
   SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.43.1.0   30 Nov 2012 08:55:02   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  800
   SCR Title:  Fix Blob Array Result index string
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Modified OnUpdate to display one based array indexes
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.43   07 Aug 2012 16:18:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated views to remove flicker.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.42   09 Jul 2012 14:15:10   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to change the include for SVIM State.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.41   02 Jul 2012 17:33:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  769
   SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to promote new display functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.40   19 Jun 2012 14:17:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  744
   SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.39   03 Oct 2011 10:04:50   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  730
   SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Updated the handshaking between the display object thread and the image view draw functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.38   18 Apr 2011 11:03:02   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.37   13 Dec 2010 11:31:12   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.36   08 Dec 2010 13:35:52   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to include changes in notification functionality using the Observer Design Pattern.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.35   05 Nov 2010 14:58:10   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  707
   SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove the un-necessary IP View Control from the application.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.34   05 Nov 2010 11:13:18   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  703
   SCR Title:  Allow SVObserver to Change Remote Input Values
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove redundent data objects.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.33   28 Oct 2010 14:11:10   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  704
   SCR Title:  Upgrade SVObserver Version for 5.33 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.32   01 Jun 2010 11:03:44   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  693
   SCR Title:  Fix Performance Issue with Inspection Process
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.31   16 Dec 2009 12:34:20   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  677
   SCR Title:  Fix problem in camera notify thread
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Fix issues with includes and comments.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.30   29 May 2008 10:00:22   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  627
   SCR Title:  Add RawData object to the Barcode Analyzer and format read string
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Quotes around strings from SVStringValueObjects displayed in ResultView so Barcode with trailing spaces would be noticed.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.29   21 Jun 2007 14:46:46   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.28   12 Sep 2005 14:23:04   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  511
   SCR Title:  Fix Result View scroll position on Run Once
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     remember scroll position on rebuild
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.27   08 Sep 2005 08:10:12   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     previous revision:
   hopefully fixed bug where view displays incorrectly
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.26   08 Sep 2005 08:08:32   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.25   01 Aug 2005 12:02:02   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     replaced SVVectorObject with SVValueObject
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.24   14 Jul 2005 12:44:18   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     implemented array value object capabilities
   cleaned up code
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.23   27 Jun 2005 09:50:56   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  497
   SCR Title:  Remove Freeze Set from SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Remove freeze variables and methods.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.22   22 Jun 2005 08:58:22   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  464
   SCR Title:  Add array indexing for value objects
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     value object functions now use SVHRESULT
   object attributes use accessor functions
   cleaned up code
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.21   17 Feb 2005 15:25:30   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  456
   SCR Title:  Update Image and Tool Objects to use the new Extent Classes
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Update class with new methodologies based on new base object, extents and reset structure.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.20   16 Feb 2005 15:10:42   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  440
   SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     updated header files
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.19   30 Apr 2003 17:49:06   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  361
   SCR Title:  SVObserver's display may become incorrect when running at very high speeds
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed display logic to make a copy of the result indexes from the product before starting the SLOW display process so that the product can't be recycled before the display finishes.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.18   22 Apr 2003 15:11:30   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.17   13 Jan 2003 13:02:14   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  307
   SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added new code to fully enable Remote Inputs in the system.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.16   20 Nov 2002 13:09:08   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  272
   SCR Title:  Outputs do not always get set correctly.
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added SVProductInfoStruct references
   Added OnRButtonUp message handler
   Added GetParameters and SetParameters functions
   Cosmetic function name changes
   Changed CalcProcessesPerSecond function behavior
   Changed to accommodate new architecture
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.15   31 Oct 2001 07:26:14   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  249
   SCR Title:  Fix SVObserver memory leaks
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
     Modified the following functions: SVResultViewClass::FreezePanel,  SVResultViewClass::addItems.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.14   13 Jun 2001 10:02:00   ed
   Project:  SVObserver
   Change Request (SCR) nbr:  200
   SCR Title:  Update code to implement SVIM COM Server
   Checked in by:  Ed;  Ed Chobanoff
   Change Description:  
     Rearranged #include statements.
   Modified the following functions: FreezePanel, addColumnHeadings, DrawItem.
   Added the following functions: addResults.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   19 Apr 2001 19:20:34   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  196
   SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
   Checked in by:  Steve;  Stephen E. Steffan
   Change Description:  
     Header file changes due to include file restructuring.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.13   19 Apr 2001 15:30:38   Steve
   Project:  SVObserver
   Change Request (SCR) nbr:  191
   SCR Title:  Restructure File Management
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Header file changes due to include file restructuring.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12.1.0   11 May 2001 09:16:32   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  197
   SCR Title:  Fix lock-ups in application when loading configuration from SVFocusNT
   Checked in by:  rSchock;  Rosco Schock
   Change Description:  
     Changed SVObserver state code in
   FreezePanel
   addItems
   DrawItem
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.12   17 May 2000 17:41:58   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  141
   SCR Title:  View DDE Inputs/Outputs on Results View
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised addItems method to add Viewed PPQ Inputs to the
   list control.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.11   07 May 2000 23:30:02   Jim
   Project:  SVObserver
   Change Request (SCR) nbr:  137
   SCR Title:  Remove the need to explicitly reference specific version numbers.
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     Changed the Serialization () functions so that versioning does not need to be explicitly addressed unless a change actually affects file compatabillity.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.10   19 Apr 2000 16:37:54   Jim
   Project:  SVObserver
   Change Request (SCR) nbr:  128
   SCR Title:  Suport for 3.11 versioning
   Checked in by:  JimAdmin;  James A. Brown
   Change Description:  
     SVResultViewClass::Serialize () was modified to support program version number 3.11 beta 1.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.9   23 Mar 2000 17:35:48   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  118
   SCR Title:  Integration of LUT Operator
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised FreezePanel(...) to freeze VectorObjects as well.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.8   23 Mar 2000 16:42:38   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  118
   SCR Title:  Integration of LUT Operator
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Introduced Vector Object into Result List.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   14 Mar 2000 14:42:58   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  101
   SCR Title:  Versioning 3.10
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised version number to 3.10 release (0x00030AFF)
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   09 Mar 2000 08:53:04   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  101
   SCR Title:  Versioning 3.10
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised Version number from 3.10 Beta 3 to 3.10 Beta 4
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   07 Mar 2000 14:07:44   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  101
   SCR Title:  Versioning 3.10
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Changed Version number to 3.1 Beta 3.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   07 Mar 2000 12:44:28   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  105
   SCR Title:  Handling of Tool Figures in Absolute, Relative, and Transformed Views
   Checked in by:  sJones;  Steve Jones
   Change Description:  
     Revised OnInitialUpdate to correct placement of view. 
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   Feb 21 2000 10:34:36   Nick
   Project:  SVObserver
   Change Request (SCR) nbr:  109
   SCR Title:  Freeze on Reject for v3.1
   Checked in by:  Nick;  F Roland "Nick" Bjorklund
   Change Description:  
     Revised to work with freeze panel
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   17 Feb 2000 09:16:08   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  96
   SCR Title:  Scroll/Zoom images displayed in the inspection document.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     Updated the standard PVCS Header
   
   /////////////////////////////////////////////////////////////////////////////////////
*/