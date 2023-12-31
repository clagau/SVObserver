//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVSharedMemoryLibrary/EditLock.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SvimState.h"
#pragma endregion Includes

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr const char* cDigitalOutputName = _T("Digital Output {:d}");
constexpr int cOutputColIndex = 0;
constexpr int cDescriptionColIndex = 1;
constexpr int cForcedColIndex = 2;
constexpr int cInvertedColIndex = 3;
constexpr int cOutputColWidth = 150;
constexpr int cDescriptionColWidth = 500;
constexpr int cForcedColWidth = 50;
constexpr int cInvertedColWidth = 55;

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


	m_rCtrl.InsertColumn(cOutputColIndex, _T( "Outputs" ), LVCFMT_LEFT, -1, -1);
	m_rCtrl.InsertColumn(cDescriptionColIndex, _T( "Description" ), LVCFMT_LEFT, -1, -1);
	m_rCtrl.InsertColumn(cForcedColIndex, _T( "Forced" ), LVCFMT_LEFT, -1, -1);
	m_rCtrl.InsertColumn(cInvertedColIndex, _T( "Inverted" ), LVCFMT_LEFT, -1, -1);
	
	m_rCtrl.SetColumnWidth(cOutputColIndex, cOutputColWidth);
	m_rCtrl.SetColumnWidth(cDescriptionColIndex, cDescriptionColWidth);
	m_rCtrl.SetColumnWidth(cForcedColIndex,  cForcedColWidth);
	m_rCtrl.SetColumnWidth(cInvertedColIndex,  cInvertedColWidth);

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
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQCount, SvStl::SourceFileParams(StdMessageParams));
			DebugBreak();
		}

		// Check if any PPQs are here yet
		if( 0 == lPPQSize ) { return; }

		// Result Outputs
		long maxOutput {SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount()};
		for(long i = 0; i < maxOutput; ++i)
		{
			// First column: Result I/O
			std::string Item = std::format(cDigitalOutputName, i + 1);
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
					for (const auto& pIOEntry : pPPQ->getUsedOutputs())
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
					e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQ, SvStl::SourceFileParams(StdMessageParams));
				}
			}
		}
		m_rCtrl.SetRedraw(true);
	}
}

void SVDiscreteOutputsView::OnLButtonDblClk(UINT, CPoint point) 
{//@TODO [Arvid][10.30][15.3.2023] this function is too long
	SVIOController* pIOController{nullptr};

	if( nullptr != m_pDocument)
	{
		pIOController = m_pDocument->GetIOController();
	}
	if (false == SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) && SvOi::isOkToEdit() && nullptr != pIOController )
	{
		UINT flags {0};

		int item = m_rCtrl.HitTest(point, &flags);

		std::vector<std::string> usedOutputList;
		for (const auto& rEntry : m_Items)
		{
			if (rEntry.second->m_ObjectType != SVIOObjectType::IO_DIGITAL_OUTPUT)
			{
				continue;
			}

			SVObjectClass* pOutput = SVObjectManagerClass::Instance().GetObject(rEntry.second->m_IOId);
			if (nullptr != pOutput)
			{
				usedOutputList.emplace_back(std::string(pOutput->GetName()));
			}
		}

		if( item >= 0 && item < m_rCtrl.GetItemCount() &&
			( flags & ( LVHT_ONITEMSTATEICON | LVHT_ONITEMICON | LVHT_ONITEMLABEL ) ) )
		{
			SVIOAdjustDialog dlg {usedOutputList};
			SVConfigurationObject* pConfig(nullptr);
			SVOutputObjectList* pOutputList(nullptr);

			SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
			if (nullptr != pConfig)
			{ 
				pOutputList = pConfig->GetOutputObjectList(); 
			}

			// Search for In or Out
			const auto iter = m_Items.find(item);
			SVIOEntryHostStructPtr pIOEntry;
			SVOutputObjectPtr pOutput;
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
						bool invert = SVIM_PRODUCT_ETHERCATIO != pConfig->GetProductType();
						pDigitalOutput->Invert(invert);
					}
				}
			}

			if( nullptr != pDigitalOutput )
			{
				dlg.m_IOName = m_rCtrl.GetItemText(item, cDescriptionColIndex);
				dlg.m_IOValue.Format( "%d", pDigitalOutput->GetValue() ? 1 : 0 );
				SVObjectClass* pLinkedObject = nullptr != pIOEntry ? pIOEntry->getObject() : nullptr;
				dlg.m_pLinkedObject = pLinkedObject;
				dlg.m_pDigOutput = pDigitalOutput;
				dlg.m_ioObjectType = SVIOObjectType::IO_DIGITAL_OUTPUT;

				SvSml::TemporaryState_Editing tse;
				if (false == tse.stateWasEntered())
				{
					return;
				}
				if (IDOK == dlg.DoModal())
				{
					SvimState::AddState(SV_STATE_MODIFIED);
					if (nullptr != dlg.m_pLinkedObject && ObjectIdEnum::ModuleReadyId == dlg.m_pLinkedObject->getObjectId())
					{
						pIOEntry = pIOController->GetModuleReady();
						pIOEntry->m_ObjectType = dlg.m_ioObjectType;
					}
					if (dlg.m_pLinkedObject != pLinkedObject)
					{
						if (nullptr == pIOEntry)
						{
							pIOEntry = std::make_shared<SVIOEntryHostStruct>();
							pIOEntry->m_ObjectType = dlg.m_ioObjectType;
						}
						else
						{
							// Make sure that we first reset the old output
							if (nullptr != pOutputList)
							{
								pOutputList->ResetOutput(pIOEntry);
							}
							pIOEntry->m_Enabled = false;
							pIOEntry->m_IOId = SvDef::InvalidObjectId;
						}

						if (nullptr == dlg.m_pLinkedObject)
						{
							if (nullptr != pOutputList)
							{
								pOutputList->DetachOutput(pOutput->getObjectId());
							}
							pOutput = nullptr;
							pIOEntry->clear();
							pIOEntry.reset();
						}
						else
						{
							pIOEntry->m_Enabled = true;
							pIOEntry->setValueObject(dlg.m_pLinkedObject);
							if (nullptr != pOutput && nullptr != pOutputList)
							{
								pIOEntry->m_IOId = pOutput->getObjectId();
								pOutput->SetName(dlg.m_pLinkedObject->GetCompleteName().c_str());
								pOutput->SetValueObjectID(dlg.m_pLinkedObject->getObjectId());
								pOutputList->AttachOutput(pOutput);
							}
						}

						long lPPQSize = 0;
						// Force the PPQs to rebuild
						if (nullptr != pConfig) { lPPQSize = pConfig->GetPPQCount(); }

						// Rebuild Outputs
						for (long k = 0; k < lPPQSize; k++)
						{
							SVPPQObject* pPPQ = pConfig->GetPPQ(k);

							if (nullptr != pPPQ) { pPPQ->RebuildOutputList(); }
						}
					}

					// Force IO board to update if they still have one selected
					if (nullptr != pIOEntry)
					{
						if (nullptr != pOutputList)
						{
							pOutputList->ResetOutput(pIOEntry);
						}// end if
					}// end if
				}
				OnUpdate( nullptr, 0, nullptr );
			}
			else
			{
				dlg.m_IOName	= _T( "" );

				if(IDOK == dlg.DoModal())
				{
					SvimState::AddState( SV_STATE_MODIFIED );
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
				m_rCtrl.SetItem(rowIndex, cOutputColIndex, LVIF_IMAGE, nullptr, 0, 0, 0, 0);
				m_Items[rowIndex] = pIOEntry;
				m_rCtrl.SetItemText(rowIndex, cDescriptionColIndex, pDigOutput->GetName());

				if (pDigOutput->IsForced())
				{
					m_rCtrl.SetItemText(rowIndex, cForcedColIndex, pDigOutput->GetForcedValue() ? "1" : "0");
				}

				m_rCtrl.SetItemText(rowIndex, cInvertedColIndex, pDigOutput->IsInverted() ? "1" : "");
				return true;
			}
		}
	}
	return false;
}