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
#include "SVProtoBuf\ConverterHelper.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "ObjectInterfaces/ObjectInfo.h"
#pragma endregion Includes


namespace SvCmd
{
/// Call an inspection command synchronous. (Send it to the inspection thread.)
/// \param rInspectionID [in] Guid for the inspection to find the inspection thread.
/// \param pRequest [in] The request message.
/// \param pResponse [in, out] The response message (must fit to the request message). If request message has no response message, it should be nullptr.
/// \returns HRESULT ErrorCode.
HRESULT InspectionCommandsSynchronous(const SVGUID& rInspectionID, SvPb::InspectionCmdMsgs* pRequest, SvPb::InspectionCmdMsgs* pResponse );

HRESULT RunOnceSynchronous(const SVGUID& rInspectionID, const SVGUID& rTaskIdID);

/// Set messages from SVObserver to a protobuf-structure.
/// \param messageContainers [in] Messages
/// \returns SvCmd::MessageContainerVector The protobuf-structure.
SvPb::MessageContainerVector setMessageContainerToMessagePB(const SvStl::MessageContainerVector& messageContainers);
/// Convert a protobuf-structure to messages from SVObserver.
/// \param messagesPB [in] The protobuf-structure.
/// \returns SvStl::MessageContainerVector The messages
SvStl::MessageContainerVector setMessageContainerFromMessagePB(const SvPb::MessageContainerVector& messagesPB);

///convert protobuf message to a vector of objectinfos
bool ResponseToObjectInfo(const SvPb::InspectionCmdMsgs& rResponse, SvOi::ObjectInfoVector&  rToolSetInfos);

} //namespace SvCmd