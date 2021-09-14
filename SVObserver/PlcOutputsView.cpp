//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "PlcOutputsView.h"
#include "SVIOAdjustDialog.h"
#include "SVPPQObject.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#include "SVIOLibrary/PlcOutputObject.h"
#include "SVIOLibrary/SVOutputObjectList.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVOResource/ConstGlobalSvOr.h"
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

IMPLEMENT_DYNCREATE(PlcOutputsView, CListView)

BEGIN_MESSAGE_MAP(PlcOutputsView, CListView)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

PlcOutputsView::PlcOutputsView() :
	m_rCtrl(GetListCtrl())
{
}

BOOL PlcOutputsView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	dwStyle |= LVS_REPORT;
	dwStyle &= ~LVS_SORTASCENDING;
	dwStyle &= ~LVS_SORTDESCENDING;

	BOOL RetVal = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	m_rCtrl.SetExtendedStyle(m_rCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	m_ImageList.Create(SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 2, 1);
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_IOITEM_ICON));
	m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_NOIOITEM_ICON));

	m_StateImageList.Create(SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 1, 1);
	m_StateImageList.Add(AfxGetApp()->LoadIcon(IDI_OUTPUT_ICON));

	m_rCtrl.SetImageList(&m_StateImageList, LVSIL_STATE);
	m_rCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);

	m_rCtrl.InsertColumn(0, _T("Outputs"), LVCFMT_LEFT, -1, -1);
	m_rCtrl.InsertColumn(1, _T("PPQ"), LVCFMT_LEFT, -1, -1);
	m_rCtrl.InsertColumn(2, _T("Link"), LVCFMT_LEFT, -1, -1);

	m_rCtrl.SetColumnWidth(0, 125);
	m_rCtrl.SetColumnWidth(1, 125);
	m_rCtrl.SetColumnWidth(2, 500);
	SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount(m_maxOutputNumber);
	return RetVal;
}

void PlcOutputsView::OnUpdate(CView*, LPARAM , CObject* )
{
	if (::IsWindow(m_hWnd))
	{
		m_rCtrl.SetRedraw(false);

		// First clean up list view
		m_rCtrl.DeleteAllItems();

		m_Items.clear();

		SVConfigurationObject* pConfig(nullptr);
		SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);

		// Get the number of PPQs
		long lPPQSize = 0;
		if (nullptr != pConfig)
		{
			lPPQSize = pConfig->GetPPQCount();
		}
		else
		{
			SvStl::MessageManager e(SvStl::MsgType::Log);
			e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQCount, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17010_ErrorGettingPPQCount);
			assert(false);
		}

		// Check if any PPQs are here yet
		if (0 == lPPQSize) { return; }

		for (int i = 0; i < lPPQSize; ++i)
		{
			SVPPQObject* pPPQ = pConfig->GetPPQ(i);
			if (nullptr == pPPQ)
			{
				SvStl::MessageManager e(SvStl::MsgType::Log);
				e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQ, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17011_ErrorGettingPPQ);
				assert(false);
			}
			else
			{
				SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount(m_maxOutputNumber);
				for (unsigned long j=0; j < m_maxOutputNumber; ++j)
				{
					int indexRow = j + m_maxOutputNumber * i;
					std::string Item = SvUl::Format(_T("PLC Output %d"), j + 1);
					m_rCtrl.InsertItem(LVIF_IMAGE | LVIF_TEXT | LVIF_STATE,
									   indexRow, Item.c_str(),
									   INDEXTOSTATEIMAGEMASK(1),
									   LVIS_STATEIMAGEMASK,
									   1, 0);

					m_rCtrl.SetItemText(indexRow, 1, pPPQ->GetName());
					// Get list of available outputs
					SVIOEntryHostStructPtrVector IOEntriesVector;
					pPPQ->GetAllOutputs(IOEntriesVector);

					for(const auto& pIOEntry : IOEntriesVector)
					{
						if (pIOEntry->m_ObjectType != IO_PLC_OUTPUT) { continue; }

						PlcOutputObject* pPlcOutput = dynamic_cast<PlcOutputObject*> (SVObjectManagerClass::Instance().GetObject(pIOEntry->m_IOId));

						if (!pPlcOutput) { continue; }

						if (indexRow == pPlcOutput->GetChannel())
						{
							m_rCtrl.SetItem(indexRow, 0, LVIF_IMAGE, nullptr, 0, 0, 0, 0);

							m_Items[indexRow] = pIOEntry;

							m_rCtrl.SetItemText(indexRow, 2, pPlcOutput->GetName());
							break;
						}
					}
				}
			}
		}
		m_rCtrl.SetRedraw(true);
	}
}

void PlcOutputsView::OnLButtonDblClk(UINT, CPoint point)
{
	UINT flags;

	int item = m_rCtrl.HitTest(point, &flags);
	if (false == SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) && SvOi::isOkToEdit())
	{
		if (item >= 0 && item < m_rCtrl.GetItemCount() && (flags & LVHT_ONITEM))
		{
			SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount(m_maxOutputNumber);
			SVConfigurationObject* pConfig(nullptr);
			SVOutputObjectList* pOutputList(nullptr);

			SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
			if (nullptr != pConfig)
			{
				pOutputList = pConfig->GetOutputObjectList();
			}

			SVIOEntryHostStructPtr pIOEntry;
			const auto iter = m_Items.find(item);
			if (m_Items.end() != iter)
			{
				pIOEntry = iter->second;
			}
			SVOutputObjectPtr pOutput;
			PlcOutputObject* pPlcOutput{nullptr};
			if (nullptr != pOutputList)
			{
				if (nullptr != pIOEntry)
				{
					pOutput = pOutputList->GetOutput(pIOEntry->m_IOId);
					pPlcOutput = dynamic_cast<PlcOutputObject*> (pOutput.get());
				}
				else
				{
					pOutputList->DetachOutput(ObjectIdEnum::PlcOutputId + item);

					pOutput = std::make_shared<PlcOutputObject>();
					pOutput->updateObjectId(item);
					pPlcOutput = dynamic_cast<PlcOutputObject*> (pOutput.get());
					if (nullptr != pPlcOutput)
					{
						pPlcOutput->SetChannel(item);
						pPlcOutput->SetName(_T(""));
					}
				}
			}

			if (nullptr != pPlcOutput)
			{
				SVIOAdjustDialog dlg;
				dlg.IOName = _T("Result ") + m_rCtrl.GetItemText(item, 0);
				dlg.IOName += _T(", ") + m_rCtrl.GetItemText(item, 1);
				dlg.m_pIOEntry = pIOEntry;
				dlg.m_pPlcOutput = pPlcOutput;
				dlg.m_PpqIndex = item / m_maxOutputNumber;

				SVSVIMStateClass::SetResetState stateEditing {SV_STATE_EDITING};
				if (IDOK == dlg.DoModal())
				{
					SVSVIMStateClass::AddState(SV_STATE_MODIFIED);

					// Check if they picked a new output
					if (dlg.m_pIOEntry != pIOEntry)
					{
						if (nullptr != pIOEntry)
						{
							// Make sure that we first reset the old output
							if (nullptr != pOutputList)
							{
								pOutputList->ResetOutput(pIOEntry);
							}// end if

							pIOEntry->m_Enabled = FALSE;
							pIOEntry->m_IOId = SvDef::InvalidObjectId;
						}// end if

						if (nullptr == dlg.m_pIOEntry)
						{
							if (nullptr != pOutputList)
							{
								pOutputList->DetachOutput(pOutput->getObjectId());
							}
							pOutput = nullptr;
						}// end if
						else
						{
							dlg.m_pIOEntry->m_Enabled = TRUE;
							dlg.m_pPlcOutput->SetName(dlg.m_pIOEntry->getObject()->GetCompleteName().c_str());
							if (nullptr == pIOEntry && nullptr != pOutputList)
							{
								pOutputList->AttachOutput(pOutput);
							}// end if
						}// end else

						// Rebuild Outputs
						SVPPQObject* pPPQ = pConfig->GetPPQ(dlg.m_PpqIndex);
						if (nullptr != pPPQ)
						{
							pPPQ->RebuildOutputList();
						}

					}// end if

				}
				OnUpdate(nullptr, 0, nullptr);
			}
			else
			{
				SVIOAdjustDialog dlg;
				dlg.IOName = _T("");
				dlg.IOValue.Format("%d", 0);

				if (IDOK == dlg.DoModal())
				{
					SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
				}
				OnUpdate(nullptr, 0, nullptr);
			}
		}
	}
}
