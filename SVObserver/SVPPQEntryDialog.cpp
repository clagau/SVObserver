//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQEntryDialog
//* .File Name       : $Workfile:   SVPPQEntryDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   15 May 2014 12:32:56  $
//******************************************************************************

#include "stdafx.h"
#include "SVPPQEntryDialog.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSVIMStateClass.h"
#include "SVVirtualCamera.h"
#include "SVPPQObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SVPPQEntryDialogCameraPageClass, CPropertyPage)

SVPPQEntryDialogCameraPageClass::SVPPQEntryDialogCameraPageClass() : CPropertyPage(SVPPQEntryDialogCameraPageClass::IDD)
{
	//{{AFX_DATA_INIT(SVPPQEntryDialogCameraPageClass)
		m_CurrentPos = _T("");
	//}}AFX_DATA_INIT
    m_bIsTaken = FALSE;
}

SVPPQEntryDialogCameraPageClass::~SVPPQEntryDialogCameraPageClass()
{
}

void SVPPQEntryDialogCameraPageClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPPQEntryDialogCameraPageClass)
	DDX_Control(pDX, IDC_SELECTED_LIST, m_SelectedList);
	DDX_Control(pDX, IDC_AVAILABLE_LIST, m_AvailableList);
	DDX_Text(pDX, IDC_CURRENT_POSITION, m_CurrentPos);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVPPQEntryDialogCameraPageClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVPPQEntryDialogCameraPageClass)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnRemoveButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPPQEntryDialogCameraPageClass message handlers

BOOL SVPPQEntryDialogCameraPageClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	ASSERT( m_pSheet );

	SVVirtualCamera *pCamera( nullptr );
	long lPosition;
	int index;

	std::deque< SVVirtualCamera* > l_Cameras;

	m_pSheet->m_pPPQ->GetCameraList( l_Cameras );

	std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

	while( l_Iter != l_Cameras.end() )
	{
		pCamera = ( *l_Iter );

		if( nullptr != pCamera )
		{
			m_pSheet->m_pPPQ->GetCameraPPQPosition( lPosition, pCamera );

			// Fill selected box...
			if( m_pSheet->m_lCurrentPosition == lPosition )			
			{
				index = m_SelectedList.AddString( pCamera->GetCompleteName().c_str() );
				m_SelectedList.SetItemData( index , reinterpret_cast<DWORD_PTR>(pCamera) );			
			}// end if

			// Fill available box...
			if( -1 == lPosition )			
			{
				index = m_AvailableList.AddString( pCamera->GetCompleteName().c_str() );
				m_AvailableList.SetItemData( index , reinterpret_cast<DWORD_PTR>(pCamera) );			
			}// end if

			GetDlgItem(IDC_ADD_BUTTON)->EnableWindow(m_pSheet->OkToAdd());
		}
		++l_Iter;
	}// end for

	m_CurrentPos.Format( "Current PPQ\r\nPosition : %d", m_pSheet->m_lCurrentPosition + 1 );

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}


void SVPPQEntryDialogCameraPageClass::OnAddButton() 
{
	ASSERT( m_pSheet );
	UpdateData( TRUE );

	int index = m_AvailableList.GetCurSel();
	if( m_pSheet && index != LB_ERR )
	{
		SVVirtualCamera* pCamera = (SVVirtualCamera*) m_AvailableList.GetItemData( index );
		m_AvailableList.DeleteString( index );

		if( nullptr != pCamera )
		{
			index = m_SelectedList.AddString( pCamera->GetName() );
			m_SelectedList.SetItemData( index, reinterpret_cast<DWORD_PTR>(pCamera) );
		}// end if
		
	}// end if

	UpdateData( FALSE );
}

void SVPPQEntryDialogCameraPageClass::OnRemoveButton() 
{
	ASSERT( m_pSheet );
	UpdateData( TRUE );

	int index = m_SelectedList.GetCurSel();
	if( m_pSheet && index != LB_ERR )
	{
		SVVirtualCamera* pCamera = (SVVirtualCamera*) m_SelectedList.GetItemData( index );
		m_SelectedList.DeleteString( index );

		if( nullptr != pCamera )
		{
			index = m_AvailableList.AddString( pCamera->GetName() );
			m_AvailableList.SetItemData( index, reinterpret_cast<DWORD_PTR>(pCamera) );
		}// end if
		
	}// end if

	UpdateData( FALSE );
}

void SVPPQEntryDialogCameraPageClass::OnOK() 
{
	UpdateData( TRUE );
	ASSERT( m_pSheet );

	SVVirtualCamera* pCamera( nullptr );
	
	m_bIsTaken = ( m_SelectedList.GetCount() > 0 );

	// Check selected camera list ( remove deselected items )...
	int k = m_SelectedList.GetCount() - 1;
	for( ; k >= 0; -- k )
	{
		pCamera = reinterpret_cast<SVVirtualCamera*> (m_SelectedList.GetItemData( k ));
		if( nullptr != pCamera )
		{
			m_pSheet->m_pPPQ->SetCameraPPQPosition( m_pSheet->m_lCurrentPosition, pCamera );
			m_SelectedList.DeleteString( k );
		}// end if

	}// end for

	// And now add new items...( remainder in selected list box control )
	for( int i = m_AvailableList.GetCount() - 1; i >= 0; -- i )
	{
		pCamera = (SVVirtualCamera*) m_AvailableList.GetItemData( i );
		if( pCamera )
		{
			m_pSheet->m_pPPQ->SetCameraPPQPosition( -1, pCamera );
			m_AvailableList.DeleteString( k );
		}// end if

	}// end for

	SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
	
	CPropertyPage::OnOK();

}// end OnOk

IMPLEMENT_DYNAMIC(SVPPQEntryDialogPropertySheetClass, CPropertySheet)

SVPPQEntryDialogPropertySheetClass::SVPPQEntryDialogPropertySheetClass(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_lCurrentPosition	= -1;
	m_bIsTaken			= FALSE;

	addPages();
}

SVPPQEntryDialogPropertySheetClass::SVPPQEntryDialogPropertySheetClass(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_lCurrentPosition	= -1;
	m_bIsTaken			= FALSE;

	addPages();
}

SVPPQEntryDialogPropertySheetClass::~SVPPQEntryDialogPropertySheetClass()
{
}

void SVPPQEntryDialogPropertySheetClass::addPages()
{
	m_oCameraPage.m_pSheet = this;
	AddPage( &m_oCameraPage );
	m_oDigitalPage.m_pSheet = this;
	AddPage( &m_oDigitalPage );
	m_oRemotePage.m_pSheet = this;
	AddPage( &m_oRemotePage );
}

BEGIN_MESSAGE_MAP(SVPPQEntryDialogPropertySheetClass, CPropertySheet)
	//{{AFX_MSG_MAP(SVPPQEntryDialogPropertySheetClass)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVPPQEntryDialogPropertySheetClass message handlers

INT_PTR SVPPQEntryDialogPropertySheetClass::DoModal() 
{
	INT_PTR nRetVal = CPropertySheet::DoModal();

	// Check IsTaken state of pages...
	m_bIsTaken = m_oCameraPage.m_bIsTaken;

	return nRetVal;
}// end DoModal

////////////////////////////////////////////////////////////////////////////////
// This function will check that the position is not the last in next trigger mode.
// Adding inputs to the last position will result in a NAK in next trigger mode.
BOOL SVPPQEntryDialogPropertySheetClass::OkToAdd()
{
	BOOL bOK = TRUE;
	SVPPQOutputModeEnum eMode = SVPPQUnknownMode;
	m_pPPQ->GetPPQOutputMode( eMode );
	if( eMode == SVPPQNextTriggerMode )
	{
		long iLen;
		m_pPPQ->GetPPQLength(iLen);
		if( m_lCurrentPosition+1 >= iLen)
		{
			bOK = FALSE;
		}
	}
	return bOK;
}

BOOL SVPPQEntryDialogCameraPageClass::OnApply() 
{
	// Set is taken flag...
	m_bIsTaken = ( m_SelectedList.GetCount() > 0 );
	
	return CPropertyPage::OnApply();
}

