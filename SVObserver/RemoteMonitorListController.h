//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteMonitorListController
//* .File Name       : $Workfile:   RemoteMonitorListController.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   28 Apr 2014 14:22:10  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "RemoteMonitorList.h"
#include "SVMonitorList.h"
#pragma endregion Includes

class SVConfigurationObject;

class RemoteMonitorListController
{
#pragma region Public
public:
	void Clear();
	bool IsEmpty() const;
	bool Setup(SVConfigurationObject* pConfig);
	const RemoteMonitorList& GetRemoteMonitorList() const;
	void SetRemoteMonitorList(const RemoteMonitorList& rList);
	void HideShowViewTab();
	void ResetObject();
	void ValidateInputs();
	void BuildPPQMonitorList(PPQMonitorList& ppqMonitorList) const;

	HRESULT ActivateRemoteMonitorList(const SVString& listName, bool bActivate);
	void GetActiveRemoteMonitorList(RemoteMonitorList& rActiveList) const;
#pragma endregion Public

#pragma region Private
private:
	RemoteMonitorList m_list;

	PPQNameListNames GetPPQMonitorLists(SVConfigurationObject* pConfig) const;
	bool ValidateMonitoredObject(MonitoredObjectList& rList);
#pragma endregion Private
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\RemoteMonitorListController.h_v  $
 * 
 *    Rev 1.3   28 Apr 2014 14:22:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added BuildPPQMonitorList method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Apr 2014 10:47:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised the GetActiveRemoteMonitorList method to use a reference rather than return a copy of the list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Apr 2014 10:36:38   sjones
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
