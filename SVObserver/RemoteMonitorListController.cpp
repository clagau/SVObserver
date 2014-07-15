//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteMonitorListController
//* .File Name       : $Workfile:   RemoteMonitorListController.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.11  $
//* .Check In Date   : $Date:   14 Jul 2014 13:47:44  $
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
#include "SVObserver.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVMainImageClass.h"
#include "RemoteMonitorListHelper.h"
#include "SVToolSet.h"
#pragma endregion Includes

extern SVObserverApp TheSVObserverApp;

PPQNameListNames RemoteMonitorListController::GetPPQMonitorLists(SVConfigurationObject* pConfig) const
{
	PPQNameListNames list;

	long lPPQCount;
	pConfig->GetPPQCount(lPPQCount);

	// get list of PPQs
	for (int i = 0;i < lPPQCount; i++)
	{
		SVPPQObject* pPPQ = NULL;
		pConfig->GetPPQ(i, &pPPQ);
		if (pPPQ)
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
	const PPQNameListNames& rPPQList = GetPPQMonitorLists(pConfig);
	MonitorListAddRemoveDlg dlg(m_list, rPPQList);
	INT_PTR rc = dlg.DoModal();
	if (IDOK == rc)
	{
		bRetVal = true;
		SetRemoteMonitorList(dlg.GetRemoteMonitorList()); // Update the list
	}
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
static bool IsValidMonitoredObject(SVObjectClass* pObject)
{
	bool bRetVal = false;
	if (nullptr != pObject)
	{
		UINT attr = pObject->ObjectAttributesSet();
		bRetVal = (attr & (SV_PUBLISHABLE | SV_PUBLISH_RESULT_IMAGE) ? true : false);
		//check to see if MainImageClass or SVRGBMainImageClass
		if (!bRetVal)
		{
			if  ( SV_IS_KIND_OF(pObject->GetOwner(),SVToolSetClass) && (SV_IS_KIND_OF(pObject, SVMainImageClass) || SV_IS_KIND_OF(pObject, SVRGBMainImageClass)) )
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
		// Check that the PPQ still exists
		const SVGUID& guid = SVObjectManagerClass::Instance().GetObjectIdFromCompleteName(ppqName);
		if (guid.empty())
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

void RemoteMonitorListController::BuildPPQMonitorList(PPQMonitorList& ppqMonitorList) const
{
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
		}
	}

	RemoteMonitorList::iterator it = m_list.find(listName);
	if (it != m_list.end())
	{
		it->second.Activate(bActivate);
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\RemoteMonitorListController.cpp_v  $
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
