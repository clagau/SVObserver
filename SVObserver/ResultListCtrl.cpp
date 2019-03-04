//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//******************************************************************************
/// A List-Ctrl-class for the Result Window. 
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObserver.h"
#include "ResultListCtrl.h"
#include "SVIPDoc.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVInspectionProcess.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "TextDefinesSvO.h"
#include "SVTimerLibrary/SVClock.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum { SV_NUMBER_RESULTVIEW_COLUMNS = 4 };
#pragma endregion Declarations

IMPLEMENT_DYNCREATE( ResultListCtrl, CListCtrl )

BEGIN_MESSAGE_MAP( ResultListCtrl, CListCtrl )
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

#pragma region Constructor
ResultListCtrl::ResultListCtrl() : CListCtrl()
{
	VERIFY(m_ContextMenuItem.LoadMenu(IDR_RESULTS_CONTEXT_MENU));
}

ResultListCtrl::~ResultListCtrl()
{
}
#pragma endregion Constructor

#pragma region Public Methods
BOOL ResultListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	dwStyle |= (LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER | LVS_OWNERDRAWFIXED);
	BOOL Result = CListCtrl::Create(dwStyle, rect, pParentWnd, nID);

	SetExtendedStyle(GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	addColumnHeadings();

	return Result;
}

void ResultListCtrl::updateList()
{
	if (nullptr == m_pDoc)
	{
		SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ResultView_InitFailed, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10197);
		return;
	}

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

	m_pDoc->GetResultData(m_ResultData);
	//If Result Definition time stamp invalid return (Initial value is 0.0)
	if (!Update || 1.0 > m_pDoc->getResultDefinitionUpdatTimeStamp())
	{
		return;
	}

	SetRedraw( false );

	bool bRedrawDefinitions = false;
	if (m_UpdateTimeStamp < m_pDoc->getResultDefinitionUpdatTimeStamp())
	{
		DeleteAllItems();
		m_pDoc->GetResultDefinitions( m_ResultDefinitions );
		///MEC_SVO_475  only necessary when Definition changed.
		bRedrawDefinitions = true;
	}

	int i = 0;
	DWORD l_Color = 0x00FFFFFF;

	for( i = 0; i < static_cast<int> (m_ResultDefinitions.size()); ++i )
	{
		std::string Name;
		std::string NameToType;
		std::string ItemIndex;
		std::string Value;

		SvIe::SVIPResultItemDefinition& l_rDef = m_ResultDefinitions[ i ];

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
					Name = SvUl::Format( _T("%s[%d]"), l_pObject->GetName(),  l_rDef.GetIndex() +1 );
				}
				else
				{
					Name = SvUl::Format( _T("%s[]"), l_pObject->GetName() );
				}
			}
			else
			{
				Name = l_pObject->GetName();
			}
			NameToType = l_pObject->GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType);
		}

		ItemIndex = SvUl::Format( _T( "%d" ), i );

		SvIe::SVIPResultData::SVResultDataMap::const_iterator l_Iter = m_ResultData.m_ResultData.find( l_rDef );

		if( l_Iter != m_ResultData.m_ResultData.end() )
		{
			Value = l_Iter->second.GetValue().c_str();
			l_Color = l_Iter->second.GetColor();

			if( l_Iter->second.IsIOTypePresent() )
			{
				ItemIndex.clear();

				if( l_Iter->second.GetIOType() == IO_DIGITAL_INPUT )
				{
					NameToType = SvUl::LoadStdString( IDS_OBJECTNAME_DIGITAL_INPUT );
				}
				else if( l_Iter->second.GetIOType() == IO_REMOTE_INPUT )
				{
					NameToType = SvUl::LoadStdString( IDS_OBJECTNAME_REMOTE_INPUT );
				}
			}
		}

		if(bRedrawDefinitions)
		{
			if( GetItemCount() <= i )
			{
				InsertItem( i, Name.c_str() );
				SetItemText( i, 2, NameToType.c_str() );
				SetItemText( i, 3, ItemIndex.c_str() );
			}
			else
			{
				std::string TempName = GetItemText( i, 0 );

				if( TempName != Name )
				{
					SetItemText( i, 0, Name.c_str() );
					SetItemText( i, 2, NameToType.c_str()	 );
				}
			}
		}

		SetItemText( i, 1, Value.c_str() );
		SetItemData( i, l_Color );
	}

	std::string Temp;

	if( bRedrawDefinitions && (GetItemCount() <= i) )
	{
		// Add the Toolset Time
		Temp = _T( "Toolset Time" );
		InsertItem( i, Temp.c_str() );
	}

	if( bRedrawDefinitions && (GetItemCount() <= ( i + 1 )) )
	{
		// Add the Toolset Complete Process per Second
		Temp = _T( "Complete Processes per Second" );
		InsertItem( i + 1, Temp.c_str() );
	}

	// Get ToolSet Time
	if( m_ResultData.m_ToolSetAvgTime < 0.0 && m_ResultData.m_ToolSetEndTime < 0.0 )
	{
		Temp.clear();
	}
	else
	{
		Temp = SvUl::Format( _T( "%.3f ms ( %.3f ms )" ), m_ResultData.m_ToolSetEndTime * 1000, m_ResultData.m_ToolSetAvgTime * 1000 );
	}

	std::string Prev = GetItemText( i, 1 );
	if( Prev != Temp )
	{
		SetItemText( i, 1, Temp.c_str() );
	}

	// Get ToolSet complete processes per second
	Temp = CalcProcessesPerSecond( m_ResultData.m_TriggerDistance );

	Prev = GetItemText( i+1 , 1 );
	if( Prev != Temp )
	{
		SetItemText( i + 1, 1, Temp.c_str() );
	}

	if(bRedrawDefinitions)
	{
		l_Color = 0x00ffffff;
		SetItemData( i, l_Color );     // toolset time
		SetItemData( i + 1, l_Color ); // processes/sec

		Temp = m_pDoc->GetCompleteToolSetName();
		SetItemText( i, 2, Temp.c_str() );     // toolset time
		SetItemText( i + 1, 2, Temp.c_str() ); // processes/sec
	}

	m_UpdateTimeStamp = SvTl::GetTimeStamp();
	SetRedraw( true );
	RedrawWindow(nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);
}
#pragma endregion Public Methods

#pragma region Private Methods
void ResultListCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
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
		GetItem(&lvi);

		// Should the item be highlighted
		BOOL bHighlight =( ( lvi.state & LVIS_DROPHILITED ) ||
			( ( lvi.state & LVIS_SELECTED ) && ( ( GetFocus() == this ) ||
			( GetStyle() & LVS_SHOWSELALWAYS) ) ) );

		// Get rectangles for drawing
		CRect rcBounds, rcLabel, rcIcon;
		GetItemRect( nItem, rcBounds, LVIR_BOUNDS );
		GetItemRect( nItem, rcLabel, LVIR_LABEL );
		GetItemRect( nItem, rcIcon, LVIR_ICON );
		CRect rcCol( rcBounds );
		std::string Label = GetItemText( nItem, 0 );
		DWORD_PTR dwColor = GetItemData( nItem );

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
		rcCol.right = rcCol.left + GetColumnWidth( 0 );
		CRgn rgn;
		rgn.CreateRectRgnIndirect( &rcCol );
		pDC->SelectClipRgn( &rgn );
		rgn.DeleteObject();

		// Draw state icon
		if( lvi.state & LVIS_STATEIMAGEMASK )
		{
			int nImage = ( ( lvi.state & LVIS_STATEIMAGEMASK ) >> 12 ) - 1;
			pImageList = GetImageList( LVSIL_STATE );
			if( pImageList )
			{
				pImageList->Draw( pDC, nImage, CPoint( rcCol.left, rcCol.top ), ILD_TRANSPARENT );
			}
		}
		// Draw normal and overlay icon
		pImageList = GetImageList( LVSIL_SMALL );
		if( pImageList )
		{
			UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;
			pImageList->Draw( pDC, lvi.iImage, CPoint( rcIcon.left, rcIcon.top ), ( bHighlight ? ILD_BLEND50 : 0 ) | ILD_TRANSPARENT | nOvlImageMask );
		}
		// Draw item label - Column 0
		rcLabel.left += offset/2;
		rcLabel.right -= offset;
		pDC->DrawText( Label.c_str(), -1, rcLabel, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);
	
		// Draw labels for remaining columns
		LV_COLUMN lvc;
		lvc.mask = LVCF_FMT | LVCF_WIDTH;
	
		rcBounds.right = rcHighlight.right > rcBounds.right ? rcHighlight.right : rcBounds.right;
		rgn.CreateRectRgnIndirect( &rcBounds );
		pDC->SelectClipRgn( &rgn );
		for(int nColumn = 1; GetColumn( nColumn, &lvc ); nColumn++)
		{
			rcCol.left = rcCol.right;
			rcCol.right += lvc.cx;

			Label = GetItemText( nItem, nColumn );
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

			pDC->DrawText(Label.c_str(), -1, rcLabel, nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);
		}
		// Draw focus rectangle if item has focus
		if ((lvi.state & LVIS_FOCUSED) && (GetFocus() == this))
		{
			pDC->DrawFocusRect(rcHighlight);
		}

		pDC->RestoreDC( nSavedDC );
	}
	else
	{
		SvStl::MessageMgrStd Msg(SvStl::MsgType::Log );
		Msg.setMessage( SVMSG_SVO_NULL_POINTER, SvStl::Tid_Unknown, SvStl::SourceFileParams(StdMessageParams) ); 
	}
}

void ResultListCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{

	//	Allow the Result Picker in every mode except Stop
	if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_EDIT | SV_STATE_TEST | SV_STATE_REGRESSION) &&
		TheSVObserverApp.m_svSecurityMgr.SVIsDisplayable(SECURITY_POINT_MODE_MENU_EDIT_TOOLSET))
	{
		CMenu* pPopupMenu = m_ContextMenuItem.GetSubMenu(0);
		assert(nullptr != pPopupMenu);
		CWnd* pOwner = this;
		while (pOwner->GetStyle() & WS_CHILD)
		{
			pOwner = pOwner->GetParent();
		}
		pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pOwner);
	}
}

std::string ResultListCtrl::CalcProcessesPerSecond(double p_LastTriggerDistance)
{
	std::string Result;
	double dTime;

	// complete processes per second
	dTime = p_LastTriggerDistance / 1000.0;

	if (dTime != 0.0)
	{
		Result = SvUl::Format(_T("%.3f / sec (%.3f / min)"), 1.0 / dTime, 1.0 / dTime * 60.0);
	}

	return Result;
}

void ResultListCtrl::addColumnHeadings()
{
	m_ColumnWidthSet = false;
	// load the Column names
	for (int i = 0; i < SV_NUMBER_RESULTVIEW_COLUMNS; i++)
	{
		std::string ColumnName = SvUl::LoadStdString(IDS_RESULTVIEW_COLUMN_NAME0 + i);
		InsertColumn(i, ColumnName.c_str(), LVCFMT_LEFT, LVSCW_AUTOSIZE, i);
	}
	setColumnWidths();
}

void ResultListCtrl::setColumnWidths()
{
	if (!m_ColumnWidthSet)
	{
		CRect viewRect;
		GetClientRect(viewRect);

		//the last column is only used so that we can use the LVSCW_AUTOSIZE_USEHEADER style
		//on autosizing the 2nd last column.

		//calc the width of the first three columns
		int columnWidth = (viewRect.Width() - 50) / (SV_NUMBER_RESULTVIEW_COLUMNS - 1);

		//set the width of the first three columns
		for (int i = 0; i < SV_NUMBER_RESULTVIEW_COLUMNS - 1; i++)
		{
			SetColumnWidth(i, columnWidth);
		}

		SetColumnWidth(SV_NUMBER_RESULTVIEW_COLUMNS - 1, LVSCW_AUTOSIZE_USEHEADER);
		m_ColumnWidthSet = true;
	}
}
#pragma endregion Private Methods

