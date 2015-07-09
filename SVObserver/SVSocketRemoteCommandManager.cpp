//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocketRemoteCommandManager
//* .File Name       : $Workfile:   SVSocketRemoteCommandManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.16  $
//* .Check In Date   : $Date:   20 Nov 2014 05:05:16  $
//******************************************************************************

#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/assign.hpp>
#include <fstream>
#include <io.h>
#include "SVSocketRemoteCommandManager.h"
#include "SVTimerLibrary\SVClock.h"
#include "JsonLib/include/json.h"
#include "SVJsonCommandServerLibrary/SVJsonUtilities.h"
#include "SVSystemLibrary/SVEncodeDecodeUtilities.h"
#include "SVSharedMemoryLibrary/SVProductFilterEnum.h"

#include "SVGlobal.h"
#include "SVRemoteControlConstants.h"
#include "SVVisionProcessorHelper.h"
#include "ObjectInterfaces/SVUserMessage.h"
#include "RemoteMonitorNamedList.h"

#define SV_DATA_TO_CONTENTS
//#define SV_OUTPUT_JSON

SVRemoteCommandFunctions::SVCommandFunctionMap SVRemoteCommandFunctions::m_Functions = boost::assign::map_list_of< std::string, SVRemoteCommandFunctions::SVCommandFunction >
(SVRC::cmdName::getMode, &SVRemoteCommandFunctions::GetDeviceMode)
(SVRC::cmdName::getState, &SVRemoteCommandFunctions::GetState)
(SVRC::cmdName::getVersion, &SVRemoteCommandFunctions::GetVersions)
(SVRC::cmdName::getOLCount, &SVRemoteCommandFunctions::GetDeviceOfflineCount)
(SVRC::cmdName::actvMonList, &SVRemoteCommandFunctions::ActivateMonitorList)
(SVRC::cmdName::getProductFilter, &SVRemoteCommandFunctions::GetProductFilter)
(SVRC::cmdName::setProductFilter, &SVRemoteCommandFunctions::SetProductFilter)
;

SVRemoteCommandFunctions::SVCommandFunctionMap SVRemoteCommandFunctions::m_AsyncFunctions = boost::assign::map_list_of< std::string, SVRemoteCommandFunctions::SVCommandFunction >
(SVRC::cmdName::getConfig, &SVRemoteCommandFunctions::GetConfig)
(SVRC::cmdName::getItems, &SVRemoteCommandFunctions::GetItems)
(SVRC::cmdName::getFile, &SVRemoteCommandFunctions::GetDeviceFile)
(SVRC::cmdName::getReport, &SVRemoteCommandFunctions::GetDeviceConfigReport)
(SVRC::cmdName::getDefList, &SVRemoteCommandFunctions::GetDataDefinitionList)
(SVRC::cmdName::putConfig, &SVRemoteCommandFunctions::PutConfig)
(SVRC::cmdName::putFile, &SVRemoteCommandFunctions::PutDeviceFile)
(SVRC::cmdName::setMode, &SVRemoteCommandFunctions::SetDeviceMode)
(SVRC::cmdName::setItems, &SVRemoteCommandFunctions::SetItems)
(SVRC::cmdName::regMon, &SVRemoteCommandFunctions::RegisterMonitorList)
(SVRC::cmdName::qryProd,  &SVRemoteCommandFunctions::QueryProductList)
(SVRC::cmdName::qryRjct, &SVRemoteCommandFunctions::QueryRejectCondList)
(SVRC::cmdName::qryFail, &SVRemoteCommandFunctions::QueryFailStatusList)
(SVRC::cmdName::qryMonListNames, &SVRemoteCommandFunctions::QueryMonitorListNames)
(SVRC::cmdName::shutdownSVIM, &SVRemoteCommandFunctions::Shutdown)
;

bool SVRemoteCommandFunctions::IsAsyncCommand( const std::string& p_rJsonCommand )
{
	bool l_Status = true;

	std::string l_Command;

	GetCommandName( p_rJsonCommand, l_Command );

	SVCommandFunctionMap::iterator l_Iter = m_AsyncFunctions.find( l_Command );

	l_Status = ( l_Iter != m_AsyncFunctions.end() );

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::ProcessCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	std::string l_Command;

	GetCommandName( p_rJsonCommand, l_Command );

	SVCommandFunctionMap::iterator l_Iter = m_Functions.find( l_Command );

	if( l_Iter != m_Functions.end() )
	{
		l_Status = (l_Iter->second)( p_rJsonCommand, p_rJsonResults );
	}
	else
	{
		l_Status = CommandNotFound( p_rJsonCommand, p_rJsonResults );
	}

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::ProcessAsyncCommand( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	std::string l_Command;

	GetCommandName( p_rJsonCommand, l_Command );

	SVCommandFunctionMap::iterator l_Iter = m_AsyncFunctions.find( l_Command );

	if( l_Iter != m_AsyncFunctions.end() )
	{
		l_Status = (l_Iter->second)( p_rJsonCommand, p_rJsonResults );
	}
	else
	{
		l_Status = CommandNotFound( p_rJsonCommand, p_rJsonResults );
	}

	return l_Status;
}

void SVRemoteCommandFunctions::GetCommandName( const std::string& p_rJsonCommand, std::string& p_rName )
{
	p_rName.clear();

	if( 0 < p_rJsonCommand.length() )
	{
		Json::Reader l_Reader;
		Json::Value l_JsonValues;

		if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
		{
			if( l_JsonValues.isObject() )
			{
				Json::Value l_JsonCommand = l_JsonValues[ SVRC::cmd::name ];

				if( l_JsonCommand.isString() )
				{
					p_rName = l_JsonCommand.asString();
				}
			}
		}
	}
}

void SVRemoteCommandFunctions::GetCommandId( const std::string& p_rJsonCommand, long& p_rId )
{
	p_rId = 0;

	if( 0 < p_rJsonCommand.length() )
	{
		Json::Reader l_Reader;
		Json::Value l_JsonValues;

		if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
		{
			if( l_JsonValues.isObject() )
			{
				Json::Value l_JsonId = l_JsonValues[ SVRC::cmd::id ];

				if( l_JsonId.isInt() )
				{
					p_rId = l_JsonId.asInt();
				}
				else if( l_JsonId.isUInt() )
				{
					p_rId = l_JsonId.asUInt();
				}
			}
		}
	}
}

HRESULT SVRemoteCommandFunctions::GetFileNameFromFilePath( std::string& p_rFileName, const std::string& p_rFilePath )
{
	HRESULT l_Status = S_OK;

	p_rFileName.clear();

	if( !( p_rFilePath.empty() ) )
	{
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath( p_rFilePath.c_str(), NULL, NULL, fname, ext );

		p_rFileName += fname;
		p_rFileName += ext;
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::GetTempFileNameFromFilePath( std::string& p_rTempFileName, const std::string& p_rFilePath )
{
	std::string l_FileName;
	HRESULT l_Status = GetFileNameFromFilePath( l_FileName, p_rFilePath );

	p_rTempFileName.clear();

	if( !( l_FileName.empty() ) )
	{
		SVString l_TempString;
		__int64 l_TimeStamp = static_cast< __int64 >( SVClock::GetTimeStamp() );

		l_TempString.Format( "%I64d", l_TimeStamp );

		p_rTempFileName += "V:\\";
		p_rTempFileName += l_TempString.c_str();
		p_rTempFileName += "-";
		p_rTempFileName += l_FileName;
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::ConvertJsonValueToFile( const Json::Value& p_rJsonValue, const std::string& p_rSourceFileName, std::string& p_rFileName )
{
	HRESULT l_Status = S_OK;

	if( p_rJsonValue.isString() && !( p_rJsonValue.asString().empty() ) )
	{
		l_Status = GetTempFileNameFromFilePath( p_rFileName, p_rSourceFileName );

		if( !( p_rFileName.empty() ) )
		{
			l_Status = SVEncodeDecodeUtilities::Base64DecodeToFileFromString( p_rFileName, p_rJsonValue.asString() );
		}
		else if( l_Status == S_OK )
		{
			l_Status = E_INVALIDARG;
		}

		if( l_Status != S_OK )
		{
			p_rFileName.clear();
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}
	
	return l_Status;
}

HRESULT SVRemoteCommandFunctions::ConvertStorageResultValueToJsonValue( const SVString& p_rName, const SVStorageResult& p_rStorage, Json::Value& p_rJsonValue )
{
	HRESULT l_Status = S_OK;

	if( ( p_rStorage.m_Storage.m_StorageType == SVVisionProcessor::SVStorageValue ) && p_rJsonValue.isObject() )
	{
		HRESULT l_ItemStatus = p_rStorage.m_Status;

		Json::Value l_JsonArray;

		p_rJsonValue[SVRC::vo::name] = p_rName.c_str();
		p_rJsonValue[SVRC::vo::count] = static_cast< Json::Value::UInt >( p_rStorage.m_TriggerCount );

		if( l_ItemStatus == S_OK )
		{
			l_ItemStatus = SVJsonUtilities::ConvertVariantToJsonValue( p_rStorage.m_Storage.m_Variant, l_JsonArray );

			if( !( l_JsonArray.isArray() ) )
			{
				Json::Value l_JsonTemp( Json::arrayValue );

				l_JsonTemp.append( l_JsonArray );

				l_JsonArray = l_JsonTemp;
			}

			p_rJsonValue[SVRC::vo::array] = l_JsonArray;
		}

		p_rJsonValue[SVRC::vo::status] = l_ItemStatus;
	}
	else
	{
		p_rJsonValue.clear();

		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::ConvertStorageResultImageFileToJsonValue( const SVString& p_rName, const SVStorageResult& p_rStorage, Json::Value& p_rJsonValue )
{
	HRESULT l_Status = S_OK;

	if( ( p_rStorage.m_Storage.m_StorageType == SVVisionProcessor::SVStorageImageFileName ) && ( p_rStorage.m_Storage.m_Variant.vt == VT_BSTR ) && p_rJsonValue.isObject() )
	{
		HRESULT l_ItemStatus = p_rStorage.m_Status;

		p_rJsonValue[SVRC::io::name] = p_rName.c_str();
		p_rJsonValue[SVRC::io::count] = static_cast< Json::Value::UInt >( p_rStorage.m_TriggerCount );

		if( l_ItemStatus == S_OK )
		{
			SVString l_FileName = p_rStorage.m_Storage.m_Variant;

			if( !( l_FileName.empty() ) )
			{
				#ifdef SV_DATA_TO_CONTENTS
					std::string l_Contents;

					l_ItemStatus = SVEncodeDecodeUtilities::Base64EncodeToStringFromFile( l_Contents, l_FileName );

					if( l_ItemStatus == S_OK )
					{
						p_rJsonValue[SVRC::io::image] = l_Contents;

						::remove( l_FileName.c_str() );
					}
				#else
					std::string l_TempFileName;

					l_ItemStatus = GetFileNameFromFilePath( l_TempFileName, l_FileName );

					if( !( l_TempFileName.empty() ) )
					{
						p_rJsonValue[SVRC::io::imageFileName] = l_TempFileName;
					}
					else if( l_ItemStatus == S_OK )
					{
						l_ItemStatus = E_UNEXPECTED;
					}
				#endif
			}
			else if( l_ItemStatus == S_OK )
			{
				l_ItemStatus = E_UNEXPECTED;
			}
		}

		p_rJsonValue[SVRC::io::status] = l_ItemStatus;

		if( l_Status == S_OK && l_ItemStatus != S_OK )
		{
			l_Status = E_UNEXPECTED;
		}
	}
	else
	{
		p_rJsonValue.clear();

		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}

/*####################################################################################################################################
*	ConvertDataDefinitionToJsonValue	Converts the data definition structure to a Json value
*	Return: 							The status result of the method  
*
*	p_rDataDefinition					The data definition structure to convert
*	p_rJsonValue						The resulting Json value after conversion
*#####################################################################################################################################
*/
HRESULT SVRemoteCommandFunctions::ConvertDataDefinitionToJsonValue( const SVDataDefinitionStruct& p_rDataDefinition , Json::Value& p_rJsonValue )
{
	HRESULT l_Status = S_OK;

	if( p_rJsonValue.isObject() )
	{
		p_rJsonValue[SVRC::ddlo::name] = p_rDataDefinition.m_Name.c_str();
		p_rJsonValue[SVRC::ddlo::writable] = p_rDataDefinition.m_Writable;
		p_rJsonValue[SVRC::ddlo::published] = p_rDataDefinition.m_Published;
		p_rJsonValue[SVRC::ddlo::type] = p_rDataDefinition.m_Type.c_str();

		Json::Value l_AddInfoArray(Json::arrayValue);
		for( SVStringArray::const_iterator l_Iter = p_rDataDefinition.m_AdditionalInfo.begin(); l_Iter != p_rDataDefinition.m_AdditionalInfo.end(); ++l_Iter )
		{
			Json::Value l_Value(Json::objectValue);
			l_Value = l_Iter->c_str();
			l_AddInfoArray.append(l_Value);
		}
		p_rJsonValue[SVRC::ddlo::add_info] = l_AddInfoArray;
	}
	else
	{
		p_rJsonValue.clear();

		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::AddJsonValuesToStorageItems( const Json::Value& p_rJsonValues, SVNameStorageMap& p_rItems )
{
	HRESULT l_Status = S_OK;

	if( p_rJsonValues.isArray() )
	{
		for( Json::Value::UInt i = 0; i < p_rJsonValues.size(); i++ )
		{
			const Json::Value& l_rElement = p_rJsonValues[ i ];

			if( l_rElement.isObject() )
			{
				const Json::Value& l_rName = l_rElement[ SVRC::vo::name ];
				const Json::Value& l_rArray = l_rElement[ SVRC::vo::array ];

				if( l_rName.isString() && !( l_rName.asString().empty() ) )
				{
					SVStorage l_Storage;

					l_Storage.m_StorageType = SVVisionProcessor::SVStorageValue;

					l_Status = SVJsonUtilities::ConvertJsonValueToVariant( l_rArray, l_Storage.m_Variant );

					if( l_Status == S_OK )
					{
						p_rItems[ l_rName.asCString() ] = l_Storage;
					}
				}
				else if( l_Status == S_OK )
				{
					l_Status = E_INVALIDARG;
				}
			}
			else if( l_Status == S_OK )
			{
				l_Status = E_INVALIDARG;
			}
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::AddJsonImagesToStorageItems( const Json::Value& p_rJsonImages, SVNameStorageMap& p_rItems )
{
	HRESULT l_Status = S_OK;

	if( p_rJsonImages.isArray() )
	{
		for( Json::Value::UInt i = 0; i < p_rJsonImages.size(); i++ )
		{
			const Json::Value& l_rElement = p_rJsonImages[ i ];

			if( l_rElement.isObject() )
			{
				const Json::Value& l_rName = l_rElement[ SVRC::io::name ];
				const Json::Value& l_rContents = l_rElement[ SVRC::io::image ];
				const Json::Value& l_rFileName = l_rElement[ SVRC::io::imageFileName ];

				if( l_rName.isString() && !( l_rName.asString().empty() ) )
				{
					std::string l_TempFileName;

					if( l_rContents.isString() && !( l_rContents.asString().empty() ) )
					{
						l_Status = ConvertJsonValueToFile( l_rContents, ( l_rName.asString() + ".bmp" ), l_TempFileName );
					}
					else if( l_rFileName.isString() && !( l_rFileName.asString().empty() ) )
					{
						_variant_t l_Variant;

						l_Status = SVJsonUtilities::ConvertJsonValueToVariant( l_rFileName, l_Variant.GetVARIANT() );

						SVString l_TempStr = l_Variant;

						if( !( l_TempStr.empty() ) )
						{
							l_TempFileName += "V:\\";
							l_TempFileName += l_TempStr.ToDataType();
						}
						else if( l_Status == S_OK )
						{
							l_Status = E_INVALIDARG;
						}
					}
					else if( l_Status == S_OK )
					{
						l_Status = E_INVALIDARG;
					}

					if( l_Status == S_OK )
					{
						SVStorage l_Storage;

						l_Storage.m_StorageType = SVVisionProcessor::SVStorageImageFileName;
						l_Storage.m_Variant = l_TempFileName.c_str();

						p_rItems[ l_rName.asCString() ] = l_Storage;
					}
				}
				else
				{
					l_Status = E_INVALIDARG;
				}
			}
			else if( l_Status == S_OK )
			{
				l_Status = E_INVALIDARG;
			}
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::CreateJsonValueWithResultErrors( const SVNameStorageMap& p_rItems, const SVNameStatusMap& rItemResultStatus, Json::Value& jsonResults )
{
	HRESULT l_Status = S_OK;

	if( jsonResults.isObject() )
	{
		Json::Value errorArray(Json::arrayValue);
		for( SVNameStorageMap::const_iterator l_ItemsIter = p_rItems.begin(); l_ItemsIter != p_rItems.end(); ++l_ItemsIter )
		{
			SVNameStatusMap::const_iterator l_StatusIter = rItemResultStatus.find( l_ItemsIter->first );

			if( ( l_StatusIter == rItemResultStatus.end() ) || ( l_StatusIter->second != S_OK ) )
			{
				l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;

				Json::Value l_Error( Json::objectValue );

				l_Error[ SVRC::error::name ] = l_ItemsIter->first.ToDataType();

				if( l_StatusIter == rItemResultStatus.end() )
				{
					l_Error[ SVRC::error::status ] = static_cast< HRESULT >( SVMSG_OBJECT_NOT_PROCESSED );
				}
				else
				{
					l_Error[ SVRC::error::status ] = l_StatusIter->second;
				}

				errorArray.append( l_Error );
			}
		}

		Json::Value jsonFaults(Json::objectValue);
		jsonFaults[ SVRC::result::errors ] = errorArray;
		jsonResults[ SVRC::result::faults ] = jsonFaults;
	}
	else
	{
		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::CreateJsonValueWithResultErrors( const SVNameStatusMap& rItemResultStatus, Json::Value& jsonResults )
{
	HRESULT hr = S_OK;

	if ( jsonResults.isObject() )
	{
		Json::Value errorArray(Json::arrayValue);
		for( SVNameStatusMap::const_iterator iter = rItemResultStatus.begin(); iter != rItemResultStatus.end(); ++iter )
		{
			if( S_OK != iter->second )
			{
				hr = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				Json::Value errorValue( Json::objectValue );
				errorValue[ SVRC::error::name ] = iter->first.c_str();
				errorValue[ SVRC::error::status ] = iter->second;
				errorArray.append( errorValue );
			}
		}

		Json::Value jsonFaults(Json::objectValue);
		jsonFaults[ SVRC::result::errors ] = errorArray;
		jsonResults[ SVRC::result::faults ] = jsonFaults;
	}
	else
	{
		hr = E_UNEXPECTED;
	}
	return hr;
}

HRESULT SVRemoteCommandFunctions::CommandNotFound( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_FALSE;

	std::string l_FileName = "C:\\temp\\CommandNotFound-cmd";
	WriteJsonCommandToFile(p_rJsonCommand, l_FileName);

	Json::Value l_Results(Json::objectValue);

	l_Status = E_FAIL;

	l_FileName = "C:\\temp\\CommandNotFound-rsp";
	WriteResultToJsonAndFile(p_rJsonCommand, p_rJsonResults, l_Results, l_FileName, l_Status);

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::GetDeviceMode( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	std::string l_FileName = "C:\\temp\\GetDeviceMode-cmd";
	WriteJsonCommandToFile(p_rJsonCommand, l_FileName);

	unsigned long l_Mode = SVIM_MODE_UNKNOWN;

	l_Status = SVVisionProcessorHelper::Instance().GetConfigurationMode( l_Mode );

	Json::Value l_Results(Json::objectValue);

	l_Results[ SVRC::result::mode ] = static_cast< Json::Value::UInt >( l_Mode );

	l_FileName = "C:\\temp\\GetDeviceMode-rsp";
	WriteResultToJsonAndFile(p_rJsonCommand, p_rJsonResults, l_Results, l_FileName, l_Status);

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::GetState( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	std::string l_FileName = "C:\\temp\\GetState-cmd";
	WriteJsonCommandToFile(p_rJsonCommand, l_FileName);

	unsigned long l_State = 0;

	l_Status = SVVisionProcessorHelper::Instance().GetState( l_State );

	Json::Value l_Results(Json::objectValue);

	l_Results[ SVRC::result::state ] = static_cast< Json::Value::UInt >( l_State );

	l_FileName = "C:\\temp\\GetState-rsp";
	WriteResultToJsonAndFile(p_rJsonCommand, p_rJsonResults, l_Results, l_FileName, l_Status);

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::GetVersions( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	std::string l_FileName = "C:\\temp\\GetVersions-cmd";
	WriteJsonCommandToFile(p_rJsonCommand, l_FileName);

	SVString l_Version;

	l_Status = SVVisionProcessorHelper::Instance().GetVersion( l_Version );

	Json::Value l_Results(Json::objectValue);

	l_Results[ SVRC::result::SVO_ver ] = l_Version.ToDataType();

	l_FileName = "C:\\temp\\GetVersions-rsp";
	WriteResultToJsonAndFile(p_rJsonCommand, p_rJsonResults, l_Results, l_FileName, l_Status);

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::GetDeviceOfflineCount( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	std::string l_FileName = "C:\\temp\\GetDeviceOfflineCount-cmd";
	WriteJsonCommandToFile(p_rJsonCommand, l_FileName);

	unsigned long l_OfflineCount;

	l_Status = SVVisionProcessorHelper::Instance().GetOfflineCount( l_OfflineCount );

	Json::Value l_Results(Json::objectValue);

	l_Results[ SVRC::result::count ] = static_cast< Json::Value::UInt >( l_OfflineCount );

	l_FileName = "C:\\temp\\GetDeviceOfflineCount-rsp";
	WriteResultToJsonAndFile(p_rJsonCommand, p_rJsonResults, l_Results, l_FileName, l_Status);

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::GetConfig( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	#ifdef SV_DATA_TO_CONTENTS
		std::string l_Contents;
	#else
		std::string l_DestinationFileName;
	#endif

	std::string l_RemoteFilePath;
	Json::Reader l_Reader;
	Json::Value l_JsonCmdValues;

	if( l_Reader.parse( p_rJsonCommand, l_JsonCmdValues, false ) )
	{
		std::string l_FileName = "C:\\temp\\GetConfig-cmd";
		WriteJsonCommandToFile(l_JsonCmdValues, l_FileName);

		Json::Value l_JsonArguments;

		if( l_JsonCmdValues.isObject() )
		{
			l_JsonArguments = l_JsonCmdValues[ SVRC::cmd::arg ];
		}

		if( l_JsonArguments.isObject() )
		{
			Json::Value l_JsonFilePath;

			l_JsonFilePath = l_JsonArguments[ SVRC::arg::filePath ];

			if( l_JsonFilePath.isString() )
			{
				l_RemoteFilePath = l_JsonFilePath.asString();
			}
		}
	}

	if( l_RemoteFilePath.empty() )
	{
		l_RemoteFilePath = "Configuration.svp";
	}
	std::string errText;
	std::string l_TempFileName;

	l_Status = GetTempFileNameFromFilePath( l_TempFileName, l_RemoteFilePath );

	if( !( l_TempFileName.empty() ) )
	{
		try
		{
			l_Status = SVVisionProcessorHelper::Instance().SaveConfiguration( l_TempFileName );
		}
		catch (SVException& e)
		{
			l_Status = e.GetErrorCode();
			errText = e.what().c_str();
		}
	}
	else if (S_OK == l_Status)
	{
		l_Status = E_UNEXPECTED;
	}

	if (S_OK == l_Status)
	{
		#ifdef SV_DATA_TO_CONTENTS
			l_Status = SVEncodeDecodeUtilities::Base64EncodeToStringFromFile( l_Contents, l_TempFileName );

		#else
			l_Status = GetFileNameFromFilePath( l_DestinationFileName, l_TempFileName );
		#endif
	}
	// cleanup
	::remove( l_TempFileName.c_str() );

	Json::Value l_Results(Json::objectValue);

	if (S_OK == l_Status)
	{
		l_Results[ SVRC::result::filePath ] = l_RemoteFilePath;

		#ifdef SV_DATA_TO_CONTENTS
			l_Results[ SVRC::result::contents ] = l_Contents;
		#else
			l_Results[ SVRC::result::destinationFileName ] = l_DestinationFileName;
		#endif
		std::string l_FileName = "C:\\temp\\GetConfig-rsp";
		WriteResultToJsonAndFile(p_rJsonCommand, p_rJsonResults, l_Results, l_FileName, l_Status);
	}
	else
	{
		// Log Exception (in case no one else did...)
		SVException e;
		SETEXCEPTION4(e, l_Status, -20202);
		if (errText.empty())
		{
			errText = e.what().c_str();
		}
		BuildErrorResponse(p_rJsonCommand, p_rJsonResults, l_Status, errText);
	}
	return l_Status;
}

HRESULT SVRemoteCommandFunctions::GetItems( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	SVNameSet l_Names;
	SVNameStorageResultMap l_Items;

	Json::Reader l_Reader;
	Json::Value l_JsonCmdValues;

	if( l_Reader.parse( p_rJsonCommand, l_JsonCmdValues, false ) )
	{
		std::string l_FileName = "C:\\temp\\GetItems-cmd";
		WriteJsonCommandToFile(l_JsonCmdValues, l_FileName);

		Json::Value l_JsonArguments;

		if( l_JsonCmdValues.isObject() )
		{
			l_JsonArguments = l_JsonCmdValues[ SVRC::cmd::arg ];
		}

		if( l_JsonArguments.isObject() )
		{
			Json::Value l_JsonItemNames;

			l_JsonItemNames = l_JsonArguments[ SVRC::arg::item_names ];

			l_Status = SVJsonUtilities::ConvertJsonArrayToStringSet( l_JsonItemNames, l_Names );
		}
	}

	Json::Value l_Results(Json::objectValue);

	if( !( l_Names.empty() ) )
	{
		l_Status = SVVisionProcessorHelper::Instance().GetItems( l_Names, l_Items );

		Json::Value l_ValueArray(Json::arrayValue);
		Json::Value l_ImageArray(Json::arrayValue);
		Json::Value l_ErrorArray(Json::arrayValue);

		for( SVNameStorageResultMap::const_iterator l_Iter = l_Items.begin(); l_Iter != l_Items.end(); ++l_Iter )
		{
			HRESULT l_LoopStatus = S_OK;

			switch( l_Iter->second.m_Storage.m_StorageType )
			{
			case SVVisionProcessor::SVStorageValue:
				{
					l_LoopStatus = l_Iter->second.m_Status;

					Json::Value l_Element(Json::objectValue);

					if( l_LoopStatus == S_OK )
					{
						l_LoopStatus = ConvertStorageResultValueToJsonValue( l_Iter->first, l_Iter->second, l_Element );

						if( l_LoopStatus == S_OK )
						{
							l_ValueArray.append( l_Element );
						}
					}

					if( l_LoopStatus != S_OK )
					{
						l_Element[SVRC::error::name] = l_Iter->first.c_str();
						l_Element[SVRC::error::status] = l_LoopStatus;

						l_ErrorArray.append( l_Element );

						if( l_Status == S_OK )
						{
							l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
						}
					}

					break;
				}
			case SVVisionProcessor::SVStorageImageFileName:
				{
					l_LoopStatus = l_Iter->second.m_Status;

					Json::Value l_Element(Json::objectValue);

					if( l_LoopStatus == S_OK )
					{
						l_LoopStatus = ConvertStorageResultImageFileToJsonValue( l_Iter->first, l_Iter->second, l_Element );

						if( l_LoopStatus == S_OK )
						{
							l_ImageArray.append( l_Element );
						}
					}

					if( l_LoopStatus != S_OK )
					{
						SVString l_FileName = l_Iter->second.m_Storage.m_Variant;

						if( !( l_FileName.empty() ) )
						{
							::remove( l_FileName.c_str() );
						}

						l_Element[SVRC::error::name] = l_Iter->first.c_str();
						l_Element[SVRC::error::status] = l_LoopStatus;

						l_ErrorArray.append( l_Element );

						if( l_Status == S_OK )
						{
							l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
						}
					}

					break;
				}
			default:
				{
					Json::Value l_Element(Json::objectValue);

					l_LoopStatus = l_Iter->second.m_Status;

					if( l_LoopStatus == S_OK )
					{
						l_LoopStatus = E_INVALIDARG;
					}

					l_Element[SVRC::error::name] = l_Iter->first.c_str();
					l_Element[SVRC::error::status] = l_LoopStatus;

					l_ErrorArray.append( l_Element );

					if( l_Status == S_OK )
					{
						l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
					}

					break;
				}
			}
		}

		Json::Value l_JsonItems(Json::objectValue);

		l_JsonItems[ SVRC::result::values ] = l_ValueArray;
		l_JsonItems[ SVRC::result::images ] = l_ImageArray;
		l_JsonItems[ SVRC::result::errors ] = l_ErrorArray;

		l_Results[ SVRC::result::items ] = l_JsonItems;
	}
	else if( l_Status == S_OK )
	{
		l_Status = E_INVALIDARG;
	}

	std::string l_FileName = "C:\\temp\\GetItems-rsp";
	WriteResultToJsonAndFile(p_rJsonCommand, p_rJsonResults, l_Results, l_FileName, l_Status);

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::GetDeviceFile( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	std::string l_DestinationPath;

	#ifdef SV_DATA_TO_CONTENTS
		std::string l_Contents;
	#else
		std::string l_DestinationFileName;
	#endif

	Json::Reader l_Reader;
	Json::Value l_JsonCmdValues;

	if( l_Reader.parse( p_rJsonCommand, l_JsonCmdValues, false ) )
	{
		std::string l_FileName = "C:\\temp\\GetDeviceFile-cmd";
		WriteJsonCommandToFile(l_JsonCmdValues, l_FileName);

		Json::Value l_JsonArguments;

		if( l_JsonCmdValues.isObject() )
		{
			l_JsonArguments = l_JsonCmdValues[ SVRC::cmd::arg ];
		}

		if( l_JsonArguments.isObject() )
		{
			std::string l_SourcePath;
			Json::Value l_JsonSourcePath;
			Json::Value l_JsonDestinationPath;

			l_JsonSourcePath = l_JsonArguments[ SVRC::arg::sourcePath ];

			if( l_JsonSourcePath.isString() )
			{
				l_SourcePath = l_JsonSourcePath.asString();
			}

			l_JsonDestinationPath = l_JsonArguments[ SVRC::arg::destinationPath ];

			if( l_JsonDestinationPath.isString() )
			{
				l_DestinationPath = l_JsonDestinationPath.asString();
			}

			if( !( l_SourcePath.empty() ) )
			{
				#ifdef SV_DATA_TO_CONTENTS
					l_Status = SVEncodeDecodeUtilities::Base64EncodeToStringFromFile( l_Contents, l_SourcePath );
				#else
					std::string l_TempFileName;

					l_Status = GetTempFileNameFromFilePath( l_TempFileName, l_SourcePath );

					if( !( l_TempFileName.empty() ) )
					{
						if( ::MoveFile( l_SourcePath.c_str(), l_TempFileName.c_str() ) )
						{
							l_Status = GetFileNameFromFilePath( l_DestinationFileName, l_TempFileName );
						}
						else
						{
							l_Status = E_UNEXPECTED;
						}
					}
					else if( l_Status == S_OK )
					{
						l_Status = E_UNEXPECTED;
					}
				#endif
			}
			else
			{
				l_Status = E_INVALIDARG;
			}
		}
	}

	Json::Value l_Results(Json::objectValue);

	if( l_Status == S_OK )
	{
		l_Results[ SVRC::result::destinationPath ] = l_DestinationPath;

		#ifdef SV_DATA_TO_CONTENTS
			l_Results[ SVRC::result::contents ] = l_Contents;
		#else
			l_Results[ SVRC::result::destinationFileName ] = l_DestinationFileName;
		#endif
	}

	std::string l_FileName = "C:\\temp\\GetDeviceFile-rsp";
	WriteResultToJsonAndFile(p_rJsonCommand, p_rJsonResults, l_Results, l_FileName, l_Status);

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::GetDeviceConfigReport( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	std::string l_FileName = "C:\\temp\\GetDeviceConfigReport-cmd";
	WriteJsonCommandToFile(p_rJsonCommand, l_FileName);

	SVString l_Report;

	l_Status = SVVisionProcessorHelper::Instance().GetConfigurationPrintReport( l_Report );

	Json::Value l_Results(Json::objectValue);

	if( l_Status == S_OK )
	{
		l_Results[ SVRC::result::report ] = l_Report.ToDataType();
	}

	l_FileName = "C:\\temp\\GetDeviceConfigReport-rsp";
	WriteResultToJsonAndFile(p_rJsonCommand, p_rJsonResults, l_Results, l_FileName, l_Status);

	return l_Status;
}

/*####################################################################################################################################
*	GetDataDefinitionList				Gets the list of data definitions for Value objects or, and Images
*	Return: 							The status result of the method  
*
*	p_rJsonCommand						The original Json command in string format
*	p_rJsonResults						The string in which the Json results is saved in
*#####################################################################################################################################
*/
HRESULT SVRemoteCommandFunctions::GetDataDefinitionList( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	std::string l_InspectionName;
	SVDataDefinitionListType l_DataDefinitionType = UnknownDataDefinition;
	Json::Reader l_Reader;
	Json::Value l_JsonCmdValues;

	if( l_Reader.parse( p_rJsonCommand, l_JsonCmdValues, false ) )
	{
		std::string l_FileName = "C:\\temp\\GetDataDefinitionList-cmd";
		WriteJsonCommandToFile(l_JsonCmdValues, l_FileName);

		Json::Value l_JsonArguments;

		if( l_JsonCmdValues.isObject() )
		{
			l_JsonArguments = l_JsonCmdValues[ SVRC::cmd::arg ];
		}

		if( l_JsonArguments.isObject() )
		{
			Json::Value l_JsonInspectionName;
			Json::Value l_JsonListType;

			l_JsonInspectionName = l_JsonArguments[ SVRC::arg::inspectionName ];

			if( l_JsonInspectionName.isString() )
			{
				l_InspectionName = l_JsonInspectionName.asString();
			}

			l_JsonListType = l_JsonArguments[ SVRC::arg::listType ];

			if( l_JsonListType.isInt() )
			{
				l_DataDefinitionType = (SVDataDefinitionListType) l_JsonListType.asInt();
			}
			else if( l_JsonListType.isUInt() )
			{
				l_DataDefinitionType = (SVDataDefinitionListType) l_JsonListType.asUInt();
			}
			else
			{
				l_Status = E_INVALIDARG;
			}
		}
	}

	if( UnknownDataDefinition == l_DataDefinitionType)
	{
		l_Status = E_INVALIDARG;
	}
	

	SVString l_Report;
	SVDataDefinitionStructArray l_DataDefinitionArray;

	if( l_Status == S_OK)
	{
		l_Status = SVVisionProcessorHelper::Instance().GetDataDefinitionList(l_InspectionName,  l_DataDefinitionType, l_DataDefinitionArray);
	}

	Json::Value l_Results(Json::objectValue);
	Json::Value l_EntryArray(Json::arrayValue);

	for( SVDataDefinitionStructArray::const_iterator l_Iter = l_DataDefinitionArray.begin(); l_Iter != l_DataDefinitionArray.end(); ++l_Iter )
	{
		HRESULT l_LoopStatus = S_OK;
		Json::Value l_Element(Json::objectValue);
		l_LoopStatus = ConvertDataDefinitionToJsonValue(*l_Iter, l_Element);
		l_EntryArray.append(l_Element);
	}

	if( l_Status == S_OK )
	{
		Json::Value l_JsonItems(Json::objectValue);

		l_JsonItems[ SVRC::result::entries ] = l_EntryArray;
		l_Results[ SVRC::result::items ] = l_JsonItems;
	}

	std::string l_FileName = "C:\\temp\\GetDataDefinitionList-rsp";
	WriteResultToJsonAndFile(p_rJsonCommand, p_rJsonResults, l_Results, l_FileName, l_Status);

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::PutConfig( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	std::string l_SourceFileName;
	Json::Reader l_Reader;
	Json::Value l_JsonCmdValues;

	if( l_Reader.parse( p_rJsonCommand, l_JsonCmdValues, false ) )
	{
		std::string l_FileName = "C:\\temp\\PutConfig-cmd";
		WriteJsonCommandToFile(l_JsonCmdValues, l_FileName);

		Json::Value l_JsonArguments;

		if( l_JsonCmdValues.isObject() )
		{
			l_JsonArguments = l_JsonCmdValues[ SVRC::cmd::arg ];
		}

		if( l_JsonArguments.isObject() )
		{
			std::string l_DestinationPath;
			Json::Value l_JsonContents = l_JsonArguments[ SVRC::arg::contents ];

			if( l_JsonContents.isString() )
			{
				std::string l_RemoteFilePath;
				std::string l_TempFileName;

				Json::Value l_JsonFilePath;

				l_JsonFilePath = l_JsonArguments[ SVRC::arg::filePath ];

				if( l_JsonFilePath.isString() )
				{
					l_RemoteFilePath = l_JsonFilePath.asString();
				}

				if( l_RemoteFilePath.empty() )
				{
					l_RemoteFilePath = "Configuration.svp";
				}

				l_Status = GetTempFileNameFromFilePath( l_TempFileName, l_RemoteFilePath );

				if (S_OK == l_Status)
				{
					l_Status = SVEncodeDecodeUtilities::Base64DecodeToFileFromString( l_TempFileName, l_JsonContents.asString() );
				
					if (S_OK == l_Status)
					{
						l_SourceFileName = l_TempFileName;
					}
					else
					{
						::remove(l_TempFileName.c_str());
					}
				}
			}
			else
			{
				Json::Value l_JsonSourceFileName;

				l_JsonSourceFileName = l_JsonArguments[ SVRC::arg::sourceFileName ];

				if( l_JsonSourceFileName.isString() && !( l_JsonSourceFileName.asString().empty() ) )
				{
					l_SourceFileName = "V:\\";
					l_SourceFileName += l_JsonSourceFileName.asString();
				}
				else
				{
					l_SourceFileName.clear();
					l_Status = E_INVALIDARG;
				}
			}
		}
	}

	std::string errText;
	if( !( l_SourceFileName.empty() ) )
	{
		try
		{
			l_Status = SVVisionProcessorHelper::Instance().LoadConfiguration( l_SourceFileName );
		}
		catch (SVException& e)
		{
			l_Status = e.GetErrorCode();
			errText = e.what().c_str();
		}
		::remove(l_SourceFileName.c_str());
	}
	else if (S_OK == l_Status)
	{
		l_Status = E_INVALIDARG;
	}

	Json::Value l_Results(Json::objectValue);

	if (S_OK == l_Status)
	{
		std::string l_FileName = "C:\\temp\\PutConfig-rsp";
		WriteResultToJsonAndFile(p_rJsonCommand, p_rJsonResults, l_Results, l_FileName, l_Status);
	}
	else
	{
		// Log Exception (in case no one else did...)
		SVException e;
		SETEXCEPTION4(e, l_Status, -10101);
		if (errText.empty())
		{
			errText = e.what().c_str();
		}
		if (!FAILED(l_Status))
		{
			l_Status = HRESULT_FROM_WIN32(l_Status);
		}
		BuildErrorResponse(p_rJsonCommand, p_rJsonResults, l_Status, errText);
	}
	return l_Status;
}

HRESULT SVRemoteCommandFunctions::PutDeviceFile( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;
	
	enum AccessMode
	{
		Exists = 0, // Existence only
		WriteOnly = 2, // Write-only
		ReadOnly = 4, // Read-only
		ReadWrite = 6 //Read and write
	};

	Json::Reader l_Reader;
	Json::Value l_JsonCmdValues;

	if( l_Reader.parse( p_rJsonCommand, l_JsonCmdValues, false ) )
	{
		std::string l_FileName = "C:\\temp\\PutDeviceFile-cmd";
		WriteJsonCommandToFile(l_JsonCmdValues, l_FileName);

		Json::Value l_JsonArguments;

		if( l_JsonCmdValues.isObject() )
		{
			l_JsonArguments = l_JsonCmdValues[ SVRC::cmd::arg ];
		}

		if( l_JsonArguments.isObject() )
		{
			std::string l_DestinationPath;
			Json::Value l_JsonDestPath;
			Json::Value l_JsonContents;

			l_JsonDestPath = l_JsonArguments[ SVRC::arg::destinationPath ];
			l_JsonContents = l_JsonArguments[ SVRC::arg::contents ];

			if( l_JsonDestPath.isString() )
			{
				l_DestinationPath = l_JsonDestPath.asString();
			}

			if( !( l_DestinationPath.empty() ) )
			{
				std::string l_SourceFileName;
					
				if( l_JsonContents.isString() )
				{
					std::string l_TempFileName;

					l_Status = GetTempFileNameFromFilePath( l_TempFileName, l_DestinationPath );

					if( l_Status == S_OK )
					{
						l_SourceFileName = l_TempFileName;

						l_Status = SVEncodeDecodeUtilities::Base64DecodeToFileFromString( l_SourceFileName, l_JsonContents.asString() );
					}

					if( l_Status != S_OK )
					{
						l_SourceFileName.clear();
					}
				}
				else
				{
					Json::Value l_JsonSourceFileName;

					l_JsonSourceFileName = l_JsonArguments[ SVRC::arg::sourceFileName ];

					if( l_JsonSourceFileName.isString() && !( l_JsonSourceFileName.asString().empty() ) )
					{
						l_SourceFileName = "V:\\";
						l_SourceFileName += l_JsonSourceFileName.asString();
					}
					else
					{
						l_Status = E_INVALIDARG;
					}
				}

				if( !( l_SourceFileName.empty() ) )
				{
					DWORD flags = MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH | MOVEFILE_COPY_ALLOWED;
					if( !::MoveFileEx( l_SourceFileName.c_str(), l_DestinationPath.c_str(), flags ) )
					{
						l_Status = E_UNEXPECTED;
						// check if read only
						if (::PathFileExists(l_DestinationPath.c_str()) && 0 == _access(l_DestinationPath.c_str(), ReadOnly))
						{
							l_Status = E_ACCESSDENIED;
						}
					}
				}
				else if( l_Status == S_OK )
				{
					l_Status = E_INVALIDARG;
				}
			}
			else
			{
				l_Status = E_INVALIDARG;
			}
		}
	}

	Json::Value l_Results(Json::objectValue);

	std::string l_FileName = "C:\\temp\\PutDeviceFile-rsp";
	WriteResultToJsonAndFile(p_rJsonCommand, p_rJsonResults, l_Results, l_FileName, l_Status);

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::SetDeviceMode( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	unsigned long l_Mode = SVIM_MODE_UNKNOWN;

	Json::Reader l_Reader;
	Json::Value l_JsonCmdValues;

	if( l_Reader.parse( p_rJsonCommand, l_JsonCmdValues, false ) )
	{
		std::string l_FileName = "C:\\temp\\SetDeviceMode-cmd";
		WriteJsonCommandToFile(l_JsonCmdValues, l_FileName);

		Json::Value l_JsonArguments;

		if( l_JsonCmdValues.isObject() )
		{
			l_JsonArguments = l_JsonCmdValues[ SVRC::cmd::arg ];
		}

		if( l_JsonArguments.isObject() )
		{
			Json::Value l_JsonMode;

			l_JsonMode = l_JsonArguments[ SVRC::arg::desiredMode ];

			if( l_JsonMode.isInt() )
			{
				l_Mode = l_JsonMode.asInt();
			}
			else if( l_JsonMode.isUInt() )
			{
				l_Mode = l_JsonMode.asUInt();
			}
			else
			{
				l_Status = E_INVALIDARG;
			}
		}
	}

	if( l_Mode != SVIM_MODE_UNKNOWN )
	{
		l_Status = SVVisionProcessorHelper::Instance().SetConfigurationMode( l_Mode );
	}
	else if( l_Status == S_OK )
	{
		l_Status = E_INVALIDARG;
	}

	Json::Value l_Results(Json::objectValue);

	std::string l_FileName = "C:\\temp\\SetDeviceMode-rsp";
	WriteResultToJsonAndFile(p_rJsonCommand, p_rJsonResults, l_Results, l_FileName, l_Status);

	return l_Status;
}

//HRESULT SVRemoteCommandFunctions::RegisterProductList( const std::string& command, std::string& jsonResults )
//{
//	HRESULT hr = S_OK;
//	Json::Reader reader;
//	Json::Value jsonCmd;
//	Json::Value result(Json::objectValue);
//	try
//	{
//		if( reader.parse( command, jsonCmd, false ) )
//		{
//			SVNameStatusMap errors;
//			WriteJsonCommandToFile(jsonCmd, "C:\\temp\\RegisterProductList-cmd");
//			hr = SVVisionProcessorHelper::Instance().RegisterProductList(ExtractRPLParams(jsonCmd), errors);
//			result[ SVRC::result::faults] = (Errors2Json(errors));
//		}
//		else
//		{
//			hr = E_INVALIDARG;
//		}
//	}
//	catch (ATL::CAtlException & ex)
//	{
//		hr = ex;
//	}
//
//	WriteResultToJsonAndFile(command, jsonResults, result, "C:\\temp\\RegisterProductList-rsp", hr);
//	return hr;
//}

HRESULT SVRemoteCommandFunctions::Shutdown( const std::string& command, std::string& jsonResults )
{
	HRESULT hr = S_OK;
	Json::Value result(Json::objectValue);
	try
	{
		long shutdownOption = -1;
		Json::Reader reader;
		Json::Value cmdValues;
		Json::Value jsonArguments;
		if( reader.parse( command, cmdValues, false ) )
		if( cmdValues.isObject() )
		{
			jsonArguments = cmdValues[ SVRC::cmd::arg ];
		}

		if( jsonArguments.isObject() )
		{
			Json::Value jsonOptions;

			jsonOptions = jsonArguments[ SVRC::arg::options ];

			if( jsonOptions.isInt() )
			{
				shutdownOption = jsonOptions.asInt();
			}
		}
		//check if shutdown.exe is exist. If not, the shutdown won't done.
		std::ifstream dllfile("SVShutdown.exe", std::ios::binary);
		CWnd* pWnd = pWnd = AfxGetApp()->m_pMainWnd;
		if ( 0 > shutdownOption && !dllfile && nullptr != pWnd)
		{
			hr = S_FALSE;
		} 
		else		
		{
			PostMessage(pWnd->m_hWnd, SV_SHUTDOWN, shutdownOption, 0 );
		}		
	}
	catch (ATL::CAtlException & ex)
	{
		hr = ex;
	}

	result[ SVRC::result::state ] = hr;
	std::string fileName = "C:\\temp\\Shutdown-rsp";
	WriteResultToJsonAndFile(command, jsonResults, result, fileName, hr);

	return hr;
}

HRESULT SVRemoteCommandFunctions::SetItems( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	SVNameStorageMap l_Items;
	SVNameStatusMap l_ResultStatus;

	Json::Reader l_Reader;
	Json::Value l_JsonCmdValues;

	if( l_Reader.parse( p_rJsonCommand, l_JsonCmdValues, false ) )
	{
		std::string l_FileName = "C:\\temp\\SetItems-cmd";
		WriteJsonCommandToFile(l_JsonCmdValues, l_FileName);

		Json::Value l_JsonArguments;
		Json::Value l_JsonItems;

		if( l_JsonCmdValues.isObject() )
		{
			l_JsonArguments = l_JsonCmdValues[ SVRC::cmd::arg ];
		}

		if( l_JsonArguments.isObject() )
		{
			l_JsonItems = l_JsonArguments[ SVRC::arg::item_names ];
		}

		if( l_JsonItems.isObject() )
		{
			Json::Value l_JsonValues;
			Json::Value l_JsonImages;

			l_JsonValues = l_JsonItems[ SVRC::arg::values ];
			l_JsonImages = l_JsonItems[ SVRC::arg::images ];

			if( l_JsonValues.isArray() && l_JsonImages.isArray() )
			{
				HRESULT l_TempStatus = AddJsonValuesToStorageItems( l_JsonValues, l_Items );

				if( l_Status == S_OK )
				{
					l_Status = l_TempStatus;
				}

				l_TempStatus = AddJsonImagesToStorageItems( l_JsonImages, l_Items );

				if( l_Status == S_OK )
				{
					l_Status = l_TempStatus;
				}
			}
			else
			{
				l_Status = E_INVALIDARG;
			}
		}
	}

	if( l_Status == S_OK )
	{
		if( !( l_Items.empty() ) )
		{
			l_Status = SVVisionProcessorHelper::Instance().SetItems( l_Items, l_ResultStatus );
		}
		else
		{
			l_Status = E_INVALIDARG;
		}
	}

	Json::Value jsonResults(Json::objectValue);

	HRESULT l_TempStatus = CreateJsonValueWithResultErrors( l_Items, l_ResultStatus, jsonResults );

	if( l_Status == S_OK )
	{
		l_Status = l_TempStatus;
	}

	std::string l_FileName = "C:\\temp\\SetItems-rsp";
	WriteResultToJsonAndFile(p_rJsonCommand, p_rJsonResults, jsonResults, l_FileName, l_Status);

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::ActivateMonitorList( const std::string& rJsonCommand, std::string& rJsonResults )
{
	HRESULT hr = S_OK;

	std::string ListName;
	bool bActive = false;
	Json::Reader Reader;
	Json::Value JsonCmdValues;

	if( Reader.parse( rJsonCommand, JsonCmdValues, false ) )
	{
		std::string FileName = "C:\\temp\\ActivateMonitorList-cmd";
		WriteJsonCommandToFile(JsonCmdValues, FileName);

		Json::Value JsonArguments;

		if( JsonCmdValues.isObject() )
		{
			JsonArguments = JsonCmdValues[ SVRC::cmd::arg ];
		}

		if( JsonArguments.isObject() )
		{
			Json::Value JsonListName = JsonArguments[ SVRC::arg::listName ];

			if( JsonListName.isString() )
			{
				ListName = JsonListName.asString();
			}
			else
			{
				hr = E_INVALIDARG;
			}
			Json::Value JsonActive = JsonArguments[ SVRC::arg::active ];
			if (JsonActive.isBool())
			{
				bActive = JsonActive.asBool();
			}
			else
			{
				hr = E_INVALIDARG;
			}
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}

	if( S_OK == hr )
	{
		if( !( ListName.empty() ) )
		{
			hr = SVVisionProcessorHelper::Instance().ActivateMonitorList( ListName, bActive );
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}

	Json::Value Results(Json::objectValue);

	std::string FileName = "C:\\temp\\ActivateMonitorList-rsp";
	WriteResultToJsonAndFile(rJsonCommand, rJsonResults, Results, FileName, hr);
	
	return hr;
}

HRESULT SVRemoteCommandFunctions::RegisterMonitorList( const std::string& rJsonCommand, std::string& rJsonResults )
{
	HRESULT hr = S_OK;
	std::string listName;
	std::string ppqName;
	SVNameSet prodList;
	SVNameSet rejectCondList;
	SVNameSet failStatusList;
	int rejectDepth = 0;
	Json::Reader reader;
	Json::Value jsonCmdValues;
	SVNameStatusMap itemErrorMap;

	if( reader.parse( rJsonCommand, jsonCmdValues, false ) )
	{
		std::string fileName = "C:\\temp\\RegisterMonitorList-cmd";
		WriteJsonCommandToFile(jsonCmdValues, fileName);

		Json::Value jsonArguments;
		if( jsonCmdValues.isObject() )
		{
			jsonArguments = jsonCmdValues[ SVRC::cmd::arg ];
		}

		if( jsonArguments.isObject() )
		{
			Json::Value jsonArg = jsonArguments[ SVRC::arg::listName ];
			if( jsonArg.isString() )
			{
				listName = jsonArg.asString();
			}
			else
			{
				hr = E_INVALIDARG;
			}

			jsonArg = jsonArguments[ SVRC::arg::ppqName ];
			if( jsonArg.isString() )
			{
				ppqName = jsonArg.asString();
			}
			else
			{
				hr = E_INVALIDARG;
			}

			jsonArg = jsonArguments[ SVRC::arg::rejectDepth ];
			if( jsonArg.isInt() )
			{
				rejectDepth = jsonArg.asInt();
				if (MinRejectQueueDepth > rejectDepth && MaxRejectQueueDepth < rejectDepth)
				{
					hr = E_INVALIDARG;
				}
			}
			else
			{
				hr = E_INVALIDARG;
			}

			if ( S_OK == hr )
			{
				jsonArg = jsonArguments[ SVRC::arg::prodList ];
				hr = SVJsonUtilities::ConvertJsonArrayToStringSet( jsonArg, prodList );
			}

			if ( S_OK == hr )
			{
				jsonArg = jsonArguments[ SVRC::arg::rjctList ];
				hr = SVJsonUtilities::ConvertJsonArrayToStringSet( jsonArg, rejectCondList );
			}

			jsonArg = jsonArguments[ SVRC::arg::failList ];
			if ( S_OK == hr )
			{
				jsonArg = jsonArguments[ SVRC::arg::failList ];
				hr = SVJsonUtilities::ConvertJsonArrayToStringSet( jsonArg, failStatusList );
			}

			if ( S_OK == hr )
			{
				hr = SVVisionProcessorHelper::Instance().RegisterMonitorList(listName, ppqName, rejectDepth, prodList, rejectCondList, failStatusList, itemErrorMap);
			}
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}

	Json::Value results(Json::objectValue);
	HRESULT tmp_hr = CreateJsonValueWithResultErrors( itemErrorMap, results );
	if (S_OK == hr)
	{
		hr = tmp_hr;
	}

	std::string fileName = "C:\\temp\\RegisterMonitorList-rsp";
	WriteResultToJsonAndFile(rJsonCommand, rJsonResults, results, fileName, hr);
	return hr;
}

HRESULT SVRemoteCommandFunctions::QueryProductList( const std::string& rJsonCommand, std::string& rJsonResults )
{
	HRESULT hr = S_OK;

	std::string ListName;
	Json::Reader Reader;
	Json::Value JsonCmdValues;

	if( Reader.parse( rJsonCommand, JsonCmdValues, false ) )
	{
		std::string FileName = "C:\\temp\\QueryProductList-cmd";
		WriteJsonCommandToFile(JsonCmdValues, FileName);

		Json::Value JsonArguments;

		if( JsonCmdValues.isObject() )
		{
			JsonArguments = JsonCmdValues[ SVRC::cmd::arg ];
		}

		if( JsonArguments.isObject() )
		{
			Json::Value JsonListName = JsonArguments[ SVRC::arg::listName ];

			if( JsonListName.isString() )
			{
				ListName = JsonListName.asString();
			}
			else
			{
				hr = E_INVALIDARG;
			}
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}

	Json::Value Results(Json::objectValue);
	Json::Value EntryArray(Json::arrayValue);

	if( S_OK == hr )
	{
		SVNameSet Items;
		hr = SVVisionProcessorHelper::Instance().QueryProductList(ListName, Items);
		if( S_OK == hr )
		{
			for( SVNameSet::const_iterator it = Items.begin();it != Items.end();++it)
			{
				Json::Value Element(Json::objectValue);
				Element = (*it).c_str();
				EntryArray.append(Element);
			}
		}
	}
	// need to set the results even on an error...
	Results[ SVRC::result::names ] = EntryArray;
	std::string FileName = "C:\\temp\\QueryProductList-rsp";
	WriteResultToJsonAndFile(rJsonCommand, rJsonResults, Results, FileName, hr);

	return hr;
}

HRESULT SVRemoteCommandFunctions::QueryRejectCondList( const std::string& rJsonCommand, std::string& rJsonResults )
{
	HRESULT hr = S_OK;

	std::string ListName;
	Json::Reader Reader;
	Json::Value JsonCmdValues;

	if( Reader.parse( rJsonCommand, JsonCmdValues, false ) )
	{
		std::string FileName = "C:\\temp\\QueryRejectCondList-cmd";
		WriteJsonCommandToFile(JsonCmdValues, FileName);

		Json::Value JsonArguments;

		if( JsonCmdValues.isObject() )
		{
			JsonArguments = JsonCmdValues[ SVRC::cmd::arg ];
		}

		if( JsonArguments.isObject() )
		{
			Json::Value JsonListName = JsonArguments[ SVRC::arg::listName ];

			if( JsonListName.isString() )
			{
				ListName = JsonListName.asString();
			}
			else
			{
				hr = E_INVALIDARG;
			}
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}

	Json::Value Results(Json::objectValue);
	Json::Value EntryArray(Json::arrayValue);

	if( S_OK == hr )
	{
		SVNameSet Items;
		hr = SVVisionProcessorHelper::Instance().QueryRejectCondList(ListName, Items);
		if( S_OK == hr )
		{
			for( SVNameSet::const_iterator it = Items.begin();it != Items.end();++it)
			{
				Json::Value Element(Json::objectValue);
				Element = (*it).c_str();
				EntryArray.append(Element);
			}
		}
	}
	// need to set the results even on an error...
	Results[ SVRC::result::names ] = EntryArray;
	std::string FileName = "C:\\temp\\QueryRejectCondList-rsp";
	WriteResultToJsonAndFile(rJsonCommand, rJsonResults, Results, FileName, hr);

	return hr;
}

HRESULT SVRemoteCommandFunctions::QueryFailStatusList( const std::string& rJsonCommand, std::string& rJsonResults )
{
	HRESULT hr = S_OK;

	std::string ListName;
	Json::Reader Reader;
	Json::Value JsonCmdValues;

	if( Reader.parse( rJsonCommand, JsonCmdValues, false ) )
	{
		std::string FileName = "C:\\temp\\QueryFailStatusList-cmd";
		WriteJsonCommandToFile(JsonCmdValues, FileName);

		Json::Value JsonArguments;

		if( JsonCmdValues.isObject() )
		{
			JsonArguments = JsonCmdValues[ SVRC::cmd::arg ];
		}

		if( JsonArguments.isObject() )
		{
			Json::Value JsonListName = JsonArguments[ SVRC::arg::listName ];

			if( JsonListName.isString() )
			{
				ListName = JsonListName.asString();
			}
			else
			{
				hr = E_INVALIDARG;
			}
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}

	Json::Value Results(Json::objectValue);
	Json::Value EntryArray(Json::arrayValue);

	if( S_OK == hr )
	{
		SVNameSet Items;
		hr = SVVisionProcessorHelper::Instance().QueryFailStatusList(ListName, Items);
		if( S_OK == hr )
		{
			for( SVNameSet::const_iterator it = Items.begin();it != Items.end();++it)
			{
				Json::Value Element(Json::objectValue);
				Element = (*it).c_str();
				EntryArray.append(Element);
			}
		}
	}
	// need to set the results even on an error...
	Results[ SVRC::result::names ] = EntryArray;
	std::string FileName = "C:\\temp\\QueryFailStatusList-rsp";
	WriteResultToJsonAndFile(rJsonCommand, rJsonResults, Results, FileName, hr);

	return hr;
}

HRESULT SVRemoteCommandFunctions::QueryMonitorListNames( const std::string& rJsonCommand, std::string& rJsonResults )
{
	Json::Value EntryArray(Json::arrayValue);
	SVNameSet names;

	HRESULT hr = SVVisionProcessorHelper::Instance().QueryMonitorListNames(names);
	if( S_OK == hr )
	{
		for( SVNameSet::const_iterator it = names.begin();it != names.end();++it)
		{
			Json::Value Element(Json::objectValue);
			Element = (*it).c_str();
			EntryArray.append(Element);
		}
	}

	// need to set the results even on an error...
	Json::Value Results(Json::objectValue);
	Results[ SVRC::result::names ] = EntryArray;
	std::string FileName = "C:\\temp\\QueryMonitorListNames-rsp";
	WriteResultToJsonAndFile(rJsonCommand, rJsonResults, Results, FileName, hr);

	return hr;
}

HRESULT SVRemoteCommandFunctions::GetProductFilter( const std::string& rJsonCommand, std::string& rJsonResults )
{
	HRESULT hr = S_OK;

	std::string ListName;
	SVProductFilterEnum filter(LastInspectedFilter);
	Json::Reader Reader;
	Json::Value JsonCmdValues;

	if( Reader.parse( rJsonCommand, JsonCmdValues, false ) )
	{
		std::string FileName = "C:\\temp\\GetProductFilter-cmd";
		WriteJsonCommandToFile(JsonCmdValues, FileName);

		Json::Value JsonArguments;

		if( JsonCmdValues.isObject() )
		{
			JsonArguments = JsonCmdValues[ SVRC::cmd::arg ];
		}

		if( JsonArguments.isObject() )
		{
			Json::Value JsonListName = JsonArguments[ SVRC::arg::listName ];

			if( JsonListName.isString() )
			{
				ListName = JsonListName.asString();
			}
			else
			{
				hr = E_INVALIDARG;
			}
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}

	if( S_OK == hr )
	{
		if( !( ListName.empty() ) )
		{
			hr = SVVisionProcessorHelper::Instance().GetProductFilter( ListName, filter );
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}

	Json::Value Results(Json::objectValue);
	if (S_OK == hr)
	{
		Results[SVRC::result::filter] = static_cast<Json::Value::Int>(filter);
	}

	std::string FileName = "C:\\temp\\GetProductFilter-rsp";
	WriteResultToJsonAndFile(rJsonCommand, rJsonResults, Results, FileName, hr);
	
	return hr;
}

HRESULT SVRemoteCommandFunctions::SetProductFilter( const std::string& rJsonCommand, std::string& rJsonResults )
{
	HRESULT hr = S_OK;

	std::string ListName;
	unsigned long filter = LastInspectedFilter;
	Json::Reader Reader;
	Json::Value JsonCmdValues;

	if( Reader.parse( rJsonCommand, JsonCmdValues, false ) )
	{
		std::string FileName = "C:\\temp\\SetProductFilter-cmd";
		WriteJsonCommandToFile(JsonCmdValues, FileName);

		Json::Value JsonArguments;

		if( JsonCmdValues.isObject() )
		{
			JsonArguments = JsonCmdValues[ SVRC::cmd::arg ];
		}

		if( JsonArguments.isObject() )
		{
			Json::Value JsonListName = JsonArguments[ SVRC::arg::listName ];

			if( JsonListName.isString() )
			{
				ListName = JsonListName.asString();
			}
			else
			{
				hr = E_INVALIDARG;
			}
			Json::Value JsonFilter = JsonArguments[ SVRC::arg::filter ];
			if (JsonFilter.isInt())
			{
				filter = JsonFilter.asInt();
			}
			else
			{
				hr = E_INVALIDARG;
			}
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}

	if( S_OK == hr )
	{
		if (!(ListName.empty()) && (LastInspectedFilter == filter || LastRejectFilter == filter))
		{
			hr = SVVisionProcessorHelper::Instance().SetProductFilter(ListName, static_cast<SVProductFilterEnum>(filter));
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}

	Json::Value Results(Json::objectValue);

	std::string FileName = "C:\\temp\\SetProductFilter-rsp";
	WriteResultToJsonAndFile(rJsonCommand, rJsonResults, Results, FileName, hr);
	
	return hr;

}

/*####################################################################################################################################
*	WriteJsonCommandToFile				This is used to write the Json values into a file if necessary
*	Return: 							The status result of the method  
*
*	p_rJsonCommand						The original command in string format to parse the Json values
*	p_rFileName							The name of the file to save the Json if necessary
*#####################################################################################################################################
*/
AFX_INLINE HRESULT SVRemoteCommandFunctions::WriteJsonCommandToFile( const std::string& p_rJsonCommand, const std::string& p_rFileName )
{
	HRESULT l_Status = S_OK;

	#ifdef SV_OUTPUT_JSON
		Json::Value l_JsonValues;
		
		Json::Reader l_Reader;
		if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
		{
			WriteJsonCommandToFile(l_JsonValues, p_rFileName);
		}
	#endif

	return l_Status;
}

/*####################################################################################################################################
*	WriteJsonCommandToFile				This is used to write the Json values into a file if necessary
*	Return: 							The status result of the method  
*
*	p_rJsonValues						The Json values to svae
*	p_rFileName							The name of the file to save the Json if necessary
*#####################################################################################################################################
*/
AFX_INLINE HRESULT SVRemoteCommandFunctions::WriteJsonCommandToFile( const Json::Value& p_rJsonValues, const std::string& p_rFileName )
{
	HRESULT l_Status = S_OK;

	#ifdef SV_OUTPUT_JSON
		std::string l_FileName;

		SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_FileName, p_rFileName.c_str(), ".json" );
		SVJsonUtilities::WriteJsonValueToFile( p_rJsonValues, l_FileName );
	#endif

	return l_Status;
}

/*####################################################################################################################################
*	WriteResultToJsonAndFile			This is used to write the Json results and if required save it to file
*	Return: 							The status result of the method  
*
*	p_rJsonCommand						The original Json command in string format
*	p_rJsonResults						The string in which the result is saved to
*	p_rJsonResultValue					The Json result value
*	p_rFileName							The name of the file to save the Json if necessary
*	p_Status							The resulting status of the method being called
*#####################################################################################################################################
*/
AFX_INLINE HRESULT SVRemoteCommandFunctions::WriteResultToJsonAndFile( const std::string& p_rJsonCommand, std::string& p_rJsonResults, 
																		const Json::Value& p_rJsonResultValue, const std::string& p_rFileName, HRESULT p_Status)
{
	HRESULT l_Status = S_OK;

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	std::string l_Name;
	long l_Id = 0;

	GetCommandName( p_rJsonCommand, l_Name );
	GetCommandId( p_rJsonCommand, l_Id );

	l_Object[ SVRC::cmd::name ] = l_Name;
	l_Object[ SVRC::cmd::id ] = l_Id;
	l_Object[ SVRC::cmd::hr ] = p_Status;
	l_Object[ SVRC::cmd::reslts ] = p_rJsonResultValue;

	p_rJsonResults = l_Writer.write( l_Object ).c_str();

	#ifdef SV_OUTPUT_JSON
		std::string l_RspFileNane;
		SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_RspFileNane, p_rFileName, ".json" );
		SVJsonUtilities::WriteJsonValueToFile( l_Object, l_RspFileNane );
	#endif

	return l_Status;
}

void SVRemoteCommandFunctions::BuildErrorResponse(const std::string& rJsonCommand, std::string& rResponse, HRESULT hr, const std::string& errorText)
{
	HRESULT l_Status = S_OK;

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	std::string l_Name;
	long l_Id = 0;

	GetCommandName( rJsonCommand, l_Name );
	GetCommandId( rJsonCommand, l_Id );

	l_Object[ SVRC::cmd::name ] = l_Name;
	l_Object[ SVRC::cmd::id ] = l_Id;
	l_Object[ SVRC::cmd::hr ] = hr;
	l_Object[ SVRC::cmd::reslts ] = Json::Value(Json::objectValue);
	if (!errorText.empty())
	{
		l_Object[ SVRC::cmd::err ] = errorText.c_str();
	}
	rResponse = l_Writer.write( l_Object ).c_str();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\svobserver\SVSocketRemoteCommandManager.cpp_v  $
 * 
 *    Rev 1.16   20 Nov 2014 05:05:16   mziegler
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
 *    Rev 1.15   10 Nov 2014 17:10:46   sjones
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
 *    Rev 1.14   02 Oct 2014 10:22:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  949
 * SCR Title:  Repeatedly Calling PutConfig Causes SVObserver to Stop Responding
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added BuildErrorResponse method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   02 Sep 2014 13:25:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised GetProductFilter method to set the filter json value as an Json:Int type.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   29 Aug 2014 17:49:06   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added support for get/set product filter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   09 Jul 2014 17:01:52   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  915
 * SCR Title:  Add command QueryMonitorListNames for RemoteControl
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add method QueryMonitorListNames to class and to function map.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   24 Jun 2014 08:21:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  905
 * SCR Title:  Implement Shutdown Command thru Remote Control
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Include fstream
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   24 Jun 2014 07:21:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  905
 * SCR Title:  Implement Shutdown Command thru Remote Control
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added remote command function Shutdown.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   25 Apr 2014 13:08:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised QueryProductList to set the results even if an error occurred.
 * Revised QueryRejectCondList to set the results even if an error occurred.
 * Revised QueryFailStatusList to set the results even if an error occurred.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   24 Apr 2014 11:27:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised QueryProductList, QueryRejectCondList, and QueryFailStatusList to improve error reporting
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   23 Apr 2014 18:07:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed ConvertNameToJsonValue method.
 * Removed DeactivateMonitorList method.
 * Revised ActivateMonitorList method.
 * Revised QueryProductList method.
 * Revised QueryRejectCondList method.
 * Revised QueryFailStatusList method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   23 Apr 2014 10:47:42   sjones
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
 *    Rev 1.4   30 Oct 2013 15:40:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  866
 * SCR Title:  Add GetDataDefinitionList Command to SVObserver's Remote Command Socket
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Implemented GetDataDefinitionList.
 *   Moved common code to WriteJsonCommandToFile and WriteResultToJsonAndFile.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Jul 2013 09:11:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Jun 2013 13:42:14   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed path issue in PutDeviceFile.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Jun 2013 15:37:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 May 2013 13:06:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated object to handle the method name changes in Vision Processor Helper class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:43:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Apr 2013 14:27:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 15:08:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Apr 2013 12:33:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to process the remote commands as specified in the IF00100.9400.001 interface document.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Mar 2013 14:12:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated GetItems method to handle issues assocoated with elements in an error stage and add them to an error list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Feb 2013 13:25:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow the socket interface to process the set items command.
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
