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
#include <algorithm>
#include <comdef.h>
#include "RemoteMonitorListController.h"
#include "RemoteMonitorList.h"
#include "MonitorListAddRemoveDlg.h"
#include "SVConfigurationObject.h"
#include "SVIODoc.h"
#include "SVIOTabbedView.h"
#include "SVSharedMemorySingleton.h"
#include "SVObserver.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVMainImageClass.h"
#include "RemoteMonitorListHelper.h"
#include "SVToolSet.h"
#include "SVSVIMStateClass.h"
#include "SVFailStatusStreamManager.h"

#pragma endregion Includes

#define SEJ_ErrorBase 15000
#define Err_15020 (SEJ_ErrorBase+20)

extern SVObserverApp TheSVObserverApp;

PPQNameListNames RemoteMonitorListController::GetPPQMonitorLists(SVConfigurationObject* pConfig) const
{
	PPQNameListNames list;

	long lPPQCount = 0;
	if( nullptr != pConfig)
	{
		pConfig->GetPPQCount(lPPQCount);
		// get list of PPQs
		for (int i = 0;i < lPPQCount; i++)
		{
			SVPPQObject* pPPQ( nullptr );
			pConfig->GetPPQ(i, &pPPQ);
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
		bRetVal = (attr & (SV_PUBLISHABLE | SV_PUBLISH_RESULT_IMAGE) ? true : false);
		//check to see if MainImageClass or SVRGBMainImageClass
		if (!bRetVal)
		{
			if ( SV_IS_KIND_OF(pObject->GetOwner(), const SVToolSetClass) && (SV_IS_KIND_OF(pObject, const SVMainImageClass) || SV_IS_KIND_OF(pObject, const SVRGBMainImageClass)) )
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
			::OutputDebugString(_T("MonitorObject reference deleted \n"));
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
		const SVGUID& guid = SVObjectManagerClass::Instance().GetObjectIdFromCompleteName(ppqName);
		if (guid.empty() || guid != ppqGuid)
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

static void WriteMonitorListToSharedMemory(const std::string& name, const std::string& ppqName, int rejectDepth, const MonitoredObjectList& values, const MonitoredObjectList& images, const MonitoredObjectList& failStatus, const MonitoredObjectList& rejectCond)
{
	const SVSharedMemorySettings& rSettings = SVSharedMemorySingleton::Instance().GetSettings();
	SeidenaderVision::SVMonitorListWriter& rWriter = SVSharedMemorySingleton::Instance().GetMonitorListWriter();
	rWriter.Create(rSettings);
	if (rWriter.IsCreated())
	{
		typedef std::vector<std::string> NameList;
		typedef std::insert_iterator<NameList> Insertor;
		NameList productItems;
		NameList failStatusItems;
		NameList rejectCondItems;

		std::transform(values.begin(), values.end(), Insertor(productItems, productItems.begin()), [](const MonitoredObject& rObj)->std::string { return RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj).c_str(); });
		std::transform(images.begin(), images.end(), Insertor(productItems, productItems.end()), [](const MonitoredObject& rObj)->std::string { return RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj).c_str(); });
		std::transform(rejectCond.begin(), rejectCond.end(), Insertor(rejectCondItems, rejectCondItems.end()), [](const MonitoredObject& rObj)->std::string { return RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj).c_str(); });
		std::transform(failStatus.begin(), failStatus.end(), Insertor(failStatusItems, failStatusItems.end()), [](const MonitoredObject& rObj)->std::string { return RemoteMonitorListHelper::GetNameFromMonitoredObject(rObj).c_str(); });

		rWriter.AddList(name, ppqName, rejectDepth);
		rWriter.FillList(name, SeidenaderVision::productItems, productItems);
		rWriter.FillList(name, SeidenaderVision::rejectCondition, rejectCondItems);
		rWriter.FillList(name, SeidenaderVision::failStatus, failStatusItems);
	}
	else
	{
		// log exception
		SVException l_Exception;
		CString l_Message;
		l_Message.Format(_T("WriteMonitorListToSharedMemory - No Shared memory"));
		SETEXCEPTION5(l_Exception, SVMSG_SVO_44_SHARED_MEMORY, Err_15020, l_Message);
		l_Exception.LogException(l_Message);
	}
}

void RemoteMonitorListController::BuildPPQMonitorList(PPQMonitorList& ppqMonitorList) const
{
	// Setup failStatus Streaming
	SVFailStatusStreamManager::Instance().AttachPPQObservers(m_list);

	// combine the lists by PPQName
	for (RemoteMonitorList::const_iterator it = m_list.begin();it != m_list.end();++it)
	{
		// Only Activated Lists are sent to the Inspections and Monitor Lists can only be activated when Offline
		bool bActive = it->second.IsActive();
		if (bActive)
		{
			const MonitoredObjectList& values = it->second.GetProductValuesList();
			const MonitoredObjectList& images = it->second.GetProductImagesList();
			const MonitoredObjectList& failStatus = it->second.GetFailStatusList();
			const MonitoredObjectList& rejectCond = it->second.GetRejectConditionList();
			const SVString& ppqName = it->second.GetPPQName();

			// write the monitorlist to shared memory...
			WriteMonitorListToSharedMemory(it->first.c_str(), ppqName.c_str(), it->second.GetRejectDepthQueue(), values, images, failStatus, rejectCond);

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

HRESULT RemoteMonitorListController::ActivateRemoteMonitorList(const SVString& listName, bool bActivate)
{
	HRESULT hr = S_OK;

	if (bActivate)
	{
		//unactivate list prior to activating
		for (RemoteMonitorList::iterator it = m_list.begin();it != m_list.end();++it)
		{
			it->second.Activate(false);
			it->second.SetProductFilter(LastInspectedFilter);
		}
	}

	RemoteMonitorList::iterator it = m_list.find(listName);
	if (it != m_list.end())
	{
		it->second.Activate(bActivate);
		it->second.SetProductFilter(LastInspectedFilter);
	}
	else
	{
		 hr = E_INVALIDARG;
	}
	return hr;
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

HRESULT RemoteMonitorListController::SetRemoteMonitorListProductFilter(const SVString& listName, SVProductFilterEnum filter)
{
	HRESULT hr = S_OK;
	RemoteMonitorList::iterator it = m_list.find(listName);
	if (it != m_list.end())
	{
		// must be active - else it's an error
		if (it->second.IsActive())
		{
			it->second.SetProductFilter(filter);
			// if we are online, update the filter for the monitorList in shared memory
			// otherwise we will pick up the change when we go online
			if (SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
			{
				SVSharedMemorySingleton::SetProductFilter(listName, filter);
			}
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

HRESULT RemoteMonitorListController::GetRemoteMonitorListProductFilter(const SVString& listName, SVProductFilterEnum& rFilter) const
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\RemoteMonitorListController.cpp_v  $
 * 
 *    Rev 1.16   18 Feb 2015 11:04:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  975
 * SCR Title:  Add Fail Status Stream (SVO-354)
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SetRemoteMonitorList method to call SVFailStatusStreamManager::ManageFailStatus.
 * Revised ReplaceOrAddMonitorList method to call SVFailStatusStreamManager::ManageFailStatus.
 * Revised BuildPPQMonitorList method to call SVFailStatusStreamManager::AttachPPQObservers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   04 Dec 2014 10:30:00   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  918
 * SCR Title:  Implement Method RegisterMonitorList for RemoteControl (SVO-369)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Simplified the method ReplaceOrAddMonitorList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   20 Nov 2014 04:59:56   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  918
 * SCR Title:  Implement Method RegisterMonitorList for RemoteControl (SVO-369)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add method ReplaceOrAddMonitorList.
 * move static function IsValidMonitoredObject to a static method of this class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   29 Aug 2014 17:49:02   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added support for get/set product filter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   14 Aug 2014 17:55:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added WriteMonitorListToSharedMemory function.
 * Revised BuildPPQMonitorList to call WriteMonitorListToSharedMemory
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   14 Jul 2014 13:47:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed IsValidMonitoredObject to check if pObject owner is the ToolSet to allow for Acquisition Tool Images to be allowed
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   08 Jul 2014 09:07:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed GetObjectNameFromGuid function.
 * Revised BuildPPQMonitorList to use RemoteMonitorListHelper.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   20 Jun 2014 10:30:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed ActivateRemoteMonitorList to de-activate the active list when activating a monitor list
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 Jun 2014 17:35:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised BuildPPQMonitorList to return the reject depth
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   28 Apr 2014 15:32:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised BuildPPQMonitorList to only retrieve Activated Monitor Lists.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   28 Apr 2014 14:22:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added BuildPPQMonitorList method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   24 Apr 2014 10:47:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised the GetActiveRemoteMonitorList method to use a reference rather than return a copy of the list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   23 Apr 2014 14:33:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added a check in IsValidMonitorObject to check if the image is a SVMainImageClass or an SVRGBMainImageClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   23 Apr 2014 10:36:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added ActivateRemoteMonitorList and GetActiveRemoteMonitorList methods.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2014 13:13:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added validation logic for RemoteMonitorList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2014 09:39:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised Validation logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Apr 2014 16:26:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
