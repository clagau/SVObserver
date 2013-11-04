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

#ifndef SVJSONUTILITIES_INL
#define SVJSONUTILITIES_INL

#include "SVJsonUtilities.h"

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

	if( l_Status != S_OK )
	{
		p_rStrings.clear();
	}

	return l_Status;
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVJsonCommandServerLibrary\SVJsonUtilities.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:14:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 11:09:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new templated JSON socket processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
