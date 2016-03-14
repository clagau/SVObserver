//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditionalHistorySheet
//* .File Name       : $Workfile:   SVConditionalHistorySheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:02:04  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVConditionalHistorySheet.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVOGui\ISVPropertyPageDialog.h"
#include "SVObjectLibrary\SVObjectReference.h"
#include "SelectedObjectsPage.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVIPDoc.h"
#include "SVScalarValue.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(SVConditionalHistorySheet, CPropertySheet)

BEGIN_MESSAGE_MAP(SVConditionalHistorySheet, CPropertySheet)
	ON_COMMAND(IDOK,OnOK)
END_MESSAGE_MAP()

static const int ControlHeight = 20;
#pragma endregion Declarations

#pragma region Constructor
SVConditionalHistorySheet::SVConditionalHistorySheet( LPCTSTR pszCaption, SVInspectionProcess& rInspection, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage), ISVCancel()
,	m_rInspection( rInspection )
{
	SVScalarValueVector CurrentValueList;
	SVScalarValueVector CurrentImageList;
	SVScalarValueVector CurrentConditionals;

	m_rInspection.GetConditionalHistoryList( CurrentValueList, CurrentImageList, CurrentConditionals );

	m_ValueList = ConvertList( CurrentValueList );
	m_ImageList = ConvertList( CurrentImageList );
	m_Conditionals = ConvertList( CurrentConditionals );

	SVScalarValueMapType mapProperties;
	m_rInspection.GetConditionalHistoryProperties( mapProperties );
	m_MaxHistory = atoi( mapProperties[PPQCH_Size].strValue );
	CString strOverwrite = mapProperties[PPQCH_Overwrite].strValue;
	SVBoolValueObjectClass bvo;
	bvo.SetValue( 1, strOverwrite );
	bvo.GetValue( m_Overwrite );

	m_MaxHistoryOriginal = m_MaxHistory;
	m_OverwriteOriginal = m_Overwrite;

	CreatePages();
}

SVConditionalHistorySheet::~SVConditionalHistorySheet()
{
	DestroyPages();
}
#pragma endregion Constructor

#pragma region Public Methods
bool SVConditionalHistorySheet::CanCancel()
{
	return true;
}

HRESULT SVConditionalHistorySheet::GetCancelData(SVCancelData*& rpData)
{
	return E_NOTIMPL;
}

HRESULT SVConditionalHistorySheet::SetCancelData(SVCancelData* pData)
{
	return E_NOTIMPL;
}
#pragma endregion Public Methods

#pragma region Private Methods
HRESULT SVConditionalHistorySheet::CreatePages()
{
	SVString name = m_rInspection.GetName();
	const SVGUID& rInspectionID = m_rInspection.GetUniqueObjectID();

	SelectedObjectsPage* pValuesDlg = new SelectedObjectsPage( name, rInspectionID, _T("Values"), m_ValueList, SV_CH_VALUE );
	AddPage(pValuesDlg);

	SelectedObjectsPage* pImagesDlg = new SelectedObjectsPage( name, rInspectionID, _T("Images"), m_ImageList, SV_CH_IMAGE );
	AddPage(pImagesDlg);

	SelectedObjectsPage* pConditionalsDlg = new SelectedObjectsPage( name, rInspectionID, _T("Conditions"), m_Conditionals, SV_CH_CONDITIONAL );
	AddPage(pConditionalsDlg);

	return S_OK;
}

void SVConditionalHistorySheet::DestroyPages()
{
	CPropertyPage* pPage;
	for( int i = GetPageCount() - 1; i >= 0; -- i )
	{
		pPage = GetPage( i );
		RemovePage( i );
		if( nullptr != pPage )
		{
			delete pPage;
		}
	}
}

void SVConditionalHistorySheet::DoDataExchange(CDataExchange* pDX)
{
	CPropertySheet::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MAX_HISTORY, m_MaxHistory);
	DDX_Check(pDX, IDC_CHECK_OVERWRITE, m_Overwrite);
}

BOOL SVConditionalHistorySheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	CRect Rect;
	bool bCancelable = CanCancel();	// from ISVCancel base //SVPropertySheetCancelImpl base
	CWnd* pWnd = GetDlgItem(IDCANCEL);
	if( nullptr != pWnd )
	{
		pWnd->ShowWindow( bCancelable ? SW_SHOW : SW_HIDE );
		pWnd->GetWindowRect( &Rect );
		ScreenToClient( &Rect );

		//Use the top coordinate of the cancel button for these controls
		int Top = Rect.top;
		//Coordinates for the control
		Rect.SetRect( 10, Top, 35, Top + ControlHeight );
		m_MaxHistoryCtrl.Create( ES_CENTER | WS_VISIBLE | WS_GROUP, Rect, this, IDC_EDIT_MAX_HISTORY );
		//Coordinates for the static text
		Rect.SetRect( 40, Top, 120, Top + ControlHeight );
		m_MaxHistoryLabel.Create(_T("Max History"), WS_VISIBLE, Rect, this );
		//Coordinates for the check box
		Rect.SetRect( 130, Top, 300, Top + ControlHeight );
		m_OverwriteCtrl.Create( _T("Overwrite"), WS_VISIBLE | BS_AUTOCHECKBOX | WS_TABSTOP, Rect, this, IDC_CHECK_OVERWRITE );

	}

	CPropertyPage* pActivePage = GetActivePage();
	CFont* pFont( nullptr );
	if( nullptr != pActivePage )
	{
		pFont = pActivePage->GetFont();
		if( nullptr != pFont )
		{
			m_MaxHistoryCtrl.SetFont( pFont );
			m_MaxHistoryLabel.SetFont( pFont );
			m_OverwriteCtrl.SetFont( pFont );
		}
	}

	UpdateData( false );
	return bResult;
}

void SVConditionalHistorySheet::OnOK() 
{
	int iNumPages = GetPageCount();
	bool Changed( false );
	bool ListsChanged( false );

	UpdateData( true );

	for( int i = 0; i < iNumPages; i++ )
	{
		SelectedObjectsPage* pPage = dynamic_cast <SelectedObjectsPage*> ( GetPage(i) );
		if ( nullptr != pPage )
		{
			if( false == pPage->QueryAllowExit() )
			{
				return;
			}
			if( setChangedData( pPage ) )
			{
				ListsChanged = true;
			}
		}
	}

	if( m_MaxHistory != m_MaxHistoryOriginal || m_Overwrite != m_OverwriteOriginal )
	{
		SVScalarValueVector vecProperties;
		vecProperties.push_back( SVScalarValue(PPQCH_Size, AsString(m_MaxHistory)) );
		vecProperties.push_back( SVScalarValue(PPQCH_Overwrite, AsString(m_Overwrite) ) );

		m_rInspection.SetConditionalHistoryProperties( vecProperties );
		Changed = true;
	}

	if( ListsChanged )
	{
		SVScalarValueVector CurrentValues;
		SVScalarValueVector CurrentImages;
		SVScalarValueVector CurrentConditionals;

		CurrentValues = ConvertList( m_ValueList );
		CurrentImages = ConvertList( m_ImageList );
		CurrentConditionals = ConvertList( m_Conditionals );
		m_rInspection.SetConditionalHistoryList( &CurrentValues, &CurrentImages, &CurrentConditionals );
		Changed = true;
	}

	if( Changed )
	{
		SVIPDoc* pIPDoc( nullptr );
		pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( m_rInspection.GetUniqueObjectID() );

		if( nullptr != pIPDoc )
		{
			pIPDoc->SetModifiedFlag();
		}
	}

	EndDialog(IDOK);
}

bool SVConditionalHistorySheet::setChangedData( SelectedObjectsPage* const pPage )
{
	bool Result( false );
	SvOsl::SelectorItemVector* pList( nullptr);

	if( nullptr != pPage && nullptr != pPage->GetSafeHwnd() )
	{
		CPropertyPage* pActivePage = GetActivePage();
		if( pPage == pActivePage )
		{
			pPage->OnKillActive();
		}

		switch( pPage->getAttributeFilter() )
		{
		case SV_CH_VALUE:
			pList = &m_ValueList;
			break;
		case SV_CH_IMAGE:
			pList = &m_ImageList;
			break;
		case SV_CH_CONDITIONAL:
			pList = &m_Conditionals;
			break;
		default:
			break;
		}

		if( nullptr != pList )
		{
			const SvOsl::SelectorItemVector& rCurrentList( pPage->getList() );

			if( rCurrentList != *pList )
			{
				*pList = rCurrentList;
				Result = true;
			}
		}
	}

	return Result;
}

SvOsl::SelectorItemVector SVConditionalHistorySheet::ConvertList( const SVScalarValueVector& rSelectedList ) const
{
	SvOsl::SelectorItemVector Result;

	std::for_each(rSelectedList.begin(), rSelectedList.end(), [&Result](const SVScalarValue& rScalarValue)->void
	{
		SvOsl::SelectorItem InsertItem;
		SVObjectReference ObjectRef( rScalarValue.object );

		InsertItem.setName( ObjectRef.GetName() );
		InsertItem.setLocation( ObjectRef.GetCompleteOneBasedObjectName() );
		InsertItem.setItemKey( ObjectRef->GetUniqueObjectID().ToVARIANT() );
		//For the list we don't need the data type
		if( ObjectRef->IsArray() )
		{
			InsertItem.setArrayIndex( ObjectRef.ArrayIndex() );
		}
		
		Result.push_back( InsertItem );
	});

	return Result;
}

SVScalarValueVector SVConditionalHistorySheet::ConvertList( const SvOsl::SelectorItemVector& rSelectedList ) const
{
	SVScalarValueVector Result;

	std::for_each( rSelectedList.begin(), rSelectedList.end(), [&Result](const SvOsl::SelectorItem& rSelectedItem)->void
	{
		SVGUID ObjectGuid( rSelectedItem.getItemKey() );
		SVObjectClass* pObject( nullptr );
		SVObjectManagerClass::Instance().GetObjectByIdentifier( ObjectGuid, pObject );

		if ( nullptr != pObject )
		{
			SVObjectReference ObjectRef( pObject );

			//Note here we don't have whole arrays only with index
			if( rSelectedItem.isArray() )
			{
				ObjectRef.SetArrayIndex( rSelectedItem.getArrayIndex() );
			}

			Result.push_back( SVScalarValue( ObjectRef ) );
		}
	});

	return Result;
}
#pragma endregion Private Methods

