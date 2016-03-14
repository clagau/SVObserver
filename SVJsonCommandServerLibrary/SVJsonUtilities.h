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

#ifndef SVJSONUTILITIES_H
#define SVJSONUTILITIES_H

#include <comdef.h>
#include <set>
#include <string>

#include "JsonLib/include/json.h"

namespace SVJsonUtilities
{
	HRESULT ConvertJsonValueToVariant( const Json::Value& p_rJsonValue, VARIANT& p_rVariant );

	template< typename SVStringType >
	HRESULT ConvertJsonArrayToStringSet( const Json::Value& p_rJsonArray, std::set< SVStringType >& p_rStrings );

	HRESULT ConvertVariantToJsonValue( const _variant_t& p_rVariant, Json::Value& p_rJsonValue );

	HRESULT AddVariantToJsonArray( const _variant_t& p_rVariant, Json::Value& p_rJsonArray );

	HRESULT UpdateNameVariantPairToJsonObject( const std::string& p_rName, const _variant_t& p_rVariant, Json::Value& p_rJsonObject );

	HRESULT GetTempFileNameUsingPrefixAndExt( std::string& p_rTempFileName, const std::string& p_rPrefix, const std::string& p_rExt );

	HRESULT WriteJsonValueToFile( const Json::Value& p_rJsonValue, const std::string& p_rFileName );

}

#include "SVJsonUtilities.inl"

#endif

