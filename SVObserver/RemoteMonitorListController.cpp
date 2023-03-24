//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteMonitorListController
//* .File Name       : $Workfile:   RemoteMonitorListController.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.16  $
//* .Check In Date   : $Date:   18 Feb 2015 11:04:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <comdef.h>
#include "EditLock.h"
#include "RemoteMonitorListController.h"
#include "MonitorListAddRemoveDlg.h"
#include "RemoteMonitorList.h"
#include "RemoteMonitorListHelper.h"
#include "SVConfigurationObject.h"
#include "SVIODoc.h"
#include "SVIOTabbedView.h"
#include "SVObserver.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVSharedMemoryLibrary/MonitorListCpy.h"
#include "SVSharedMemoryLibrary/SharedMemWriter.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#pragma endregion Includes

LPCTSTR RemoteMonitorListController::s_DefaultMonitorListName = _T("MonitorList_");

PPQNameListNames RemoteMonitorListController::GetPPQMonitorLists(SVConfigurationObject* pConfig) const
{
	PPQNameListNames list;

	if (nullptr != pConfig)
	{
		long lPPQCount = pConfig->GetPPQCount();
		// get list of PPQs
		for (int i = 0; i < lPPQCount; i++)
		{
			SVPPQObject* pPPQ = pConfig->GetPPQ(i);
			if (nullptr != pPPQ)
			{
				list.insert(std::make_pair(pPPQ->GetName(), NameDepthPairList()));
			}
		}
		// build collection of listnames and reject depth keyed by ppq name
		for (RemoteMonitorListMap::const_iterator it = m_list.begin(); it != m_list.end(); ++it)
		{
			const RemoteMonitorNamedList& namedList = it->second;
			const std::string& ppqName = namedList.GetPPQName();
			NameDepthPair nameDepthPair(namedList.GetName(), namedList.GetRejectDepthQueue());
			NameDepthPairList& nameDepthList = list[ppqName];
			if (std::find(nameDepthList.begin(), nameDepthList.end(), nameDepthPair) == nameDepthList.end())
			{
				nameDepthList.push_back(nameDepthPair);
			}
		}
	}
	return list;
}

void RemoteMonitorListController::Clear()
{
	m_list.clear();
}

// This function is used to tell if the view should be hidden
bool RemoteMonitorListController::IsEmpty() const
{
	return m_list.empty();
}

bool RemoteMonitorListController::Setup(SVConfigurationObject* pConfig)
{
	bool bRetVal = false;
	SVSVIMStateClass::SetResetState srs(SV_STATE_EDITING, EditLock::acquire, EditLock::release);
	if (false == srs.conditionOk())
	{
		return false;
	}
	const PPQNameListNames& rPPQList = GetPPQMonitorLists(pConfig);
	MonitorListAddRemoveDlg dlg(m_list, rPPQList);
	INT_PTR rc = dlg.DoModal();
	if (IDOK == rc)
	{
		SvStl::MessageContainerVector errorMessages;
		bRetVal = SetRemoteMonitorList(dlg.GetRemoteMonitorList(), &errorMessages); // Update the list
		if (0 < errorMessages.size())
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Display);
			Msg.setMessage(errorMessages[0].getMessage());
		}
	}
	return bRetVal;
}

const RemoteMonitorListMap& RemoteMonitorListController::GetRemoteMonitorList() const
{
	return m_list;
}

bool RemoteMonitorListController::SetRemoteMonitorList(const RemoteMonitorListMap& rList, SvStl::MessageContainerVector *pErrorMessages/*=nullptr */)
{
	bool result = true;
	RemoteMonitorListMap oldActiveList, newActiveList;
	GetActiveRemoteMonitorList(oldActiveList);
	m_list = rList;
	GetActiveRemoteMonitorList(newActiveList);
	for (auto oldActive : oldActiveList)
	{
		auto ppqId = oldActive.second.GetPPQObjectID();
		SVPPQObject* pPPQ = dynamic_cast<SVPPQObject*>(SVObjectManagerClass::Instance().GetObject(ppqId));
		auto newActiveIter = find_if(newActiveList.begin(), newActiveList.end(), [ppqId](auto value) { return value.second.GetPPQObjectID() == ppqId; });
		if (newActiveIter != newActiveList.end())
		{
			if (oldActive.second.GetRejectDepthQueue() != newActiveIter->second.GetRejectDepthQueue() || nullptr != pPPQ)
			{
				result = pPPQ->setRejectDepth(newActiveIter->second.GetRejectDepthQueue(), pErrorMessages) && result;
			}
			newActiveList.erase(newActiveIter);
		}
		else
		{
			if (nullptr != pPPQ)
			{
				pPPQ->setRejectDepth(0);
			}
		}
	}
	for (auto newActive : newActiveList)
	{
		SVPPQObject* pPPQ = dynamic_cast<SVPPQObject*>(SVObjectManagerClass::Instance().GetObject(newActive.second.GetPPQObjectID()));
		if (nullptr != pPPQ)
		{
			result = pPPQ->setRejectDepth(newActive.second.GetRejectDepthQueue(), pErrorMessages) && result;
		}
	}

	HideShowViewTab();
	return result;
}

void RemoteMonitorListController::ReplaceOrAddMonitorList(const RemoteMonitorNamedList& rList)
{
	m_list[rList.GetName()] = rList;
}

void RemoteMonitorListController::HideShowViewTab()
{
	if (IsEmpty())
	{
		HideIOTabIfPossible(SVRemoteMonitorListViewID);
	}
	else
	{
		ShowIOTabIfPossible(SVRemoteMonitorListViewID);
	}

	TheSVObserverApp().UpdateAllIOViews();
}

void RemoteMonitorListController::ResetObject()
{
	ValidateInputs();
}


// must exist and from correct type
bool RemoteMonitorListController::IsValidMonitoredObject(const SVObjectClass* pObject)
{
	bool bRetVal = false;
	if (nullptr != pObject)
	{
		if (SV_IS_KIND_OF(pObject, const SvIe::SVImageClass))
		{
			bRetVal = true;
		}
		else if (SV_IS_KIND_OF(pObject, const SvOi::IValueObject))
		{
			bRetVal = true;
		}
	}
	return bRetVal;
}

bool RemoteMonitorListController::ValidateMonitoredObject(MonitoredObjectList& rList)
{
	bool bModified = false;

	for (MonitoredObjectList::iterator it = rList.begin(); it != rList.end();)
	{
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(it->m_objectId);
		if (!IsValidMonitoredObject(pObject))
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_OTHER)
			::OutputDebugString(_T("MonitorObject reference deleted \n"));
#endif
			it = rList.erase(it);
			bModified = true;
		}
		else
		{
			++it;
		}
	}
	return bModified;
}

void RemoteMonitorListController::ValidateInputs()
{
	for (RemoteMonitorListMap::iterator it = m_list.begin(); it != m_list.end();)
	{
		RemoteMonitorNamedList& namedList = it->second;
		const std::string& ppqName = namedList.GetPPQName();
		uint32_t ppqId = namedList.GetPPQObjectID();
		// Check that the PPQ still exists
		uint32_t id = SVObjectManagerClass::Instance().GetObjectIdFromCompleteName(ppqName.c_str());
		if (SvDef::InvalidObjectId == id || id != ppqId)
		{
			// remove this list
			it = m_list.erase(it);
		}
		else
		{
			++it;
			MonitoredObjectList productValues = namedList.GetProductValuesList();
			bool bModified = ValidateMonitoredObject(productValues);
			if (bModified)
			{
				namedList.SetProductValuesList(productValues);
			}
			MonitoredObjectList productImages = namedList.GetProductImagesList();
			bModified = ValidateMonitoredObject(productImages);
			if (bModified)
			{
				namedList.SetProductImagesList(productImages);
			}
			MonitoredObjectList rejectConditionList = namedList.GetRejectConditionList();
			bModified = ValidateMonitoredObject(rejectConditionList);
			if (bModified)
			{
				namedList.SetRejectConditionList(rejectConditionList);
			}
			MonitoredObjectList failStatusList = namedList.GetFailStatusList();
			bModified = ValidateMonitoredObject(failStatusList);
			if (bModified)
			{
				namedList.SetFailStatusList(failStatusList);
			}
		}
	}
	if (IsEmpty())
	{
		HideIOTabIfPossible(SVRemoteMonitorListViewID);
	}
}


size_t RemoteMonitorListController::CalcSizeForMonitorList(const RemoteMonitorListMap& rList)
{
	size_t size(0);
	for (RemoteMonitorListMap::const_iterator it = rList.begin(); it != rList.end(); ++it)
	{
		const RemoteMonitorNamedList & remoteMonitorNamedList = it->second;

		bool isActive = remoteMonitorNamedList.IsActive();
		if (isActive)
		{
			SvSml::MonitorListCpyPointer  MLCpPtr = RemoteMonitorListHelper::CreateMLcopy(remoteMonitorNamedList);
			size += MLCpPtr->CalcMemorySize();
		}
	}
	return size;
}


void RemoteMonitorListController::WriteMonitorListToMLContainer(const std::string&, const RemoteMonitorNamedList& remoteMonitorNamedlist)
{
	SvSml::MonitorListCpyPointer  MLCpPtr = RemoteMonitorListHelper::CreateMLcopy(remoteMonitorNamedlist);
	SvSml::SharedMemWriter::Instance().Insert(MLCpPtr);
}

////////////////////////////////////////////////////////////////////////////
// Build the PPQ monitor list. and  Insert the monitorlist copies SvSml::SharedMemWriter Singelton 
// It will separate and group the monitor list items by PPQ and Inspections
// If successful, it will return S_OK
////////////////////////////////////////////////////////////////////////////
HRESULT RemoteMonitorListController::BuildPPQMonitorList(PPQMonitorList& ppqMonitorList) const
{
	// combine the lists by PPQName
	SvSml::SharedMemWriter::Instance().ClearMonitorListCpyVector();

	for (RemoteMonitorListMap::const_iterator it = m_list.begin(); it != m_list.end(); ++it)
	{
		// Only Activated Lists are sent to the Inspections and Monitor Lists can only be activated when Offline

		bool bActive = it->second.IsActive();
		if (bActive)
		{
			const MonitoredObjectList& values = it->second.GetProductValuesList();
			const MonitoredObjectList& images = it->second.GetProductImagesList();
			const MonitoredObjectList& failStatus = it->second.GetFailStatusList();
			const MonitoredObjectList& rejectCond = it->second.GetRejectConditionList();
			const std::string& ppqName = it->second.GetPPQName();

			// Insert the monitorlist copies SvSml::SharedMemWriter Singelton 
			WriteMonitorListToMLContainer(it->first.c_str(), it->second);

			typedef std::set<std::string> SVMonitorItemList;
			SVMonitorItemList remoteValueList;
			SVMonitorItemList remoteImageList;
			SVMonitorItemList remoteRejectCondList;

			typedef std::insert_iterator<SVMonitorItemList> Insertor;
			std::string (*pGetName)(const MonitoredObject&) = static_cast<std::string (*)(const MonitoredObject&)> (RemoteMonitorListHelper::GetNameFromMonitoredObject);
			std::transform(values.begin(), values.end(), Insertor(remoteValueList, remoteValueList.begin()), pGetName);
			std::transform(images.begin(), images.end(), Insertor(remoteImageList, remoteImageList.begin()), pGetName);
			std::transform(failStatus.begin(), failStatus.end(), Insertor(remoteValueList, remoteValueList.begin()), pGetName);
			for (MonitoredObjectList::const_iterator rejectCondIt = rejectCond.begin(); rejectCondIt != rejectCond.end(); ++rejectCondIt)
			{
				const std::string& name = RemoteMonitorListHelper::GetNameFromMonitoredObject(*rejectCondIt);
				if (!name.empty())
				{
					remoteValueList.insert(name);
					remoteRejectCondList.insert(name);
				}
			}
			// If there is something to monitor, add it to the PPQ Monitor List
			if (!remoteValueList.empty() || !remoteImageList.empty() || !remoteRejectCondList.empty())
			{
				ppqMonitorList[ppqName] = MonitorListAttributeStruct( bActive, it->second.GetRejectDepthQueue() );
			}
		}
	}
	return S_OK;
}

HRESULT  RemoteMonitorListController::ActivateRemoteMonitorList(RemoteMonitorListMap& rRemoteMonitorList, const std::string& listName, bool bActivate)
{
	HRESULT hr = S_OK;
	RemoteMonitorListMap::iterator it = rRemoteMonitorList.find(listName);
	if (it != rRemoteMonitorList.end())
	{
		it->second.Activate(bActivate);
		it->second.SetProductFilter(SvSml::LastInspectedFilter);
		if (true == bActivate)
		{
			long rejectDepth = it->second.GetRejectDepthQueue();
			uint32_t PPQ_ID = it->second.GetPPQObjectID();
			for (RemoteMonitorListMap::iterator it_S = rRemoteMonitorList.begin(); it_S != rRemoteMonitorList.end(); ++it_S)
			{
				if (it_S->second.GetPPQObjectID() == PPQ_ID && it_S != it)
				{
					if (it_S->second.IsActive())
					{
						if (it_S->second.GetRejectDepthQueue() != rejectDepth)
						{
							SVPPQObject* pPPQ = dynamic_cast<SVPPQObject*>(SVObjectManagerClass::Instance().GetObject(PPQ_ID));
							if (nullptr != pPPQ)
							{
								pPPQ->setRejectDepth(rejectDepth);
							}
						}
						it_S->second.Activate(false);
					}
				}
			}
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}
	return hr;
}

HRESULT RemoteMonitorListController::ActivateRemoteMonitorList(const std::string& listName, bool bActivate)
{
	return ActivateRemoteMonitorList(m_list, listName, bActivate);
}


void RemoteMonitorListController::GetActiveRemoteMonitorList(RemoteMonitorListMap& rActiveList) const
{
	// Get all active lists
	for (RemoteMonitorListMap::const_iterator it = m_list.begin(); it != m_list.end(); ++it)
	{
		if (it->second.IsActive())
		{
			rActiveList[it->first] = it->second;
		}
	}
}

int RemoteMonitorListController::GetActiveMonitorListCount() const
{
	return static_cast<int>(std::count_if(m_list.cbegin(), m_list.cend(), [](const auto& rEntry) { return rEntry.second.IsActive(); }));
}

HRESULT RemoteMonitorListController::SetRemoteMonitorListProductFilter(const std::string& listName, SvSml::SVProductFilterEnum filter)
{
	HRESULT hr = S_OK;
	RemoteMonitorListMap::iterator it = m_list.find(listName);
	if (it != m_list.end())
	{
		// must be active - else it's an error
		if (it->second.IsActive())
		{
			it->second.SetProductFilter(filter);
			// we will pick up the change when we go online
		}
		else
		{
			hr = E_ACCESSDENIED;
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}
	return hr;
}

HRESULT RemoteMonitorListController::GetRemoteMonitorListProductFilter(const std::string& listName, SvSml::SVProductFilterEnum& rFilter) const
{
	HRESULT hr = S_OK;
	RemoteMonitorListMap::const_iterator it = m_list.find(listName);
	if (it != m_list.end())
	{
		rFilter = it->second.GetProductFilter();
	}
	else
	{
		hr = E_INVALIDARG;
	}
	return hr;
}

