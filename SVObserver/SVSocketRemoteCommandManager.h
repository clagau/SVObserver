//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocketRemoteCommandManager
//* .File Name       : $Workfile:   SVSocketRemoteCommandManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Oct 2013 15:41:46  $
//******************************************************************************

#ifndef SVSOCKETREMOTECOMMANDMANAGER_H
#define SVSOCKETREMOTECOMMANDMANAGER_H

#include <map>
#include <string>
#include <boost/function.hpp>

#include "SVJsonCommandServerLibrary/SVJsonCommandManager.h"
#include "SVStorageResult.h"
#include "SVDataDefinitionStruct.h"

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
	static HRESULT AddResultStatusToJsonValue( const SVNameStorageMap& p_rItems, const SVNameStatusMap& p_rResultStatus, Json::Value& p_rJsonErrors );

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

	AFX_INLINE static HRESULT WriteJsonCommandToFile(const Json::Value& p_rJsonValues, const std::string& p_rFileName);
	AFX_INLINE static HRESULT WriteJsonCommandToFile(const std::string& p_rJsonCommand, const std::string& p_rFileName);

	AFX_INLINE static HRESULT WriteResultToJsonAndFile( const std::string& p_rJsonCommand, std::string& p_rJsonResults, 
														const Json::Value& p_rJsonResultValue, const std::string& p_rFileName , HRESULT p_Status);

	static SVCommandFunctionMap m_Functions;
	static SVCommandFunctionMap m_AsyncFunctions;

private:
	SVRemoteCommandFunctions();
	SVRemoteCommandFunctions( const SVRemoteCommandFunctions& p_rObject );

	virtual ~SVRemoteCommandFunctions();

	SVRemoteCommandFunctions& operator=( const SVRemoteCommandFunctions& p_rObject );

};

typedef SVJsonCommandManager< SVRemoteCommandFunctions > SVSocketRemoteCommandManager;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVSocketRemoteCommandManager.h_v  $
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
