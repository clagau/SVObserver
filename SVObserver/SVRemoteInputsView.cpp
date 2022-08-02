//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteInputsView
//* .File Name       : $Workfile:   SVRemoteInputsView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   02 Oct 2013 07:58:46  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVConfigurationObject.h"
#include "SVRemoteInputsView.h"
#include "SVRemoteIOAdjustDialog.h"
#include "TextDefinesSvO.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#include "SVIOLibrary/SVInputObjectList.h"
#include "SVIOLibrary/SVRemoteInputObject.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVValueObjectLibrary/SVVariantValueObjectClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

IMPLEMENT_DYNCREATE(SVRemoteInputsView, CListView)

BEGIN_MESSAGE_MAP(SVRemoteInputsView, CListView)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

SVRemoteInputsView::SVRemoteInputsView() :
	m_rCtrl(GetListCtrl())
{
}

BOOL SVRemoteInputsView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	dwStyle |= LVS_REPORT;
	dwStyle &= ~LVS_SORTASCENDING;
	dwStyle &= ~LVS_SORTDESCENDING;

	BOOL RetVal = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	m_ImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 2, 1 );
	m_ImageList.Add( AfxGetApp()->LoadIcon( IDI_IOITEM_ICON ) );
	m_ImageList.Add( AfxGetApp()->LoadIcon( IDI_NOIOITEM_ICON ) );

	m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 2, 1 );
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_INPUT_ICON ) );
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_OUTPUT_ICON ) );

	m_rCtrl.SetImageList( &m_StateImageList, LVSIL_STATE );
	m_rCtrl.SetImageList( &m_ImageList, LVSIL_SMALL );

	m_rCtrl.InsertColumn( 0, _T( "Inputs" ), LVCFMT_LEFT, -1, -1 );
	m_rCtrl.InsertColumn( 1, _T( "Description" ), LVCFMT_LEFT, -1, -1 );
	m_rCtrl.InsertColumn( 2, _T( "Forced" ), LVCFMT_LEFT, -1, -1 );
	m_rCtrl.InsertColumn( 3, _T( "Inverted" ), LVCFMT_LEFT, -1, -1 );

	m_rCtrl.SetColumnWidth( 0, 125 );
	m_rCtrl.SetColumnWidth( 1, 500 );
	m_rCtrl.SetColumnWidth( 2,  50 );
	m_rCtrl.SetColumnWidth( 3,  55 );

	return RetVal;
}

void SVRemoteInputsView::OnUpdate( CView* , LPARAM , CObject*  )
{
	if(::IsWindow( m_hWnd ) )
	{
		m_rCtrl.SetRedraw(false);

		// First clean up list view
		m_rCtrl.DeleteAllItems();

		m_Items.clear();

		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		SVInputObjectList* pInputList = nullptr;
		// Get list of available inputs
		if( nullptr != pConfig ) { pInputList = pConfig->GetInputObjectList(); }
		if( nullptr == pInputList )
		{
			SvStl::MessageManager e(SvStl::MsgType::Log );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingInputObjectList, SvStl::SourceFileParams(StdMessageParams));
			DebugBreak();
			return;
		}

		SVIOEntryHostStructPtrVector inputEntryVector = pInputList->getInputList();

		int itemIndex {0};
		// Find each remote input
		for(int i = 0; i < static_cast<int>(inputEntryVector.size()); ++i)
		{
			std::string Item = SvUl::Format(SvO::cRemoteInputNumberLabel, itemIndex + 1 );

			bool bFound = false;
			SVIOEntryHostStructPtr pIOEntryFound;
			std::string name;
			for(const auto& pIOEntry : inputEntryVector)
			{
				if( pIOEntry->m_ObjectType != IO_REMOTE_INPUT )
				{
					continue;
				}

				SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(pIOEntry->m_IOId);
				if(nullptr == pObject || SvPb::SVRemoteInputObjectType != pObject->GetObjectSubType())
				{
					continue;
				}
				if (Item == pObject->GetName())
				{
					pIOEntryFound = pIOEntry;
					name = pObject->GetName();
					bFound = true;
					break;
				}
			}

			if ( bFound )
			{
				m_rCtrl.InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE, itemIndex, Item.c_str(),
					INDEXTOSTATEIMAGEMASK( 1 ),	// state
					LVIS_STATEIMAGEMASK,		// stateMask
					1, 0 );						// Set item data to Nothing

				m_rCtrl.SetItem(itemIndex, 0, LVIF_IMAGE, nullptr, 0, 0, 0, 0);

				m_Items[itemIndex] = pIOEntryFound;

				// Column: Description
				m_rCtrl.SetItemText(itemIndex, 1, name.c_str());
				++itemIndex;
			}
		}
		m_rCtrl.SetRedraw(true);
	}
}

void SVRemoteInputsView::OnLButtonDblClk(UINT, CPoint point)
{
	UINT flags;

	int item = m_rCtrl.HitTest( point, &flags );
	if ( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING | SV_STATE_TEST ) && SvOi::isOkToEdit())
	{
		if( item >= 0 && item < m_rCtrl.GetItemCount() && flags & LVHT_ONITEM)
		{
			SVIOEntryHostStructPtr pIOEntry;
			const auto iter = m_Items.find(item);
			if (m_Items.end() != iter)
			{
				pIOEntry = iter->second;
			}

			if(nullptr != pIOEntry)
			{
				SVRemoteInputObject* pRemoteInput = dynamic_cast< SVRemoteInputObject* >(SVObjectManagerClass::Instance().GetObject(pIOEntry->m_IOId));
				if( pRemoteInput )
				{
					_variant_t value;
					SVRemoteIOAdjustDialog dlg;

					std::string Name = m_rCtrl.GetItemText(item, 1).GetString();

					pRemoteInput->Read(value);

					dlg.SetIOName( Name.c_str() );
					dlg.SetIOValue( value );

					SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};
					if(IDOK ==  dlg.DoModal())
					{
						dlg.GetIOValue( value );

						pRemoteInput->writeCache( value );

						SvVol::SVVariantValueObjectClass* pValueObject = dynamic_cast<SvVol::SVVariantValueObjectClass*> ( pIOEntry->getObject() );
						if( nullptr != pValueObject )
						{
							pValueObject->SetDefaultValue( value, true );
						}

						SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
					}
					OnUpdate( nullptr, 0, nullptr );
				}
			}
		}
	}
}
