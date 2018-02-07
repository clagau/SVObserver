//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file BasicStructureHelper.h
/// All Rights Reserved
//*****************************************************************************
/// This file contains the helper function for the extern use.
//******************************************************************************
#pragma once

#pragma region Includes
#pragma warning( push )
#pragma warning( disable : 4800 ) 
#include "SVProtoBuf\BasicStructure.pb.h"
#pragma warning( pop )
#pragma endregion Includes

namespace SvPB
{
	///Set a Guid to a bytes variable in a protobufmessage 
	void SetGuidInProtoBytes(std::string  *pString, const GUID& guid);
	
	///Get a Guid from a bytes variable in a protobufmessage. 
	void GetGuidFromProtoBytes(const std::string& strguid, GUID& rGuid);
	
	//!set the in th eprotobuf messager  	
	void SetGuidInMessage(SvPB::UUID* pMessage, const GUID& rGuid);
	
	//!GUID From protobufmessage 
	void GetGuidFromMessage(const SvPB::UUID& rMessage, GUID& rGuid);

	//!GUID From protobufmessage 
	GUID getGuidFromMessage(const SvPB::UUID& rUUId);

} //namespace SvPB