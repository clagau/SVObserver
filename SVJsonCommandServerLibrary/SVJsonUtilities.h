//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonUtilities
//* .File Name       : $Workfile:   SVJsonUtilities.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 13:15:10  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <string>
#include "JsonLib/include/json.h"
#pragma endregion Includes

namespace SVJsonUtilities
{
	HRESULT ConvertJsonValueToVariant( const Json::Value& p_rJsonValue, VARIANT& p_rVariant );

	template< typename SVStringType >
	HRESULT ConvertJsonArrayToStringSet( const Json::Value& p_rJsonArray, std::set< SVStringType >& p_rStrings );

	HRESULT ConvertVariantToJsonValue( const _variant_t& p_rVariant, Json::Value& p_rJsonValue );

	HRESULT AddVariantToJsonArray( const _variant_t& p_rVariant, Json::Value& p_rJsonArray );

	HRESULT GetTempFileNameUsingPrefixAndExt( std::string& p_rTempFileName, const std::string& p_rPrefix, const std::string& p_rExt );

	HRESULT WriteJsonValueToFile( const Json::Value& p_rJsonValue, const std::string& p_rFileName );

}

#include "SVJsonUtilities.inl"

