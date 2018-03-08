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
	HRESULT InspectionRunOnce(const SvPB::InspectionRunOnceRequest& rMessage);

	/// Destroyed a child object.
	/// \param rMessage [in] a request-protoBuf-message.
	/// \returns HRESULT
	HRESULT DestroyChildObject(const SvPB::DestroyChildRequest& rMessage);

	/// Get the last (Error)MessageList from an taskObject.
	/// \param rRequestMessage [in] The request-protoBuf-message.
	/// \param rResponseMessage [in,out] The response-protoBuf-message.
	/// \returns HRESULT
	HRESULT GetMessageList(const SvPB::GetMessageListRequest& rRequestMessage, SvPB::GetMessageListResponse& rResponseMessage);

	/// Reset an object.
	/// \param rRequestMessage [in] The request-protoBuf-message.
	/// \param rResponseMessage [in,out] The response-protoBuf-message.
	/// \returns HRESULT
	HRESULT ResetObject(const SvPB::ResetObjectRequest& rRequestMessage, SvPB::ResetObjectResponse* pResponseMessage);

	/// Create a model for the pattern-analyzer. The model will copy from the current source image of the analyzer.
	/// \param rRequestMessage [in] The request-protoBuf-message.
	/// \param rResponseMessage [in,out] The response-protoBuf-message.
	/// \returns HRESULT
	HRESULT CreateModel(const SvPB::CreateModelRequest& rRequestMessage, SvPB::CreateModelResponse& rResponseMessage);

	/// Return the state if an object is valid.
	/// \param rRequestMessage [in] The request-protoBuf-message.
	/// \param rResponseMessage [in,out] The response-protoBuf-message.
	/// \returns HRESULT
	HRESULT IsValid(const SvPB::IsValidRequest& rRequestMessage, SvPB::IsValidResponse& rResponseMessage);

	/// Return the equation string.
	/// \param rRequestMessage [in] The request-protoBuf-message.
	/// \param rResponseMessage [in,out] The response-protoBuf-message.
	/// \returns HRESULT
	HRESULT GetEquation(const SvPB::GetEquationRequest& rRequestMessage, SvPB::GetEquationResponse& rResponseMessage);

	/// Validate and if required set an equation. Return the status and if successfully the result.
	/// \param rRequestMessage [in] The request-protoBuf-message.
	/// \param rResponseMessage [in,out] The response-protoBuf-message.
	/// \returns HRESULT
	HRESULT ValidateAndSetEquation(const SvPB::ValidateAndSetEquationRequest& rRequestMessage, SvPB::ValidateAndSetEquationResponse& rResponseMessage);
} //namespace SvCmd