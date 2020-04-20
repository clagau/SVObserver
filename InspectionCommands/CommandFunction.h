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
/// \param request [in] a request-protoBuf-message.
/// \returns InspectionCmdResult
SvPb::InspectionCmdResponse InspectionRunOnce(SvPb::InspectionRunOnceRequest request);

/// Delete an object.
/// \param request [in] a request-protoBuf-message.
/// \returns InspectionCmdResult
SvPb::InspectionCmdResponse DeleteObject(SvPb::DeleteObjectRequest request);

/// Get the last (Error)MessageList from an taskObject.
/// \param request [in] The request-protoBuf-message.
/// \returns InspectionCmdResult
SvPb::InspectionCmdResponse GetMessageList(SvPb::GetMessageListRequest request);

/// Reset an object.
/// \param request [in] The request-protoBuf-message.
/// \returns InspectionCmdResult
SvPb::InspectionCmdResponse ResetObject(SvPb::ResetObjectRequest request);

/// Create a model for the pattern-analyzer. The model will copy from the current source image of the analyzer.
/// \param request [in] The request-protoBuf-message.
/// \returns InspectionCmdResult
SvPb::InspectionCmdResponse CreateModel(SvPb::CreateModelRequest request);

//Return the parameter of an object.
SvPb::InspectionCmdResponse getObjectParameters(SvPb::GetObjectParametersRequest request);

/// Return the equation string.
/// \param request [in] The request-protoBuf-message.
/// \returns InspectionCmdResult
SvPb::InspectionCmdResponse GetEquation(SvPb::GetEquationRequest request);

/// Validate and if required set an equation. Return the status and if successfully the result.
/// \param request [in] The request-protoBuf-message.
/// \returns InspectionCmdResult
SvPb::InspectionCmdResponse ValidateAndSetEquation(SvPb::ValidateAndSetEquationRequest request);

/// Return a list of object which should be added to the monitor list (e.g. HMI purpose)
/// \param request [in] The request-protoBuf-message.
/// \returns InspectionCmdResult
SvPb::InspectionCmdResponse getObjectsForMonitorList(SvPb::GetObjectsForMonitorListRequest request);

/// Moved a taskObject to a new position in the taskObject- or friend-list.
/// \param request [in] The request-protoBuf-message.
/// \returns InspectionCmdResult
SvPb::InspectionCmdResponse MoveObject(SvPb::MoveObjectRequest request);

/// Get Definitions for all Taskobject in taskobjectlist
SvPb::InspectionCmdResponse GetTaskObjectsList(SvPb::TaskObjectListRequest request);

/// Get an required image
SvPb::InspectionCmdResponse getImage(SvPb::GetImageRequest request);

// Get if Auxiliary extents are available
SvPb::InspectionCmdResponse areAuxiliaryExtentsAvailable(SvPb::AreAuxiliaryExtentsAvailableRequest request);

// Get the list of possible auxiliary extents images.
SvPb::InspectionCmdResponse getAvailableAuxImages(SvPb::GetAvailableAuxImagesRequest request);

// Get the connected auxiliary extents image.
SvPb::InspectionCmdResponse getAuxImageObject(SvPb::GetAuxImageObjectRequest request);

// Set the connected auxiliary extents image.
SvPb::InspectionCmdResponse setAuxImageObject(SvPb::SetAuxImageObjectRequest request);

// Get the connected input of this object.
SvPb::InspectionCmdResponse getInputs(SvPb::GetInputsRequest request);

// Connect an object to an input.
SvPb::InspectionCmdResponse connectToObject(SvPb::ConnectToObjectRequest request);

// Save an image to a file.
SvPb::InspectionCmdResponse saveImage(SvPb::SaveImageRequest request);

//Set object name.
SvPb::InspectionCmdResponse setObjectName(SvPb::SetObjectNameRequest request);

//Get a list of available objects.
SvPb::InspectionCmdResponse getAvailableObjects(SvPb::GetAvailableObjectsRequest request);

//Get a name list of special image of a task.
SvPb::InspectionCmdResponse getSpecialImageList(SvPb::GetSpecialImageListRequest request);

//Export the mask to a file.
SvPb::InspectionCmdResponse exportMask(SvPb::ExportMaskRequest request);

//Import the mask from a file.
SvPb::InspectionCmdResponse importMask(SvPb::ImportMaskRequest request);

//Get ID of a object
SvPb::InspectionCmdResponse getObjectId(SvPb::GetObjectIdRequest request);

//Create a new object and insert it in the friend- or taskObject-list.
SvPb::InspectionCmdResponse createObject(SvPb::CreateObjectRequest request);

//Get the creatable Objects based on the type.
SvPb::InspectionCmdResponse getCreatableObjects(SvPb::GetCreatableObjectsRequest request);

//Return if the inspection should be reset if the task has changed. (in the first step it works only with filters.)
SvPb::InspectionCmdResponse shouldInspectionReset(SvPb::ShouldInspectionResetRequest request);

//Get the ppqName of the inspection
SvPb::InspectionCmdResponse getPPQName(SvPb::GetPPQNameRequest request);

SvPb::InspectionCmdResponse getValueObjectEnums(SvPb::GetValueObjectEnumsRequest request);

SvPb::InspectionCmdResponse getEmbeddedValues(SvPb::GetEmbeddedValuesRequest request);

SvPb::InspectionCmdResponse setEmbeddedValues(SvPb::SetEmbeddedValuesRequest request);

SvPb::InspectionCmdResponse getOutputRectangle(SvPb::GetOutputRectangleRequest request);

SvPb::InspectionCmdResponse getToolSizeAdjustParameter(SvPb::GetToolSizeAdjustParameterRequest request);

SvPb::InspectionCmdResponse getExtentParameter(SvPb::GetExtentParameterRequest request);

SvPb::InspectionCmdResponse setExtentParameter(SvPb::SetExtentParameterRequest request);
	
SvPb::InspectionCmdResponse isAllowedLocation(SvPb::IsAllowedLocationRequest request);

SvPb::InspectionCmdResponse propagateSizeAndPosition(SvPb::PropagateSizeAndPositionRequest request);

SvPb::InspectionCmdResponse usePropagateSizeAndPosition(SvPb::UsePropagateSizeAndPositionRequest request);

SvPb::InspectionCmdResponse getOverlayStruct(SvPb::GetOverlayStructRequest request);

SvPb::InspectionCmdResponse getObjectSelectorItems(SvPb::GetObjectSelectorItemsRequest request);
SvPb::InspectionCmdResponse setDefaultInputsRequest(SvPb::SetDefaultInputsRequest request);

std::vector<SvPb::TreeItem> getSelectorList(SvPb::GetObjectSelectorItemsRequest request, SvPb::ObjectSelectorType selectorType);
} //namespace SvCmd