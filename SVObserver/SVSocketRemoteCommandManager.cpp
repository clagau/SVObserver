//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocketRemoteCommandManager
//* .File Name       : $Workfile:   SVSocketRemoteCommandManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   05 Jul 2013 09:11:20  $
//******************************************************************************

#include "stdafx.h"
#include <boost/config.hpp>
#include <boost/assign.hpp>
#include "SVSocketRemoteCommandManager.h"
#include "SVTimerLibrary\SVClock.h"
#include "JsonLib/include/json.h"
#include "SVJsonCommandServerLibrary/SVJsonUtilities.h"
#include "SVSystemLibrary/SVEncodeDecodeUtilities.h"

#include "SVGlobal.h"
#include "SVRemoteControlConstants.h"
#include "SVVisionProcessorHelper.h"

#define SV_DATA_TO_CONTENTS
//#define SV_OUTPUT_JSON

SVRemoteCommandFunctions::SVCommandFunctionMap SVRemoteCommandFunctions::m_Functions = boost::assign::map_list_of< std::string, SVRemoteCommandFunctions::SVCommandFunction >
(SVRC::cmdName::getMode, &SVRemoteCommandFunctions::GetDeviceMode)
(SVRC::cmdName::getState, &SVRemoteCommandFunctions::GetState)
(SVRC::cmdName::getVersion, &SVRemoteCommandFunctions::GetVersions)
(SVRC::cmdName::getOLCount, &SVRemoteCommandFunctions::GetDeviceOfflineCount)
;

SVRemoteCommandFunctions::SVCommandFunctionMap SVRemoteCommandFunctions::m_AsyncFunctions = boost::assign::map_list_of< std::string, SVRemoteCommandFunctions::SVCommandFunction >
(SVRC::cmdName::getConfig, &SVRemoteCommandFunctions::GetConfig)
(SVRC::cmdName::getItems, &SVRemoteCommandFunctions::GetItems)
(SVRC::cmdName::getFile, &SVRemoteCommandFunctions::GetDeviceFile)
(SVRC::cmdName::getReport, &SVRemoteCommandFunctions::GetDeviceConfigReport)
(SVRC::cmdName::putConfig, &SVRemoteCommandFunctions::PutConfig)
(SVRC::cmdName::putFile, &SVRemoteCommandFunctions::PutDeviceFile)
(SVRC::cmdName::setMode, &SVRemoteCommandFunctions::SetDeviceMode)
(SVRC::cmdName::setItems, &SVRemoteCommandFunctions::SetItems)
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

HRESULT SVRemoteCommandFunctions::AddResultStatusToJsonValue( const SVNameStorageMap& p_rItems, const SVNameStatusMap& p_rResultStatus, Json::Value& p_rJsonErrors )
{
	HRESULT l_Status = S_OK;

	if( p_rJsonErrors.isArray() )
	{
		for( SVNameStorageMap::const_iterator l_ItemsIter = p_rItems.begin(); l_ItemsIter != p_rItems.end(); ++l_ItemsIter )
		{
			SVNameStatusMap::const_iterator l_StatusIter = p_rResultStatus.find( l_ItemsIter->first );

			if( ( l_StatusIter == p_rResultStatus.end() ) || ( l_StatusIter->second != S_OK ) )
			{
				l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;

				Json::Value l_Error( Json::objectValue );

				l_Error[ SVRC::error::name ] = l_ItemsIter->first.ToDataType();

				if( l_StatusIter == p_rResultStatus.end() )
				{
					l_Error[ SVRC::error::status ] = static_cast< HRESULT >( SVMSG_OBJECT_NOT_PROCESSED );
				}
				else
				{
					l_Error[ SVRC::error::status ] = l_StatusIter->second;
				}

				p_rJsonErrors.append( l_Error );
			}
		}
	}
	else
	{
		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::CommandNotFound( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_FALSE;

	#ifdef SV_OUTPUT_JSON
		Json::Reader l_Reader;
		Json::Value l_JsonValues;

		if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
		{
			std::string l_FileName;

			SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_FileName, "C:\\temp\\CommandNotFound-cmd", ".json" );
			SVJsonUtilities::WriteJsonValueToFile( l_JsonValues, l_FileName );
		}
	#endif

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	Json::Value l_Results(Json::objectValue);
	std::string l_Name;
	long l_Id = 0;

	GetCommandName( p_rJsonCommand, l_Name );
	GetCommandId( p_rJsonCommand, l_Id );

	l_Object[ SVRC::cmd::name ] = l_Name;
	l_Object[ SVRC::cmd::id ] = l_Id;
	l_Object[ SVRC::cmd::hr ] = E_FAIL;
	l_Object[ SVRC::cmd::reslts ] = l_Results;

	p_rJsonResults = l_Writer.write( l_Object ).c_str();
	
	#ifdef SV_OUTPUT_JSON
		std::string l_RspFileNane;
		SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_RspFileNane, "C:\\temp\\CommandNotFound-rsp", ".json" );
		SVJsonUtilities::WriteJsonValueToFile( l_Object, l_RspFileNane );
	#endif

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::GetDeviceMode( const std::string& p_rJsonCommand, std::string& p_rJsonResult )
{
	HRESULT l_Status = S_OK;

	#ifdef SV_OUTPUT_JSON
		Json::Reader l_Reader;
		Json::Value l_JsonValues;

		if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
		{
			std::string l_FileName;

			SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_FileName, "C:\\temp\\GetDeviceMode-cmd", ".json" );
			SVJsonUtilities::WriteJsonValueToFile( l_JsonValues, l_FileName );
		}
	#endif

	unsigned long l_Mode = SVIM_MODE_UNKNOWN;

	l_Status = SVVisionProcessorHelper::Instance().GetConfigurationMode( l_Mode );

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	Json::Value l_Results(Json::objectValue);
	std::string l_Name;
	long l_Id = 0;

	GetCommandName( p_rJsonCommand, l_Name );
	GetCommandId( p_rJsonCommand, l_Id );

	l_Object[ SVRC::cmd::name ] = l_Name;
	l_Object[ SVRC::cmd::id ] = l_Id;
	l_Object[ SVRC::cmd::hr ] = l_Status;

	l_Results[ SVRC::result::mode ] = static_cast< Json::Value::UInt >( l_Mode );

	l_Object[ SVRC::cmd::reslts ] = l_Results;

	p_rJsonResult = l_Writer.write( l_Object ).c_str();
	
	#ifdef SV_OUTPUT_JSON
		std::string l_RspFileNane;
		SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_RspFileNane, "C:\\temp\\GetDeviceMode-rsp", ".json" );
		SVJsonUtilities::WriteJsonValueToFile( l_Object, l_RspFileNane );
	#endif

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::GetState( const std::string& p_rJsonCommand, std::string& p_rJsonResult )
{
	HRESULT l_Status = S_OK;

	#ifdef SV_OUTPUT_JSON
		Json::Reader l_Reader;
		Json::Value l_JsonValues;

		if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
		{
			std::string l_FileName;

			SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_FileName, "C:\\temp\\GetState-cmd", ".json" );
			SVJsonUtilities::WriteJsonValueToFile( l_JsonValues, l_FileName );
		}
	#endif

	unsigned long l_State = 0;

	l_Status = SVVisionProcessorHelper::Instance().GetState( l_State );

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	Json::Value l_Results(Json::objectValue);
	std::string l_Name;
	long l_Id = 0;

	GetCommandName( p_rJsonCommand, l_Name );
	GetCommandId( p_rJsonCommand, l_Id );

	l_Object[ SVRC::cmd::name ] = l_Name;
	l_Object[ SVRC::cmd::id ] = l_Id;
	l_Object[ SVRC::cmd::hr ] = l_Status;

	l_Results[ SVRC::result::state ] = static_cast< Json::Value::UInt >( l_State );

	l_Object[ SVRC::cmd::reslts ] = l_Results;

	p_rJsonResult = l_Writer.write( l_Object ).c_str();

	#ifdef SV_OUTPUT_JSON
		std::string l_RspFileNane;
		SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_RspFileNane, "C:\\temp\\GetState-rsp", ".json" );
		SVJsonUtilities::WriteJsonValueToFile( l_Object, l_RspFileNane );
	#endif

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::GetVersions( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	#ifdef SV_OUTPUT_JSON
		Json::Reader l_Reader;
		Json::Value l_JsonValues;

		if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
		{
			std::string l_FileName;

			SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_FileName, "C:\\temp\\GetVersions-cmd", ".json" );
			SVJsonUtilities::WriteJsonValueToFile( l_JsonValues, l_FileName );
		}
	#endif

	SVString l_Version;

	l_Status = SVVisionProcessorHelper::Instance().GetVersion( l_Version );

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	Json::Value l_Results(Json::objectValue);
	std::string l_Name;
	long l_Id = 0;

	GetCommandName( p_rJsonCommand, l_Name );
	GetCommandId( p_rJsonCommand, l_Id );

	l_Object[ SVRC::cmd::name ] = l_Name;
	l_Object[ SVRC::cmd::id ] = l_Id;
	l_Object[ SVRC::cmd::hr ] = l_Status;

	l_Results[ SVRC::result::SVO_ver ] = l_Version.ToDataType();

	l_Object[ SVRC::cmd::reslts ] = l_Results;

	p_rJsonResults = l_Writer.write( l_Object ).c_str();
	
	#ifdef SV_OUTPUT_JSON
		std::string l_RspFileNane;
		SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_RspFileNane, "C:\\temp\\GetVersions-rsp", ".json" );
		SVJsonUtilities::WriteJsonValueToFile( l_Object, l_RspFileNane );
	#endif

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::GetDeviceOfflineCount( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	#ifdef SV_OUTPUT_JSON
		Json::Reader l_Reader;
		Json::Value l_JsonValues;

		if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
		{
			std::string l_FileName;

			SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_FileName, "C:\\temp\\GetDeviceOfflineCount-cmd", ".json" );
			SVJsonUtilities::WriteJsonValueToFile( l_JsonValues, l_FileName );
		}
	#endif

	unsigned long l_OfflineCount;

	l_Status = SVVisionProcessorHelper::Instance().GetOfflineCount( l_OfflineCount );

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	Json::Value l_Results(Json::objectValue);
	std::string l_Name;
	long l_Id = 0;

	GetCommandName( p_rJsonCommand, l_Name );
	GetCommandId( p_rJsonCommand, l_Id );

	l_Object[ SVRC::cmd::name ] = l_Name;
	l_Object[ SVRC::cmd::id ] = l_Id;
	l_Object[ SVRC::cmd::hr ] = l_Status;

	l_Results[ SVRC::result::count ] = static_cast< Json::Value::UInt >( l_OfflineCount );

	l_Object[ SVRC::cmd::reslts ] = l_Results;

	p_rJsonResults = l_Writer.write( l_Object ).c_str();
	
	#ifdef SV_OUTPUT_JSON
		std::string l_RspFileNane;
		SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_RspFileNane, "C:\\temp\\GetDeviceOfflineCount-rsp", ".json" );
		SVJsonUtilities::WriteJsonValueToFile( l_Object, l_RspFileNane );
	#endif

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
	Json::Value l_JsonValues;

	if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
	{
		#ifdef SV_OUTPUT_JSON
			std::string l_FileName;

			SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_FileName, "C:\\temp\\GetConfig-cmd", ".json" );
			SVJsonUtilities::WriteJsonValueToFile( l_JsonValues, l_FileName );
		#endif

		Json::Value l_JsonArguments;

		if( l_JsonValues.isObject() )
		{
			l_JsonArguments = l_JsonValues[ SVRC::cmd::arg ];
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

	std::string l_TempFileName;

	l_Status = GetTempFileNameFromFilePath( l_TempFileName, l_RemoteFilePath );

	if( !( l_TempFileName.empty() ) )
	{
		l_Status = SVVisionProcessorHelper::Instance().SaveConfiguration( l_TempFileName );
	}
	else if( l_Status == S_OK )
	{
		l_Status = E_UNEXPECTED;
	}

	if( l_Status == S_OK )
	{
		#ifdef SV_DATA_TO_CONTENTS
			l_Status = SVEncodeDecodeUtilities::Base64EncodeToStringFromFile( l_Contents, l_TempFileName );

			::remove( l_TempFileName.c_str() );
		#else
			l_Status = GetFileNameFromFilePath( l_DestinationFileName, l_TempFileName );
		#endif
	}

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	Json::Value l_Results(Json::objectValue);
	std::string l_Name;
	long l_Id = 0;

	GetCommandName( p_rJsonCommand, l_Name );
	GetCommandId( p_rJsonCommand, l_Id );

	l_Object[ SVRC::cmd::name ] = l_Name;
	l_Object[ SVRC::cmd::id ] = l_Id;
	l_Object[ SVRC::cmd::hr ] = l_Status;

	if( l_Status == S_OK )
	{
		l_Results[ SVRC::result::filePath ] = l_RemoteFilePath;

		#ifdef SV_DATA_TO_CONTENTS
			l_Results[ SVRC::result::contents ] = l_Contents;
		#else
			l_Results[ SVRC::result::destinationFileName ] = l_DestinationFileName;
		#endif
	}

	l_Object[ SVRC::cmd::reslts ] = l_Results;

	p_rJsonResults = l_Writer.write( l_Object ).c_str();

	#ifdef SV_OUTPUT_JSON
		std::string l_RspFileNane;
		SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_RspFileNane, "C:\\temp\\GetConfig-rsp", ".json" );
		SVJsonUtilities::WriteJsonValueToFile( l_Object, l_RspFileNane );
	#endif

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::GetItems( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	SVNameSet l_Names;
	SVNameStorageResultMap l_Items;

	Json::Reader l_Reader;
	Json::Value l_JsonValues;

	if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
	{
		#ifdef SV_OUTPUT_JSON
			std::string l_FileName;

			SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_FileName, "C:\\temp\\GetItems-cmd", ".json" );
			SVJsonUtilities::WriteJsonValueToFile( l_JsonValues, l_FileName );
		#endif

		Json::Value l_JsonArguments;

		if( l_JsonValues.isObject() )
		{
			l_JsonArguments = l_JsonValues[ SVRC::cmd::arg ];
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

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	std::string l_Name;
	long l_Id = 0;

	GetCommandName( p_rJsonCommand, l_Name );
	GetCommandId( p_rJsonCommand, l_Id );

	l_Object[ SVRC::cmd::name ] = l_Name;
	l_Object[ SVRC::cmd::id ] = l_Id;
	l_Object[ SVRC::cmd::hr ] = l_Status;
	l_Object[ SVRC::cmd::reslts ] = l_Results;

	p_rJsonResults = l_Writer.write( l_Object ).c_str();

	#ifdef SV_OUTPUT_JSON
		std::string l_RspFileNane;
		SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_RspFileNane, "C:\\temp\\GetItems-rsp", ".json" );
		SVJsonUtilities::WriteJsonValueToFile( l_Object, l_RspFileNane );
	#endif

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
	Json::Value l_JsonValues;

	if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
	{
		#ifdef SV_OUTPUT_JSON
			std::string l_FileName;

			SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_FileName, "C:\\temp\\GetDeviceFile-cmd", ".json" );
			SVJsonUtilities::WriteJsonValueToFile( l_JsonValues, l_FileName );
		#endif

		Json::Value l_JsonArguments;

		if( l_JsonValues.isObject() )
		{
			l_JsonArguments = l_JsonValues[ SVRC::cmd::arg ];
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

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	Json::Value l_Results(Json::objectValue);
	std::string l_Name;
	long l_Id = 0;

	GetCommandName( p_rJsonCommand, l_Name );
	GetCommandId( p_rJsonCommand, l_Id );

	l_Object[ SVRC::cmd::name ] = l_Name;
	l_Object[ SVRC::cmd::id ] = l_Id;
	l_Object[ SVRC::cmd::hr ] = l_Status;

	if( l_Status == S_OK )
	{
		l_Results[ SVRC::result::destinationPath ] = l_DestinationPath;

		#ifdef SV_DATA_TO_CONTENTS
			l_Results[ SVRC::result::contents ] = l_Contents;
		#else
			l_Results[ SVRC::result::destinationFileName ] = l_DestinationFileName;
		#endif
	}

	l_Object[ SVRC::cmd::reslts ] = l_Results;

	p_rJsonResults = l_Writer.write( l_Object ).c_str();

	#ifdef SV_OUTPUT_JSON
		std::string l_RspFileNane;
		SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_RspFileNane, "C:\\temp\\GetDeviceFile-rsp", ".json" );
		SVJsonUtilities::WriteJsonValueToFile( l_Object, l_RspFileNane );
	#endif

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::GetDeviceConfigReport( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	#ifdef SV_OUTPUT_JSON
		Json::Reader l_Reader;
		Json::Value l_JsonValues;

		if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
		{
			std::string l_FileName;

			SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_FileName, "C:\\temp\\GetDeviceConfigReport-cmd", ".json" );
			SVJsonUtilities::WriteJsonValueToFile( l_JsonValues, l_FileName );
		}
	#endif

	SVString l_Report;

	l_Status = SVVisionProcessorHelper::Instance().GetConfigurationPrintReport( l_Report );

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	Json::Value l_Results(Json::objectValue);
	std::string l_Name;
	long l_Id = 0;

	GetCommandName( p_rJsonCommand, l_Name );
	GetCommandId( p_rJsonCommand, l_Id );

	l_Object[ SVRC::cmd::name ] = l_Name;
	l_Object[ SVRC::cmd::id ] = l_Id;
	l_Object[ SVRC::cmd::hr ] = l_Status;

	if( l_Status == S_OK )
	{
		l_Results[ SVRC::result::report ] = l_Report.ToDataType();
	}

	l_Object[ SVRC::cmd::reslts ] = l_Results;

	p_rJsonResults = l_Writer.write( l_Object ).c_str();
	
	#ifdef SV_OUTPUT_JSON
		std::string l_RspFileNane;
		SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_RspFileNane, "C:\\temp\\GetDeviceConfigReport-rsp", ".json" );
		SVJsonUtilities::WriteJsonValueToFile( l_Object, l_RspFileNane );
	#endif

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::PutConfig( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	std::string l_SourceFileName;
	Json::Reader l_Reader;
	Json::Value l_JsonValues;

	if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
	{
		#ifdef SV_OUTPUT_JSON
			std::string l_FileName;

			SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_FileName, "C:\\temp\\PutConfig-cmd", ".json" );
			SVJsonUtilities::WriteJsonValueToFile( l_JsonValues, l_FileName );
		#endif

		Json::Value l_JsonArguments;

		if( l_JsonValues.isObject() )
		{
			l_JsonArguments = l_JsonValues[ SVRC::cmd::arg ];
		}

		if( l_JsonArguments.isObject() )
		{
			std::string l_DestinationPath;
			Json::Value l_JsonContents;

			l_JsonContents = l_JsonArguments[ SVRC::arg::contents ];

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

				if( l_Status == S_OK )
				{
					l_Status = SVEncodeDecodeUtilities::Base64DecodeToFileFromString( l_TempFileName, l_JsonContents.asString() );
				}

				if( l_Status == S_OK )
				{
					l_SourceFileName = l_TempFileName;
				}
				else
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
		}
	}

	if( !( l_SourceFileName.empty() ) )
	{
		l_Status = SVVisionProcessorHelper::Instance().LoadConfiguration( l_SourceFileName );

		::remove( l_SourceFileName.c_str() );
	}
	else if( l_Status == S_OK )
	{
		l_Status = E_INVALIDARG;
	}

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	Json::Value l_Results(Json::objectValue);
	std::string l_Name;
	long l_Id = 0;

	GetCommandName( p_rJsonCommand, l_Name );
	GetCommandId( p_rJsonCommand, l_Id );

	l_Object[ SVRC::cmd::name ] = l_Name;
	l_Object[ SVRC::cmd::id ] = l_Id;
	l_Object[ SVRC::cmd::hr ] = l_Status;
	l_Object[ SVRC::cmd::reslts ] = l_Results;

	p_rJsonResults = l_Writer.write( l_Object ).c_str();

	#ifdef SV_OUTPUT_JSON
		std::string l_RspFileNane;
		SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_RspFileNane, "C:\\temp\\PutConfig-rsp", ".json" );
		SVJsonUtilities::WriteJsonValueToFile( l_Object, l_RspFileNane );
	#endif

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::PutDeviceFile( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	Json::Reader l_Reader;
	Json::Value l_JsonValues;

	if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
	{
		#ifdef SV_OUTPUT_JSON
			std::string l_FileName;

			SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_FileName, "C:\\temp\\PutDeviceFile-cmd", ".json" );
			SVJsonUtilities::WriteJsonValueToFile( l_JsonValues, l_FileName );
		#endif	

		Json::Value l_JsonArguments;

		if( l_JsonValues.isObject() )
		{
			l_JsonArguments = l_JsonValues[ SVRC::cmd::arg ];
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
					if( !::MoveFile( l_SourceFileName.c_str(), l_DestinationPath.c_str() ) )
					{
						l_Status = E_UNEXPECTED;
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

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	Json::Value l_Results(Json::objectValue);
	std::string l_Name;
	long l_Id = 0;

	GetCommandName( p_rJsonCommand, l_Name );
	GetCommandId( p_rJsonCommand, l_Id );

	l_Object[ SVRC::cmd::name ] = l_Name;
	l_Object[ SVRC::cmd::id ] = l_Id;
	l_Object[ SVRC::cmd::hr ] = l_Status;
	l_Object[ SVRC::cmd::reslts ] = l_Results;

	p_rJsonResults = l_Writer.write( l_Object ).c_str();

	#ifdef SV_OUTPUT_JSON
		std::string l_RspFileNane;
		SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_RspFileNane, "C:\\temp\\PutDeviceFile-rsp", ".json" );
		SVJsonUtilities::WriteJsonValueToFile( l_Object, l_RspFileNane );
	#endif

	return l_Status;
}

HRESULT SVRemoteCommandFunctions::SetDeviceMode( const std::string& p_rJsonCommand, std::string& p_rJsonResults )
{
	HRESULT l_Status = S_OK;

	unsigned long l_Mode = SVIM_MODE_UNKNOWN;

	Json::Reader l_Reader;
	Json::Value l_JsonValues;

	if( l_Reader.parse( p_rJsonCommand, l_JsonValues, false ) )
	{
		#ifdef SV_OUTPUT_JSON
			std::string l_FileName;

			SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_FileName, "C:\\temp\\SetDeviceMode-cmd", ".json" );
			SVJsonUtilities::WriteJsonValueToFile( l_JsonValues, l_FileName );
		#endif

		Json::Value l_JsonArguments;

		if( l_JsonValues.isObject() )
		{
			l_JsonArguments = l_JsonValues[ SVRC::cmd::arg ];
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

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	Json::Value l_Results(Json::objectValue);
	std::string l_Name;
	long l_Id = 0;

	GetCommandName( p_rJsonCommand, l_Name );
	GetCommandId( p_rJsonCommand, l_Id );

	l_Object[ SVRC::cmd::name ] = l_Name;
	l_Object[ SVRC::cmd::id ] = l_Id;
	l_Object[ SVRC::cmd::hr ] = l_Status;
	l_Object[ SVRC::cmd::reslts ] = l_Results;

	p_rJsonResults = l_Writer.write( l_Object ).c_str();

	#ifdef SV_OUTPUT_JSON
		std::string l_RspFileNane;
		SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_RspFileNane, "C:\\temp\\SetDeviceMode-rsp", ".json" );
		SVJsonUtilities::WriteJsonValueToFile( l_Object, l_RspFileNane );
	#endif

	return l_Status;
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
		#ifdef SV_OUTPUT_JSON
			std::string l_FileName;

			SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_FileName, "C:\\temp\\SetItems-cmd", ".json" );
			SVJsonUtilities::WriteJsonValueToFile( l_JsonCmdValues, l_FileName );
		#endif

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

	Json::Value l_Results(Json::objectValue);
	Json::Value l_ErrorArray(Json::arrayValue);
	Json::Value l_JsonFaults(Json::objectValue);

	HRESULT l_TempStatus = AddResultStatusToJsonValue( l_Items, l_ResultStatus, l_ErrorArray );

	if( l_Status == S_OK )
	{
		l_Status = l_TempStatus;
	}

	Json::FastWriter l_Writer;
	Json::Value l_Object(Json::objectValue);
	std::string l_Name;
	long l_Id = 0;

	GetCommandName( p_rJsonCommand, l_Name );
	GetCommandId( p_rJsonCommand, l_Id );

	l_Object[ SVRC::cmd::name ] = l_Name;
	l_Object[ SVRC::cmd::id ] = l_Id;
	l_Object[ SVRC::cmd::hr ] = l_Status;

	l_JsonFaults[ SVRC::result::errors ] = l_ErrorArray;
	l_Results[ SVRC::result::faults ] = l_JsonFaults;
	l_Object[ SVRC::cmd::reslts ] = l_Results;

	p_rJsonResults = l_Writer.write( l_Object ).c_str();

	#ifdef SV_OUTPUT_JSON
		std::string l_RspFileNane;
		SVJsonUtilities::GetTempFileNameUsingPrefixAndExt( l_RspFileNane, "C:\\temp\\SetItems-rsp", ".json" );
		SVJsonUtilities::WriteJsonValueToFile( l_Object, l_RspFileNane );
	#endif

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVSocketRemoteCommandManager.cpp_v  $
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
