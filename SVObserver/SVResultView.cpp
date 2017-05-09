//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVResultView
//* .File Name       : $Workfile:   SVResultView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.8  $
//* .Check In Date   : $Date:   07 Jan 2015 17:50:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObserver.h"
#include "SVResultView.h"
#include "SVIPDoc.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/GlobalConst.h"
#include "SVOCore/SVSVIMStateClass.h"
#include "SVToolSet.h"
#include "SVInspectionProcess.h"
#include "SVXMLLibrary/SVNavigateTree.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVIPChildFrm.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/ErrorNumbers.h"
#include "TextDefinesSvO.h"
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

#pragma region Constructor
SVResultViewClass::SVResultViewClass()
: CListView()
{
	CRect r( 0, 0, 0, 0 );
	m_Rect = r;
	m_OldValue = 0.0;
	m_OldEndTime = 0.0;

	m_ColumnWidthSet = FALSE;
}

SVResultViewClass::~SVResultViewClass()
{
}
#pragma endregion Constructor

SVIPDoc* SVResultViewClass::GetIPDoc() const
{
	return dynamic_cast< SVIPDoc* >( GetDocument() );
}

BOOL SVResultViewClass::OnEraseBkgnd( CDC* p_pDC )
{
	BOOL l_bOk = CListView::OnEraseBkgnd( p_pDC );

	return l_bOk;
}

void SVResultViewClass::OnTimer( UINT_PTR nIDEvent )
{
	CListView::OnTimer(nIDEvent);
}

void SVResultViewClass::OnInitialUpdate()
{
	if( nullptr == GetIPDoc() )
	{
		SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_92_GENERAL_ERROR, SvOi::Tid_ResultView_InitFailed, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10197 ); 
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

	bool Update = true;
	SVObjectClass* pObject(nullptr);
	SVObjectManagerClass::Instance().GetObjectByIdentifier(EnvironmentResultUpdateUidGuid, pObject);
	if (nullptr != pObject)
	{
		double Value;
		//Use the getValue with double as it is faster (no dynamic casting)
		pObject->getValue(Value);
		Update = 0.0 < Value ? true : false;
	}
	Update = Update || !SVSVIMStateClass::CheckState( SV_STATE_RUNNING );

	if( !Update )
	{
		return;
	}

	if( nullptr != l_pIPDoc )
	{
		CListCtrl& listCtrl = GetListCtrl();

		listCtrl.SetRedraw( false );

		l_pIPDoc->GetResultData( m_ResultData );

		bool bRedrawDefinitions = false;
		if( l_pIPDoc->IsResultDefinitionsOutdated() )
		{
			listCtrl.DeleteAllItems();
			l_pIPDoc->GetResultDefinitions( m_ResultDefinitions );
			///MEC_SVO_475  only necessary when Definition changed.
			bRedrawDefinitions = true;
		}

		int i = 0;
		DWORD l_Color = 0x00FFFFFF;

		for( i = 0; i < static_cast< int >( m_ResultDefinitions.size() ); ++i )
		{
			SVString Name;
			SVString NameToType;
			SVString ItemIndex;
			SVString Value;

			SVIPResultItemDefinition& l_rDef = m_ResultDefinitions[ i ];

			SVObjectClass* l_pObject = nullptr;
			if(bRedrawDefinitions)
			{
				l_pObject = SVObjectManagerClass::Instance().GetObject( l_rDef.GetObjectID() );
			}

			if( nullptr != l_pObject )
			{
				if( l_rDef.GetIndexPresent())
				{
					if (0 <= l_rDef.GetIndex())
					{
						Name = SvUl_SF::Format( _T("%s[%d]"), l_pObject->GetName(),  l_rDef.GetIndex() +1 );
					}
					else
					{
						Name = SvUl_SF::Format( _T("%s[]"), l_pObject->GetName() );
					}
				}
				else
				{
					Name = l_pObject->GetName();
				}
				NameToType = l_pObject->GetCompleteObjectNameToObjectType( nullptr, SVToolObjectType );
			}

			ItemIndex = SvUl_SF::Format( _T( "%d" ), i );

			SVIPResultData::SVResultDataMap::const_iterator l_Iter = m_ResultData.m_ResultData.find( l_rDef );

			if( l_Iter != m_ResultData.m_ResultData.end() )
			{
				Value = l_Iter->second.GetValue().c_str();
				l_Color = l_Iter->second.GetColor();

				if( l_Iter->second.IsIOTypePresent() )
				{
					ItemIndex.clear();

					if( l_Iter->second.GetIOType() == IO_DIGITAL_INPUT )
					{
						NameToType = SvUl_SF::LoadSVString( IDS_OBJECTNAME_DIGITAL_INPUT );
					}
					else if( l_Iter->second.GetIOType() == IO_REMOTE_INPUT )
					{
						NameToType = SvUl_SF::LoadSVString( IDS_OBJECTNAME_REMOTE_INPUT );
					}
				}
			}

			if(bRedrawDefinitions)
			{
				if( listCtrl.GetItemCount() <= i )
				{
					listCtrl.InsertItem( i, Name.c_str() );
					listCtrl.SetItemText( i, 2, NameToType.c_str() );
					listCtrl.SetItemText( i, 3, ItemIndex.c_str() );
				}
				else
				{
					SVString TempName = listCtrl.GetItemText( i, 0 );

					if( TempName != Name )
					{
						listCtrl.SetItemText( i, 0, Name.c_str() );
						listCtrl.SetItemText( i, 2, NameToType.c_str()	 );
					}
				}
			}

			listCtrl.SetItemText( i, 1, Value.c_str() );
			listCtrl.SetItemData( i, l_Color );
		}

		SVString Temp;

		if( bRedrawDefinitions && (listCtrl.GetItemCount() <= i) )
		{
			// Add the Toolset Time
			Temp = _T( "Toolset Time" );
			listCtrl.InsertItem( i, Temp.c_str() );
		}

		if( bRedrawDefinitions && (listCtrl.GetItemCount() <= ( i + 1 )) )
		{
			// Add the Toolset Complete Process per Second
			Temp = _T( "Complete Processes per Second" );
			listCtrl.InsertItem( i + 1, Temp.c_str() );
		}

		// Get ToolSet Time
		if( m_ResultData.m_ToolSetAvgTime < 0.0 && m_ResultData.m_ToolSetEndTime < 0.0 )
		{
			Temp.clear();
		}
		else
		{
			Temp = SvUl_SF::Format( _T( "%.3f ms ( %.3f ms )" ), m_ResultData.m_ToolSetEndTime * 1000, m_ResultData.m_ToolSetAvgTime * 1000 );
		}

		SVString Prev = listCtrl.GetItemText( i, 1 );
		if( Prev != Temp )
		{
			listCtrl.SetItemText( i, 1, Temp.c_str() );
		}

		// Get ToolSet complete processes per second
		Temp = CalcProcessesPerSecond( m_ResultData.m_TriggerDistance );

		Prev = listCtrl.GetItemText( i+1 , 1 );
		if( Prev != Temp )
		{
			listCtrl.SetItemText( i + 1, 1, Temp.c_str() );
		}

		if(bRedrawDefinitions)
		{
			l_Color = 0x00ffffff;
			listCtrl.SetItemData( i, l_Color );     // toolset time
			listCtrl.SetItemData( i + 1, l_Color ); // processes/sec

			Temp = l_pIPDoc->GetCompleteToolSetName();
			listCtrl.SetItemText( i, 2, Temp.c_str() );     // toolset time
			listCtrl.SetItemText( i + 1, 2, Temp.c_str() ); // processes/sec
		}

		listCtrl.SetRedraw( true );
	}
}

SVString SVResultViewClass::CalcProcessesPerSecond( double p_LastTriggerDistance )
{
	SVString Result;
	double dTime;

	// complete processes per second
	dTime = p_LastTriggerDistance / 1000.0;

	if ( dTime != 0.0 )
	{
		Result = SvUl_SF::Format( _T( "%.3f / sec (%.3f / min)" ), 1.0 / dTime, 1.0 / dTime * 60.0 );
	}// end if

	return Result;
}// end CalcProcessesPerSecond

BOOL SVResultViewClass::Create( LPCTSTR LPSZClassName, LPCTSTR LPSZWindowName, DWORD DwStyle, const RECT& Rect, CWnd* PParentWnd, UINT NID, CCreateContext* PContext )
{
	DWORD viewStyle = LVS_REPORT | LVS_NOSORTHEADER | LVS_SINGLESEL | LVS_OWNERDRAWFIXED;
	BOOL rc = CWnd::Create( LPSZClassName, _T( "Result Page" ), DwStyle |viewStyle, Rect, PParentWnd, NID, PContext );

	return rc;
}

void SVResultViewClass::addColumnHeadings()
{
	CListCtrl& listCtrl = GetListCtrl();

	// load the Column names
	for( int i = 0; i < SV_NUMBER_RESULTVIEW_COLUMNS; i++ )
	{
		SVString ColumnName = SvUl_SF::LoadSVString(IDS_RESULTVIEW_COLUMN_NAME0 + i );
		listCtrl.InsertColumn( i, ColumnName.c_str(), LVCFMT_LEFT, LVSCW_AUTOSIZE, i );
	}
	m_ColumnWidthSet = false;
}

void SVResultViewClass::setColumnWidths()
{
	CListCtrl& listCtrl = GetListCtrl();

	if (!m_ColumnWidthSet )
	{
		CRect viewRect;
		listCtrl.GetClientRect( viewRect );

		//the last column is only used so that we can use the LVSCW_AUTOSIZE_USEHEADER style
		//on autosizing the 2nd last column.

		//calc the width of the first three columns
		int columnWidth = ( viewRect.Width() - 50 ) / ( SV_NUMBER_RESULTVIEW_COLUMNS - 1 );

		//set the width of the first three columns
		for( int i = 0; i < SV_NUMBER_RESULTVIEW_COLUMNS - 1; i++ )
		{
			listCtrl.SetColumnWidth( i, columnWidth );
		}

		listCtrl.SetColumnWidth(SV_NUMBER_RESULTVIEW_COLUMNS - 1,LVSCW_AUTOSIZE_USEHEADER);

		m_ColumnWidthSet = TRUE;
	}
}

void SVResultViewClass::OnSize(UINT nType, int cx, int cy)
{
	CListView::OnSize(nType, cx, cy);
}

void SVResultViewClass::SetViewSize(CSize &Size)
{
	CWnd *l_pcWnd = this;
	SVIPSplitterFrame *l_pSplitterFrame = nullptr;

	do
	{
		l_pcWnd = l_pcWnd->GetParent();

		if ( nullptr != l_pcWnd )
		{
			l_pSplitterFrame = dynamic_cast<SVIPSplitterFrame *>( l_pcWnd );
		}
	}
	while ( nullptr == l_pSplitterFrame && nullptr != l_pcWnd );

	if ( nullptr != l_pSplitterFrame )
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
	CImageList* pImageList( nullptr );

	if( nullptr != pDC )
	{
		// Save dc state
		int nSavedDC = pDC->SaveDC();

		// Get item image and state info
		LV_ITEM lvi;
		lvi.mask = LVIF_IMAGE | LVIF_STATE;
		lvi.iItem = nItem;
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
		SVString Label = listCtrl.GetItemText( nItem, 0 );
		DWORD_PTR dwColor = listCtrl.GetItemData( nItem );

		// Labels are offset by a certain amount
		// This offset is related to the width of a space character
		int offset = pDC->GetTextExtent( _T( " " ), 1 ).cx * 2;
		CRect rcHighlight;
		CRect rcWnd;
		int nExt;

		nExt = pDC->GetOutputTextExtent( Label.c_str() ).cx + offset;
		rcHighlight = rcLabel;

		if( rcLabel.left + nExt < rcLabel.right )
		{
			rcHighlight.right = rcLabel.left + nExt;
		}

		GetClientRect( &rcWnd );
		rcHighlight = rcBounds;
		rcHighlight.left = rcLabel.left;
		rcHighlight.right = rcWnd.right-1;

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
				pImageList->Draw( pDC, nImage, CPoint( rcCol.left, rcCol.top ), ILD_TRANSPARENT );
			}
		}
		// Draw normal and overlay icon
		pImageList = listCtrl.GetImageList( LVSIL_SMALL );
		if( pImageList )
		{
			UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;
			pImageList->Draw( pDC, lvi.iImage, CPoint( rcIcon.left, rcIcon.top ), ( bHighlight ? ILD_BLEND50 : 0 ) | ILD_TRANSPARENT | nOvlImageMask );
		}
		// Draw item label - Column 0
		rcLabel.left += offset/2;
		rcLabel.right -= offset;
		pDC->DrawText( Label.c_str(), -1, rcLabel, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP 
			| DT_VCENTER | DT_END_ELLIPSIS);
	
		// Draw labels for remaining columns
		LV_COLUMN lvc;
		lvc.mask = LVCF_FMT | LVCF_WIDTH;
	
		rcBounds.right = rcHighlight.right > rcBounds.right ? rcHighlight.right : rcBounds.right;
		rgn.CreateRectRgnIndirect( &rcBounds );
		pDC->SelectClipRgn( &rgn );
		for(int nColumn = 1; listCtrl.GetColumn( nColumn, &lvc ); nColumn++)
		{
			rcCol.left = rcCol.right;
			rcCol.right += lvc.cx;

			Label = listCtrl.GetItemText( nItem, nColumn );
			if( Label.empty() )
			{
				continue;
			}

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

			if( nColumn == 1 )
			{
				int h = rcLabel.Height();
				CRect rcState( rcLabel.left, rcLabel.top + 2, rcLabel.left + h, rcLabel.bottom - 2 );

				pDC->FillSolidRect( rcState, static_cast<COLORREF>(dwColor) );
				rcLabel.left += (h + 4);
			}

			pDC->DrawText(Label.c_str(), -1, rcLabel, nJustify | DT_SINGLELINE | 
				DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
		}
		// Draw focus rectangle if item has focus
		if ( ( lvi.state & LVIS_FOCUSED ) && ( GetFocus() == this ) )
			pDC->DrawFocusRect(rcHighlight);

		pDC->RestoreDC( nSavedDC );
	}
	else
	{
		SvStl::MessageMgrStd Msg( SvStl::LogOnly );
		Msg.setMessage( SVMSG_SVO_NULL_POINTER, SvOi::Tid_Unknown, SvStl::SourceFileParams(StdMessageParams) ); 
	}
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

	//	Allow the Result Picker in every mode except Stop
	if( SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_EDIT | SV_STATE_TEST | SV_STATE_REGRESSION ) &&
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

	bOk = SVNavigateTree::GetItem( rTree, CTAG_CELL_HEIGHT, htiParent, svVariant );
	if ( bOk )
	{
		l_Size.cy = svVariant;
	}

	if ( bOk )
	{
		bOk = SVNavigateTree::GetItem( rTree, CTAG_CELL_WIDTH, htiParent, svVariant );
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

	bOk = SVNavigateTree::GetItem( rTree, CTAG_CELL_HEIGHT, htiParent, svVariant );
	if ( bOk )
	{
		l_Size.cy = svVariant;
	}

	if ( bOk )
	{
		bOk = SVNavigateTree::GetItem( rTree, CTAG_CELL_WIDTH, htiParent, svVariant );
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

