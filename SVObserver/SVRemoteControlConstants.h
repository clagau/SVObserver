//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteControlConstants
//* .File Name       : $Workfile:   SVRemoteControlConstants.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.9  $
//* .Check In Date   : $Date:   20 Nov 2014 05:04:28  $
//******************************************************************************
//include file is shared between SVobserver  and SVRC Project!

#pragma once

namespace SVRC
{
	namespace cmd
	{
		const std::string id = "ID";
		const std::string name = "Name";
		const std::string hr = "HResult";
		const std::string err = "ErrorText";
		const std::string arg = "Arguments";
		const std::string results = "Results";
		const std::string notfctn = "Notification";
		const std::string dataItems = "DataItems";
		const std::string command = "Command";
		const std::string response = "Response";
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
		const std::string getInspectionNames = "GetInspectionNames";
		const std::string getProductFilter = "GetProductFilter";
		const std::string setProductFilter = "SetProductFilter";
		const std::string getMonitorListProperties = "GetMonitorListProperties";
		const std::string getMaxRejectDeptCmd = "GetMaxRejectQeueDepth";
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
		const std::string rejectDepth = "rejectDepth";
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
		const std::string active = "active";
		const std::string rejectDepth = "rejectDepth";
		const std::string ppqName = "ppqName";
		const std::string maxRejectDepth = "maxRejectDepth";
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

	namespace stream
	{
		const std::string streamName = "StreamName";
		const std::string startStream = "StartStream";
		const std::string stopStream = "StopStream";
		const std::string command = "Command";
		const std::string response = "Response";
		const std::string status = "Status";
		const std::string notification = "Notification";
		const std::string dataItems = "DataItems";
		const std::string outputDataItems = "OutputDataItems";
	}

	namespace iobroker
	{
		const std::string queryStreamNames = "QueryStreamNames";
		const std::string queryDataItems = "QueryDataItems";
		const std::string streamNames = "StreamNames";
		const std::string triggerCount = "TriggerCount"; // obsolete, only here for backwards compatibility
	}

	namespace notification
	{
		const std::string notification = "Notification";
		const std::string lastmodified = "LastModified";
		const std::string currentmode = "CurrentMode";
		const std::string dataitems = "DataItems";
		const std::string timestamp = "TimeStamp";
		const std::string mode = "Mode";
		const std::string	MessageType  =  "MessageType" ;
		const std::string	MessageNumber = "MessageNumber" ;
		const std::string	MessageText =	"MessageText" ;
		const std::string	MessageNotification = "MessageNotification";
	}
}
