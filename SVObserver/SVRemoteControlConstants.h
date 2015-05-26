//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRemoteControlConstants
//* .File Name       : $Workfile:   SVRemoteControlConstants.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.9  $
//* .Check In Date   : $Date:   20 Nov 2014 05:04:28  $
//******************************************************************************

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
		const std::string queryStreamNames = "QueryStreamNames";
		const std::string queryDataItems = "QueryDataItems";
	}

	namespace notification
	{
		const std::string notification = "Notification";
		const std::string lastmodified = "LastModified";
		const std::string currentmode = "CurrentMode";
		const std::string dataitems = "DataItems";
		const std::string timestamp = "TimeStamp";
		const std::string mode = "Mode";
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\svobserver\SVRemoteControlConstants.h_v  $
 * 
 *    Rev 1.9   20 Nov 2014 05:04:28   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  918
 * SCR Title:  Implement Method RegisterMonitorList for RemoteControl (SVO-369)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add string rejectDepth to arg
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   28 Aug 2014 18:46:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added getProductFilter to cmd namespace
 * Added setProductFilter to cmd namespace
 * Added filter to arg namespace
 * Added filter to result namespace
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   09 Jul 2014 17:02:48   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  915
 * SCR Title:  Add command QueryMonitorListNames for RemoteControl
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   rename RegisterProductList into RegisterMonitorList
 * add GetFailStatus and QueryMonitorListNames
 * add fetch
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   24 Jun 2014 07:28:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  905
 * SCR Title:  Implement Shutdown Command thru Remote Control
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added constants to support remote shutdown command.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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
