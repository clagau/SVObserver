//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file CommandFunctionHelper.h
/// All Rights Reserved
//*****************************************************************************
/// This file contains the helper function for the extern use.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVStatusLibrary\MessageContainer.h"
#include "SVUtilityLibrary\SVGUID.h"
#pragma warning( push )
#pragma warning( disable : 4800 ) 
#include "SVProtoBuf\InspectionCommands.pb.h"
#pragma warning( pop )
#pragma endregion Includes

namespace SvCmd
{
	/// Call an inspection command synchronous. (Send it to the inspection thread.)
	/// \param rInspectionID [in] Guid for the inspection to find the inspection thread.
	/// \param pRequest [in] The request message.
	/// \param pResponse [in, out] The response message (must fit to the request message). If request message has no response message, it should be nullptr.
	/// \returns HRESULT ErrorCode.
	HRESULT InspectionCommandsSynchronous(const SVGUID& rInspectionID, const google::protobuf::Message* pRequest, google::protobuf::Message* pResponse = nullptr);

	/// Set a SVGUID to the protobuf-structure.
	/// \param rSVGuidID [in] The guid.
	/// \returns SvCmd::UUID The new protobuf-structure.
	SvPB::UUID setGuidToMessage(const SVGUID& rSVGuidID);
	/// Convert a guid in protobuf-structure to a SVGUID.
	/// \param rUUId [in] The protobuf-structure
	/// \returns SVGUID The guid.
	SVGUID getGuidFromMessage(const SvPB::UUID& rUUId);

	/// Set messages from SVObserver to a protobuf-structure.
	/// \param messageContainers [in] Messages
	/// \returns SvCmd::MessageContainerVector The protobuf-structure.
	SvPB::MessageContainerVector setMessageContainerToMessagePB(const SvStl::MessageContainerVector& messageContainers);
	/// Convert a protobuf-structure to messages from SVObserver.
	/// \param messagesPB [in] The protobuf-structure.
	/// \returns SvStl::MessageContainerVector The messages
	SvStl::MessageContainerVector setMessageContainerFromMessagePB(const SvPB::MessageContainerVector& messagesPB);
} //namespace SvCmd