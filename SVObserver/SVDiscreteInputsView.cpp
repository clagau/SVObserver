//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDiscreteInputsView.cpp
//* .File Name       : $Workfile:   SVDiscreteInputsView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   01 Oct 2013 12:48:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVConfigurationObject.h"
#include "SVDiscreteInputsView.h"
#include "SVIOAdjustDialog.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#include "SVIOLibrary/SVDigitalInputObject.h"
#include "SVIOLibrary/SVInputObjectList.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOResource/ConstGlobalSvOr.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr const char* cDigitalInputName = ("Digital Input {:d}");

IMPLEMENT_DYNCREATE(SVDiscreteInputsView, CListView)

BEGIN_MESSAGE_MAP(SVDiscreteInputsView, CListView)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY( )
END_MESSAGE_MAP()

SVDiscreteInputsView::SVDiscreteInputsView() :
	m_rCtrl(GetListCtrl())
{
}

BOOL SVDiscreteInputsView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	dwStyle |= LVS_REPORT;
	dwStyle &= ~LVS_SORTASCENDING;
	dwStyle &= ~LVS_SORTDESCENDING;

	BOOL RetVal = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	m_ImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 2, 1 );
	m_ImageList.Add( AfxGetApp()->LoadIcon( IDI_IOITEM_ICON ) );
	m_ImageList.Add( AfxGetApp()->LoadIcon( IDI_NOIOITEM_ICON ) );

	m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 1, 1 );
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_INPUT_ICON ) );

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

void SVDiscreteInputsView::OnUpdate( CView* , LPARAM , CObject*  )
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
		if ( nullptr != pConfig ) { pInputList = pConfig->GetInputObjectList(); }
		if ( nullptr == pInputList )
		{
			SvStl::MessageManager e(SvStl::MsgType::Log );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingInputObjectList, SvStl::SourceFileParams(StdMessageParams));
			DebugBreak();
			return; 
		}

		SVIOEntryHostStructPtrVector inputEntryVector = pInputList->getInputList();

		// Module Inputs
		long maxInput {SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount()};
		for(long  i = 0; i < maxInput; ++i )
		{
			// First column: Module I/O
			std::string Item = std::format(cDigitalInputName, i + 1 );
			m_rCtrl.InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE,
									  i, Item.c_str(),
									  INDEXTOSTATEIMAGEMASK(1),
									  LVIS_STATEIMAGEMASK,
									  1, 0 );

			for(const auto& pIOEntry : inputEntryVector)
			{
				if(pIOEntry->m_ObjectType != IO_DIGITAL_INPUT)
				{
					continue;
				}

				SVDigitalInputObject* pDigInput = dynamic_cast<SVDigitalInputObject*> (SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId ));

				if(nullptr == pDigInput)
				{
					continue;
				}

				if( i == pDigInput->GetChannel() )
				{
					m_rCtrl.SetItem( i, 0, LVIF_IMAGE, nullptr, 0, 0, 0, 0 );

					m_Items[i] = pIOEntry;

					// Column: Description
					m_rCtrl.SetItemText( i, 1, pDigInput->GetName() );

					// Column: Force
					if( pDigInput->IsForced() )
					{
						Item = std::format( _T( "{:d}" ), pDigInput->GetForcedValue() ? 1 : 0 );
						m_rCtrl.SetItemText( i, 2, Item.c_str() );
					}// end if

					// Column: Inverted
					Item = pDigInput->IsInverted() ? _T( "1" ) : _T( "" );
					m_rCtrl.SetItemText( i, 3, Item.c_str() );

					break;
				}
			}
		}
		m_rCtrl.SetRedraw(true);
	}

	//CListView::OnUpdate( pSender, lHint, pHint );   //This call will cause flicker
}

void SVDiscreteInputsView::OnLButtonDblClk(UINT, CPoint point)
{
	SVDigitalInputObject *pDigInput;
	SVIOEntryHostStructPtrVector ppIOEntries;
	SVIOEntryHostStructPtr pIOEntry;
	UINT flags;

	int item = m_rCtrl.HitTest( point, &flags );
	if (false == SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) && SvOi::isOkToEdit())
	{
		if( item >= 0 && item < m_rCtrl.GetItemCount() && flags & LVHT_ONITEM)
		{
			const auto iter = m_Items.find(item);
			if (m_Items.end() != iter)
			{
				pIOEntry = iter->second;
			}

			pDigInput = dynamic_cast< SVDigitalInputObject* >( SVObjectManagerClass::Instance().GetObject( pIOEntry->m_IOId ) );
			if( pDigInput )
			{
				SVIOAdjustDialog dlg;
				dlg.m_IOName = m_rCtrl.GetItemText( item, 1 );
				dlg.m_IOValue.Format( "%d", pDigInput->GetValue() ? 1 : 0 );
				dlg.m_pDigInput = pDigInput;
				dlg.m_pLinkedObject = nullptr != pIOEntry ? pIOEntry->getObject() : nullptr;
				dlg.m_ioObjectType = SVIOObjectType::IO_DIGITAL_INPUT;

				SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};
				if(ID_OK == dlg.DoModal())
				{
					SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
				}

				OnUpdate( nullptr, 0, nullptr );
			}
		}
	}
}
