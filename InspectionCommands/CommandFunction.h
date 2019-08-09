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
struct InspectionCmdResult
{
	InspectionCmdResult() = default;
	~InspectionCmdResult() = default;

	SvPb::InspectionCmdMsgs m_response;
	HRESULT m_hResult {S_OK};
};

/// Do a runOnce for an inspection or taskObject.
/// \param request [in] a request-protoBuf-message.
/// \returns InspectionCmdResult
InspectionCmdResult InspectionRunOnce(SvPb::InspectionRunOnceRequest request);

/// Destroyed a child object.
/// \param request [in] a request-protoBuf-message.
/// \returns InspectionCmdResult
InspectionCmdResult DestroyChildObject(SvPb::DestroyChildRequest request);

/// Get the last (Error)MessageList from an taskObject.
/// \param request [in] The request-protoBuf-message.
/// \returns InspectionCmdResult
InspectionCmdResult GetMessageList(SvPb::GetMessageListRequest request);

/// Reset an object.
/// \param request [in] The request-protoBuf-message.
/// \returns InspectionCmdResult
InspectionCmdResult ResetObject(SvPb::ResetObjectRequest request);

/// Create a model for the pattern-analyzer. The model will copy from the current source image of the analyzer.
/// \param request [in] The request-protoBuf-message.
/// \returns InspectionCmdResult
InspectionCmdResult CreateModel(SvPb::CreateModelRequest request);

//Return the parameter of an object.
InspectionCmdResult getObjectParameters(SvPb::GetObjectParametersRequest request);

/// Return the equation string.
/// \param request [in] The request-protoBuf-message.
/// \returns InspectionCmdResult
InspectionCmdResult GetEquation(SvPb::GetEquationRequest request);

/// Validate and if required set an equation. Return the status and if successfully the result.
/// \param request [in] The request-protoBuf-message.
/// \returns InspectionCmdResult
InspectionCmdResult ValidateAndSetEquation(SvPb::ValidateAndSetEquationRequest request);

/// Return a list of object which should be added to the monitor list (e.g. HMI purpose)
/// \param request [in] The request-protoBuf-message.
/// \returns InspectionCmdResult
InspectionCmdResult getObjectsForMonitorList(SvPb::GetObjectsForMonitorListRequest request);

/// Moved a taskObject to a new position in the taskObject- or friend-list.
/// \param request [in] The request-protoBuf-message.
/// \returns InspectionCmdResult
InspectionCmdResult MoveObject(SvPb::MoveObjectRequest request);

/// Get Definitions for all Taskobject in taskobjectlist
InspectionCmdResult GetTaskObjectsList(SvPb::TaskObjectListRequest request);

/// Get an required image
InspectionCmdResult getImage(SvPb::GetImageRequest request);

// Get if Auxiliary extents are available
InspectionCmdResult areAuxiliaryExtentsAvailable(SvPb::AreAuxiliaryExtentsAvailableRequest request);

// Get the list of possible auxiliary extents images.
InspectionCmdResult getAvailableAuxImages(SvPb::GetAvailableAuxImagesRequest request);

// Get the connected auxiliary extents image.
InspectionCmdResult getAuxImageObject(SvPb::GetAuxImageObjectRequest request);

// Set the connected auxiliary extents image.
InspectionCmdResult setAuxImageObject(SvPb::SetAuxImageObjectRequest request);

// Get the connected input of this object.
InspectionCmdResult getInputs(SvPb::GetInputsRequest request);

// Connect an object to an input.
InspectionCmdResult connectToObject(SvPb::ConnectToObjectRequest request);

// Save an image to a file.
InspectionCmdResult saveImage(SvPb::SaveImageRequest request);

//Set object name.
InspectionCmdResult setObjectName(SvPb::SetObjectNameRequest request);

//Get a list of available objects.
InspectionCmdResult getAvailableObjects(SvPb::GetAvailableObjectsRequest request);

//Get a name list of special image of a task.
InspectionCmdResult getSpecialImageList(SvPb::GetSpecialImageListRequest request);

//Export the mask to a file.
InspectionCmdResult exportMask(SvPb::ExportMaskRequest request);

//Import the mask from a file.
InspectionCmdResult importMask(SvPb::ImportMaskRequest request);

//Get GUID of a object
InspectionCmdResult getObjectId(SvPb::GetObjectIdRequest request);

//Construct a new object and insert it in the friend- or taskObject-list.
InspectionCmdResult constructAndInsert(SvPb::ConstructAndInsertRequest request);

//Get the creatable Objects based on the type.
InspectionCmdResult getCreatableObjects(SvPb::GetCreatableObjectsRequest request);

//Return if the inspection should be reset if the task has changed. (in the first step it works only with filters.)
InspectionCmdResult shouldInspectionReset(SvPb::ShouldInspectionResetRequest request);

//Get the ppqName of the inspection
InspectionCmdResult getPPQName(SvPb::GetPPQNameRequest request);

InspectionCmdResult getValueObjectEnums(SvPb::GetValueObjectEnumsRequest request);

InspectionCmdResult getEmbeddedValues(SvPb::GetEmbeddedValuesRequest request);

InspectionCmdResult setEmbeddedValues(SvPb::SetEmbeddedValuesRequest request);

InspectionCmdResult getOutputRectangle(SvPb::GetOutputRectangleRequest request);

InspectionCmdResult getToolSizeAdjustParameter(SvPb::GetToolSizeAdjustParameterRequest request);

InspectionCmdResult getExtentParameter(SvPb::GetExtentParameterRequest request);

InspectionCmdResult setExtentParameter(SvPb::SetExtentParameterRequest request);
	
InspectionCmdResult isAllowedLocation(SvPb::IsAllowedLocationRequest request);

InspectionCmdResult propagateSizeAndPosition(SvPb::PropagateSizeAndPositionRequest request);

InspectionCmdResult usePropagateSizeAndPosition(SvPb::UsePropagateSizeAndPositionRequest request);

InspectionCmdResult getOverlayStruct(SvPb::GetOverlayStructRequest request);

InspectionCmdResult getObjectSelectorItems(SvPb::GetObjectSelectorItemsRequest request);

std::vector<SvPb::TreeItem> getSelectorList(SvPb::GetObjectSelectorItemsRequest request, SvPb::ObjectSelectorType selectorType);
} //namespace SvCmd