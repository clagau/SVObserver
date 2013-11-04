//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEncodeDecodeUtilities
//* .File Name       : $Workfile:   SVEncodeDecodeUtilities.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:50:34  $
//******************************************************************************

#ifndef SVENCODEDECODEUTILITIES_H
#define SVENCODEDECODEUTILITIES_H

#include <string>
#include "SVUtilityLibrary/SVString.h"

namespace SVEncodeDecodeUtilities
{
	HRESULT Base64EncodeToByteBufferFromByteBuffer( int& p_rOutputBufferSize, char* p_pOutput, size_t p_InputBufferSize, const unsigned char* p_pInput );
	HRESULT Base64EncodeToStringFromByteBuffer( std::string& p_rOutput, size_t p_InputBufferSize, const unsigned char* p_pInput );
	HRESULT Base64EncodeToStringFromString( std::string& p_rOutput, const std::string& p_rInput );
	HRESULT Base64EncodeToStringFromFile( std::string& p_rOutput, const SVString& p_rFileName );

	HRESULT Base64DecodeToByteBufferFromString( int& p_rOutputBufferSize, unsigned char* p_pOutput, const std::string& p_rInput );
	HRESULT Base64DecodeToFileFromString( const SVString& p_rFileName, const std::string& p_rInput );
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVEncodeDecodeUtilities.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:50:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 12:29:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new encode and decode utilities.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

