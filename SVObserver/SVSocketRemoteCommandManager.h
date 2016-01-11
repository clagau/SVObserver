//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocketRemoteCommandManager
//* .File Name       : $Workfile:   SVSocketRemoteCommandManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.9  $
//* .Check In Date   : $Date:   20 Nov 2014 05:05:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <map>
#include <string>
#include <boost/function.hpp>

#include "SVJsonCommandServerLibrary/SVJsonCommandManager.h"
#include "SVStorageResult.h"
#include "SVDataDefinitionStruct.h"
#pragma endregion Includes

namespace Json
{
	class Value;
}

class SVRemoteCommandFunctions
{
public:
	typedef boost::function<HRESULT ( const std::string& p_rCommand, std::string& p_rResponse )> SVCommandFunction;
	typedef std::map< std::string, SVCommandFunction > SVCommandFunctionMap;
	typedef SVCommandFunctionMap::iterator iterator;

	static bool IsAsyncCommand( const std::string& p_rJsonCommand );

	static HRESULT ProcessCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults );
	static HRESULT ProcessAsyncCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults );
	static void BuildErrorResponse(const std::string& rCommand, std::string& rResponse, HRESULT hr, const std::string& errorText=std::string());

protected:
	static void GetCommandName( const std::string& p_rJsonCommand, std::string& p_rName );
	static void GetCommandId( const std::string& p_rJsonCommand, long& p_rId );

	static HRESULT GetFileNameFromFilePath( std::string& p_rFileName, const std::string& p_rFilePath );
	static HRESULT GetTempFileNameFromFilePath( std::string& p_rTempFileName, const std::string& p_rFilePath );

	static HRESULT ConvertJsonValueToFile( const Json::Value& p_rJsonValue, const std::string& p_rSourceFileName, std::string& p_rFileName );

	static HRESULT ConvertStorageResultValueToJsonValue( const SVString& p_rName, const SVStorageResult& p_rStorage, Json::Value& p_rJsonValue );
	static HRESULT ConvertStorageResultImageFileToJsonValue( const SVString& p_rName, const SVStorageResult& p_rStorage, Json::Value& p_rJsonValue );

	static HRESULT ConvertDataDefinitionToJsonValue( const SVDataDefinitionStruct& p_rDataDefinition , Json::Value& p_rJsonValue );

	static HRESULT AddJsonValuesToStorageItems( const Json::Value& p_rJsonValues, SVNameStorageMap& p_rItems );
	static HRESULT AddJsonImagesToStorageItems( const Json::Value& p_rJsonImages, SVNameStorageMap& p_rItems );

	//************************************
	// Method:    CreateJsonValueWithResultErrors
	// Description:  Create a json value for returning an error list. Check if a status is missing for an item (not processed) or a status is failed.
	// Parameter: const SVNameStorageMap& p_rItems   List of items which should be processed. 
	// Parameter: const SVNameStatusMap & rItemResultStatus  Status for each item.
	// Parameter: Json::Value & jsonResult [in/out] This object must be of the type object. The return value is an array of errors in this object.
	// Returns:   HRESULT Is S_OK if no error was found.
	//************************************
	static HRESULT CreateJsonValueWithResultErrors( const SVNameStorageMap& p_rItems, const SVNameStatusMap& rItemResultStatus, Json::Value& jsonResult );

	//************************************
	// Method:    CreateJsonValueWithResultErrors
	// Description:  Create a json value for returning an error list from a statusMap.
	// Parameter: const SVNameStatusMap & rItemResultStatus  List of status of the result to the items.
	// Parameter: Json::Value & jsonResult [in/out] This object must be of the type object. The return value is an array of errors in this object.
	// Returns:   HRESULT Is S_OK if no error was found.
	//************************************
	static HRESULT CreateJsonValueWithResultErrors( const SVNameStatusMap& rItemResultStatus, Json::Value& jsonResult );
	static HRESULT CommandNotFound( const std::string& p_rJsonCommand, std::string& p_rJsonResults );

	static HRESULT GetDeviceMode( const std::string& p_rJsonCommand, std::string& p_rJsonResults );
	static HRESULT GetState( const std::string& p_rJsonCommand, std::string& p_rJsonResults );
	static HRESULT GetVersions( const std::string& p_rJsonCommand, std::string& p_rJsonResults );
	static HRESULT GetDeviceOfflineCount( const std::string& p_rJsonCommand, std::string& p_rJsonResults );

	static HRESULT GetConfig( const std::string& p_rJsonCommand, std::string& p_rJsonResults );
	static HRESULT GetItems( const std::string& p_rJsonCommand, std::string& p_rJsonResults );
	static HRESULT GetDeviceFile( const std::string& p_rJsonCommand, std::string& p_rJsonResults );
	static HRESULT GetDeviceConfigReport( const std::string& p_rJsonCommand, std::string& p_rJsonResults );
	static HRESULT GetDataDefinitionList( const std::string& p_rJsonCommand, std::string& p_rJsonResults );
	static HRESULT PutConfig( const std::string& p_rJsonCommand, std::string& p_rJsonResults );
	static HRESULT PutDeviceFile( const std::string& p_rJsonCommand, std::string& p_rJsonResults );
	static HRESULT SetDeviceMode( const std::string& p_rJsonCommand, std::string& p_rJsonResults );
	static HRESULT SetItems( const std::string& p_rJsonCommand, std::string& p_rJsonResults );

	static HRESULT Shutdown( const std::string& command, std::string& jsonResults );
	static HRESULT ActivateMonitorList( const std::string& rJsonCommand, std::string& rJsonResults );

	//************************************
	// Method:    RegisterMonitorList
	// Description:  Add a monitor list to or replaces a monitor list in the current configuration.
	// Parameter: const std::string& rJsonCommand
	// Parameter: std::string rJsonResults
	// Returns:   HRESULT Is S_OK if no error was found.
	//************************************
	static HRESULT RegisterMonitorList( const std::string& rJsonCommand, std::string& rJsonResults );

	static HRESULT QueryProductList( const std::string& rJsonCommand, std::string& rJsonResults );
	static HRESULT QueryRejectCondList( const std::string& rJsonCommand, std::string& rJsonResults );
	static HRESULT QueryFailStatusList( const std::string& rJsonCommand, std::string& rJsonResults );
	static HRESULT QueryMonitorListNames( const std::string& rJsonCommand, std::string& rJsonResults );
	static HRESULT GetInspectionNames( const std::string& rJsonCommand, std::string& rJsonResults );
	static HRESULT GetProductFilter( const std::string& rJsonCommand, std::string& rJsonResults );
	static HRESULT SetProductFilter( const std::string& rJsonCommand, std::string& rJsonResults );

	inline static HRESULT WriteJsonCommandToFile(const Json::Value& p_rJsonValues, const std::string& p_rFileName);
	inline static HRESULT WriteJsonCommandToFile(const std::string& p_rJsonCommand, const std::string& p_rFileName);

	inline static HRESULT WriteResultToJsonAndFile( const std::string& p_rJsonCommand, std::string& p_rJsonResults, 
													const Json::Value& p_rJsonResultValue, const std::string& p_rFileName, HRESULT p_Status);

	static SVCommandFunctionMap m_Functions;
	static SVCommandFunctionMap m_AsyncFunctions;

private:
	SVRemoteCommandFunctions();
	SVRemoteCommandFunctions( const SVRemoteCommandFunctions& p_rObject );

	virtual ~SVRemoteCommandFunctions();

	SVRemoteCommandFunctions& operator=( const SVRemoteCommandFunctions& p_rObject );

};

typedef SVJsonCommandManager< SVRemoteCommandFunctions > SVSocketRemoteCommandManager;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\svobserver\SVSocketRemoteCommandManager.h_v  $
 * 
 *    Rev 1.9   20 Nov 2014 05:05:16   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  918
 * SCR Title:  Implement Method RegisterMonitorList for RemoteControl (SVO-369)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add regMon and method RegisterMonitorList
 * remove AddResultStatusToJsonValue and replace it with two methods CreateJsonValueWithResultErrors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   10 Nov 2014 17:10:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  970
 * SCR Title:  GetConfig and PutConfig cause a crash when there is not enough disk space
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised BuildErrorResponse to iclude the error text in the Json if it's supplied.
 * Revised GetConfig to catch any SVException thrown and call BuildErrorResponse on any error.
 * Revised PutConfig to catch any SVException thrown and call BuildErrorResponse on any error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   02 Oct 2014 10:22:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  949
 * SCR Title:  Repeatedly Calling PutConfig Causes SVObserver to Stop Responding
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added BuildErrorResponse method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Aug 2014 17:49:06   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added support for get/set product filter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Jul 2014 17:04:06   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  915
 * SCR Title:  Add command QueryMonitorListNames for RemoteControl
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add method QueryMonitorListNames
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Jun 2014 07:21:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  905
 * SCR Title:  Implement Shutdown Command thru Remote Control
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added remote command function Shutdown.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   23 Apr 2014 18:07:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed ConvertNameToJsonValue method.
 * Removed DeactivateMonitorList method.
 * Revised ActivateMonitorList method.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   23 Apr 2014 10:47:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added ConvertNameToJsonValue method.
 * Added QueryProductList method.
 * Added QueryRejectCondList method.
 * Added QueryFailStatusList method.
 * Added ActivateMonitorList method.
 * Added DeactivateMonitorList method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Oct 2013 15:41:46   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  866
 * SCR Title:  Add GetDataDefinitionList Command to SVObserver's Remote Command Socket
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added ConvertDataDefinitionToJsonValue, GetDataDefinitionList, WriteJsonCommandToFile, and WriteResultToJsonAndFile.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:08:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Apr 2013 12:33:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to process the remote commands as specified in the IF00100.9400.001 interface document.
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
