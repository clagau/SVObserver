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

	//Return the parameter of an object.
	HRESULT getObjectParameters(const SvPb::GetObjectParametersRequest& rRequest, SvPb::GetObjectParametersResponse& rResponse);

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

	/// Moved a taskObject to a new position in the taskObject- or friend-list.
	/// \param rRequestMessage [in] The request-protoBuf-message.
	/// \returns HRESULT
	HRESULT MoveObject(const SvPb::MoveObjectRequest& rRequestMessage);

	/// Get Definitions for all Taskobject in taskobjectlist
	HRESULT GetTaskObjectsList(const SvPb::TaskObjectListRequest& rRequest, SvPb::TaskObjectListResponse &rResponse);

	/// Get an required image
	HRESULT getImage(const SvPb::GetImageRequest& rRequest, SvPb::GetImageResponse& rResponse);

	// Get if Auxiliary extents are available
	HRESULT areAuxiliaryExtentsAvailable(const SvPb::AreAuxiliaryExtentsAvailableRequest& rRequest, SvPb::AreAuxiliaryExtentsAvailableResponse& rResponse);

	// Get the list of possible auxiliary extents images.
	HRESULT getAvailableAuxImages(const SvPb::GetAvailableAuxImagesRequest& rRequest, SvPb::GetAvailableAuxImagesResponse& rResponse);

	// Get the connected auxiliary extents image.
	HRESULT getAuxImageObject(const SvPb::GetAuxImageObjectRequest& rRequest, SvPb::GetAuxImageObjectResponse& rResponse);

	// Set the connected auxiliary extents image.
	HRESULT setAuxImageObject(const SvPb::SetAuxImageObjectRequest& rRequest, SvPb::SetAuxImageObjectResponse& rResponse);

	// Get the connected input of this object.
	HRESULT getInputs(const SvPb::GetInputsRequest& rRequest, SvPb::GetInputsResponse& rResponse);

	// Connect an object to an input.
	HRESULT connectToObject(const SvPb::ConnectToObjectRequest& rRequest);

	// Save an image to a file.
	HRESULT saveImage(const SvPb::SaveImageRequest& rRequest);

	//Set object name.
	HRESULT setObjectName(const SvPb::SetObjectNameRequest& rRequest);

	//Get a list of available objects.
	HRESULT getAvailableObjects(const SvPb::GetAvailableObjectsRequest& rRequest, SvPb::GetAvailableObjectsResponse& rResponse);

	//Get a name list of special iamge of a task.
	HRESULT getSpecialImageList(const SvPb::GetSpecialImageListRequest& rRequest, SvPb::GetSpecialImageListResponse& rResponse);

	//Export the mask to a file.
	HRESULT exportMask(const SvPb::ExportMaskRequest& rRequest);

	//Import the mask from a file.
	HRESULT importMask(const SvPb::ImportMaskRequest& rRequest);

	//Get GUID of a object
	HRESULT getObjectId(const SvPb::GetObjectIdRequest& rRequest, SvPb::GetObjectIdResponse& rResponse);

	//Construct a new object and insert it in the friend- or taskObject-list.
	HRESULT constructAndInsert(const SvPb::ConstructAndInsertRequest& rRequest, SvPb::ConstructAndInsertResponse& rResponse);

	//Get the creatable Objects based on the type.
	HRESULT getCreatableObjects(const SvPb::GetCreatableObjectsRequest& rRequest, SvPb::GetCreatableObjectsResponse& rResponse);

	//Return if the inspection should be reset if the task has changed. (in the first step it works only with filters.)
	HRESULT shouldInspectionReset(const SvPb::ShouldInspectionResetRequest& rRequest, SvPb::ShouldInspectionResetResponse& rResponse);

	//Get the ppqName of the inspection
	HRESULT getPPQName(const SvPb::GetPPQNameRequest& rRequest, SvPb::GetPPQNameResponse& rResponse);

	HRESULT getValueObjectEnums(const SvPb::GetValueObjectEnumsRequest& rRequest, SvPb::GetValueObjectEnumsResponse& rResponse);

	HRESULT getEmbeddedValues(const SvPb::GetEmbeddedValuesRequest& rRequest, SvPb::GetEmbeddedValuesResponse& rResponse);

	HRESULT setEmbeddedValues(const SvPb::SetEmbeddedValuesRequest& rRequest, SvPb::SetEmbeddedValuesResponse& rResponse);

	HRESULT getOutputRectangle(const SvPb::GetOutputRectangleRequest& rRequest, SvPb::GetOutputRectangleResponse& rResponse);

	HRESULT getToolSizeAdjustParameter(const SvPb::GetToolSizeAdjustParameterRequest& rRequest, SvPb::GetToolSizeAdjustParameterResponse& rResponse);

	HRESULT getExtentParameter(const SvPb::GetExtentParameterRequest& rRequest, SvPb::GetExtentParameterResponse& rResponse);

	HRESULT setExtentParameter(const SvPb::SetExtentParameterRequest& rRequest, SvPb::SetExtentParameterResponse& rResponse);
	
	HRESULT isAllowedLocation(const SvPb::IsAllowedLocationRequest& rRequest, SvPb::IsAllowedLocationResponse& rResponse);

	HRESULT propagateSizeAndPosition(const SvPb::PropagateSizeAndPositionRequest& rRequest);

	HRESULT usePropagateSizeAndPosition(const SvPb::UsePropagateSizeAndPositionRequest& rRequest, SvPb::UsePropagateSizeAndPositionResponse& rResponse);

	HRESULT getObjectSelectorItems(const SvPb::GetObjectSelectorItemsRequest& rRequest, SvPb::GetObjectSelectorItemsResponse& rResponse);

	HRESULT getSelectorList(const SvPb::GetObjectSelectorItemsRequest& rRequest, SvPb::GetObjectSelectorItemsResponse& rResponse, SvPb::ObjectSelectorType selectorType);
} //namespace SvCmd