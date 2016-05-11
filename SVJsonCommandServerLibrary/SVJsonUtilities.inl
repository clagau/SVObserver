//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonUtilities
//* .File Name       : $Workfile:   SVJsonUtilities.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 13:14:10  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVJsonUtilities.h"
#pragma region Includes

template< typename SVStringType >
HRESULT SVJsonUtilities::ConvertJsonArrayToStringSet( const Json::Value& p_rJsonArray, std::set< SVStringType >& p_rStrings )
{
	HRESULT l_Status = S_OK;

	if( p_rJsonArray.isArray() )
	{
		for( Json::Value::UInt i = 0; i < p_rJsonArray.size(); i++ )
		{
			if( p_rJsonArray[ i ].isString() )
			{
				p_rStrings.insert( p_rJsonArray[ i ].asCString() );
			}
			else if( S_OK == l_Status )
			{
				l_Status = E_INVALIDARG;
			}
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	if( S_OK != l_Status )
	{
		p_rStrings.clear();
	}

	return l_Status;
}
