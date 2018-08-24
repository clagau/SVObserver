//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file CommandFunction.h
/// All Rights Reserved
//*****************************************************************************
/// This file contains the intern commands to the inspection.
//******************************************************************************
#pragma once
#pragma region Includes
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes

namespace SvCmd
{
	/// Do a runOnce for an inspection or taskObject.
	/// \param rMessage [in] a request-protoBuf-message.
	/// \returns HRESULT
	HRESULT InspectionRunOnce(const SvPb::InspectionRunOnceRequest& rMessage);

	/// Destroyed a child object.
	/// \param rMessage [in] a request-protoBuf-message.
	/// \returns HRESULT
	HRESULT DestroyChildObject(const SvPb::DestroyChildRequest& rMessage);

	/// Get the last (Error)MessageList from an taskObject.
	/// \param rRequestMessage [in] The request-protoBuf-message.
	/// \param rResponseMessage [in,out] The response-protoBuf-message.
	/// \returns HRESULT
	HRESULT GetMessageList(const SvPb::GetMessageListRequest& rRequestMessage, SvPb::GetMessageListResponse& rResponseMessage);

	/// Reset an object.
	/// \param rRequestMessage [in] The request-protoBuf-message.
	/// \param rResponseMessage [in,out] The response-protoBuf-message.
	/// \returns HRESULT
	HRESULT ResetObject(const SvPb::ResetObjectRequest& rRequestMessage, SvPb::ResetObjectResponse* pResponseMessage);

	/// Create a model for the pattern-analyzer. The model will copy from the current source image of the analyzer.
	/// \param rRequestMessage [in] The request-protoBuf-message.
	/// \param rResponseMessage [in,out] The response-protoBuf-message.
	/// \returns HRESULT
	HRESULT CreateModel(const SvPb::CreateModelRequest& rRequestMessage, SvPb::CreateModelResponse& rResponseMessage);

	/// Return the state if an object is valid.
	/// \param rRequestMessage [in] The request-protoBuf-message.
	/// \param rResponseMessage [in,out] The response-protoBuf-message.
	/// \returns HRESULT
	HRESULT IsValid(const SvPb::IsValidRequest& rRequestMessage, SvPb::IsValidResponse& rResponseMessage);

	/// Return the equation string.
	/// \param rRequestMessage [in] The request-protoBuf-message.
	/// \param rResponseMessage [in,out] The response-protoBuf-message.
	/// \returns HRESULT
	HRESULT GetEquation(const SvPb::GetEquationRequest& rRequestMessage, SvPb::GetEquationResponse& rResponseMessage);

	/// Validate and if required set an equation. Return the status and if successfully the result.
	/// \param rRequestMessage [in] The request-protoBuf-message.
	/// \param rResponseMessage [in,out] The response-protoBuf-message.
	/// \returns HRESULT
	HRESULT ValidateAndSetEquation(const SvPb::ValidateAndSetEquationRequest& rRequestMessage, SvPb::ValidateAndSetEquationResponse& rResponseMessage);

	/// Return a list of object which should be added to the monitor list (e.g. HMI purpose)
	/// \param rRequestMessage [in] The request-protoBuf-message.
	/// \param rResponseMessage [in,out] The response-protoBuf-message.
	/// \returns HRESULT
	HRESULT getObjectsForMonitorList(const SvPb::GetObjectsForMonitorListRequest& rRequestMessage, SvPb::GetObjectsForMonitorListResponse& rResponseMessage);

	/// Moved a taskObject to a new position in the taskObjectList.
	/// \param rRequestMessage [in] The request-protoBuf-message.
	/// \returns HRESULT
	HRESULT MoveTaskObject(const SvPb::MoveTaskObjectRequest& rRequestMessage);

	/// Get Definitions for all Taskobject in taskobjectlist
	HRESULT GetTaskObjectsList(const SvPb::TaskObjectListRequest& rRequest, SvPb::TaskObjectListResponse &rResponse);
} //namespace SvCmd