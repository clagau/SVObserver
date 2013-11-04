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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVJsonCommandServerLibrary\SVJsonUtilities.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:15:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Apr 2013 12:21:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added two new methods for generating files and file names for logging JSON commands and responses.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 11:09:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new templated JSON socket processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
