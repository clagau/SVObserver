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
#include "SVIPDoc.h"
#include "InspectionCommands/CommandExternalHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(SVDataDefinitionSheet, CPropertySheet)
	ON_COMMAND(IDOK,OnOK)
END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
SVDataDefinitionSheet::SVDataDefinitionSheet(SVIPDoc* pDoc, LPCTSTR pszCaption, const std::string& rInspectionName, uint32_t inspectionID, CWnd* pParentWnd, UINT iSelectPage)
: CPropertySheet(pszCaption, pParentWnd, iSelectPage)
, m_InspectionName( rInspectionName )
, m_InspectionID( inspectionID )
, m_pDoc(pDoc) 
{
	CreatePages();
}

SVDataDefinitionSheet::~SVDataDefinitionSheet()
{
	DestroyPages();
}
#pragma endregion Constructor

#pragma region Public Methods
#pragma endregion Public Methods

#pragma region Private Methods
HRESULT SVDataDefinitionSheet::CreatePages()
{
	initSelectedList( &m_ValueList, SvPb::dataDefinitionValue, SvPb::allValueObjects );
	initSelectedList( &m_ImageList, SvPb::dataDefinitionImage, SvPb::allImageObjects );

	SelectedObjectsPage* pValuesDlg = new SelectedObjectsPage( m_InspectionName, m_InspectionID, _T("Value Names"), m_ValueList, SvPb::dataDefinitionValue, SvPb::allValueObjects );
	AddPage(pValuesDlg);

	SelectedObjectsPage* pImagesDlg = new SelectedObjectsPage( m_InspectionName, m_InspectionID, _T("Image Names"), m_ImageList, SvPb::dataDefinitionImage, SvPb::allImageObjects );
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
		if( nullptr != m_pDoc )
		{
			m_pDoc->SetModifiedFlag();
		}
	}

	EndDialog(IDOK);
}

void SVDataDefinitionSheet::initSelectedList(SvDef::StringVector* pList, UINT Attribute, SvPb::ObjectSelectorType type)
{
	if( nullptr != pList )
	{
		pList->clear();
		//This is used to retrieve the list which have the attribute set
		SvPb::InspectionCmdRequest requestCmd;
		SvPb::InspectionCmdResponse responseCmd;
		*requestCmd.mutable_getobjectselectoritemsrequest() = SvCmd::createObjectSelectorRequest(
		{ SvPb::SearchArea::toolsetItems }, m_InspectionID, static_cast<SvPb::ObjectAttributes> (Attribute), SvDef::InvalidObjectId, false, type, SvPb::GetObjectSelectorItemsRequest::kAttributesSet);

		SvCmd::InspectionCommands(m_InspectionID, requestCmd, &responseCmd);
		if (responseCmd.has_getobjectselectoritemsresponse())
		{
			insertObjectsToList(responseCmd.getobjectselectoritemsresponse().tree(), pList);
		}
	}
}

bool SVDataDefinitionSheet::setChangedData( SelectedObjectsPage* const pPage )
{
	bool Result( false );

	if( nullptr != pPage && nullptr != pPage->GetSafeHwnd() )
	{
		const CPropertyPage* pActivePage = GetActivePage();
		if( pPage == pActivePage )
		{
			pPage->OnKillActive();
		}

		SvDef::StringVector* pList(nullptr);
		switch( pPage->getAttributeFilter() )
		{
		case SvPb::dataDefinitionValue:
			pList = &m_ValueList;
			break;
		case SvPb::dataDefinitionImage:
			pList = &m_ImageList;
			break;
		default:
			break;
		}

		if( nullptr != pList )
		{
			const SvDef::StringVector& rCurrentList( pPage->getList() );

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

void SVDataDefinitionSheet::setAttributes( const SvDef::StringVector& rList, UINT Attribute, bool Clear ) const
{
	for(auto const& rEntry : rList)
	{
		SVObjectReference ObjectRef{rEntry};
		//Reset the attribute 
		SvOi::SetAttributeType AddRemoveType = !Clear ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		ObjectRef.SetObjectAttributesSet( Attribute, AddRemoveType );
	}
}

void SVDataDefinitionSheet::insertObjectsToList(const SvPb::TreeItem& rTree, SvDef::StringVector* pList)
{
	for (const auto& rItem : rTree.children())
	{
		if(0 == rItem.children_size())
		{
			pList->emplace_back(rItem.objectidindex());
		}
		else
		{
			insertObjectsToList(rItem, pList);
		}
	}
}
