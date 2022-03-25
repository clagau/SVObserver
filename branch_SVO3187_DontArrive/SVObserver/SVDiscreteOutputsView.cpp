//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDiscreteOutputsView
//* .File Name       : $Workfile:   SVDiscreteOutputsView.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   01 Oct 2013 12:48:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVConfigurationObject.h"
#include "SVDiscreteOutputsView.h"
#include "SVIOAdjustDialog.h"
#include "SVIOController.h"
#include "SVIODoc.h"
#include "SVPPQObject.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVOResource\ConstGlobalSvOr.h"
#include "SVIOLibrary/SVDigitalOutputObject.h"
#include "SVIOLibrary/SVOutputObjectList.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr const char* cDigitalOutputName = ("Digital Output %ld");

IMPLEMENT_DYNCREATE(SVDiscreteOutputsView, CListView)

BEGIN_MESSAGE_MAP(SVDiscreteOutputsView, CListView)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
END_MESSAGE_MAP()

SVDiscreteOutputsView::SVDiscreteOutputsView() :
	m_rCtrl(GetListCtrl())
{
}

BOOL SVDiscreteOutputsView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	dwStyle |= LVS_REPORT;
	dwStyle &= ~LVS_SORTASCENDING;
	dwStyle &= ~LVS_SORTDESCENDING;

	BOOL RetVal = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	m_ImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 2, 1 );
	m_ImageList.Add( AfxGetApp()->LoadIcon( IDI_IOITEM_ICON ) );
	m_ImageList.Add( AfxGetApp()->LoadIcon( IDI_NOIOITEM_ICON ) );

	m_StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 1, 1 );
	m_StateImageList.Add( AfxGetApp()->LoadIcon( IDI_OUTPUT_ICON ) );


	m_rCtrl.SetImageList( &m_StateImageList, LVSIL_STATE );
	m_rCtrl.SetImageList( &m_ImageList, LVSIL_SMALL );


	m_rCtrl.InsertColumn( 0, _T( "Outputs" ), LVCFMT_LEFT, -1, -1 );
	m_rCtrl.InsertColumn( 1, _T( "Description" ), LVCFMT_LEFT, -1, -1 );
	m_rCtrl.InsertColumn( 2, _T( "Forced" ), LVCFMT_LEFT, -1, -1 );
	m_rCtrl.InsertColumn( 3, _T( "Inverted" ), LVCFMT_LEFT, -1, -1 );
	
	m_rCtrl.SetColumnWidth( 0, 125 );
	m_rCtrl.SetColumnWidth( 1, 500 );
	m_rCtrl.SetColumnWidth( 2,  50 );
	m_rCtrl.SetColumnWidth( 3,  55 );

	m_pDocument = dynamic_cast<SVIODoc*> (CListView::GetDocument());
	return RetVal;
}

void SVDiscreteOutputsView::OnUpdate( CView* , LPARAM , CObject*  ) 
{
	SVIOController* pIOController{nullptr};

	if( nullptr != m_pDocument)
	{
		pIOController = m_pDocument->GetIOController();
	}

	if( nullptr != pIOController && ::IsWindow( m_hWnd ) )
	{
		m_rCtrl.SetRedraw(false);

		// First clean up list view
		m_rCtrl.DeleteAllItems();

		m_Items.clear();
		SVConfigurationObject* pConfig( nullptr );
		SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

		// Get the number of PPQs
		long lPPQSize = 0;
		if( nullptr != pConfig )
		{
			lPPQSize = pConfig->GetPPQCount( );
		}
		else
		{
			SvStl::MessageManager e(SvStl::MsgType::Log );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQCount, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17010_ErrorGettingPPQCount );
			DebugBreak();
		}

		// Check if any PPQs are here yet
		if( 0 == lPPQSize ) { return; }

		// Result Outputs
		long maxOutput {SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount()};
		for(long i = 0; i < maxOutput; ++i)
		{
			// First column: Result I/O
			std::string Item = SvUl::Format(cDigitalOutputName, i + 1);
			m_rCtrl.InsertItem( LVIF_IMAGE | LVIF_TEXT | LVIF_STATE, 
				i, Item.c_str(), 
				INDEXTOSTATEIMAGEMASK(1),
				LVIS_STATEIMAGEMASK,
				1, 0 );

			// Check Module Ready first
			if(setListItem(i, pIOController->GetModuleReady())) {continue;}

			for(int j = 0; j < lPPQSize; ++j)
			{
				SVPPQObject* pPPQ = pConfig->GetPPQ(j);
				if (nullptr != pPPQ)
				{
					// Find each digital output
					for (const auto& pIOEntry : pPPQ->GetAllOutputs())
					{
						if (pIOEntry->m_ObjectType != SVIOObjectType::IO_DIGITAL_OUTPUT)
						{
							continue;
						}

						if (setListItem(i, pIOEntry))
						{
							break;
						}
					}
				}
				else
				{
					SvStl::MessageManager e(SvStl::MsgType::Log );
					e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQ, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17011_ErrorGettingPPQ );
				}
			}
		}
		m_rCtrl.SetRedraw(true);
	}
}

void SVDiscreteOutputsView::OnLButtonDblClk(UINT, CPoint point) 
{
	SVIOEntryHostStructPtr pIOEntry;
	SVOutputObjectPtr pOutput;
	UINT flags;

	int item = m_rCtrl.HitTest( point, &flags );
	SVIOController* pIOController{nullptr};

	if( nullptr != m_pDocument)
	{
		pIOController = m_pDocument->GetIOController();
	}
	if (false == SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) && SvOi::isOkToEdit() && nullptr != pIOController )
	{
		if( item >= 0 && item < m_rCtrl.GetItemCount() && 
			( flags & ( LVHT_ONITEMSTATEICON | LVHT_ONITEMICON | LVHT_ONITEMLABEL ) ) )
		{
			SVIOAdjustDialog dlg;
			SVConfigurationObject* pConfig(nullptr);
			SVOutputObjectList* pOutputList(nullptr);

			SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
			if (nullptr != pConfig)
			{ 
				pOutputList = pConfig->GetOutputObjectList(); 
			}

			// Search for In or Out
			const auto iter = m_Items.find(item);
			if(m_Items.end() !=  iter)
			{
				pIOEntry = iter->second;
			}
			SVDigitalOutputObject* pDigitalOutput{nullptr};
			if(nullptr != pOutputList)
			{
				if(nullptr != pIOEntry)
				{
					pOutput = pOutputList->GetOutput(pIOEntry->m_IOId);
					pDigitalOutput = dynamic_cast<SVDigitalOutputObject*> (pOutput.get());
				}
				else
				{
					pOutputList->DetachOutput(ObjectIdEnum::DigitalOutputId + item);

					pOutput = std::make_shared<SVDigitalOutputObject>();
					pOutput->updateObjectId(item);
					pDigitalOutput = dynamic_cast<SVDigitalOutputObject*> (pOutput.get());
					if(nullptr != pDigitalOutput)
					{
						pDigitalOutput->SetChannel(item);
						pDigitalOutput->SetName(_T(""));
						pDigitalOutput->Force(false, false);
						pDigitalOutput->Invert(true);
					}
				}
			}

			if( nullptr != pDigitalOutput )
			{
				dlg.IOName = _T( "Result " ) + m_rCtrl.GetItemText( item, 0 );
				dlg.IOName += _T( ", " ) + m_rCtrl.GetItemText( item, 1 );
				dlg.IOValue.Format( "%d", pDigitalOutput->GetValue() ? 1 : 0 );
				dlg.m_pIOEntry   = pIOEntry;
				dlg.m_pDigOutput = pDigitalOutput;

				SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};
				switch( dlg.DoModal() )
				{
				case IDOK:
					{
						SVSVIMStateClass::AddState( SV_STATE_MODIFIED );

						// Check if they picked a new output
						if( dlg.m_pIOEntry != pIOEntry )
						{
							if(nullptr != pIOEntry)
							{									
								// Make sure that we first reset the old output
								if( nullptr != pOutputList )
								{
									pOutputList->ResetOutput( pIOEntry );
								}// end if

								pIOEntry->m_Enabled = FALSE;
								pIOEntry->m_IOId = SvDef::InvalidObjectId;
							}// end if

							if(nullptr == dlg.m_pIOEntry)
							{
								if( nullptr != pOutputList )
								{
									pOutputList->DetachOutput( pOutput->getObjectId() );
								}
								pOutput = nullptr;

								if( pIOEntry == pIOController->GetModuleReady() )
								{
									pIOEntry->m_ObjectType = IO_INVALID_OBJECT;
									pIOEntry->m_IOId = SvDef::InvalidObjectId;
								}// end if

							}// end if
							else
							{
								dlg.m_pIOEntry->m_Enabled = TRUE;
								dlg.m_pDigOutput->SetName( dlg.m_pIOEntry->getObject()->GetCompleteName().c_str() );
								if(nullptr == pIOEntry && nullptr != pOutputList )
								{
									pOutputList->AttachOutput( pOutput );
								}// end if

								if( dlg.m_pIOEntry == pIOController->GetModuleReady() )
								{
									dlg.m_pIOEntry->m_ObjectType = IO_DIGITAL_OUTPUT;
									dlg.m_pIOEntry->m_IOId = dlg.m_pDigOutput->getObjectId();
								}// end if

							}// end else

							long lPPQSize = 0;
							// Force the PPQs to rebuild
							if ( nullptr != pConfig ){ lPPQSize = pConfig->GetPPQCount( ); }

							// Rebuild Outputs
							for(long k = 0; k < lPPQSize; k++ )
							{
								SVPPQObject* pPPQ = pConfig->GetPPQ( k );

								if( nullptr != pPPQ ){ pPPQ->RebuildOutputList(); }
							}// end for

						}// end if

						// Force IO board to update if they still have one selected
						if(nullptr != dlg.m_pIOEntry)
						{
							if( nullptr != pOutputList )
							{
								pOutputList->ResetOutput( dlg.m_pIOEntry );
							}// end if
						}// end if
						break;
					}

				case IDCANCEL:
				default:
					break;
				}// end switch

				OnUpdate( nullptr, 0, nullptr );
			}
			else
			{
				dlg.IOName	= _T( "" );
				dlg.IOValue.Format( "%d", 0 );

				if(IDOK == dlg.DoModal())
				{
					SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
				}
				OnUpdate( nullptr, 0, nullptr );
			}
		}
	}
}

bool SVDiscreteOutputsView::setListItem(int rowIndex, SVIOEntryHostStructPtr pIOEntry)
{
	if(nullptr != pIOEntry)
	{
		const SVDigitalOutputObject* pDigOutput = dynamic_cast<const SVDigitalOutputObject*>(SVObjectManagerClass::Instance().GetObject(pIOEntry->m_IOId));
		if (pDigOutput)
		{
			if (rowIndex == pDigOutput->GetChannel())
			{
				m_rCtrl.SetItem(rowIndex, 0, LVIF_IMAGE, nullptr, 0, 0, 0, 0);
				m_Items[rowIndex] = pIOEntry;
				m_rCtrl.SetItemText(rowIndex, 1, pDigOutput->GetName());

				if (pDigOutput->IsForced())
				{
					m_rCtrl.SetItemText(rowIndex, 2, pDigOutput->GetForcedValue() ? "1" : "0");
				}

				m_rCtrl.SetItemText(rowIndex, 3, pDigOutput->IsInverted() ? "1" : "");
				return true;
			}
		}
	}
	return false;
}