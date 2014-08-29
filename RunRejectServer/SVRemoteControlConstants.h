//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteControlConstants
//* .File Name       : $Workfile:   SVRemoteControlConstants.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   29 Aug 2014 17:35:56  $
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
		const std::string regMon = "RegisterMonitorList";
		const std::string qryProd = "QueryProductList";
		const std::string qryRjct = "QueryRejectCondList";
		const std::string qryFail = "QueryFailStatusList";
		const std::string getProd = "GetProduct";
		const std::string getDefList = "GetDataDefinitionList";
		const std::string getRjct = "GetReject";
		const std::string getFail = "GetFailStatus";
		const std::string actvMonList = "ActivateMonitorList";
		const std::string shutdownSVIM = "Shutdown";
		const std::string qryMonListNames = "QueryMonitorListNames";
		const std::string getProductFilter = "GetProductFilter";
		const std::string setProductFilter = "SetProductFilter";
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
		const std::string options = "options";
		const std::string filter = "filter";
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
		const std::string filter = "filter";
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
		const std::string fetch = "Fetch";
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\RunRejectServer\SVRemoteControlConstants.h_v  $
 * 
 *    Rev 1.1   29 Aug 2014 17:35:56   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added support for product filter, bug fixes, refactoring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Aug 2014 14:51:30   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial Check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 May 2013 14:33:32   jHanebach
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 4
 * SCR Title: Changes for 2.01 version
 * Checked in by: jHanebach; Jack Hanebach
 * Change Description: Initial Check-In
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
