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
#include "JsonLib/include/json.h"
#pragma endregion Includes

namespace SVJsonUtilities
{
	HRESULT ConvertJsonValueToVariant( const Json::Value& p_rJsonValue, VARIANT& p_rVariant );

	template< typename SVStringType >
	HRESULT ConvertJsonArrayToStringSet( const Json::Value& rJsonArray, std::set< SVStringType >& rStrings );

	HRESULT ConvertVariantToJsonValue( const _variant_t& rVariant, Json::Value& rJsonValue );

	HRESULT AddVariantToJsonArray( const _variant_t& rVariant, Json::Value& rJsonArray );

	HRESULT GetTempFileNameUsingPrefixAndExt( std::string& rTempFileName, const std::string& rPrefix, const std::string& rExt );

	HRESULT WriteJsonValueToFile( const Json::Value& rJsonValue, const std::string& rFileName );

}

#include "SVJsonUtilities.inl"

