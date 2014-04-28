//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteControlConstants
//* .File Name       : $Workfile:   SVRemoteControlConstants.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   24 Apr 2014 11:25:50  $
//******************************************************************************

#ifndef SVREMOTECONTROLCONSTANTS_H
#define SVREMOTECONTROLCONSTANTS_H

namespace SVRC
{
	namespace cmd
	{
		const std::string id = "ID";
		const std::string name = "Name";
		const std::string hr = "HResult";
		const std::string err = "ErrorText";
		const std::string arg = "Arguments";
		const std::string reslts = "Results";
		const std::string notfctn = "Notification";
	}

	namespace cmdName
	{
		const std::string beginGetCfg = "BeginGetConfig"; // Not for SVObserver
		const std::string getConfig = "GetConfig";
		const std::string getItems = "GetItems";
		const std::string getFile = "GetDeviceFile";
		const std::string getMode = "GetDeviceMode";
		const std::string getOLCount = "GetDeviceOfflineCount";
		const std::string getReport = "GetDeviceConfigReport";
		const std::string getState = "GetState";
		const std::string getVersion = "GetVersions";
		const std::string getVPName = "GetVPName"; // Not for SVObserver 
		const std::string putConfig = "PutConfig";
		const std::string putFile = "PutDeviceFile";
		const std::string setItems = "SetItems";
		const std::string setMode = "SetDeviceMode";
		const std::string regProd = "RegisterProductList";
		const std::string qryProd = "QueryProductList";
		const std::string qryRjct = "QueryRejectCondList";
		const std::string qryFail = "QueryFailStatusList";
		const std::string getProd = "GetProduct";
		const std::string getDefList = "GetDataDefinitionList";
		const std::string getRjct = "GetReject";
		const std::string actvMonList = "ActivateMonitorList";
	}

	namespace arg
	{
		const std::string contents = "contents";
		const std::string item_names = "itemNames";
		const std::string scale = "imageScale";
		const std::string desiredMode = "desiredMode";
		const std::string values = "Values";
		const std::string images = "Images";
		const std::string errors = "Errors";
		const std::string filePath = "filePath";
		const std::string productName = "productName";
		const std::string sourcePath = "sourcePath";
		const std::string sourceFileName = "sourceFileName";
		const std::string destinationPath = "destinationPath";
		const std::string inspectionName = "inspectionName";
		const std::string listName = "listName";
		const std::string ppqName = "ppqName";
		const std::string prodList = "productItemList";
		const std::string rjctList = "rejectCondList";
		const std::string failList = "failStatusList";
		const std::string trgrCount = "triggerCount";
		const std::string currentList = "currentViewList";
		const std::string currentItems = "currentViewItems";
		const std::string url = "url";
		const std::string listType = "listType";
		const std::string active = "active";
		const std::string id = "id";
	}

	namespace result
	{
		const std::string items = "items";
		const std::string contents = "contents";
		const std::string url = "url";
		const std::string names = "names";
		const std::string name = "name";
		const std::string faults = "faults";
		const std::string mode = "mode";
		const std::string state = "state";
		const std::string count = "count";
		const std::string report = "report";
		const std::string values = "Values";
		const std::string images = "Images";
		const std::string errors = "Errors";
		const std::string SVO_ver = "SVO_ver";
		const std::string SVC_ver = "SVC_ver";
		const std::string SVW_ver = "SVW_ver";
		const std::string filePath = "filePath";
		const std::string destinationPath = "destinationPath";
		const std::string destinationFileName = "destinationFileName";
		const std::string entries = "Entries";
	}

	//Value Object
	namespace vo
	{
		const std::string name = "Name";
		const std::string count = "ProcessCount";
		const std::string array = "Array";
		const std::string status = "Status";
	}

	//Image Object
	namespace io
	{
		const std::string name = "Name";
		const std::string count = "ProcessCount";
		const std::string image = "Image";
		const std::string imageFileName = "ImageFileName";
		const std::string status = "Status";
	}

	namespace error
	{
		const std::string name = "Name";
		const std::string status = "Status";
	}

	//Data Definition List Object
	namespace ddlo
	{
		const std::string name = "Name";
		const std::string writable = "Writable";
		const std::string published = "Published";
		const std::string type = "Type";
		const std::string add_info = "AdditionalInfo";
	}
}
#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRemoteControlConstants.h_v  $
 * 
 *    Rev 1.5   24 Apr 2014 11:25:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed DeactivateMonitorList constant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   23 Apr 2014 18:02:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added active constant.
 * Revised activate/deactivate monitor list constants.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   23 Apr 2014 10:45:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added ActiveMonitorList command constant.
 * Added DeactiveMonitorList command constant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Apr 2014 17:01:22   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   new constants for the Remote Monitor List
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Oct 2013 15:39:14   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  866
 * SCR Title:  Add GetDataDefinitionList Command to SVObserver's Remote Command Socket
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added "GetDataDefinitionList" to cmdName.
 *   Removed "Errors" from arg.  ("Errors" was never an argument.)
 *   Added "inspectionName" and "listType" to arg.
 *   Added "Entries" to result.
 *   Added namespace ddlo for Data Definition List Objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:46:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Apr 2013 12:33:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to process the remote commands as specified in the IF00100.9400.001 interface document.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Mar 2013 14:09:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new constants for GetItems Error List.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 12:50:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new framework objects used for new remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
