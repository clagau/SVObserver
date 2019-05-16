//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPictureTabs
//* .File Name       : $Workfile:   SVPictureTabs.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   :     $Date:   14 Aug 2014 17:39:10  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "resource.h"
#include "SVPictureDialog.h"
#include "SVPictureTabs.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

IMPLEMENT_DYNAMIC(SVPictureTabs, CDialog)

SVPictureTabs::SVPictureTabs(CWnd* pParent /*=nullptr*/)
: CDialog(SVPictureTabs::IDD, pParent)
, m_lLastTab(0L)
, m_lAppearance( TopButtons )
, m_lButtonJustify( 0L )
, m_lButtonSpacing( 4 )
, m_lScrollBarWidth( 24 )
, m_isClearOverlayByNewImage( TRUE )
, m_isZoomSimultaneous(FALSE)
, m_isBoundaryCheck(TRUE)
, m_ShowZoomSlider(TRUE)
, m_HideTabCtrl(FALSE)
, m_HideScrollBar(FALSE)
, m_TabHandle(0)
{
}

SVPictureTabs::~SVPictureTabs()
{
	// release SVDisplayPicture objects
}

// Validates the tab index.
bool SVPictureTabs::IsValidTabIndex(int p_iTab) const
{
	return ( p_iTab >= 0 && p_iTab < m_TabCtrl.GetItemCount() );
}

// Validates the tab handle. The tab handle is actually the data index.
bool SVPictureTabs::IsValidTabHandle(long p_lTabHandle) const
{
	return m_PicDialogs.find( p_lTabHandle ) != m_PicDialogs.end();
}

// This function Gets the data index handle from the Tab control
long SVPictureTabs::GetHandleFromTabIndex( int p_lTabIndex ) const
{
	TCITEM l_TabCtrlItem;
	l_TabCtrlItem.mask = TCIF_PARAM;
	for( TabPictureDialogList::const_iterator l_it = m_PicDialogs.begin();
		l_it != m_PicDialogs.end(); ++l_it )
	{
		if( l_it->second.GetTabIndex() == p_lTabIndex )
		{
			return l_it->first;
		}
	}

	return -1;
}

// This function takes a tab handle and returns a tab control index.
// If this handle is not found the function returns -1.
int SVPictureTabs::GetTabIndexFromTabHandle( long p_lTabHandle ) const
{
	int iRet = -1;
	TabPictureDialogList::const_iterator l_it = m_PicDialogs.find(p_lTabHandle);
	if( l_it != m_PicDialogs.end() )
	{
		iRet = l_it->second.GetTabIndex();
	}
	return iRet;
}

// Returns S_OK if tab was alive. Regardless the name is stored for later use, if it's a valid Tab.
HRESULT SVPictureTabs::RenameTab( long p_lTabHandle, LPCTSTR p_Name )
{
	HRESULT l_hr = S_OK;

	if( IsValidTabHandle( p_lTabHandle ) )
	{
		// Update TabPictureList
		if( m_PicDialogs[p_lTabHandle].GetTabName() != p_Name )
		{
			m_PicDialogs[p_lTabHandle].SetTabName( p_Name );

			// Update TabCtrl if this Tab is not Hidden
			int lTabIndex = GetTabIndexFromTabHandle( p_lTabHandle );
			if( lTabIndex >= 0 )
			{
				TCITEM l_TabCtrlItem;
				l_TabCtrlItem.mask = TCIF_TEXT;
				l_TabCtrlItem.pszText = const_cast<LPTSTR>(p_Name);
				m_TabCtrl.SetItem( lTabIndex, &l_TabCtrlItem );

				m_TabCtrl.Invalidate(false);
			}
		}
	}
	else
	{
		l_hr = E_InvalidTab;
	}
	return l_hr;
}

// Re-Index Handles to indexes...
void SVPictureTabs::ReIndexHandles()
{
	int count = 0;

	for( TabPictureDialogListIt it = m_PicDialogs.begin(); it != m_PicDialogs.end(); ++it )
	{
		if( it->second.GetTabIndex() >= 0 )
		{
			it->second.SetTabIndex(count++);
		}
	}
}

// This function will hide or show the tab
// If successful it will return true;
HRESULT SVPictureTabs::ShowTab( long p_lTabHandle, bool p_bShow )
{
	HRESULT l_hr = S_OK;

	if (IsValidTabHandle(p_lTabHandle))
	{
		int iCurrentTab = m_TabCtrl.GetCurSel();
		long lCurrentLogicalTab = GetHandleFromTabIndex( iCurrentTab );

		bool bRet = false;
		int l_TabIndex = GetTabIndexFromTabHandle( p_lTabHandle );
		if( l_TabIndex >= 0 )
		{
			if( p_bShow == false )
			{
				m_TabCtrl.DeleteItem( l_TabIndex );
				m_PicDialogs[p_lTabHandle].SetTabIndex(-1);
				// Re-Index Handles to indexes...
				ReIndexHandles();
				bRet = true;
			}
		}
		else
		{
			if( p_bShow == true )
			{
				TabPictureDialogList::iterator it = m_PicDialogs.find(p_lTabHandle);
				if (it != m_PicDialogs.end())
				{
					// Get previous tab index of previous handle
					long lPreviousTabHandle = p_lTabHandle > 0 ? p_lTabHandle-1 : 0;
					int iPreviousTabIndex = GetTabIndexFromTabHandle( lPreviousTabHandle );
					while( iPreviousTabIndex == -1 && lPreviousTabHandle > -1 )
					{
						iPreviousTabIndex = GetTabIndexFromTabHandle( lPreviousTabHandle );
						lPreviousTabHandle--;
					}

					if( iPreviousTabIndex >= m_TabCtrl.GetItemCount() )
					{
						iPreviousTabIndex = m_TabCtrl.GetItemCount() - 1;
					}

					TCITEM l_TabCtrlItem;
					l_TabCtrlItem.mask = TCIF_TEXT | TCIF_PARAM;
					CString cs = m_PicDialogs[p_lTabHandle].GetTabName().c_str();
					l_TabCtrlItem.cchTextMax = cs.GetLength();
					l_TabCtrlItem.pszText = cs.LockBuffer();

					int iNewTab = 0;
					if( iPreviousTabIndex + 1 < m_TabCtrl.GetItemCount() )
					{
						iNewTab = m_TabCtrl.InsertItem( iPreviousTabIndex + 1, &l_TabCtrlItem );
					}
					else
					{
						iNewTab = m_TabCtrl.InsertItem( m_TabCtrl.GetItemCount(), &l_TabCtrlItem );
					}
					cs.UnlockBuffer();
					m_PicDialogs[p_lTabHandle].SetTabIndex(iNewTab);
					ReIndexHandles();
					bRet = true;
				}
				else
				{
					l_hr = E_InvalidTab;
				}
			}
		}

		if( bRet )
		{
			if( lCurrentLogicalTab != p_lTabHandle )
			{
				int iCurrentTab = GetTabIndexFromTabHandle( lCurrentLogicalTab );
				SelectActiveTab( iCurrentTab );
			}
			else
			{
				for( int i = 0; i < m_TabCtrl.GetItemCount(); i++ )
				{
					if( IsValidTabIndex(i) )
					{
						SelectActiveTab( i );
						break;
					}
				}
			}
		}
	}
	else
	{
		l_hr = E_InvalidTab;
	}
	return l_hr;
}

void SVPictureTabs::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl );
}

void SVPictureTabs::OnTcnSelChangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int l_iSel = m_TabCtrl.GetCurSel();
	SelectActiveTab( l_iSel );
	*pResult = 0;
	GetParent()->SendMessage( TAB_CHANGED, static_cast<WPARAM>(l_iSel), -1 );
}

bool SVPictureTabs::SelectActiveTab( int p_iTabSelect )
{
	bool bRetVal = true;
	if( IsValidTabIndex( p_iTabSelect ) )
	{
		long lTabHandle = GetHandleFromTabIndex( p_iTabSelect );
		m_PicDialogs[m_lLastTab].GetPictureDialog()->ShowWindow( SW_HIDE );
		m_PicDialogs[lTabHandle].GetPictureDialog()->ShowWindow( SW_SHOW );
		m_PicDialogs[lTabHandle].GetPictureDialog()->SetScrollBars();
		m_PicDialogs[lTabHandle].GetPictureDialog()->CloneScrollPosition(SB_BOTH);
		m_PicDialogs[lTabHandle].GetPictureDialog()->SetZoomSlider(TRUE);
		m_PicDialogs[lTabHandle].GetPictureDialog()->AdjustSizeToImage();
		m_lLastTab = lTabHandle;
		m_TabCtrl.SetCurSel( p_iTabSelect );
	}
	else
	{
		bRetVal = false;
	}
	return bRetVal;
}

BEGIN_MESSAGE_MAP(SVPictureTabs, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &SVPictureTabs::OnTcnSelChangeTab)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// SVPictureTabs message handlers

BOOL SVPictureTabs::OnInitDialog()
{
	CDialog::OnInitDialog();

	//ModifyStyleEx(0, WS_EX_TRANSPARENT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

HRESULT SVPictureTabs::SetZoom( long p_lTabHandle, double fZoom )
{
	HRESULT l_hr = S_OK;
	if( IsValidTabHandle( p_lTabHandle ) )
	{
		double min, max;
		m_PicDialogs[p_lTabHandle].GetPictureDialog()->GetZoomRanges(min, max);
		if(fZoom >= min && fZoom <= max)
		{
			m_PicDialogs[p_lTabHandle].GetPictureDialog()->SetZoomFactor( fZoom );
		}
		else if (fZoom < 0.0)
		{
			m_PicDialogs[p_lTabHandle].GetPictureDialog()->SetZoomFit();
		}
		else
		{
			l_hr = E_ZoomOutOfRange;
		}
	}
	else
	{
		l_hr = E_InvalidTab;
	}

	return l_hr;
}

HRESULT SVPictureTabs::GetZoom( long p_lTabHandle, double& rZoomVal ) const
{
	HRESULT l_hr = E_InvalidTab;
	if( IsValidTabHandle( p_lTabHandle ) )
	{
		TabPictureDialogList::const_iterator it = m_PicDialogs.find( p_lTabHandle );

		if ( it != m_PicDialogs.end() )
		{
			rZoomVal = it->second.GetPictureDialog()->GetZoomFactor();
			l_hr = S_OK;
		}
	}
	return l_hr;
}

void SVPictureTabs::UpdateTabClient( SVPictureDialogRef p_Dialog )
{
	CRect TabClient;
	GetClientRect( &TabClient );
	m_TabCtrl.AdjustRect( FALSE, &TabClient );
	p_Dialog.get()->Resize(SIZE_RESTORED, 1, 1, TabClient.Width(), TabClient.Height());
}

void SVPictureTabs::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TabCtrl.m_hWnd ) )
	{
		CRect rect;
		GetClientRect( &rect );
		m_TabCtrl.SetWindowPos( &wndTop, 0, 0, rect.Width(), rect.Height(), SWP_SHOWWINDOW );

		CRect TabClient;
		m_TabCtrl.GetClientRect( &TabClient );

		int l_iTabSelect = m_TabCtrl.GetCurSel();
		long lTabHandle = GetHandleFromTabIndex( l_iTabSelect );

		for ( TabPictureDialogListIt it = m_PicDialogs.begin(); it != m_PicDialogs.end(); ++it )
		{
			UpdateTabClient( it->second.GetPictureDialogRef() );
			if(it->second.GetTabIndex() == lTabHandle )
			{
				it->second.GetPictureDialog()->ShowWindow(SW_SHOW);
			}
			else
			{
				it->second.GetPictureDialog()->ShowWindow(SW_HIDE);
			}
		}
	}
}

void SVPictureTabs::SetAppearance( long p_lType )
{
	m_lAppearance = static_cast<AppearanceTypes>(p_lType);

	UpdateAppearance();
}

void SVPictureTabs::UpdateAppearance()
{
	for ( TabPictureDialogListIt it = m_PicDialogs.begin(); it != m_PicDialogs.end(); ++it )
	{
		it->second.GetPictureDialog()->SetAppearance( m_lAppearance );
	}
}

void SVPictureTabs::SetButtonSpacing( long p_lSpacing )
{
	m_lButtonSpacing = p_lSpacing;
	UpdateButtonSpacing();
}

void SVPictureTabs::UpdateButtonSpacing()
{
	for ( TabPictureDialogListIt it = m_PicDialogs.begin(); it != m_PicDialogs.end(); ++it )
	{
		it->second.GetPictureDialog()->SetButtonSpacing( m_lButtonSpacing );
	}
}

void SVPictureTabs::SetButtonJustify( long p_lJustify )
{
	m_lButtonJustify = p_lJustify;

	UpdateButtonJustify();
}

void SVPictureTabs::UpdateButtonJustify()
{
	for ( TabPictureDialogListIt it = m_PicDialogs.begin(); it != m_PicDialogs.end(); ++it )
	{
		it->second.GetPictureDialog()->SetButtonJustify( m_lButtonJustify );
	}
}

void SVPictureTabs::SetIsClearOverlayByNewImage( BOOL isValue )
{
	m_isClearOverlayByNewImage = isValue;

	for ( TabPictureDialogListIt it = m_PicDialogs.begin(); it != m_PicDialogs.end(); ++it )
	{
		it->second.GetPictureDialog()->SetIsClearOverlayByNewImage( isValue );
	}
}

void SVPictureTabs::SetZoomSimultaneous( BOOL value )
{
	m_isZoomSimultaneous = value;
}

BOOL SVPictureTabs::GetZoomSimultaneous() const
{
	return m_isZoomSimultaneous;
}

void SVPictureTabs::SetShowZoomSlider( BOOL showZoomSlider )
{
	m_ShowZoomSlider = showZoomSlider;
}

BOOL SVPictureTabs::GetShowZoomSlider() const
{
	return m_ShowZoomSlider;
}

void SVPictureTabs::SetBoundaryCheck( BOOL value )
{
	m_isBoundaryCheck = value;
}

BOOL SVPictureTabs::GetBoundaryCheck() const
{
	return m_isBoundaryCheck;
}

HRESULT SVPictureTabs::CloneZoomSettings(long TabHandle)
{
	HRESULT l_hr = S_OK;
	
	if( !IsValidTabHandle( TabHandle ) )
	{
		l_hr = E_InvalidTab;
		return l_hr;
		
	}
	const ZoomHelper& zoomHelper = m_PicDialogs[TabHandle].GetPictureDialog()->GetZoomHelper();

	for ( TabPictureDialogListIt it = m_PicDialogs.begin(); it != m_PicDialogs.end(); ++it )
	{
		if(it->first != TabHandle)
		{
			it->second.GetPictureDialog()->CloneZoomHelper(zoomHelper);
		}
	}

	return l_hr;
}

HRESULT SVPictureTabs::CloneScrollPosition(int Orientation, long TabHandle)
{
	HRESULT hr = S_OK;
	int pos = 0;

	ASSERT(Orientation == SB_VERT || Orientation == SB_HORZ);
	
	if( IsValidTabHandle( TabHandle ) )
	{
		pos = m_PicDialogs[TabHandle].GetPictureDialog()->GetScrollPosition(Orientation);
	}
	else
	{
		ASSERT(FALSE);
		return E_InvalidTab;
	}

	for ( TabPictureDialogListIt it = m_PicDialogs.begin(); it != m_PicDialogs.end(); ++it )
	{
		if(it->first != TabHandle)
		{
			it->second.GetPictureDialog()->SetScrollPosition(Orientation, pos);
		}
	}
	return hr;
}

HRESULT SVPictureTabs::AddOverlay(long TabHandle, const VariantParamMap& ParameterMap, long &handle)
{
	HRESULT hr = S_OK;

	if( !IsValidTabHandle( TabHandle ) )
	{
		hr = E_InvalidTab;
		return hr;
	}

	try
	{
		handle = m_PicDialogs[TabHandle].GetPictureDialog()->AddOverlay(ParameterMap, true);
		if (0 > handle)
		{
			hr = E_INVALIDARG;
		}
	}
	catch (...)
	{
		hr = E_INVALIDARG;
		return hr;
	}
	return hr;
}

HRESULT SVPictureTabs::AddOverlayToGroup(long group, long tabhandle, const VariantParamMap& ParameterMap, long& handle)
{
	HRESULT hr = S_OK;

	if( !IsValidTabHandle( tabhandle ) )
	{
		hr = E_InvalidTab;
		return hr;
	}

	try
	{
		handle = m_PicDialogs[tabhandle].GetPictureDialog()->AddOverlayToGroup(group, ParameterMap, true);
		if (0 > handle)
		{
			hr = E_INVALIDARG;
		}
	}
	catch (...)
	{
		hr = E_INVALIDARG;
		return hr;
	}
	return hr;
}

HRESULT SVPictureTabs::EditOverlay(LONG tab, LONG Handle, const VariantParamMap& ParameterMap)
{
	HRESULT hr = S_OK;

	if( IsValidTabHandle( tab ) )
	{
		hr = m_PicDialogs[tab].GetPictureDialog()->EditOverlay( Handle, ParameterMap );
	}
	else
	{
		hr = E_InvalidTab;
	}

	return hr;
}

HRESULT SVPictureTabs::RemoveOverlay(long p_lTabHandle, long Handle)
{
	HRESULT l_hr = S_OK;
	if( IsValidTabHandle( p_lTabHandle ) )
	{
		l_hr = m_PicDialogs[p_lTabHandle].GetPictureDialog()->RemoveOverlay(Handle);
	}
	else
	{
		l_hr = E_InvalidTab;
	}
	return l_hr;
}

HRESULT SVPictureTabs::RemoveAllOverlays(long p_lTabHandle)
{
	HRESULT l_hr = S_OK;
	if( IsValidTabHandle( p_lTabHandle ) )
	{
		m_PicDialogs[p_lTabHandle].GetPictureDialog()->RemoveAllOverlays();
	}
	else
	{
		l_hr = E_InvalidTab;
	}
	return l_hr;
}

void SVPictureTabs::SetScrollBarWidth( long p_lWidth )
{
	m_lScrollBarWidth = p_lWidth;
	UpdateScrollBarWidth();
}

void SVPictureTabs::UpdateScrollBarWidth()
{
	for ( TabPictureDialogListIt it = m_PicDialogs.begin(); it != m_PicDialogs.end(); ++it )
	{
		it->second.GetPictureDialog()->SetScrollBarWidth( m_lScrollBarWidth );
	}
}

HRESULT SVPictureTabs::SetPicture( long p_lTabHandle, IPictureDisp* l_pPicture, COLORREF color )
{
	HRESULT l_hr = S_OK;

	if( IsValidTabHandle( p_lTabHandle ) )
	{
		int iCurrentTab = m_TabCtrl.GetCurSel();
		long CurrentTabHandle = GetHandleFromTabIndex( iCurrentTab );

		//zoom should only be changed if tab is selected
		bool IsCurrentTab = ( CurrentTabHandle == p_lTabHandle );

		l_hr = m_PicDialogs[p_lTabHandle].GetPictureDialog()->SetPicture( l_pPicture, color, IsCurrentTab );

		if( IsCurrentTab )
		{
			m_PicDialogs[p_lTabHandle].GetPictureDialog()->Invalidate( false );
		}
	}
	else
	{
		l_hr = E_InvalidTab;
	}
	return l_hr;
}

HRESULT SVPictureTabs::SetPictureWithROI( long p_lTabHandle, IPictureDisp* l_pPicture, COLORREF color, ISVROIList* p_RoiList )
{
	HRESULT l_hr = S_OK;

	if( IsValidTabHandle( p_lTabHandle ) )
	{
		int iCurrentTab = m_TabCtrl.GetCurSel();
		long CurrentTabHandle = GetHandleFromTabIndex( iCurrentTab );

		//zoom should only be changed if tab is selected
		bool IsCurrentTab = ( CurrentTabHandle == p_lTabHandle );

		l_hr = m_PicDialogs[p_lTabHandle].GetPictureDialog()->SetPictureWithROI( l_pPicture, color, p_RoiList, IsCurrentTab );

		if( IsCurrentTab )
		{
			m_PicDialogs[p_lTabHandle].GetPictureDialog()->Invalidate( false );
		}
	}
	else
	{
		l_hr = E_InvalidTab;
	}
	return l_hr;
}

HRESULT SVPictureTabs::AddTab( LPCTSTR p_Name, long *phandle )
{
	HRESULT l_hr = S_OK;

	// Check if there is a tab named the same as the one we are trying to add
	int newIndex = m_TabCtrl.GetItemCount(); 
	
	for( int index = 0; index < newIndex; index++ )
	{
		TCHAR szName[128];
		TCITEM ItemStruct;
		ItemStruct.pszText = szName;
		ItemStruct.cchTextMax = 128;
		ItemStruct.mask = TCIF_TEXT;
		if( m_TabCtrl.GetItem( index, &ItemStruct ) )
		{
			if( _tcscmp( ItemStruct.pszText, p_Name ) == 0 )
			{
				return E_DuplicateTabName;
			}
		}
	}

	long Index = m_TabCtrl.InsertItem(newIndex, p_Name); 
	if(Index != newIndex)
	{
		return E_FAIL;
	}
	
	long tabHandle = m_TabHandle++;
	// Create the Picture Dialog to be associated with the newly added Tab
	SVPictureDialogRef picDlg(new SVPictureDialog(tabHandle));
	picDlg.get()->Create( SVPictureDialog::IDD, &m_TabCtrl );

	CRect rect;
	m_TabCtrl.GetClientRect(&rect);
	rect.bottom -= 24;
	UINT flags = SWP_NOCOPYBITS;
	if (newIndex)
	{
		flags |= SWP_HIDEWINDOW;
	}
	picDlg.get()->SetWindowPos(&CWnd::wndTop, 0, 0, rect.Width(), rect.Height(), flags);
	picDlg.get()->SetIsClearOverlayByNewImage( m_isClearOverlayByNewImage );

	// Add
	SVPictureTabInfo PicTab;
	PicTab.SetPictureDialogRef( picDlg );
	PicTab.SetTabName( p_Name );
	PicTab.SetTabIndex(newIndex);
	
	std::pair  < TabPictureDialogListIt, bool > pr;

	pr = m_PicDialogs.insert(std::make_pair(tabHandle, PicTab)); 
	if(pr.second == false)
	{
		return E_FAIL;
	}


	UpdateAppearance();
	UpdateButtonJustify();
	UpdateButtonSpacing();
	UpdateScrollBarWidth();
	UpdateTabClient( picDlg );
	if( nullptr != phandle )
	{
		*phandle = tabHandle;
	}
	return l_hr;
}

HRESULT SVPictureTabs::RemoveTab(long p_lTabHandle)
{
	HRESULT l_hr = S_OK;

	if( IsValidTabHandle( p_lTabHandle ) )
	{
		// Remove from TabCtrl
		int iTabIndex = GetTabIndexFromTabHandle( p_lTabHandle );
		if( iTabIndex >= 0 )
		{
			m_TabCtrl.DeleteItem(iTabIndex);
			m_PicDialogs[p_lTabHandle].SetTabIndex( -1 );
			ReIndexHandles();
		}

		// Remove from PictureDisplayList
		TabPictureDialogListIt it = m_PicDialogs.find(p_lTabHandle);
		if ( it != m_PicDialogs.end())
		{
			m_PicDialogs.erase(it);
		}
	}
	else
	{
		l_hr = E_InvalidTab;
	}
	return l_hr;
}

HRESULT SVPictureTabs::SelectTab( long p_lTabHandle )
{
	HRESULT l_hr = S_OK;

	int iTabIndex = GetTabIndexFromTabHandle( p_lTabHandle );
	if( IsValidTabIndex( iTabIndex ) )
	{
		SelectActiveTab(iTabIndex);
	}
	else
	{
		l_hr = E_InvalidTab;
	}
	return l_hr;
}

HRESULT SVPictureTabs::GetSelectedTab(long& Tab) const
{
	HRESULT l_hr = S_OK;
	int l_iCurrentTab = m_TabCtrl.GetCurSel();
	Tab = GetHandleFromTabIndex( l_iCurrentTab );

	if( Tab < 0 )
	{
		l_hr = E_InvalidTab;
	}

	return l_hr;
}

BOOL SVPictureTabs::OnEraseBkgnd(CDC* pDC)
{
	return true;
}

void SVPictureTabs::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// Do not call CDialog::OnPaint() for painting messages
}

HRESULT SVPictureTabs::SetEditAllow( long p_lTabHandle, long p_lHandle, long p_lAllowEdit )
{
	HRESULT l_hr = S_OK;
	if( IsValidTabHandle( p_lTabHandle ) )
	{
		l_hr = m_PicDialogs[p_lTabHandle].GetPictureDialog()->SetAllowEdit( p_lHandle, p_lAllowEdit );
	}
	else
	{
		l_hr = E_InvalidTab;
	}
	return l_hr;
}

HRESULT SVPictureTabs::SelectDrawObject( long p_lTabHandle, long p_lHandle )
{
	HRESULT l_hr = S_OK;
	if( IsValidTabHandle( p_lTabHandle ) )
	{
		l_hr = m_PicDialogs[p_lTabHandle].GetPictureDialog()->SelectDrawObject( p_lHandle );
	}
	else
	{
		l_hr = E_InvalidTab;
	}
	return l_hr;
}

HRESULT SVPictureTabs::GetObjectCoords( long lTab, long lHandle, long& rlX1, long& rlY1, long& rlX2, long& rlY2 ) const
{
	HRESULT l_hr = E_InvalidTab;
	TabPictureDialogList::const_iterator it = m_PicDialogs.find( lTab );

	if ( it != m_PicDialogs.end() )
	{
		l_hr = it->second.GetPictureDialog()->GetObjectCoords( lHandle, rlX1, rlY1, rlX2, rlY2 );
	}
	return l_hr;
}

HRESULT SVPictureTabs::GetObjectParameter( long lTab, long lHandle, VariantParamMap& parameterMap ) const
{
	HRESULT hr = E_InvalidTab;
	TabPictureDialogList::const_iterator it = m_PicDialogs.find( lTab );

	if( it != m_PicDialogs.end() )
	{
		hr = it->second.GetPictureDialog()->GetObjectParameter(lHandle, parameterMap);
	}
	return hr;
}

// This is called from the control...
void SVPictureTabs::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int i = m_TabCtrl.GetCurSel();
	long lTabHandle = GetHandleFromTabIndex( i );
	TabPictureDialogListIt it = m_PicDialogs.find(lTabHandle);

	if( it != m_PicDialogs.end() )
	{
		long l_lObjectDeleted = it->second.GetPictureDialog()->DeleteDrawObject();
		if( l_lObjectDeleted > 0 )
		{
			GetParent()->SendMessage( OBJECT_DELETED, static_cast<WPARAM>(i), static_cast<LPARAM>(l_lObjectDeleted) );
		}
	}
}

HRESULT SVPictureTabs::EnableSaveButton( long p_lTabHandle, long p_lEnable )
{
	HRESULT l_hr = S_OK;
	if( IsValidTabHandle( p_lTabHandle ) )
	{
		l_hr = m_PicDialogs[p_lTabHandle].GetPictureDialog()->EnableSaveButton( p_lEnable );
	}
	else
	{
		l_hr = E_InvalidTab;
	}
	return l_hr;
}

HRESULT SVPictureTabs::GetViewRect( long p_lTab, long& pX1, long& pY1, long& pX2, long& pY2 ) const
{
	HRESULT l_hr = E_InvalidTab;
	TabPictureDialogList::const_iterator it = m_PicDialogs.find( p_lTab );

	if( it != m_PicDialogs.end() )
	{
		l_hr = it->second.GetPictureDialog()->GetViewRect( pX1, pY1, pX2, pY2 );
	}
	return l_hr;
}

void SVPictureTabs::SetHideTabControl(VARIANT_BOOL newVal)
{
	m_HideTabCtrl = newVal;
	if(m_HideTabCtrl)
	{
		m_TabCtrl.ModifyStyle( 0, TCS_FIXEDWIDTH | TCS_RIGHT | TCS_VERTICAL );
		//@HACK MEC With CSize(0,0)  The y size seems to be ignored when tabs at bottom.
		m_TabCtrl.SetItemSize(CSize(0, 1));
	}
	else
	{
		m_TabCtrl.ModifyStyle( TCS_FIXEDWIDTH, 0 );
	}
}

void SVPictureTabs::SetHideScrollBar(VARIANT_BOOL newVal)
{
	m_HideScrollBar = newVal;
}

BOOL SVPictureTabs::IsScrollbarHidden() const
{
	return m_HideScrollBar;
}

