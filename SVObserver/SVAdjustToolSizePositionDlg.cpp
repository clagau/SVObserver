//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAdjustToolSizePositionDlg
//* .File Name       : $Workfile:   SVAdjustToolSizePositionDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.8  $
//* .Check In Date   : $Date:   17 Sep 2014 08:07:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <limits>
#include "SVAdjustToolSizePositionDlg.h"
#include "SVObjectLibrary/SVObjectSynchronousCommandTemplate.h"
#include "svobserver.h"
#include "SVTaskObject.h"
#include "SVInspectionProcess.h"
#include "SVGuiExtentUpdater.h"
#include "SVImageListClass.h"
#include "SVTool.h"
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"
#include "SVSVIMStateClass.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVLinearToolClass.h"
#include "SVExtentPropertiesInfoStruct.h"
#include "SVMainFrm.h"
#include "ToolSizeAdjustTask.h"
#include <array>
#include  <functional>

#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma endregion Declarations

BEGIN_MESSAGE_MAP(SVAdjustToolSizePositionDlg, CDialog)
	//{{AFX_MSG_MAP(SVAdjustToolSizePositionDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_TOP_LEFT_RADIO, OnModeRadio)
	ON_MESSAGE( SvMc::WM_SV_NOTIFY_LBUTTONDOWN, OnNotifyLButtonDown )
	ON_MESSAGE( SvMc::WM_SV_NOTIFY_LBUTTONUP, OnNotifyLButtonUp )
	ON_BN_CLICKED(IDC_MOVETOOL_RADIO, OnModeRadio)
	ON_BN_CLICKED(IDC_ROTATION_RADIO, OnModeRadio)
	ON_BN_CLICKED(IDC_BOTTOM_RIGHT_RADIO, OnModeRadio)
	ON_BN_CLICKED(IDC_FULL_ROI_BTN, OnBnClickedFullROI)
	ON_BN_CLICKED(IDC_BUT_PROPAGATE, OnBnClickedPropagate)
	ON_BN_CLICKED(IDC_BUT_EDIT_TOOL, OnBnClickedEditTool)
	ON_NOTIFY(PTN_ITEMCHANGED, IDC_RESULT_LIST, OnItemChanged)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(SVAdjustToolSizePositionDlg, CDialog)

#pragma region Constructor
SVAdjustToolSizePositionDlg::SVAdjustToolSizePositionDlg(LPCTSTR pCaption, CWnd* pParentWnd, SVTaskObjectClass* pToolTask)
	: CDialog(SVAdjustToolSizePositionDlg::IDD, pParentWnd)
{
	m_pToolTask = pToolTask;
	ASSERT( m_pToolTask );

	if ( m_pToolTask )
	{
		if ( S_OK == m_pToolTask->DoesObjectHaveExtents() )
		{
			m_pToolTask->GetImageExtent(m_svExtents);
		}
	}
	m_svOriginalExtents = m_svExtents;
	m_sTitle = pCaption;

	//{{AFX_DATA_INIT(SVAdjustToolSizePositionDlg)
	m_iMode = -1;
	//}}AFX_DATA_INIT
}

SVAdjustToolSizePositionDlg::~SVAdjustToolSizePositionDlg()
{
	::DestroyIcon( m_icoArrowDown  );
	::DestroyIcon( m_icoArrowUp );
	::DestroyIcon( m_icoArrowLeft );
	::DestroyIcon( m_icoArrowRight );
	::DestroyIcon( m_icoArrowClockwise );
	::DestroyIcon( m_icoArrowCounterclockwise );
}
#pragma endregion Constructor

#pragma region Protected Methods
void SVAdjustToolSizePositionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVAdjustToolSizePositionDlg)
	DDX_Control(pDX, IDC_UP_BUTTON, m_btnUp);
	DDX_Control(pDX, IDC_RIGHT_BUTTON, m_btnRight);
	DDX_Control(pDX, IDC_LEFT_BUTTON, m_btnLeft);
	DDX_Control(pDX, IDC_DOWN_BUTTON, m_btnDown);
	DDX_Radio(pDX, IDC_TOP_LEFT_RADIO, m_iMode);
	//}}AFX_DATA_MAP
}

BOOL SVAdjustToolSizePositionDlg::OnInitDialog()
{
	ASSERT( nullptr != m_pToolTask );
	m_iMode = MODE_MOVE;	// Default action: Move
	CDialog::OnInitDialog();

	SetWindowText(m_sTitle);

	createIcons();
	bool l_bShow = ( nullptr != dynamic_cast< AllowResizeToParent* >( m_pToolTask ) );

	bool bShowEditTool =  (nullptr !=	ToolSizeAdjustTask::GetToolSizeAdjustTask(m_pToolTask));
	GetDlgItem(IDC_BUT_EDIT_TOOL)->ShowWindow( bShowEditTool? SW_SHOW : SW_HIDE );

	// If it's a Linear Tool, hide the "Full Image" button if rotation is enabled.
	SVLinearToolClass* lt = dynamic_cast< SVLinearToolClass* >( m_pToolTask );
	if( nullptr != lt )
	{
		BOOL rotation = lt->GetRotation();
		if ( rotation )
		{
			l_bShow = false;
		}
	}

	GetDlgItem(IDC_FULL_ROI_BTN)->ShowWindow( l_bShow ? SW_SHOW : SW_HIDE );
	GetDlgItem(IDC_FULL_ROI_BTN)->EnableWindow( !IsFullSize() && IsFullSizeAllowed());

	SVExtentPropertyListType list;
	m_pToolTask->GetFilteredImageExtentPropertyList( list );

	if ( std::find( list.begin(), list.end(), SVExtentPropertyRotationAngle ) == list.end() )	// if item is not in filtered list, skip
	{
		GetDlgItem( IDC_ROTATION_RADIO )->ShowWindow( SW_HIDE );	// hide rotation option
	}

	OnModeRadio();

	// Init the control's size to cover the entire client area
	CRect rc;

	// Init the control's size to cover the entire client area
	GetDlgItem(IDC_RESULT_LIST)->GetWindowRect(rc);
	ScreenToClient(rc);
	GetDlgItem(IDC_RESULT_LIST)->DestroyWindow();
	// PTS_NOTIFY - SVRPropTree will send notification messages to the parent window
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|PTS_NOTIFY;
	// Create SVRPropTree control
	m_Tree.Create(dwStyle, rc, this, IDC_RESULT_LIST);
	//first column should be 60% of the size and second 40%  
	m_Tree.SetColumn( static_cast<long>(rc.Width() * 0.6) );
	m_Tree.ShowInfoText( false );
	BuildTreeFromExtents();

	return TRUE;  // return TRUE unless you set the focus to a control
}

LRESULT SVAdjustToolSizePositionDlg::OnNotifyLButtonDown( WPARAM wParam, LPARAM lParam )
{
	SvMc::SVUpDownButton* pButton = reinterpret_cast <SvMc::SVUpDownButton*> (lParam);
	m_pButton = pButton;

	ButtonAction( m_pButton );

	SetTimer( TIMER_PAUSE, 600, NULL ); //@TODO:  Document why 600.
	return 0;
}

LRESULT SVAdjustToolSizePositionDlg::OnNotifyLButtonUp( WPARAM wParam, LPARAM lParam )
{
	SvMc::SVUpDownButton* pButton = reinterpret_cast <SvMc::SVUpDownButton*> (lParam);
	KillTimer( TIMER_PAUSE );
	KillTimer( TIMER_REPEAT );
	m_pButton = nullptr;
	return 0;
}

void SVAdjustToolSizePositionDlg::OnTimer( UINT_CUSTOM nIDEvent )
{
	switch ( nIDEvent )
	{
		case TIMER_PAUSE:
			SetTimer( TIMER_REPEAT, 100, nullptr ); //@TODO:  Document why 100.
			break;

		case TIMER_REPEAT:
			ButtonAction( m_pButton);
			break;

		default:
			CDialog::OnTimer(nIDEvent);
	}
}

void SVAdjustToolSizePositionDlg::OnModeRadio() 
{
	UpdateData();
	if ( MODE_ROTATE == m_iMode )
	{
		m_btnDown.SetIcon( nullptr );
		m_btnUp.SetIcon( nullptr );
		m_btnLeft.SetIcon( m_icoArrowClockwise );
		m_btnRight.SetIcon( m_icoArrowCounterclockwise );
	}
	else
	{
		m_btnDown.SetIcon( m_icoArrowDown );
		m_btnUp.SetIcon( m_icoArrowUp );
		m_btnLeft.SetIcon( m_icoArrowLeft );
		m_btnRight.SetIcon( m_icoArrowRight );
	}

	UpdateData(FALSE);
}

void SVAdjustToolSizePositionDlg::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
	LPNMPROPTREE pNMPropTree = reinterpret_cast< LPNMPROPTREE >( pNotifyStruct );
	*plResult = S_OK;
	if ( pNMPropTree->pItem )
	{
		SVRPropertyItem* pItem = pNMPropTree->pItem;

		// do validation
		SVExtentPropertyEnum eProperty = static_cast< SVExtentPropertyEnum >( pItem->GetCtrlID() - ID_BASE );
		CString sValue;
		pItem->GetItemValue( sValue );
		double dValue = atof( sValue );

		m_pToolTask->GetImageExtent( m_svExtents );

		double dOldValue;
		m_svExtents.GetExtentProperty( eProperty, dOldValue );
		if ( !ApproxEqual( dOldValue, dValue ) )
		{
			m_svExtents.SetExtentProperty( eProperty, dValue );

			m_svExtents.UpdateData();

			m_svExtents.GetExtentProperty( eProperty, dValue );
			sValue = AsString(dValue);
			pItem->SetItemValue( sValue );
			pItem->OnRefresh();

			HRESULT hr = SVGuiExtentUpdater::SetImageExtent(m_pToolTask, m_svExtents,false);
			m_pToolTask->GetImageExtent( m_svExtents );
			FillTreeFromExtents();
		}
	}
}

void SVAdjustToolSizePositionDlg::OnOK() 
{
	//check if extents have change.  if they change set SV_STATE_MODIFIED
	if (m_svOriginalExtents != m_svExtents)
	{
		SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
	}
	CDialog::OnOK();
}

void SVAdjustToolSizePositionDlg::OnCancel() 
{
	SVGuiExtentUpdater::SetImageExtent(m_pToolTask, m_svOriginalExtents,false);
	CDialog::OnCancel();
}

void SVAdjustToolSizePositionDlg::OnBnClickedFullROI()
{
	HRESULT hr = SVGuiExtentUpdater::SetImageExtentToParent(m_pToolTask,false );

	if( hr == S_OK )
	{
		m_pToolTask->GetImageExtent(m_svExtents);

		FillTreeFromExtents();
		m_Tree.Invalidate();
	}
}
void SVAdjustToolSizePositionDlg::OnBnClickedPropagate()
{
	HRESULT hr = SVGuiExtentUpdater::ForwardSizeAndPosition(m_pToolTask, true);
}

void SVAdjustToolSizePositionDlg::OnBnClickedEditTool()
{
	SVMainFrame* pFrame = dynamic_cast<SVMainFrame*>( AfxGetMainWnd() );
	if(nullptr != pFrame)
	{
		pFrame->PostMessage(WM_COMMAND, ID_EDIT_EDITTOOLTAB1, 0 );
	}
	
	OnOK();
}
#pragma endregion Protected Methods

#pragma region Private Methods
HRESULT SVAdjustToolSizePositionDlg::ButtonAction(SvMc::SVUpDownButton* pButton)
{
	UpdateData();

	static SVExtentLocationPropertyEnum aActions[2][4] =	// rows are buttons (4), columns are modes (2)
	{// TOP_LEFT, BOTTOM_RIGHT, MOVE, ROTATE
		{ SVExtentLocationPropertyTop, SVExtentLocationPropertyBottom, SVExtentLocationPropertyCenter, SVExtentLocationPropertyUnknown },	// btnUp & btnDown
		{ SVExtentLocationPropertyLeft, SVExtentLocationPropertyRight, SVExtentLocationPropertyCenter, SVExtentLocationPropertyRotate },	// btnLeft & btnRight
	};
	enum { BTN_UP_DOWN, BTN_LEFT_RIGHT };
	int dx = 0;
	int dy = 0;
	SVExtentLocationPropertyEnum eAction = SVExtentLocationPropertyUnknown;

	SVExtentDirectionsEnum Direction = SVExtentDirectionBoth; 
	if ( pButton == &m_btnUp )
	{
		eAction = aActions[ BTN_UP_DOWN ][ m_iMode ];
		dy = -1;
		Direction = SVExtentDirectionVertical;
	}
	else if ( pButton == &m_btnDown )
	{
		eAction = aActions[ BTN_UP_DOWN ][ m_iMode ];
		dy = 1;
		Direction = SVExtentDirectionVertical;
	}
	else if ( pButton == &m_btnLeft )
	{
		eAction = aActions[ BTN_LEFT_RIGHT ][ m_iMode ];
		dx = -1;
		Direction = SVExtentDirectionHorizontal;
	}
	else if ( pButton == &m_btnRight )
	{
		eAction = aActions[ BTN_LEFT_RIGHT ][ m_iMode ];
		dx = 1;
		Direction = SVExtentDirectionHorizontal;
	}

	
	bool bAllowedAction(true);
	SVToolClass *pToolClass(nullptr);
	
	pToolClass = dynamic_cast<SVToolClass* >(m_pToolTask);
	

	if(nullptr != pToolClass)
	{
		bAllowedAction = pToolClass->IsAllowedLocation(eAction,Direction );
	}
	
	
	HRESULT hr = S_OK;
	if ( bAllowedAction && eAction == SVExtentLocationPropertyRotate )
	{
		hr = AdjustToolAngle( dx );
	}
	else if (bAllowedAction)
	{
		hr = AdjustTool( eAction, dx, dy );
	}

	m_pToolTask->GetImageExtent(m_svExtents);
	FillTreeFromExtents();
	m_Tree.Invalidate();
	return hr;
}

HRESULT SVAdjustToolSizePositionDlg::AdjustTool( SVExtentLocationPropertyEnum eAction, int dx, int dy )
{
	HRESULT hr = S_OK;

	SVImageExtentClass l_pExtents;

	hr = m_pToolTask->GetImageExtent(l_pExtents);

	if ( hr == S_OK )
	{
		hr = l_pExtents.UpdateFromOutputSpace( eAction, static_cast< long >( dx ),static_cast< long >( dy ) );
	}

	if ( hr == S_OK )
	{
		hr = SVGuiExtentUpdater::SetImageExtent(m_pToolTask, l_pExtents, false);
	}

	return hr;
}

HRESULT SVAdjustToolSizePositionDlg::AdjustToolAngle(double dDAngle)
{
	HRESULT hr = S_OK;
	SVImageExtentClass l_Extents;
	double dCurrentAngle=0.0;
	hr = m_pToolTask->GetImageExtent(l_Extents);
	hr = l_Extents.GetExtentProperty( SVExtentPropertyRotationAngle, dCurrentAngle );
	if ( l_Extents.GetTranslation() == SVExtentTranslationPolarUnwrap )
	{
		dCurrentAngle += dDAngle;
	}
	else
	{
		dCurrentAngle -= dDAngle;
	}

	if ( hr == S_OK )
	{
		hr = l_Extents.SetExtentProperty( SVExtentPropertyRotationAngle, dCurrentAngle );
		if ( hr == S_OK )
		{
			hr = SVGuiExtentUpdater::SetImageExtent(m_pToolTask, l_Extents,false);
			m_svExtents = l_Extents;
		}
	}
	return hr;
}

void SVAdjustToolSizePositionDlg::BuildTreeFromExtents()
{
	SVRPropertyItem* pRoot = m_Tree.InsertItem(new SVRPropertyItem());
	ASSERT( pRoot );
	pRoot->SetCanShrink(false);
	pRoot->SetInfoText(_T(""));
	pRoot->HideItem();
	pRoot->SetHeight(2);  //@TODO:  Document why 2 is used here.

	FillTreeFromExtents(pRoot, true);

	SVRPropertyItem* pChild = pRoot->GetChild();
	while ( pChild )
	{
		pChild->Expand( TRUE );
		pChild = pChild->GetSibling();
	}
	pRoot->Expand( true );	// needed for redrawing
}

void SVAdjustToolSizePositionDlg::FillTreeFromExtents()
{
	SVRPropertyItem* pRoot = m_Tree.GetRootItem();
	FillTreeFromExtents(pRoot, false);

	GetDlgItem(IDC_FULL_ROI_BTN)->EnableWindow( !IsFullSize() && IsFullSizeAllowed());
}

void SVAdjustToolSizePositionDlg::FillTreeFromExtents( SVRPropertyItem* pRoot, bool shouldCreate )
{
	ASSERT( pRoot );
	ASSERT( m_pToolTask );
	SVExtentPropertyListType list;
	HRESULT hr = m_pToolTask->GetFilteredImageExtentPropertyList( list );
	SVExtentPropertyMapType map;
	m_svExtents.GetExtentPropertyList((SVExtentPropertyEnum) m_iPropertyFilter, map);

	for(SVExtentPropertyMapType::const_iterator iter = map.begin(); iter != map.end(); ++iter )
	{
		if ( std::find( list.begin(), list.end(), iter->first ) == list.end() )	// if item is not in filtered list, skip
		{
			continue;
		}



		SVRPropertyItemEdit* pEdit = nullptr;
		if (shouldCreate)
		{
			pEdit = dynamic_cast<SVRPropertyItemEdit*>(m_Tree.InsertItem(new SVRPropertyItemEdit(), pRoot));
		}
		else
		{
			pEdit = dynamic_cast<SVRPropertyItemEdit*> (m_Tree.FindItem(ID_BASE + (int) iter->first));
		}
		ASSERT( pEdit );

		if (pEdit != nullptr)
		{
			CString sName = iter->second;
			double dValue = 0.0;
			m_svExtents.GetExtentProperty( iter->first, dValue);

			//pEdit->SetCtrlID( iID++ );	// needs to be a postincrement here
			pEdit->SetCtrlID( ID_BASE + (int) iter->first );
			pEdit->SetLabelText( sName );
			pEdit->SetBold( false );
			pEdit->SetHeight(16);

			CString sValue;
			if ((iter->first&g_SVExtentPropertyNoDecimalPlaces) != 0)
			{
				sValue = AsString((int)dValue);
			}
			else if ((iter->first&g_SVExtentProperty2DecimalPlaces) != 0)
			{
				sValue.Format("%.2f", dValue);
			}
			else
			{
				sValue = AsString(dValue);
			}
			
			bool bReadonly(false);
			SVExtentPropertyInfoStruct info;
			SVToolClass *pTool = dynamic_cast<SVToolClass*> (m_pToolTask) ;
			if(nullptr != pTool &&  (S_OK == pTool->GetPropertyInfo(iter->first, info) ))
			{
				
				if( pTool->GetAutoSizeEnabled() !=  EnableNone)
				{
					bReadonly =  info.bSetByReset; 
				}
				bReadonly = bReadonly ||    info.bFormula;

			}

			if(bReadonly)
			{
				pEdit->SetForeColor(::GetSysColor(COLOR_INACTIVECAPTION));
				pEdit->ReadOnly(true);
			}
			pEdit->SetItemValue( sValue );
			pEdit->OnRefresh();
		}
	}//end for( iter = map.begin(); iter != map.end(); iter++ )
}


bool SVAdjustToolSizePositionDlg::IsFullSizeAllowed()
{
	SVToolClass *pTool = dynamic_cast<SVToolClass*> (m_pToolTask) ;
	bool bAllowFullsize(true);
	if(!pTool )
	{
		bAllowFullsize = false;
	}
	if( bAllowFullsize && pTool->GetAutoSizeEnabled() == EnableNone)
	{	
		bAllowFullsize = false;
	}


	SVExtentPropertyInfoStruct info;
	array<SVExtentPropertyEnum, 4> PropArray = { SVExtentPropertyWidth, SVExtentPropertyHeight,SVExtentPropertyPositionPointX, SVExtentPropertyPositionPointY  };
	std::for_each(PropArray.begin(),PropArray.end(),[&](SVExtentPropertyEnum p)
	{
		if (bAllowFullsize && S_OK == pTool->GetPropertyInfo(p, info) )
		{
			if(info.bSetByReset)
			{
				bAllowFullsize = false;
			}
		}

	} 
	);


	return bAllowFullsize;
}

bool SVAdjustToolSizePositionDlg::IsFullSize()
{
	bool l_bFull = false;
	long lWidth = 0;
	long lHeight = 0;
	long lTop = 0;
	long lLeft = 0;
	HRESULT hr1 = m_svExtents.GetExtentProperty( SVExtentPropertyOutputWidth, lWidth );
	HRESULT hr2 = m_svExtents.GetExtentProperty( SVExtentPropertyOutputHeight, lHeight );
	HRESULT hr3 = m_svExtents.GetExtentProperty( SVExtentPropertyPositionPointX, lLeft );
	HRESULT hr4 = m_svExtents.GetExtentProperty( SVExtentPropertyPositionPointY, lTop );

	m_pToolTask->GetImageExtent(m_svExtents);
	SVImageExtentClass l_ParentExtent;
	if( m_pToolTask->GetTool()->GetParentExtent( l_ParentExtent ) == S_OK )
	{
		long l_lParentWidth=0;
		long l_lParentHeight=0;
		long l_lParentTop=0;
		long l_lParentLeft=0;
		l_ParentExtent.GetExtentProperty( SVExtentPropertyOutputWidth, l_lParentWidth );
		l_ParentExtent.GetExtentProperty( SVExtentPropertyOutputHeight, l_lParentHeight );
		l_ParentExtent.GetExtentProperty( SVExtentPropertyPositionPointX, l_lParentTop );
		l_ParentExtent.GetExtentProperty( SVExtentPropertyPositionPointY, l_lParentLeft );
		if( l_lParentWidth == lWidth
			&& l_lParentHeight == lHeight
			&& l_lParentTop == l_lParentTop
			&& l_lParentLeft == l_lParentLeft )
		{
			l_bFull = true;
		}
	}
	return l_bFull;
}

void SVAdjustToolSizePositionDlg::createIcons()
{
	HINSTANCE hInstance = ::AfxGetResourceHandle();
	m_icoArrowDown = ::LoadImage(hInstance,
		MAKEINTRESOURCE(IDI_ARROW_DOWN),
		IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR);

	m_icoArrowUp = ::LoadImage(hInstance,
		MAKEINTRESOURCE(IDI_ARROW_UP),
		IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR);

	m_icoArrowLeft = ::LoadImage(hInstance,
		MAKEINTRESOURCE(IDI_ARROW_LEFT),
		IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR);

	m_icoArrowRight = ::LoadImage(hInstance,
		MAKEINTRESOURCE(IDI_ARROW_RIGHT),
		IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR);

	m_icoArrowClockwise = ::LoadImage(hInstance,
		MAKEINTRESOURCE(IDI_ARROW_CLOCKWISE),
		IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR);

	m_icoArrowCounterclockwise = ::LoadImage(hInstance,
		MAKEINTRESOURCE(IDI_ARROW_COUNTERCLOCKWISE),
		IMAGE_ICON, SvOr::IconSize, SvOr::IconSize, LR_DEFAULTCOLOR);
}
#pragma endregion Private Methods
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVAdjustToolSizePositionDlg.cpp_v  $
 * 
 *    Rev 1.8   17 Sep 2014 08:07:18   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  935
 * SCR Title:  Linear Tool Adjust Position Dialog - Disable Rotation Buttons When No Rotation
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use in OnInitDialog filtered list instead of map to search if rotation is used or not
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   25 Jun 2014 10:34:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  894
 * SCR Title:  Enhancements to Adjust Tool Position Dialog
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Made changes to OnOK to check to see if any of the extents have changed.  If they do, set the SVSVIMState to Modified.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   26 May 2014 10:49:56   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  894
 * SCR Title:  Enhancements to Adjust Tool Position Dialog
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   combine the three classes and made the changes like described in SCR894
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   03 Oct 2013 13:31:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   64 bit platform types.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Oct 2013 12:05:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Sep 2013 16:13:46   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  860
 * SCR Title:  Resize Icons Should Change Based on a Tool's Rotation
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated comment regarding m_icoArrowDown.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Jul 2013 15:01:22   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 021.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   14 Jun 2013 06:28:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added user message to prevent button clicked message from getting lost when loosing focus from PropTree.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 08:20:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 011.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   19 Apr 2013 08:26:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  827
 * SCR Title:  Add button to the adjust tool size/position to set size to parent image.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added dialog components to support Full Image button.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:27:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   04 Oct 2012 10:36:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Aug 2012 16:01:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with updating tool extents when the tool is off the image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Aug 2012 11:43:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new command functionality to allow for synchronous command execution through the inspection thread.  This will fix problems with race conditions with the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   07 Apr 2011 14:16:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed issues when setting the properties for the Adjust Tool Size/Position
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Mar 2011 08:04:30   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   fixed issues with Adjust Tool position
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Mar 2011 13:19:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Oct 2007 16:35:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added DestroyWindow Function to free GDI Resources
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Jan 2006 16:58:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnItemChanged method to fix an issue with invalid extent data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Feb 2005 12:08:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Adding new files to implement new extent functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
