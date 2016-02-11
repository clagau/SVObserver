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
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVOGui\ISVPropertyPageDialog.h"
#include "SVObjectLibrary\SVObjectReference.h"
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
	SVScalarValueVector CurrentValues;
	SVScalarValueVector CurrentImages;
	SVScalarValueVector CurrentConditionals;

	m_rInspection.GetConditionalHistoryList( CurrentValues, CurrentImages, CurrentConditionals );

	m_Values = ConvertList( CurrentValues );
	m_Images = ConvertList( CurrentImages );
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

	SelectedObjectsPage* pValuesDlg = new SelectedObjectsPage( m_rInspection, _T("Values"), m_Values, SV_CH_VALUE );
	AddPage(pValuesDlg);

	SelectedObjectsPage* pImagesDlg = new SelectedObjectsPage( m_rInspection, _T("Images"), m_Images, SV_CH_IMAGE );
	AddPage(pImagesDlg);

	SelectedObjectsPage* pConditionalsDlg = new SelectedObjectsPage( m_rInspection, _T("Conditions"), m_Conditionals, SV_CH_CONDITIONAL );
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

	UpdateData( FALSE );
	return bResult;
}

void SVConditionalHistorySheet::OnOK() 
{
	int iNumPages = GetPageCount();
	bool Changed( false );
	bool ListsChanged( false );
	int i=0;

	UpdateData( TRUE );

	for( i = 0; i < iNumPages; i++ )
	{
		if ( SvOg::ISVPropertyPageDialog* pIDlg = dynamic_cast <SvOg::ISVPropertyPageDialog*> ( GetPage(i) ) )
		{
			if ( false == pIDlg->QueryAllowExit() )
			{
				return;
			}
		}

		SelectedObjectsPage* pPage = dynamic_cast <SelectedObjectsPage*> ( GetPage(i) );
		if( setChangedData( pPage ) )
		{
			ListsChanged = true;
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

		CurrentValues = ConvertList( m_Values );
		CurrentImages = ConvertList( m_Images );
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

	if( nullptr != pPage && NULL != pPage->GetSafeHwnd() )
	{
		CPropertyPage* pActivePage = GetActivePage();
		if( pPage == pActivePage )
		{
			pPage->OnKillActive();
		}

		switch( pPage->getAttributeFilter() )
		{
		case SV_CH_VALUE:
			pList = &m_Values;
			break;
		case SV_CH_IMAGE:
			pList = &m_Images;
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVConditionalHistorySheet.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:02:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Jul 2012 16:39:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Dec 2010 07:51:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   25 Jan 2006 12:36:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added object list functionality
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Jan 2006 14:17:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed Conditional History bugs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Jan 2006 14:55:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed property updating
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:19:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

