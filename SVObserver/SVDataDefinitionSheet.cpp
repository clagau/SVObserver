//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataDefinitionSheet
//* .File Name       : $Workfile:   SVDataDefinitionSheet.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:07:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDataDefinitionSheet.h"
#include "SelectedObjectsPage.h"
#include "SVOGui/ToolSetItemSelector.h"
#include "SVIPDoc.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(SVDataDefinitionSheet, CPropertySheet)

BEGIN_MESSAGE_MAP(SVDataDefinitionSheet, CPropertySheet)
	ON_COMMAND(IDOK,OnOK)
END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
SVDataDefinitionSheet::SVDataDefinitionSheet(LPCTSTR pszCaption, const SVString& rInspectionName, const SVGUID& rInspectionID, CWnd* pParentWnd, UINT iSelectPage)
: CPropertySheet(pszCaption, pParentWnd, iSelectPage), ISVCancel()
, m_InspectionName( rInspectionName )
, m_InspectionID( rInspectionID )
{
	CreatePages();
}

SVDataDefinitionSheet::~SVDataDefinitionSheet()
{
	DestroyPages();
}
#pragma endregion Constructor

#pragma region Public Methods
bool SVDataDefinitionSheet::CanCancel()
{
	return true;
}

HRESULT SVDataDefinitionSheet::GetCancelData(SVCancelData*& rpData)
{
	return E_NOTIMPL;
}

HRESULT SVDataDefinitionSheet::SetCancelData(SVCancelData* pData)
{
	return E_NOTIMPL;
}
#pragma endregion Public Methods

#pragma region Private Methods
HRESULT SVDataDefinitionSheet::CreatePages()
{
	initSelectedList( &m_ValueList, SV_DD_VALUE );
	initSelectedList( &m_ImageList, SV_DD_IMAGE );

	SelectedObjectsPage* pValuesDlg = new SelectedObjectsPage( m_InspectionName, m_InspectionID, _T("Value Names"), m_ValueList, SV_DD_VALUE );
	AddPage(pValuesDlg);

	SelectedObjectsPage* pImagesDlg = new SelectedObjectsPage( m_InspectionName, m_InspectionID, _T("Image Names"), m_ImageList, SV_DD_IMAGE );
	AddPage(pImagesDlg);

	return S_OK;
}

void SVDataDefinitionSheet::DestroyPages()
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

BOOL SVDataDefinitionSheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	

	bool bCancelable = CanCancel();
	CWnd* pWnd = GetDlgItem(IDCANCEL);
	if ( nullptr != pWnd )
	{
		pWnd->ShowWindow( bCancelable ? SW_SHOW : SW_HIDE );
	}

	UpdateData( false );
	return bResult;
}

void SVDataDefinitionSheet::OnOK() 
{
	int iNumPages = GetPageCount();
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

	if( ListsChanged )
	{
		SVIPDoc* pIPDoc( nullptr );
		pIPDoc = SVObjectManagerClass::Instance().GetIPDoc( m_InspectionID );

		if( nullptr != pIPDoc )
		{
			pIPDoc->SetModifiedFlag();
		}
	}

	EndDialog(IDOK);
}

void SVDataDefinitionSheet::initSelectedList( SvOsl::SelectorItemVector* pList, UINT Attribute )
{
	if( nullptr != pList )
	{
		SvOsl::SelectorOptions BuildOptions( m_InspectionID, Attribute );
		SvOg::ToolSetItemSelector<GuiCmd::AttributesSetFilterType> toolsetItemSelector;
		SvOi::ISelectorItemVectorPtr pToolsetList =  toolsetItemSelector( BuildOptions );
		//Copy list to member variable for easier use
		if( !pToolsetList.empty() )
		{
			SvOsl::SelectorItemVector* pSelectorList = dynamic_cast<SvOsl::SelectorItemVector*> (pToolsetList.get());
			if( nullptr != pSelectorList )
			{
				pList->swap(*pSelectorList);
			}
		}
	}
}

bool SVDataDefinitionSheet::setChangedData( SelectedObjectsPage* const pPage )
{
	bool Result( false );
	SvOsl::SelectorItemVector* pList( nullptr);

	if( nullptr != pPage && nullptr != pPage->GetSafeHwnd() )
	{
		const CPropertyPage* pActivePage = GetActivePage();
		if( pPage == pActivePage )
		{
			pPage->OnKillActive();
		}

		switch( pPage->getAttributeFilter() )
		{
		case SV_DD_VALUE:
			pList = &m_ValueList;
			break;
		case SV_DD_IMAGE:
			pList = &m_ImageList;
			break;
		default:
			break;
		}

		if( nullptr != pList )
		{
			const SvOsl::SelectorItemVector& rCurrentList( pPage->getList() );

			if( rCurrentList != *pList )
			{
				//List are different so first reset all attributes
				setAttributes( *pList, pPage->getAttributeFilter(), true );
				*pList = rCurrentList;
				//Now set the new attributes
				setAttributes( *pList, pPage->getAttributeFilter(), false );
				Result = true;
			}
		}
	}

	return Result;
}

void SVDataDefinitionSheet::setAttributes( const SvOsl::SelectorItemVector& rList, UINT Attribute, bool Clear ) const
{
	SvOsl::SelectorItemVector::const_iterator Iter;
	for( Iter = rList.begin(); rList.end() != Iter ; ++Iter )
	{
		if( Iter->isSelected() )
		{
			//The item key is the object GUID
			SVGUID ObjectGuid( Iter->getItemKey() );

			SVObjectClass* pObject( nullptr );
			SVObjectManagerClass::Instance().GetObjectByIdentifier( ObjectGuid, pObject );

			if ( nullptr != pObject )
			{
				SVObjectReference ObjectRef = pObject;
				//If an array must set the index
				if( Iter->isArray() )
				{
					ObjectRef.SetArrayIndex( Iter->getArrayIndex() );
				}
				UINT AttributeResult = ObjectRef.ObjectAttributesSet();
				//Reset the attribute 
				if( Clear )
				{
					AttributeResult &= ~Attribute;
				}
				else
				{
					AttributeResult |= Attribute;
				}

				ObjectRef.ObjectAttributesSetRef() = AttributeResult;
			}
		}
	}
}

