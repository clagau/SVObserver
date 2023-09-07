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
#include "SVSharedMemoryLibrary/EditLock.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/SvimState.h"
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

constexpr const char* cPlcOutputName = _T("PLC Output {:d}");
constexpr int cOutputColIndex = 0;
constexpr int cPpqColIndex = 1;
constexpr int cLinkColIndex = 2;
constexpr int cStandradColWidth = 125;
constexpr int cLinkColWidth = 250;

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

	m_rCtrl.InsertColumn(cOutputColIndex, _T("Outputs"), LVCFMT_LEFT, -1, -1);
	m_rCtrl.SetColumnWidth(cOutputColIndex, cStandradColWidth);
	m_rCtrl.InsertColumn(cPpqColIndex, _T("PPQ"), LVCFMT_LEFT, -1, -1);
	m_rCtrl.SetColumnWidth(cPpqColIndex, cStandradColWidth);

	for (long int i = 0; i < SvDef::cObjectIndexMaxNr; ++i)
	{
		std::string linkName = std::format(_T("Link Object {:d}"), i);
		m_rCtrl.InsertColumn(cLinkColIndex + i, linkName.c_str(), LVCFMT_LEFT, -1, -1);
		m_rCtrl.SetColumnWidth(cLinkColIndex + i, cLinkColWidth);
	}
	m_maxOutputNumber = SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount();
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
			e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQCount, SvStl::SourceFileParams(StdMessageParams));
			Log_Assert(false);
		}

		// Check if any PPQs are here yet
		if (0 == lPPQSize) { return; }
		m_maxOutputNumber = SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount();

		m_maxObjectIDCount = 0L;
		for (int i = 0; i < lPPQSize; ++i)
		{
			SVPPQObject* pPPQ = pConfig->GetPPQ(i);
			if (nullptr == pPPQ)
			{
				SvStl::MessageManager e(SvStl::MsgType::Log);
				e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingPPQ, SvStl::SourceFileParams(StdMessageParams));
				Log_Assert(false);
			}
			else
			{
				long ppqIDNrZerobased {pPPQ->GetPpqIDNr() - 1};
				long objectIDCount {pPPQ->GetObjectIDCount()};
				m_maxObjectIDCount = std::max(m_maxObjectIDCount, objectIDCount);
				for (long j = 0; j < m_maxOutputNumber; ++j)
				{
					int indexRow = (m_maxOutputNumber * i) + j;
					int channelNr = (m_maxOutputNumber * ppqIDNrZerobased) + j;
					std::string Item = std::format(cPlcOutputName, j + 1);
					m_rCtrl.InsertItem(LVIF_IMAGE | LVIF_TEXT | LVIF_STATE, indexRow, Item.c_str(), INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK, 1, 0);

					m_rCtrl.SetItemText(indexRow, cPpqColIndex, pPPQ->GetName());
					for (const auto& pIOEntry : pPPQ->getUsedOutputs())
					{
						if (pIOEntry->m_ObjectType != SVIOObjectType::IO_PLC_OUTPUT) { continue; }

						PlcOutputObject* pPlcOutput = dynamic_cast<PlcOutputObject*> (SVObjectManagerClass::Instance().GetObject(pIOEntry->m_IOId));

						if (nullptr == pPlcOutput) { continue; }

						if (channelNr == pPlcOutput->GetChannel())
						{
							m_rCtrl.SetItem(indexRow, cOutputColIndex, LVIF_IMAGE, nullptr, 0, 0, 0, 0);

							m_Items[indexRow] = pIOEntry;
							for (int k = 0; k < objectIDCount; ++k)
							{
								SVObjectClass* pLinkedObj = pIOEntry->getObject(k);
								if (nullptr != pLinkedObj)
								{
									m_rCtrl.SetItemText(indexRow, cLinkColIndex + k, pLinkedObj->GetCompleteName().c_str());
								}
							}
							break;
						}
					}
				}
			}
		}

		for (long int i = 0; i < SvDef::cObjectIndexMaxNr; ++i)
		{
			m_rCtrl.SetColumnWidth(cLinkColIndex + i, (i < m_maxObjectIDCount) ? cLinkColWidth : 0);
		}
		m_rCtrl.SetRedraw(true);
	}
}

void PlcOutputsView::OnLButtonDblClk(UINT, CPoint point)
{ //@TODO [Arvid][10.30][15.3.2023] this function is too long

	if (false == SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST) && SvOi::isOkToEdit())
	{
		std::vector<std::string> usedOutputList;
		for (const auto& rEntry : m_Items)
		{
			if (rEntry.second->m_ObjectType != SVIOObjectType::IO_PLC_OUTPUT)
			{ 
				continue;
			}

			SVObjectClass* pOutput = SVObjectManagerClass::Instance().GetObject(rEntry.second->m_IOId);
			if (nullptr !=pOutput)
			{ 
				usedOutputList.emplace_back(std::string(pOutput->GetName()));
			}
		}
		UINT flags {0};
		int item = m_rCtrl.HitTest(point, &flags);
		if (item >= 0 && item < m_rCtrl.GetItemCount() && (flags & LVHT_ONITEM))
		{
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
			CString ppqName = m_rCtrl.GetItemText(item, cPpqColIndex);
			int ppqIDNr = atoi(ppqName.Mid(static_cast<int> (strlen(SvDef::cPpqFixedName))).GetString());

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
					int outputNr = item % m_maxOutputNumber;
					int channelNr = ((ppqIDNr - 1) * m_maxOutputNumber) + outputNr;
					pOutputList->DetachOutput(ObjectIdEnum::PlcOutputId + channelNr);

					pOutput = std::make_shared<PlcOutputObject>();
					pOutput->updateObjectId(channelNr);
					pPlcOutput = dynamic_cast<PlcOutputObject*> (pOutput.get());
					if (nullptr != pPlcOutput)
					{
						pPlcOutput->SetChannel(channelNr);
						pPlcOutput->SetName(_T(""));
					}
				}
			}

			if (nullptr != pPlcOutput)
			{
				SVIOAdjustDialog dlg {usedOutputList};
				std::array<SVObjectClass*, SvDef::cObjectIndexMaxNr> pObjectLinkList {nullptr, nullptr, nullptr, nullptr};
				for (long i = 0; i < m_maxObjectIDCount; ++i)
				{
					pObjectLinkList[i] = nullptr != pIOEntry ? pIOEntry->getObject(i) : nullptr;
				}
				dlg.m_pObjectLinkList = pObjectLinkList;
				dlg.m_pPlcOutput = pPlcOutput;
				dlg.m_ppqIDNr = ppqIDNr;
				dlg.m_ioObjectType = SVIOObjectType::IO_PLC_OUTPUT;

				SvSml::TemporaryState_Editing tse;
				if (false == tse.stateWasEntered())
				{
					return;
				}
				if (IDOK == dlg.DoModal())
				{
					SvimState::AddState(SV_STATE_MODIFIED);

					for (long i = 0; i < m_maxObjectIDCount; ++i)
					{
						// Check if they picked a new output
						if (dlg.m_pObjectLinkList[i] != pObjectLinkList[i])
						{
							if (nullptr == pIOEntry)
							{
								pIOEntry = std::make_shared<SVIOEntryHostStruct>();
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

							if (nullptr == dlg.m_pObjectLinkList[i])
							{
								pOutput->SetValueObjectID(SvDef::InvalidObjectId, i);
							}
							else
							{
								pIOEntry->m_Enabled = true;
								pIOEntry->setValueObject(dlg.m_pObjectLinkList[i], i);
								if (nullptr != pOutput && nullptr != pOutputList)
								{
									pIOEntry->m_IOId = pOutput->getObjectId();
									pOutput->SetName(dlg.m_pObjectLinkList[i]->GetCompleteName().c_str());
									pOutput->SetValueObjectID(dlg.m_pObjectLinkList[i]->getObjectId(), i);
									pOutputList->AttachOutput(pOutput);
								}
							}
						}
					}
					bool outputUsed {false};
					for (long i = 0; i < m_maxObjectIDCount; ++i)
					{
						outputUsed |= pOutput->GetValueObjectID(i) != 0;
					}
					if (false == outputUsed)
					{
						if (nullptr != pOutputList)
						{
							pOutputList->DetachOutput(pOutput->getObjectId());
						}
						pOutput.reset();
						if (nullptr != pIOEntry)
						{
							pIOEntry->clear();
							pIOEntry.reset();
						}
					}

					// Rebuild Outputs
					for (long i = 0; i < pConfig->GetPPQCount(); ++i)
					{
						SVPPQObject* pPPQ = pConfig->GetPPQ(i);
						if (nullptr != pPPQ && pPPQ->GetPpqIDNr() == dlg.m_ppqIDNr)
						{
							pPPQ->RebuildOutputList();
							break;
						}
					}

				}
				OnUpdate(nullptr, 0, nullptr);
			}
			else
			{
				SVIOAdjustDialog dlg {usedOutputList};
				dlg.m_IOName = _T("");

				if (IDOK == dlg.DoModal())
				{
					SvimState::AddState(SV_STATE_MODIFIED);
				}
				OnUpdate(nullptr, 0, nullptr);
			}
		}
	}
}
