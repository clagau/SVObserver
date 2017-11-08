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

#pragma once

//Moved to precompiled header: #include <string>


namespace SVEncodeDecodeUtilities
{
	HRESULT Base64EncodeToByteBufferFromByteBuffer( int& p_rOutputBufferSize, char* p_pOutput, size_t p_InputBufferSize, const unsigned char* p_pInput );
	HRESULT Base64EncodeToStringFromByteBuffer( std::string& p_rOutput, size_t p_InputBufferSize, const unsigned char* p_pInput );
	HRESULT Base64EncodeToStringFromString( std::string& p_rOutput, const std::string& p_rInput );
	HRESULT Base64EncodeToStringFromFile( std::string& p_rOutput, const std::string& p_rFileName );

	HRESULT Base64DecodeToByteBufferFromString( int& p_rOutputBufferSize, unsigned char* p_pOutput, const std::string& p_rInput );
	HRESULT Base64DecodeToFileFromString( const std::string& p_rFileName, const std::string& p_rInput );
}

