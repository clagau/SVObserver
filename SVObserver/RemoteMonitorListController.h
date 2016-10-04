//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteMonitorListController
//* .File Name       : $Workfile:   RemoteMonitorListController.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   20 Nov 2014 04:59:56  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "RemoteMonitorList.h"
#include "SVMonitorList.h"
#include "SVSharedMemoryLibrary/SVProductFilterEnum.h"
#pragma endregion Includes

class SVConfigurationObject;
class SVObjectClass;

class RemoteMonitorListController
{
#pragma region Public
public:
	void Clear();
	bool IsEmpty() const;
	bool Setup(SVConfigurationObject* pConfig);
	const RemoteMonitorList& GetRemoteMonitorList() const;
	void SetRemoteMonitorList(const RemoteMonitorList& rList);

	//************************************
	// Method:    ReplaceOrAddMonitorList
	// Description:  Replace a monitor list if already exist or add a monitor list to the monitor lists.
	// Parameter: const RemoteMonitorNamedList & rList
	// Returns:   void
	//************************************
	void ReplaceOrAddMonitorList( const RemoteMonitorNamedList& rList );
	void HideShowViewTab();
	void ResetObject();
	void ValidateInputs();
	HRESULT BuildPPQMonitorList(PPQMonitorList& ppqMonitorList) const;

	//! Activate or deactivate the Monitorlist.
	//!	When the Monitorlist is activated all Other Monitorlist which belong to the same PPQ are deactivated
	//! \param listName [in]
	//! \param bActivate [in]
	//! \returns S_OK when unsuccessfully 
	HRESULT ActivateRemoteMonitorList(const SVString& listName, bool bActivate);
	
	void GetActiveRemoteMonitorList(RemoteMonitorList& rActiveList) const;

	HRESULT SetRemoteMonitorListProductFilter(const SVString& listName, SvSml::SVProductFilterEnum filter);
	HRESULT GetRemoteMonitorListProductFilter(const SVString& listName, SvSml::SVProductFilterEnum& rFilter) const;

	//************************************
	// Method:    IsValidMonitoredObject
	// Description:  Static method to check if an object is allowed to be part of a monitor list.
	// Parameter: pObject - pointer to the object in question
	// Returns:   bool - true if the object is allowed to be part of a monitor list
	//************************************
	static bool IsValidMonitoredObject(const SVObjectClass* pObject);


	static LPCTSTR s_DefaultMonitorListName;



#pragma endregion Public

#pragma region Private	
private:
	//! Write the monitorlist to shared memory
	//! \param name [in] name of monitorlist
	//! \param remoteMonitorNamedlist [in] monitorlist
	//! \returns void
	static void WriteMonitorListToSharedMemory(const std::string& name, const RemoteMonitorNamedList& remoteMonitorNamedlist);
	
	//! Initialize the monitor writer 
	static HRESULT InitMonitorListInSharedMemory(size_t size);

	RemoteMonitorList m_list;

	PPQNameListNames GetPPQMonitorLists(SVConfigurationObject* pConfig) const;
	bool ValidateMonitoredObject(MonitoredObjectList& rList);
#pragma endregion Private
};

