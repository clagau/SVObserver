//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
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
#include "RemoteMonitorListController.h"
#include "RemoteMonitorList.h"
#include "MonitorListAddRemoveDlg.h"
#include "SVConfigurationObject.h"
#include "SVIODoc.h"
#include "SVIOTabbedView.h"
#include "SVSharedMemoryLibrary/SharedMemWriter.h"
#include "SVObserver.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVMainImageClass.h"
#include "RemoteMonitorListHelper.h"
#include "SVToolSet.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVFailStatusStreamManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "SVSharedMemoryLibrary\MonitorListCpy.h"
#pragma endregion Includes

extern SVObserverApp TheSVObserverApp;

LPCTSTR RemoteMonitorListController::s_DefaultMonitorListName =_T("MonitorList_");

PPQNameListNames RemoteMonitorListController::GetPPQMonitorLists(SVConfigurationObject* pConfig) const
{
	PPQNameListNames list;

	if( nullptr != pConfig)
	{
		long lPPQCount = pConfig->GetPPQCount();
		// get list of PPQs
		for (int i = 0;i < lPPQCount; i++)
		{
			SVPPQObject* pPPQ = pConfig->GetPPQ(i);
			if ( nullptr != pPPQ )
			{
				list.insert(std::make_pair(pPPQ->GetName(), NameDepthPairList()));
			}
		}
		// build collection of listnames and reject depth keyed by ppq name
		for (RemoteMonitorList::const_iterator it = m_list.begin();it != m_list.end(); ++it)
		{
			const RemoteMonitorNamedList& namedList = it->second;
			const SVString& ppqName = namedList.GetPPQName();
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
	SVSVIMStateClass::AddState(SV_STATE_EDITING);
	const PPQNameListNames& rPPQList = GetPPQMonitorLists(pConfig);
	MonitorListAddRemoveDlg dlg(m_list, rPPQList);
	INT_PTR rc = dlg.DoModal();
	if (IDOK == rc)
	{
		bRetVal = true;
		SetRemoteMonitorList(dlg.GetRemoteMonitorList()); // Update the list
	}
	SVSVIMStateClass::RemoveState(SV_STATE_EDITING);
	return bRetVal;
}

const RemoteMonitorList& RemoteMonitorListController::GetRemoteMonitorList() const
{
	return m_list;
}

void RemoteMonitorListController::SetRemoteMonitorList(const RemoteMonitorList& rList)
{
	m_list = rList;
	HideShowViewTab();
	SVFailStatusStreamManager::Instance().ManageFailStatus(m_list);
}

void RemoteMonitorListController::ReplaceOrAddMonitorList( const RemoteMonitorNamedList& rList )
{
	m_list[rList.GetName()] = rList;
	SVFailStatusStreamManager::Instance().ManageFailStatus(m_list);
}

void RemoteMonitorListController::HideShowViewTab()
{
	if (IsEmpty())
	{
		TheSVObserverApp.HideRemoteMonitorListTab();
	}
	else
	{
		TheSVObserverApp.ShowIOTab(SVRemoteMonitorListViewID);
		// Set Active IO Tabbed view to the RemoteMonitorListView Tab
		TheSVObserverApp.SetActiveIOTabView(SVRemoteMonitorListViewID);
	}
	TheSVObserverApp.OnUpdateAllIOViews();
}

// This seems to never be called...
void RemoteMonitorListController::ResetObject()
{
	ValidateInputs();
}

// must exist and must be published
bool RemoteMonitorListController::IsValidMonitoredObject(const SVObjectClass* pObject)
{
	bool bRetVal = false;
	if (nullptr != pObject)
	{
		UINT attr = pObject->ObjectAttributesSet();
		bRetVal = (attr & (SvOi::SV_PUBLISHABLE | SvOi::SV_PUBLISH_RESULT_IMAGE) ? true : false);
		//check to see if MainImageClass
		if (!bRetVal)
		{
			if ( SV_IS_KIND_OF(pObject->GetOwner(), const SVToolSetClass) && (SV_IS_KIND_OF(pObject, const SVMainImageClass)) )
			{
				bRetVal = true;
			}
		}
	}
	return bRetVal;
}

bool RemoteMonitorListController::ValidateMonitoredObject(MonitoredObjectList& rList)
{
	bool bModified = false;
	
	for (MonitoredObjectList::iterator it = rList.begin();it != rList.end();)
	{
		const SVGUID& guid = (*it).guid;
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(guid);
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
	for (RemoteMonitorList::iterator it = m_list.begin();it != m_list.end();)
	{
		RemoteMonitorNamedList& namedList = it->second;
		const SVString& ppqName = namedList.GetPPQName();
		const SVGUID& ppqGuid = namedList.GetPPQObjectID();
		// Check that the PPQ still exists
		const SVGUID& guid = SVObjectManagerClass::Instance().GetObjectIdFromCompleteName( ppqName.c_str() );
		if ( guid.empty() || guid != ppqGuid )
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
		TheSVObserverApp.HideRemoteMonitorListTab();
	}
}

////////////////////////////////////////////////////////////////////////////
// Create and Initialize the Monitor List Share in SharedMemory
////////////////////////////////////////////////////////////////////////////
HRESULT RemoteMonitorListController::InitMonitorListInSharedMemory(size_t size)
{
	const SvSml::SVSharedMemorySettings& rSettings =SvSml::SharedMemWriter::Instance().GetSettings();
	SvSml::SVMonitorListWriter& rWriter =SvSml::SharedMemWriter::Instance().GetMonitorListWriter();
	HRESULT hr = rWriter.Create(rSettings, size);
	if (S_OK != hr)
	{
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ErrorInitMonitorListInSharedMemory, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15044_InitMonitorListInSharedMemory );
	}
	return hr;
}


size_t RemoteMonitorListController::CalcSharedMemorySize(const RemoteMonitorList& rList)
{
	size_t size(0);
	for (RemoteMonitorList::const_iterator it = rList.begin();it != rList.end();++it)
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


void RemoteMonitorListController::WriteMonitorListToMLContainer(const std::string& name, const RemoteMonitorNamedList& remoteMonitorNamedlist)
{
	SvSml::MonitorListCpy*  pMonitorListCpy = new SvSml::MonitorListCpy;
	const SvSml::SVSharedMemorySettings& rSettings = SvSml::SharedMemWriter::Instance().GetSettings();
	SvSml::SVMonitorListWriter& rWriter = SvSml::SharedMemWriter::Instance().GetMonitorListWriter();
	SvSml::MonitorListCpyPointer  MLCpPtr = RemoteMonitorListHelper::CreateMLcopy(remoteMonitorNamedlist);
	SvSml::SharedMemWriter::Instance().Insert(MLCpPtr);
}

////////////////////////////////////////////////////////////////////////////
// Build the PPQ monitor list.
// It will separate and group the monitor list items by PPQ and Inspections
// If successful, it will return S_OK
////////////////////////////////////////////////////////////////////////////
HRESULT RemoteMonitorListController::BuildPPQMonitorList(PPQMonitorList& ppqMonitorList) const
{
	// Setup failStatus Streaming
	SVFailStatusStreamManager::Instance().AttachPPQObservers(m_list);

	size_t requiredSize = CalcSharedMemorySize(m_list);

	///Clear monitorLists in shared Memory
	HRESULT hr = InitMonitorListInSharedMemory(requiredSize);
	if (S_OK == hr)
	{
		// combine the lists by PPQName
		SvSml::SharedMemWriter::Instance().ClearMonitorListCpyVector();

		for (RemoteMonitorList::const_iterator it = m_list.begin();it != m_list.end();++it)
		{
			// Only Activated Lists are sent to the Inspections and Monitor Lists can only be activated when Offline

			///Insert Monitorlistcpy to singelton 
			
			bool bActive = it->second.IsActive();
			if (bActive)
			{
				const MonitoredObjectList& values = it->second.GetProductValuesList();
				const MonitoredObjectList& images = it->second.GetProductImagesList();
				const MonitoredObjectList& failStatus = it->second.GetFailStatusList();
				const MonitoredObjectList& rejectCond = it->second.GetRejectConditionList();
				const SVString& ppqName = it->second.GetPPQName();

				// write the monitorlist to shared memory...
				WriteMonitorListToMLContainer(it->first.c_str(), it->second);

				SVMonitorItemList remoteValueList;
				SVMonitorItemList remoteImageList;
				SVMonitorItemList remoteRejectCondList;

				typedef std::insert_iterator<SVMonitorItemList> Insertor;
				std::transform(values.begin(), values.end(), Insertor(remoteValueList, remoteValueList.begin()), RemoteMonitorListHelper::GetNameFromMonitoredObject);
				std::transform(images.begin(), images.end(), Insertor(remoteImageList, remoteImageList.begin()), RemoteMonitorListHelper::GetNameFromMonitoredObject);
				std::transform(failStatus.begin(), failStatus.end(), Insertor(remoteValueList, remoteValueList.begin()), RemoteMonitorListHelper::GetNameFromMonitoredObject);
				for (MonitoredObjectList::const_iterator rejectCondIt = rejectCond.begin();rejectCondIt != rejectCond.end();++rejectCondIt)
				{
					const SVString& name = RemoteMonitorListHelper::GetNameFromMonitoredObject(*rejectCondIt);
					if (!name.empty())
					{
						remoteValueList.insert(name);
						remoteRejectCondList.insert(name);
					}
				}
				// If there is something to monitor, add it to the PPQ Monitor List
				if (!remoteValueList.empty() || !remoteImageList.empty() || !remoteRejectCondList.empty())
				{
					RejectDepthAndMonitorList list;
					list.rejectDepth = it->second.GetRejectDepthQueue();
					list.monitorList = SVMonitorList(remoteValueList, remoteImageList, remoteValueList, remoteImageList, remoteRejectCondList);
					ppqMonitorList[ppqName] = std::make_pair(bActive, list);
				}
			}
		}
	}
	return hr;
}

HRESULT  RemoteMonitorListController::ActivateRemoteMonitorList(RemoteMonitorList& rRemoteMonitorList , const SVString& listName, bool bActivate) 
{
	HRESULT hr = S_OK;
	RemoteMonitorList::iterator it = rRemoteMonitorList.find(listName);
	if (it != rRemoteMonitorList.end())
	{
		it->second.Activate(bActivate);
		it->second.SetProductFilter(SvSml::LastInspectedFilter);
		if(true == bActivate)
		{	
			SVGUID PPQ_GUID = it->second.GetPPQObjectID();
			for(RemoteMonitorList::iterator it_S = rRemoteMonitorList.begin(); it_S != rRemoteMonitorList.end();it_S++ )
			{
				if(it_S->second.GetPPQObjectID() == PPQ_GUID && it_S != it)
				{
					it_S->second.Activate(false);
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

HRESULT RemoteMonitorListController::ActivateRemoteMonitorList(const SVString& listName, bool bActivate)
{
	return ActivateRemoteMonitorList(m_list, listName,bActivate);
}	


 void RemoteMonitorListController::GetActiveRemoteMonitorList(RemoteMonitorList& rActiveList) const
{
	// Get all active lists
	for (RemoteMonitorList::const_iterator it = m_list.begin();it != m_list.end();++it)
	{
		if (it->second.IsActive())
		{
			rActiveList[it->first] = it->second;
		}
	}
}

HRESULT RemoteMonitorListController::SetRemoteMonitorListProductFilter(const SVString& listName, SvSml::SVProductFilterEnum filter)
{
	HRESULT hr = S_OK;
	RemoteMonitorList::iterator it = m_list.find(listName);
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

HRESULT RemoteMonitorListController::GetRemoteMonitorListProductFilter(const SVString& listName, SvSml::SVProductFilterEnum& rFilter) const
{
	HRESULT hr = S_OK;
	RemoteMonitorList::const_iterator it = m_list.find(listName);
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

