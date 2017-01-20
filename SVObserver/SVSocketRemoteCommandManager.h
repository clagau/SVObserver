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
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <boost/function.hpp>

#include "SVJsonCommandServerLibrary/SVJsonCommandManager.h"
#include "SVStorageResult.h"
#include "SVDataDefinitionStruct.h"
#include "SVUtilityLibrary/SVString.h"
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
	static HRESULT GetMonitorListProperties( const std::string& rJsonCommand, std::string& rJsonResults );
	static HRESULT GetMaxRejectDepth( const std::string& rJsonCommand, std::string& rJsonResults );

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

