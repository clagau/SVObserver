//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file CommandFunction.cpp
/// All Rights Reserved
//*****************************************************************************
/// This file contains the intern commands to the inspection.
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "CommandFunction.h"
#include "ObjectInterfaces\IAnalyzer.h"
#include "ObjectInterfaces\IBarCode.h"
#include "ObjectInterfaces\IBlobAnalyzer2.h"
#include "ObjectInterfaces\IConfigurationObject.h"
#include "ObjectInterfaces\IEnumerateValueObject.h"
#include "ObjectInterfaces\IEquation.h"
#include "ObjectInterfaces\IFormulaController.h"
#include "ObjectInterfaces\ISVFilter.h"
#include "ObjectInterfaces\IInspectionProcess.h"
#include "ObjectInterfaces\IInputObject.h"
#include "ObjectInterfaces\IMask.h"
#include "ObjectInterfaces\IObjectClass.h"
#include "ObjectInterfaces\IParameterTask.h"
#include "ObjectInterfaces\IRootObject.h"
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\ITaskObject.h"
#include "ObjectInterfaces\ITaskObjectListClass.h"
#include "ObjectInterfaces\ITool.h"
#include "ObjectInterfaces\IToolSizeAdjustTask.h"
#include "ObjectInterfaces\IObjectAppClass.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\IPatternAnalyzer.h"
#include "ObjectInterfaces\IBlobAnalyzer.h"
#include "ObjectInterfaces\IExternalToolTask.h"
#include "ObjectInterfaces\IExternalToolTaskDataAdmin.h"
#include "ObjectInterfaces\IInputValueDefinition.h"
#include "ObjectInterfaces\IResultValueDefinition.h"
#include "Definitions\ObjectDefines.h"
#include "Definitions\TextDefinesSvDef.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/MessageTextGenerator.h"
#include "SVStatusLibrary\SvimState.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "FilesystemUtilities/ImageFileFormats.h"
#include "SVMatroxLibrary/SVMatroxPatternInterface.h"
#include "SVMessage/SVMessage.h"
#include "SVUtilityLibrary\SVGUID.h"
#include "SVObjectLibrary\SVObjectBuilder.h"
#include "SVObjectLibrary\SVObjectClass.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVObjectLibrary\DependencyManager.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "CommandInternalHelper.h"
#include "ObjectSelectorFilter.h"
#include "ObjectInterfaces\ILinkedObject.h"
#include "ObjectInterfaces\ISVLinearAnalyzer.h"
#include <atltypes.h>
#include "Tools\GroupTool.h"
#pragma endregion Includes

namespace SvCmd
{
namespace
{
auto addEmbeddedListEntry(SvPb::GetEmbeddedValuesResponse& rResp, SvOi::IObjectClass& rObject, bool isExternallySettable)
{
	auto* pElement = rResp.add_list();
	pElement->set_objectid(rObject.getObjectId());
	pElement->set_embeddedid(rObject.GetEmbeddedID());
	pElement->set_is_readonly(false == isExternallySettable);
	return pElement;
}

void addTaskInputImage(uint32_t taskObjectID, SvPb::EmbeddedIdEnum embeddedID, SvPb::GetSourceResultImageListResponse* pSourceResultImageResponse)
{
	SvPb::ObjectNameIdPair result;
	SvPb::GetInputDataRequest requestCmd;
	requestCmd.set_objectid(taskObjectID);
	requestCmd.set_embeddedid(embeddedID);
	requestCmd.set_desired_first_object_type_for_connected_name(SvPb::SVToolSetObjectType);
	requestCmd.set_exclude_first_object_name_in_conntected_name(true);
	SvPb::InspectionCmdResponse responseGetInputData = getInputData(requestCmd);
	if (S_OK == responseGetInputData.hresult() && responseGetInputData.has_getinputdataresponse())
	{
		auto pIObject = SvOi::getObject(responseGetInputData.getinputdataresponse().data().inputid());
		if (nullptr != pIObject && pIObject->ObjectAttributesAllowed() != SvPb::noAttributes)
		{
			result.set_objectid(responseGetInputData.getinputdataresponse().data().connected_objectid());
			result.set_objectname(responseGetInputData.getinputdataresponse().data().inputname());
			auto* pSourceImageList = pSourceResultImageResponse->add_sourceimagelist();
			*pSourceImageList = result;
		}
	}
}

void addTaskOutputImage(uint32_t taskObjectID, SvPb::EmbeddedIdEnum embeddedID, SvPb::GetSourceResultImageListResponse* pSourceResultImageResponse)
{
	SvPb::ObjectNameIdPair result;
	SvPb::GetObjectIdRequest requestCmd;
	auto* pRequest = requestCmd.mutable_info();
	pRequest->set_ownerid(taskObjectID);
	auto* pInfo = pRequest->mutable_infostruct();
	pInfo->set_objecttype(SvPb::SVImageObjectType);
	pInfo->set_embeddedid(embeddedID);
	SvPb::InspectionCmdResponse response = getObjectId(requestCmd);
	if (S_OK == response.hresult() && response.has_getobjectidresponse())
	{
		auto pIObject = SvOi::getObject(response.getobjectidresponse().objectid());
		if (nullptr != pIObject && pIObject->ObjectAttributesAllowed() != SvPb::noAttributes)
		{
			result.set_objectid(response.getobjectidresponse().objectid());
			result.set_objectname(pIObject->GetObjectNameBeforeObjectType(SvPb::SVToolObjectType));
			auto* pResultImageList = pSourceResultImageResponse->add_resultimagelist();
			*pResultImageList = result;
		}
	}
}

void addColorToolImages(uint32_t taskObjectID, SvPb::GetSourceResultImageListResponse* pSourceResultImageResponse)
{
	std::vector<uint32_t> taskList;
	taskList.push_back(taskObjectID);
	SvPb::GetObjectIdRequest requestCmd;
	auto* pRequest = requestCmd.mutable_info();
	pRequest->set_ownerid(taskObjectID);
	auto* pInfo = pRequest->mutable_infostruct();
	pInfo->set_objecttype(SvPb::SVOperatorObjectType);
	pInfo->set_subtype(SvPb::SVColorThresholdObjectType);
	SvPb::InspectionCmdResponse response = getObjectId(requestCmd);
	if (S_OK == response.hresult() && response.has_getobjectidresponse())
	{
		taskList.push_back(response.getobjectidresponse().objectid());
	}
	addTaskInputImage(taskObjectID, SvPb::ImageInputEId, pSourceResultImageResponse);
	for (auto objectID : taskList)
	{
		constexpr std::array<SvPb::EmbeddedIdEnum, 4> colorOutputImageList {SvPb::Band0ImageEId, SvPb::Band1ImageEId, SvPb::Band2ImageEId, SvPb::OutputImageEId};
		for (const auto& rEmbeddedID : colorOutputImageList)
		{
			addTaskOutputImage(objectID, rEmbeddedID, pSourceResultImageResponse);
		}
	}
}
}

SvPb::InspectionCmdResponse InspectionRunOnce(SvPb::InspectionRunOnceRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;
	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*>(SvOi::getObject(request.inspectionid()));
	if (nullptr != pInspection)
	{
		cmdResponse.set_hresult(pInspection->RunOnce());
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse DeleteObject(SvPb::DeleteObjectRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	if (nullptr != pObject)
	{
		DWORD flag = 0;
		switch (request.flag())
		{
			case SvPb::DeleteObjectRequest::Flag_SetDefaultInputs:
				flag = SvDef::SVMFSetDefaultInputs;
				break;
			case SvPb::DeleteObjectRequest::Flag_ResetInspection:
				flag = SvDef::SVMFResetInspection;
				break;
			case SvPb::DeleteObjectRequest::Flag_SetDefaultInputs_And_ResetInspection:
				flag = SvDef::SVMFSetDefaultInputs | SvDef::SVMFResetInspection;
				break;
			default:
				flag = 0;
				break;
		}
		SvOi::IObjectClass* pParent = SvOi::getObject(pObject->GetParentID());
		auto* pTaskObjectList = dynamic_cast<SvOi::ITaskObjectListClass*> (pParent);
		auto* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (pObject);
		if (SvimState::getModuleEditingId() == pObject->getObjectId())
		{
			SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ActionNotPossibleInModuleEditingMode, {SVObjectManagerClass::Instance().GetCompleteObjectName(SvimState::getModuleEditingId())}, SvStl::SourceFileParams(StdMessageParams));
			SvPb::convertMessageToProtobuf(message, cmdResponse.mutable_errormessage()->add_messages());
			cmdResponse.set_hresult(E_FAIL);
			return cmdResponse;
		}
		if (nullptr != pTaskObject && nullptr != pTaskObjectList)
		{
			pTaskObjectList->DestroyChild(*pTaskObject, flag);
			pParent->resetAllObjects();
		}
	}
	else
	{
		cmdResponse.set_hresult(E_FAIL);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse GetMessageList(SvPb::GetMessageListRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ITaskObject* pTask = dynamic_cast<SvOi::ITaskObject*>(SvOi::getObject(request.objectid()));
	if (pTask)
	{
		cmdResponse.mutable_errormessage()->CopyFrom(SvPb::convertMessageVectorToProtobuf(pTask->getErrorMessages()));
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse ResetObject(SvPb::ResetObjectRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());

	if (nullptr != pObject)
	{
		SvStl::MessageContainerVector messageContainers;
		HRESULT result = pObject->resetAllObjects(&messageContainers) ? S_OK : E_FAIL;
		cmdResponse.set_hresult(result);
		cmdResponse.mutable_errormessage()->CopyFrom(SvPb::convertMessageVectorToProtobuf(messageContainers));
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse CreateModel(SvPb::CreateModelRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvStl::MessageContainer message;
	SvOi::IPatternAnalyzer* pPatAnalyzer = dynamic_cast<SvOi::IPatternAnalyzer*>(SvOi::getObject(request.patternanalyzerid()));

	if (nullptr != pPatAnalyzer && pPatAnalyzer->UpdateModelFromInputImage(request.posx(), request.posy(), request.modelwidth(), request.modelheight()))
	{
		SvOi::SVImageBufferHandlePtr imageHandle;
		SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*>(pPatAnalyzer);
		if (nullptr != pTaskObject && pTaskObject->getSpecialImage(SvDef::PatternModelImageName, imageHandle) && !imageHandle->empty())
		{
			std::string FileName = request.filename();
			// Now save the Model Image buffer to a file
			ImageFileFormat fileFormat = inferredMilImageFileFormat(FileName);

			SVMatroxBuffer milBuffer = imageHandle->GetBuffer();
			HRESULT result = SVMatroxBufferInterface::Export(milBuffer, FileName, fileFormat);
			if (S_OK != result)
			{
				cmdResponse.set_hresult(E_FAIL);
				message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), request.patternanalyzerid());
			}
		}
		else
		{
			cmdResponse.set_hresult(E_FAIL);
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), request.patternanalyzerid());
		}
	}
	else
	{
		cmdResponse.set_hresult(E_FAIL);
		message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), request.patternanalyzerid());
	}

	if (S_OK != cmdResponse.hresult())
	{
		SvPb::convertMessageToProtobuf(message, cmdResponse.mutable_errormessage()->add_messages());
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getObjectParameters(SvPb::GetObjectParametersRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	if (nullptr != pObject)
	{
		SvPb::GetObjectParametersResponse* pObjParamResponse = cmdResponse.mutable_getobjectparametersresponse();
		pObjParamResponse->set_name(pObject->GetName());
		pObjParamResponse->set_allowedattributes(pObject->ObjectAttributesAllowed());
		pObjParamResponse->mutable_typeinfo()->set_objecttype(pObject->GetObjectType());
		pObjParamResponse->mutable_typeinfo()->set_subtype(pObject->GetObjectSubType());
		SvOi::ITaskObject* pTask = dynamic_cast<SvOi::ITaskObject*>(pObject);
		if (nullptr != pTask)
		{
			pObjParamResponse->set_isvalid(pTask->isErrorMessageEmpty());
		}
		pObjParamResponse->set_embeddedid(pObject->GetEmbeddedID());
		pObjParamResponse->set_comment(pObject->getComment());
		pObjParamResponse->set_classid(pObject->GetClassID());
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse GetEquation(SvPb::GetEquationRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IEquation* pEquation = dynamic_cast<SvOi::IEquation*> (SvOi::getObject(request.objectid()));
	if (pEquation)
	{
		SvPb::GetEquationResponse* pResponse = cmdResponse.mutable_getequationresponse();
		pResponse->set_equationtext(pEquation->GetEquationText());
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse ValidateAndSetEquation(SvPb::ValidateAndSetEquationRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IEquation* pEquation = dynamic_cast<SvOi::IEquation*> (SvOi::getObject(request.objectid()));
	if (nullptr != pEquation)
	{
		std::string oldString;
		//save old string
		oldString = pEquation->GetEquationText();
		pEquation->SetEquationText(request.equationtext());
		SvStl::MessageContainerVector messageContainers;
		SvOi::EquationTestResult testResult = pEquation->Test(&messageContainers);

		SvPb::ValidateAndSetEquationResponse* pResponse = cmdResponse.mutable_validateandsetequationresponse();
		cmdResponse.mutable_errormessage()->CopyFrom(SvPb::convertMessageVectorToProtobuf(messageContainers));
		int retValue = 0;
		if (testResult.bPassed)
		{// set result and set return value to successful
			pResponse->set_result(pEquation->GetYACCResult());
			retValue = SvOi::IFormulaController::validateSuccessful;
		}
		else
		{  // set return value to position of failed
			retValue = testResult.iPositionFailed;
		}
		pResponse->set_validatestatus(retValue);
		if (!request.bsetvalue() || SvOi::IFormulaController::validateSuccessful != retValue)
		{
			//reset old string
			pEquation->SetEquationText(oldString);
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getObjectsForMonitorList(SvPb::GetObjectsForMonitorListRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*>(SvOi::getObject(request.objectid()));
	if (nullptr != pTool)
	{
		SvStl::MessageContainerVector messageContainers;
		SvOi::ParametersForML paramList = pTool->getParameterForMonitorList(messageContainers);
		SvPb::GetObjectsForMonitorListResponse* pResponse = cmdResponse.mutable_getobjectsformonitorlistresponse();
		cmdResponse.mutable_errormessage()->CopyFrom(SvPb::convertMessageVectorToProtobuf(messageContainers));
		for (auto& item : paramList)
		{
			auto* pEntry = pResponse->add_list();
			pEntry->set_objectname(item.first.c_str());
			pEntry->set_objectid(item.second);
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}

	return cmdResponse;
}

SvPb::InspectionCmdResponse MoveObject(SvPb::MoveObjectRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.parentid());
	if (nullptr != pObject)
	{
		switch (request.listmode())
		{
			case SvPb::MoveObjectRequest::TaskObjectList:
			{
				SvOi::ITaskObjectListClass* pTaskObject = dynamic_cast<SvOi::ITaskObjectListClass*>(pObject);
				if (nullptr != pTaskObject)
				{
					pTaskObject->moveTaskObject(request.objectid(), request.movepreid());
				}
				else
				{
					cmdResponse.set_hresult(E_FAIL);
				}
				break;
			}
			case SvPb::MoveObjectRequest::FriendList:
			{
				auto* pTask = dynamic_cast<SvOi::ITaskObject*>(pObject);
				if (nullptr != pTask)
				{
					pTask->moveFriendObject(request.objectid(), request.movepreid());
				}
				else
				{
					cmdResponse.set_hresult(E_FAIL);
				}
			}
			break;
			default:
				cmdResponse.set_hresult(E_FAIL);
				break;
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}

	return cmdResponse;
}

SvPb::InspectionCmdResponse GetTaskObjectsList(SvPb::TaskObjectListRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ITaskObjectListClass* pTaskObjectList = dynamic_cast<SvOi::ITaskObjectListClass*>(SvOi::getObject(request.taskobjectid()));
	if (nullptr != pTaskObjectList)
	{
		SvPb::TaskObjectListResponse* pResponse = cmdResponse.mutable_taskobjectlistresponse();
		pTaskObjectList->GetTaskObjectListInfo(*pResponse);
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getImage(SvPb::GetImageRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;
	SvOi::SVImageBufferHandlePtr data;

	SvPb::GetImageResponse* pResponse = cmdResponse.mutable_getimageresponse();
	if (SvDef::InvalidObjectId != request.imageid())
	{
		SvOi::ISVImage* pImage;
		auto* pObject = SvOi::getObject(request.imageid());
		SvOi::ILinkedObject* pLinked = dynamic_cast<SvOi::ILinkedObject*>(pObject);
		if (nullptr != pLinked)
		{
			try
			{
				pImage = dynamic_cast<SvOi::ISVImage*>(const_cast<SvOi::IObjectClass*>(pLinked->getLinkedObject()));
			}
			catch (const SvStl::MessageContainer& rExp)
			{
				SvPb::convertMessageToProtobuf(rExp, cmdResponse.mutable_errormessage()->add_messages());
				cmdResponse.set_hresult(E_FAIL);
				return cmdResponse;
			}
			catch (...)
			{
				cmdResponse.set_hresult(E_FAIL);
				return cmdResponse;
			}
		}
		else
		{
			pImage = dynamic_cast<SvOi::ISVImage*>(pObject);
		}

		if (pImage)
		{
			data = pImage->getLastImage();
		}
	}
	else if (SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject*>(SvOi::getObject(request.parentid()));  nullptr != pObject)
	{
		if (SvPb::NoEmbeddedId != request.embeddedid())
		{
			if (!pObject->getImage(request.embeddedid(), data))
			{
				SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams));
				SvPb::convertMessageToProtobuf(message, cmdResponse.mutable_errormessage()->add_messages());
				cmdResponse.set_hresult(E_FAIL);
				return cmdResponse;
			}
		}
		else if (false == request.imagename().empty())
		{
			if (!pObject->getSpecialImage(request.imagename(), data))
			{
				SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams));
				SvPb::convertMessageToProtobuf(message, cmdResponse.mutable_errormessage()->add_messages());
				cmdResponse.set_hresult(E_FAIL);
				return cmdResponse;
			}
		}
	}

	SvOi::SVImageBufferHandleInterface* pImageData = data.get();
	if (nullptr != pImageData && !pImageData->empty())
	{
		std::string ImageBuffer;
		SVBitmapInfo bitmapInfo;
		cmdResponse.set_hresult(SVMatroxBufferInterface::CopyBufferToFileDIB(ImageBuffer, bitmapInfo, pImageData->GetBuffer()));
		pResponse->mutable_imagedata()->mutable_rgbdata()->swap(ImageBuffer);
		pResponse->mutable_imagedata()->set_width(std::abs(bitmapInfo.GetWidth()));
		pResponse->mutable_imagedata()->set_height(std::abs(bitmapInfo.GetHeight()));
	}
	else
	{
		SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams));
		SvPb::convertMessageToProtobuf(message, cmdResponse.mutable_errormessage()->add_messages());
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}



SvPb::InspectionCmdResponse areAuxiliaryExtentsAvailable(SvPb::AreAuxiliaryExtentsAvailableRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*>(SvOi::getObject(request.objectid()));
	if (nullptr != pTool)
	{
		SvPb::AreAuxiliaryExtentsAvailableResponse* pResponse = cmdResponse.mutable_areauxiliaryextentsavailableresponse();
		pResponse->set_areavailable(pTool->areAuxExtentsAvailable());
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getAvailableAncestorImages(SvPb::GetAvailableAncestorImagesRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*> (SvOi::getObject(request.objectid()));
	if (nullptr != pTool)
	{
		SvPb::GetAvailableAncestorImagesResponse* pResponse = cmdResponse.mutable_getavailableancestorimagesresponse();
		*pResponse = pTool->getAvailableAncestorImages();
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getInputs(SvPb::GetInputsRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (SvOi::getObject(request.objectid()));
	if (nullptr != pTaskObject)
	{
		SvDef::SVObjectTypeInfoStruct typeInfo {request.typeinfo().objecttype(), request.typeinfo().subtype(), request.typeinfo().embeddedid()};
		SvPb::GetInputsResponse* pResponse = cmdResponse.mutable_getinputsresponse();
		pTaskObject->GetInputs(*pResponse->mutable_list(), typeInfo, request.desired_first_object_type_for_connected_name(), request.exclude_first_object_name_in_conntected_name(), request.maxrequested());
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getInputData(const SvPb::GetInputDataRequest& request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	auto* pObject = SvOi::getObject(request.objectid());
	if (SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (pObject); nullptr != pTaskObject)
	{
		pTaskObject->getInputData(request, cmdResponse);
	}
	else if (auto* pInputObject = dynamic_cast<SvOi::IInputObject*>(pObject); nullptr != pInputObject)
	{
		cmdResponse.mutable_getinputdataresponse()->CopyFrom(pInputObject->getInputData(request.desired_first_object_type_for_connected_name(), request.exclude_first_object_name_in_conntected_name()));
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse connectToObject(SvPb::ConnectToObjectRequest request)
{
	SvPb::InspectionCmdResponse result;

	auto* pObject = SvOi::getObject(request.objectid());
	if (SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (pObject); nullptr != pTaskObject)
	{
		result.set_hresult(pTaskObject->connectToObject(request));
	}
	else if (auto* pInputObject = dynamic_cast<SvOi::IInputObject*>(pObject); nullptr != pInputObject)
	{
		result.set_hresult(pInputObject->checkAndSetInput(request.newconnectedid()));
	}
	else
	{
		result.set_hresult(E_POINTER);
	}
	return result;
}

SvPb::InspectionCmdResponse saveImage(SvPb::SaveImageRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ISVImage* pObject = dynamic_cast<SvOi::ISVImage*>(SvOi::getObject(request.objectid()));
	if (pObject)
	{
		cmdResponse.set_hresult(pObject->Save(request.imagename()));
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse setObjectName(SvPb::SetObjectNameRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	std::string newName = SvUl::RemoveCharactersByRegexAndTrim(request.objectname(), SvDef::cPatternAllExceptAlnumUnderscoreAndBlank);
	if (newName != request.objectname())
	{
		SvStl::MessageContainer message;
		message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NameContainsInvalidChars, SvStl::SourceFileParams(StdMessageParams));
		SvPb::convertMessageToProtobuf(message, cmdResponse.mutable_errormessage()->add_messages());
		cmdResponse.set_hresult(E_FAIL);
		return cmdResponse;
	}

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	if (nullptr != pObject)
	{
		auto errorMessages = pObject->verifyAndSetName(newName);
		if (false == errorMessages.empty())
		{
			cmdResponse.mutable_errormessage()->CopyFrom(SvPb::convertMessageVectorToProtobuf(errorMessages));
			cmdResponse.set_hresult(E_FAIL);
		}
	}
	else
	{
		SvDef::StringVector msgList;
		msgList.emplace_back(std::to_string(request.objectid()));
		SvStl::MessageContainer message;
		message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ObjectNotFound, msgList, SvStl::SourceFileParams(StdMessageParams));
		SvPb::convertMessageToProtobuf(message, cmdResponse.mutable_errormessage()->add_messages());
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getAvailableObjects(SvPb::GetAvailableObjectsRequest request)
{
	switch (request.SearchType_case())
	{
		case SvPb::GetAvailableObjectsRequest::kTreeSearch:
		{
			const auto& parameter = request.tree_search();
			auto [startingObjectId, isStopAtBorder] = correctStartId(parameter.pre_search_start_id(), parameter.search_start_id());

			return getAvailableObjects(startingObjectId, {parameter.type_info().objecttype(), parameter.type_info().subtype(), parameter.type_info().embeddedid()},
				parameter.has_defaultplushidden(), isStopAtBorder, request.desired_first_object_type_for_name(), request.exclude_first_object_name_in_name(),
				getAllowedFunc(parameter));
		}
		case SvPb::GetAvailableObjectsRequest::kInputSearch:
		{
			const auto& parameter = request.input_search();
			auto* pInputObject = dynamic_cast<SvOi::IInputObject*>(SvOi::getObject(parameter.input_connected_objectid()));
			if (nullptr != pInputObject)
			{
				auto [infoType, preStartingObjectId, allowedId, toolId] = pInputObject->getAvailableObjectParameter();
				auto [startingObjectId, isStopAtBorder] = correctStartId(parameter.input_connected_objectid(), preStartingObjectId);
				SvPb::TreeSearchParameters treeParameter;
				switch (allowedId)
				{
					case SvOi::InputAllowedMode::IsBeforeTool:
					{
						treeParameter.mutable_isbeforetoolmethod()->set_toolid(toolId);
						break;
					}
					case SvOi::InputAllowedMode::DefaultPlusHidden:
						treeParameter.mutable_defaultplushidden();
						break;
					case SvOi::InputAllowedMode::HasExtension:
						treeParameter.mutable_hasextension();
						break;
					default:
						break;
				}
				return getAvailableObjects(startingObjectId, infoType,
					SvOi::InputAllowedMode::DefaultPlusHidden == allowedId, isStopAtBorder, request.desired_first_object_type_for_name(), request.exclude_first_object_name_in_name(),
					getAllowedFunc(treeParameter));
			}
			else
			{
				SvPb::InspectionCmdResponse cmdResponse;
				cmdResponse.set_hresult(E_NOINTERFACE);
				return cmdResponse;
			}
		}
		default:
		{
			SvPb::InspectionCmdResponse cmdResponse;
			cmdResponse.set_hresult(E_NOINTERFACE);
			return cmdResponse;
		}
	}

}

SvPb::InspectionCmdResponse getSpecialImageList(SvPb::GetSpecialImageListRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	const SvOi::ITaskObject* pObject = dynamic_cast<const SvOi::ITaskObject*>(SvOi::getObject(request.taskobjectid()));
	if (nullptr != pObject)
	{
		SvDef::StringVector specialImageList = pObject->getSpecialImageList();
		SvPb::GetSpecialImageListResponse* pResponse = cmdResponse.mutable_getspecialimagelistresponse();
		for (auto tmp : specialImageList)
		{
			pResponse->add_specialimagenames(tmp);
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse exportMask(SvPb::ExportMaskRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IMask* pObject = dynamic_cast<SvOi::IMask*>(SvOi::getObject(request.objectid()));
	if (nullptr != pObject)
	{
		cmdResponse.set_hresult(pObject->Export(request.filename()));
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse importMask(SvPb::ImportMaskRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IMask* pObject = dynamic_cast<SvOi::IMask*>(SvOi::getObject(request.objectid()));
	if (pObject)
	{
		cmdResponse.set_hresult(pObject->Import(request.filename()));
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getObjectId(SvPb::GetObjectIdRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = nullptr;
	switch (request.message_case())
	{
		case SvPb::GetObjectIdRequest::kName:
		{
			pObject = SvOi::getObjectByDottedName(request.name());
			break;
		}
		case SvPb::GetObjectIdRequest::kInfo:
		{
			auto& rInfoStruct = request.info().infostruct();
			SvDef::SVObjectTypeInfoStruct typeInfo {rInfoStruct.objecttype(), rInfoStruct.subtype(), rInfoStruct.embeddedid()};
			pObject = SvOi::FindObject(request.info().ownerid(), typeInfo);
			break;
		}
		default:break;
	}
	if (pObject)
	{
		SvPb::GetObjectIdResponse* pResponse = cmdResponse.mutable_getobjectidresponse();
		pResponse->set_objectid(pObject->getObjectId());
		pResponse->set_classid(pObject->GetClassID());
		pResponse->mutable_typeinfo()->set_objecttype(pObject->GetObjectType());
		pResponse->mutable_typeinfo()->set_subtype(pObject->GetObjectSubType());
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse createObject(SvPb::CreateObjectRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject {nullptr};
	switch (request.message_case())
	{
		case SvPb::CreateObjectRequest::kFriend:
		{
			pObject = SVObjectBuilder::CreateFriendObject(request.classid(), SvDef::InvalidObjectId, request.friend_().name(), request.ownerid(), request.friend_().preid());
			break;
		}
		case SvPb::CreateObjectRequest::kTaskObjectInsertBeforeId:
		case SvPb::CreateObjectRequest::kTaskObjectPos:
		{
			SvOi::IObjectClass* pParent = SvOi::getObject(request.ownerid());
			SvOi::ITaskObjectListClass* pParentTaskObjectList = dynamic_cast<SvOi::ITaskObjectListClass*>(pParent);
			SvOi::IObjectAppClass* pObjectApp = dynamic_cast<SvOi::IObjectAppClass*>(pParentTaskObjectList);

			bool notAddedYet = true;
			if (SvPb::ModuleToolClassId != request.classid())
			{
				pObject = SvOi::ConstructObject(request.classid());
			}
			else
			{
				try
				{
					pObject = SvOi::ConstructAndAddModuleInstance(request.index(), request.ownerid());
					notAddedYet = false;
				}
				catch (const SvStl::MessageContainer& rMsg)
				{
					cmdResponse.set_hresult(E_FAIL);
					SvPb::convertMessageToProtobuf(rMsg, cmdResponse.mutable_errormessage()->add_messages());
				}
			}
			SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (pObject);

			if (nullptr != pParentTaskObjectList && nullptr != pTaskObject && nullptr != pObjectApp && nullptr != pObject)
			{
				if (SvPb::SVToolObjectType == pObject->GetObjectType() &&
				(SvPb::GroupToolObjectType == pObject->GetObjectSubType() || SvPb::LoopToolObjectType == pObject->GetObjectSubType()))
				{
					auto depth = pParentTaskObjectList->getToolDepth();
					if (SvDef::c_maxLoopGroupDepth <= depth)
					{
						delete pTaskObject;
						cmdResponse.set_hresult(E_FAIL);
						SvStl::MessageContainer message;
						message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_MaxDepthOfToolsError, SvStl::SourceFileParams(StdMessageParams));
						SvPb::convertMessageToProtobuf(message, cmdResponse.mutable_errormessage()->add_messages());
						break;
					}
				}

				if (notAddedYet)
				{
					if (SvPb::CreateObjectRequest::kTaskObjectInsertBeforeId == request.message_case())
					{
						pParentTaskObjectList->InsertBefore(request.taskobjectinsertbeforeid(), *pTaskObject);
					}
					else
					{
						pParentTaskObjectList->InsertAt(request.taskobjectpos(), *pTaskObject);
					}
				}
				else
				{
					Log_Assert(SvPb::CreateObjectRequest::kTaskObjectInsertBeforeId == request.message_case());
					pParentTaskObjectList->moveTaskObject(pObject->getObjectId(), request.taskobjectinsertbeforeid());
				}

				// And last - Create (initialize) it
				if (!pObject->is_Created())
				{
					// And finally try to create the child object...
					if (!pObjectApp->CreateChildObject(*pObject, SvDef::SVMFResetObject))
					{
						cmdResponse.set_hresult(E_FAIL);

						// Remove it from the Tool TaskObjectList ( Destruct it )
						pParentTaskObjectList->Delete(pObject);
						pObject = nullptr;
					}
					else
					{
						SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*> (pObject);
						if (nullptr != pTool)
						{
							pTool->finishAddTool();
						}
					}
				}
				pParent->resetAllObjects();
			}
			break;
		}
		default:break;
	}

	if (S_OK == cmdResponse.hresult())
	{
		if (nullptr != pObject)
		{
			SvPb::CreateObjectResponse* pResponse = cmdResponse.mutable_createobjectresponse();
			pResponse->set_objectid(pObject->getObjectId());
			pResponse->set_name(pObject->GetName());
		}
		else
		{
			cmdResponse.set_hresult(E_FAIL);
		}
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getCreatableObjects(SvPb::GetCreatableObjectsRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	if (nullptr != pObject)
	{
		SvPb::GetCreatableObjectsResponse* pResponse = cmdResponse.mutable_getcreatableobjectsresponse();
		SvDef::SVObjectTypeInfoStruct typeInfo {request.typeinfo().objecttype(), request.typeinfo().subtype(), request.typeinfo().embeddedid()};
		auto list = pObject->GetCreatableObjects(typeInfo);
		for (auto& item : list)
		{
			auto* pEntry = pResponse->add_list();
			pEntry->set_objectname(item.first.c_str());
			pEntry->set_classid(item.second);
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse shouldInspectionReset(SvPb::ShouldInspectionResetRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ISVFilter* pFilter = dynamic_cast<SvOi::ISVFilter*>(SvOi::getObject(request.objectid()));
	if (nullptr != pFilter)
	{
		SvPb::ShouldInspectionResetResponse* pResponse = cmdResponse.mutable_shouldinspectionresetresponse();
		pResponse->set_shouldreset(pFilter->shouldResetInspection());
	}
	else
	{
		cmdResponse.set_hresult(E_INVALIDARG);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getPPQName(SvPb::GetPPQNameRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*>(SvOi::getObject(request.inspectionid()));
	if (nullptr != pInspection)
	{
		SvOi::IObjectClass* pPPQ = pInspection->GetPPQInterface();
		if (nullptr != pPPQ)
		{
			SvPb::GetPPQNameResponse* pResponse = cmdResponse.mutable_getppqnameresponse();
			pResponse->set_ppqname(pPPQ->GetName());
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getValueObjectEnums(SvPb::GetValueObjectEnumsRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IEnumerateValueObject* pValueObject = dynamic_cast<SvOi::IEnumerateValueObject*> (SvOi::getObject(request.objectid()));
	if (nullptr != pValueObject)
	{
		SvPb::GetValueObjectEnumsResponse* pResponse = cmdResponse.mutable_getvalueobjectenumsresponse();
		for (const auto pairValue : pValueObject->GetEnumVector())
		{
			auto* pEntry = pResponse->add_list();
			pEntry->set_name(pairValue.first);
			pEntry->set_value(pairValue.second);
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getEmbeddedValues(SvPb::GetEmbeddedValuesRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ITaskObject* pTaskObj = dynamic_cast<SvOi::ITaskObject*> (SvOi::getObject(request.objectid()));
	Log_Assert(nullptr != pTaskObj);
	if (nullptr != pTaskObj)
	{
		auto EmbeddedVector = pTaskObj->getEmbeddedList();

		SvPb::GetEmbeddedValuesResponse* pResponse = cmdResponse.mutable_getembeddedvaluesresponse();
		for (auto const& rEntry : EmbeddedVector)
		{
			SvOi::IObjectClass* pObject = SvOi::getObject(rEntry);
			SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject);
			SvOi::ILinkedObject* pLinkedObject = dynamic_cast<SvOi::ILinkedObject*> (pObject);
			if (nullptr != pValueObject)
			{
				if (nullptr == pLinkedObject)
				{
					_variant_t DefaultValue {pValueObject->getDefaultValue()};
					_variant_t Value;
					HRESULT result = pValueObject->getValue(Value);
					
					if (S_OK == result)
					{
						auto* pElement = addEmbeddedListEntry(*pResponse, *pObject, pValueObject->isExternallySettable());
						auto* pValue = pElement->mutable_value();
						ConvertVariantToProtobuf(Value, pValue->mutable_value());
						ConvertVariantToProtobuf(DefaultValue, pValue->mutable_defaultvalue());
						pValue->clear_min_max();
						_variant_t MinValue, MaxValue;
						if (pValueObject->getMinMaxValues(MinValue, MaxValue))
						{
							auto minmax = pValue->mutable_min_max();

							ConvertVariantToProtobuf(MinValue, minmax->mutable_minvalue());
							ConvertVariantToProtobuf(MaxValue, minmax->mutable_maxvalue());
						}
					}
					else if (E_BOUNDS == result || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == result || SVMSG_SVO_105_CIRCULAR_REFERENCE == result)
					{
						Value.Clear();
						auto* pElement = addEmbeddedListEntry(*pResponse, *pObject, pValueObject->isExternallySettable());
						auto* pValue = pElement->mutable_value();
						ConvertVariantToProtobuf(Value, pValue->mutable_value());
						ConvertVariantToProtobuf(DefaultValue, pValue->mutable_defaultvalue());
					}
					else
					{
						cmdResponse.set_hresult(result);
					}
				}
				else
				{
					auto* pElement = addEmbeddedListEntry(*pResponse, *pObject, pValueObject->isExternallySettable());
					auto* pValue = pElement->mutable_linkedvalue();
					pLinkedObject->fillLinkedData(*pValue);					
				}
			}
		}

		if (auto* pList = pResponse->mutable_list(); nullptr != pList)
		{
			pTaskObj->fillInputInList(*pList);
		}
	}
	else
	{
		cmdResponse.set_hresult(E_FAIL);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse setEmbeddedValues(SvPb::SetEmbeddedValuesRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::SetValueStructVector ChangedValues;
	for (const auto& rItem : request.list())
	{
		if (rItem.ismodified() || rItem.isdefaultmodified())
		{
			SvOi::IObjectClass* pObject = SvOi::getObject(rItem.values().objectid());
			SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject);
			SvOi::ILinkedObject* pLinkedObject = dynamic_cast<SvOi::ILinkedObject*> (pObject);
			if (nullptr != pValueObject && rItem.has_values())
			{
				if (pValueObject->isExternallySettable())
				{
					auto& rValues = rItem.values();
					switch (rValues.valueData_case())
					{
						case SvPb::ValueObjectValues::kValue:
							if (nullptr == pLinkedObject)
							{
								variant_t value;
								SvPb::ConvertProtobufToVariant(rValues.value().value(), value);
								// Default value has no array index
								_variant_t defaultValue;
								SvPb::ConvertProtobufToVariant(rValues.value().defaultvalue(), defaultValue);
								ChangedValues.push_back(SvOi::SetValueStruct {pValueObject, value, defaultValue, rItem.arrayindex()});
							}
							else
							{
								Log_Assert(false);
								cmdResponse.set_hresult(E_FAIL);
							}
							break;
						case SvPb::ValueObjectValues::kLinkedValue:
							if (nullptr != pLinkedObject)
							{
								ChangedValues.push_back(SvOi::SetLinkedStruct {pLinkedObject, rValues.linkedvalue()});
							}
							else
							{
								Log_Assert(false);
								cmdResponse.set_hresult(E_FAIL);
							}
							break;
						default:
							Log_Assert(false);
							cmdResponse.set_hresult(E_FAIL);
							break;
					}
				}
				else
				{
					Log_Assert(false);
					cmdResponse.mutable_errormessage()->CopyFrom(SvPb::createErrorMessages(pObject->getObjectId(), SvStl::SourceFileParams(StdMessageParams), SvStl::Tid_SetOfReadOnlyValueFailed));
					cmdResponse.set_hresult(E_FAIL);
				}
			}
			else
			{
				cmdResponse.set_hresult(E_POINTER);
			}
		}
	}

	SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (SvOi::getObject(request.objectid()));

	if (nullptr != pTaskObject)
	{
		SvOi::ResetParameter resetParameter(request.resettype());
		SvStl::MessageContainerVector messageContainers = pTaskObject->validateAndSetEmbeddedValues(ChangedValues, true, &resetParameter);
		if (0 != messageContainers.size())
		{
			cmdResponse.mutable_errormessage()->CopyFrom(SvPb::convertMessageVectorToProtobuf(messageContainers));

			cmdResponse.set_hresult(E_FAIL);
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse validateLinkedValue(SvPb::ValidateLinkedValueRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;
	SvOi::ILinkedObject* pLinkedObject = dynamic_cast<SvOi::ILinkedObject*> (SvOi::getObject(request.data().objectid()));
	if (nullptr != pLinkedObject && SvPb::ValueObjectValues::kLinkedValue == request.data().valueData_case())
	{
		try
		{
			auto value = pLinkedObject->validateValue(request.data().linkedvalue());
			if (VT_EMPTY != value.vt || VT_EMPTY == request.data().linkedvalue().defaultvalue().type())
			{
				auto* pResponse = cmdResponse.mutable_validatelinkedvalueresponse();
				ConvertVariantToProtobuf(value, pResponse->mutable_value());
			}
			else
			{
				cmdResponse.set_hresult(E_FAIL);
			}
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			cmdResponse.set_hresult(E_FAIL);
			SvPb::convertMessageToProtobuf(rSvE, cmdResponse.mutable_errormessage()->add_messages());
		}
	}
	else
	{
		Log_Assert(false);
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getOutputRectangle(SvPb::GetOutputRectangleRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage*> (SvOi::getObject(request.imageid()));
	if (nullptr != pImage)
	{
		SvPb::GetOutputRectangleResponse* pResponse = cmdResponse.mutable_getoutputrectangleresponse();
		auto rect = pImage->GetOutputRectangle();
		pResponse->set_left(rect.left);
		pResponse->set_top(rect.top);
		pResponse->set_right(rect.right);
		pResponse->set_bottom(rect.bottom);
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getToolSizeAdjustParameter(SvPb::GetToolSizeAdjustParameterRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	auto pObject = SvOi::getObject(request.objectid());
	SvOi::IToolSizeAdjustTask* pTask = dynamic_cast<SvOi::IToolSizeAdjustTask*> (pObject);
	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*>(pObject);
	if (nullptr != pTask && nullptr == pTool)
	{
		pTool = pTask->getTool();
	}
	if (nullptr == pTask && nullptr != pTool)
	{
		SvPb::GetAvailableObjectsRequest getObjectsRequest;
		auto* pTreeSearchParameter = getObjectsRequest.mutable_tree_search();
		pTreeSearchParameter->set_search_start_id(request.objectid());
		pTreeSearchParameter->mutable_type_info()->set_objecttype(SvPb::SVToolSizeAdjustTaskType);
		cmdResponse = getAvailableObjects(getObjectsRequest);
		const SvPb::GetAvailableObjectsResponse& rObjectResponse = cmdResponse.getavailableobjectsresponse();
		if (S_OK == cmdResponse.hresult() && 0 < rObjectResponse.list_size())
		{
			pTask = dynamic_cast<SvOi::IToolSizeAdjustTask*>(SvOi::getObject(rObjectResponse.list(0).objectid()));
		}
	}

	SvPb::GetToolSizeAdjustParameterResponse* pResponse = cmdResponse.mutable_gettoolsizeadjustparameterresponse();
	if (nullptr != pTask)
	{
		pResponse->set_isfullsizeallowed(pTask->IsFullSizeAllowed());
		pResponse->set_isadjustsizeallowed(pTask->IsAdjustSizeAllowed());
		pResponse->set_isadjustpositionallowed(pTask->IsAdjustPositionAllowed());
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	if (nullptr != pTool)
	{
		pResponse->set_enableautosize(pTool->getAutoSizeEnabled());
		pResponse->set_canresizetoparent(pTool->canResizeToParent());
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getExtentParameter(SvPb::GetExtentParameterRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	auto* pObject = SvOi::getObject(request.objectid());
	if (auto* pTool = dynamic_cast<SvOi::ITool*> (pObject); nullptr != pTool)
	{
		SvPb::GetExtentParameterResponse* pResponse = cmdResponse.mutable_getextentparameterresponse();
		auto* pParameter = pResponse->mutable_parameters();
		SvPb::SVExtentTranslationEnum translationType = SvPb::SVExtentTranslationUnknown;
		if (request.shouldfromparent())
		{
			cmdResponse.set_hresult(pTool->getParentExtentProperties(*pParameter, translationType));
		}
		else
		{
			pTool->getExtentProperties(*pParameter, translationType);
		}
		pResponse->set_translationtype(translationType);
	}
	else if (auto* pAnalyzer = dynamic_cast<SvOi::IAnalyzer*> (pObject); nullptr != pAnalyzer)
	{
		if (false == request.shouldfromparent())
		{
			cmdResponse = pAnalyzer->getAnalyzerExtentProperties();
		}
		else
		{
			cmdResponse.mutable_errormessage()->CopyFrom(SvPb::createErrorMessages(pObject->getObjectId(), SvStl::SourceFileParams(StdMessageParams), SvStl::Tid_NoParentExtentForAnalyzer));
			Log_Assert(false);
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse setExtentParameter(SvPb::SetExtentParameterRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*>(SvOi::getObject(request.objectid()));
	SvPb::SetExtentParameterResponse* pResponse = cmdResponse.mutable_setextentparameterresponse();
	auto* pParameter = pResponse->mutable_parameters();

	if (nullptr != pTool && nullptr != pParameter)
	{
		HRESULT result {S_OK};
		switch (request.message_case())
		{
			case SvPb::SetExtentParameterRequest::kUpdateFromOutputSpace:
				result = pTool->updateExtentFromOutputSpace(request.updatefromoutputspace().action(), request.updatefromoutputspace().dx(), request.updatefromoutputspace().dy());
				break;
			case SvPb::SetExtentParameterRequest::kSetProperty:
				result = pTool->setExtentProperty(request.setproperty().propertyflag(), request.setproperty().value());
				break;
			case SvPb::SetExtentParameterRequest::kExtentList:
				result = pTool->setExtentList(request.extentlist().extentlist());
				break;
			case SvPb::SetExtentParameterRequest::kSetToParent:
				result = pTool->setExtentToParent();
				break;
			default:
				result = E_INVALIDARG;
				break;
		}
		SvPb::SVExtentTranslationEnum translationType = SvPb::SVExtentTranslationUnknown;
		pTool->getExtentProperties(*pParameter, translationType);
		pResponse->set_translationtype(translationType);
		cmdResponse.set_hresult(result);
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}

	return cmdResponse;
}

SvPb::InspectionCmdResponse isAllowedLocation(SvPb::IsAllowedLocationRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*>(SvOi::getObject(request.objectid()));
	if (nullptr != pTool)
	{
		SvPb::IsAllowedLocationResponse* pResponse = cmdResponse.mutable_isallowedlocationresponse();
		pResponse->set_isallowed(pTool->isAllowedLocation(request.location(), request.direction()));
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse propagateSizeAndPosition(SvPb::PropagateSizeAndPositionRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*>(SvOi::getObject(request.objectid()));
	if (nullptr != pInspection)
	{
		pInspection->propagateSizeAndPosition();
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse usePropagateSizeAndPosition(SvPb::UsePropagateSizeAndPositionRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*>(SvOi::getObject(request.objectid()));
	if (nullptr != pInspection)
	{
		SvPb::UsePropagateSizeAndPositionResponse* pResponse = cmdResponse.mutable_usepropagatesizeandpositionresponse();
		pResponse->set_isused(pInspection->usePropagateSizeAndPosition());
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getOverlayStruct(SvPb::GetOverlayStructRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage*>(SvOi::getObject(request.imageid()));
	if (nullptr != pImage)
	{
		try
		{
			SvPb::OverlayDesc overlay = pImage->getOverlayStruct();
			SvPb::GetOverlayStructResponse* pResponse = cmdResponse.mutable_getoverlaystructresponse();
			SvPb::OverlayDesc* pOverlay = pResponse->mutable_overlays();
			*pOverlay = overlay;
		}
		catch (const SvStl::MessageContainer& rExp)
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(rExp.getMessage());
			cmdResponse.set_hresult(E_FAIL);
		}
		catch (...)
		{
			cmdResponse.set_hresult(E_FAIL);
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getObjectSelectorItems(SvPb::GetObjectSelectorItemsRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;
	std::vector<SvPb::TreeItem> resultItemVector;

	SvPb::GetObjectSelectorItemsResponse* pResponse = cmdResponse.mutable_getobjectselectoritemsresponse();
	for (int i = 0; i < request.areas_size(); i++)
	{
		fillSelectorList(std::back_inserter(resultItemVector), request, request.areas(i));
	}
	*pResponse->mutable_tree()->mutable_children() = {resultItemVector.begin(), resultItemVector.end()};

	return cmdResponse;
}

SvPb::InspectionCmdResponse getObjectSelectorItems(SvPb::GetObjectSelectorItems2Request request)
{
	SvPb::InspectionCmdResponse cmdResponse;
	auto* pObject = SvOi::getObject(request.objectid());
	if (nullptr != pObject)
	{
		std::vector<SvPb::TreeItem> resultItemVector;
		if (auto* pLinkedValue = dynamic_cast<SvOi::ILinkedObject*>(pObject); nullptr != pLinkedValue)
		{
			pLinkedValue->fillSelectorListForLink(std::back_inserter(resultItemVector));
		}
		else if (auto* pEquation = dynamic_cast<SvOi::IEquation*>(pObject); nullptr != pEquation)
		{
			pEquation->fillSelectorListForEquation(std::back_inserter(resultItemVector));
		}
		else
		{
			cmdResponse.set_hresult(E_POINTER);
			return cmdResponse;
		}
		*cmdResponse.mutable_getobjectselectoritemsresponse()->mutable_tree()->mutable_children() = {resultItemVector.begin(), resultItemVector.end()};
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}

	return cmdResponse;
}

void fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, const SvPb::GetObjectSelectorItemsRequest& request, SvPb::SearchArea area)
{
	SvOi::IObjectClass* pImportantObject = SvOi::getObject(request.importantobjectforstopatborder());
	bool noBlockBecauseOfClosedGroup {nullptr == pImportantObject || SvDef::InvalidObjectId == pImportantObject->getFirstClosedParent(SvDef::InvalidObjectId)};
	switch (area)
	{
		case SvPb::globalConstantItems:
		{
			if (noBlockBecauseOfClosedGroup)
			{
				SvOi::fillRootChildSelectorList(treeInserter, _T(""), request.attribute(), request.type());
			}
			else
			{
				SvOi::fillRootChildSelectorList(treeInserter, SvDef::FqnEnvironment, request.attribute(), request.type());
			}
			return;
		}
		case SvPb::ppqItems:
		{
			if (noBlockBecauseOfClosedGroup)
			{
				SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (SvOi::getObject(request.inspectionid()));
				if (nullptr != pInspection)
				{
					pInspection->fillPPQSelectorList(treeInserter, request.attribute(), request.type());
				}
			}
			return;
		}
		case SvPb::cameraObject:
		{
			if (noBlockBecauseOfClosedGroup)
			{
				SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (SvOi::getObject(request.inspectionid()));
				if (nullptr != pInspection)
				{
					pInspection->fillCameraSelectorList(treeInserter, request.attribute(), request.type());
				}
			}
			return;
		}
		case SvPb::toolsetItems:
		default:
			break;
	}

	uint32_t id = (SvDef::InvalidObjectId != request.instanceid()) ? request.instanceid() : request.inspectionid();
	SvOi::IObjectClass* pObject = SvOi::getObject(id);
	if (nullptr != pObject)
	{
		SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (pObject);
		if (nullptr != pInspection)
		{
			pObject = dynamic_cast<SvOi::IObjectClass*>(pInspection->GetToolSetInterface());
			if (nullptr == pObject)
			{
				return;
			}
		}

		//If instance ID is set to the inspection ID then we need the name including the inspection name
		SvPb::SVObjectTypeEnum 	objectTypeToName {SvPb::SVNotSetObjectType};
		if (request.inspectionid() == request.instanceid())
		{
			objectTypeToName = SvPb::SVInspectionObjectType;
		}

		bool isStopAtBorder {nullptr != pImportantObject};
		if (isStopAtBorder)
		{
			auto* pTmpObject = SvOi::getObject(pImportantObject->getFirstClosedParent(pObject->getObjectId()));
			if (nullptr != pTmpObject)
			{
				objectTypeToName = (SvPb::SVNotSetObjectType == objectTypeToName) ? pObject->GetObjectType() : objectTypeToName;
				pObject = pTmpObject;
			}
		}

		if (nullptr != pObject)
		{
			IsObjectInfoAllowed pFunc = getObjectSelectorFilterFunc(request);
			if (nullptr != pFunc)
			{
				pObject->fillSelectorList(treeInserter, pFunc, request.attribute(), request.wholearray(), objectTypeToName, request.type(), isStopAtBorder, true);
			}
		}
	}
}

SvPb::InspectionCmdResponse getNormalizerValues(SvPb::GetNormalizerValuesRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ISVLinearAnalyzer* pAnalyzer = dynamic_cast<SvOi::ISVLinearAnalyzer*>(SvOi::getObject(request.linearanalyzerid()));

	if (nullptr != pAnalyzer)
	{
		auto tempResponse = pAnalyzer->getNormalizerValues();
		SvPb::GetNormalizerValuesResponse* pResponse = cmdResponse.mutable_getnormalizervaluesresponse();
		if (nullptr != pResponse)
		{
			pResponse->Swap(&tempResponse);
		}
		else
		{
			cmdResponse.set_hresult(E_POINTER);
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}

	return cmdResponse;
}

SvPb::InspectionCmdResponse setNormalizerRanges(SvPb::SetNormalizerRangesRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ISVLinearAnalyzer* pAnalyzer = dynamic_cast<SvOi::ISVLinearAnalyzer*>(SvOi::getObject(request.linearanalyzerid()));
	if (nullptr != pAnalyzer)
	{
		auto tempResponse = pAnalyzer->setNormalizerRanges(request);
		SvPb::GetNormalizerValuesResponse* pResponse = cmdResponse.mutable_getnormalizervaluesresponse();
		if (nullptr != pResponse)
		{
			pResponse->Swap(&tempResponse);
		}
		else
		{
			cmdResponse.set_hresult(E_POINTER);
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}

	return cmdResponse;
}

SvPb::InspectionCmdResponse getImageInfo(SvPb::GetImageInfoRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage*>(SvOi::getObject(request.imageid()));

	if (nullptr != pImage)
	{
		SvPb::GetImageInfoResponse* pResponse = cmdResponse.mutable_getimageinforesponse();
		pResponse->set_imagetype(pImage->GetImageType());
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}

	return cmdResponse;
}

SvPb::InspectionCmdResponse getBlobAnalyzerInfo(SvPb::GetBlobAnalyzerInfoRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IBlobAnalyzer* pBlobAnalyzer = dynamic_cast<SvOi::IBlobAnalyzer*>(SvOi::getObject(request.taskobjectid()));

	if (nullptr != pBlobAnalyzer)
	{
		SvPb::GetBlobAnalyzerInfoResponse* pResponse = cmdResponse.mutable_getblobanalyzerinforesponse();
		auto resultObject = pBlobAnalyzer->getResultObject(request.feature());
		if (nullptr != resultObject)
		{
			pResponse->set_resultobjectid(resultObject->getObjectId());
			pResponse->set_resultobjectclassid(resultObject->GetClassID());
		}
		auto resultBlob = pBlobAnalyzer->getResultBlob();
		if (nullptr != resultBlob)
		{
			pResponse->set_resultblobid(resultBlob->getObjectId());
			pResponse->set_resultblobclassid(resultBlob->GetClassID());
		}

		auto featureList = pBlobAnalyzer->getFeatureList(request.featureenabled());
		for (auto const& feature : featureList)
		{
			if (!feature.first.empty())
			{
				auto newFeature = pResponse->mutable_list()->Add();
				newFeature->set_name(feature.first);
				newFeature->set_value(feature.second);
			}
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}

	return cmdResponse;
}

SvPb::InspectionCmdResponse computeOverscanRect(SvPb::ComputeOverscanRectRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;


	auto rect = RECT {0};
	if (SvPb::OverscanRectKindEnum::InnerRect == request.rectkind())
	{
		rect = SVMatroxPatternInterface::CalculateOverscanInnerRect(CPoint(request.point().x(), request.point().y()),
			CSize(request.size().sizex(), request.size().sizey()));
	}
	else if (SvPb::OverscanRectKindEnum::OuterRect == request.rectkind())
	{
		rect = SVMatroxPatternInterface::CalculateOverscanOuterRect(CPoint(request.point().x(), request.point().y()),
			CSize(request.size().sizex(), request.size().sizey()));
	}
	else
	{
	}


	SvPb::ComputeOverscanRectResponse* pResponse = cmdResponse.mutable_computeoverscanrectresponse();
	pResponse->mutable_overscanrect()->set_left(rect.left);
	pResponse->mutable_overscanrect()->set_right(rect.right);
	pResponse->mutable_overscanrect()->set_top(rect.top);
	pResponse->mutable_overscanrect()->set_bottom(rect.bottom);

	return cmdResponse;
}

SvPb::InspectionCmdResponse setDefaultInputsRequest(SvPb::SetDefaultInputsRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (pObject);

	if (nullptr != pInspection)
	{
		pInspection->SetDefaultInputs();
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getBarCodeTypeInfos(SvPb::GetBarCodeTypeInfosRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	auto* pResponseTmp = cmdResponse.mutable_getbarcodetypeinfosresponse();
	auto tmp = SvOi::getBarCodeTypeInfo();
	pResponseTmp->Swap(&tmp);
	return cmdResponse;
}

SvPb::InspectionCmdResponse getFeatures(SvPb::GetFeaturesRequest request)
{
	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	auto* pAnalyzer = dynamic_cast<SvOi::IBlobAnalyzer2*> (pObject);

	if (nullptr == pAnalyzer)
	{
		SvPb::InspectionCmdResponse cmdResponse;
		cmdResponse.set_hresult(E_POINTER);
		return cmdResponse;
	}
	
	return pAnalyzer->getFeaturesData();
}

SvPb::InspectionCmdResponse setFeatures(SvPb::SetFeaturesRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	auto* pAnalyzer = dynamic_cast<SvOi::IBlobAnalyzer2*> (pObject);

	if (nullptr != pAnalyzer)
	{
		cmdResponse = pAnalyzer->setFeatures(request);
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getAvailableFeatures(SvPb::GetAvailableFeaturesRequest request)
{
	return SvOi::getAvailableFeatures();
}

SvPb::InspectionCmdResponse getToolsWithReplaceableSourceImage(SvPb::GetToolsWithReplaceableSourceImageRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ITaskObject* pTaskObject;
	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (SvOi::getObject(request.objectid()));
	if (nullptr != pInspection)
	{
		pTaskObject = pInspection->GetToolSetInterface();
	}
	else
	{
		pTaskObject = dynamic_cast<SvOi::ITaskObject*> (SvOi::getObject(request.objectid()));
	}

	if (nullptr != pTaskObject)
	{
		SvPb::GetToolsWithReplaceableSourceImageResponse* pResponse = cmdResponse.mutable_gettoolswithreplaceablesourceimageresponse();
		pTaskObject->getToolsWithReplaceableSourceImage(*pResponse);
	}
	return cmdResponse;
}


/// ExternalToolTask commands

SvPb::InspectionCmdResponse initializeExternalToolTask(SvPb::InitializeExternalToolTaskRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	auto* pExternalToolTask = dynamic_cast<SvOi::IExternalToolTask*> (pObject);

	if (nullptr != pExternalToolTask)
	{
		auto* pResponse = cmdResponse.mutable_initializeexternaltooltaskresponse();

		Log_Assert(pResponse != nullptr);
		try
		{
			std::vector<std::string> statusMessages;
			cmdResponse.set_hresult(pExternalToolTask->Initialize(statusMessages, request.increationprocess(), request.initializeall()));

			for (const auto& message : statusMessages)
			{
				pResponse->add_statusmessages(message);
			}
		}
		catch (const SvStl::MessageContainer& rExp)
		{
			SvPb::convertMessageToProtobuf(rExp, cmdResponse.mutable_errormessage()->add_messages());
			cmdResponse.set_hresult(E_FAIL);
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse resetAllObjects(SvPb::ResetAllObjectsRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());

	if (nullptr != pObject)
	{
		if (false == pObject->resetAllObjects())
		{
			SvOi::ITaskObject* pTask = dynamic_cast<SvOi::ITaskObject*> (pObject);
			if (nullptr != pTask)
			{
				cmdResponse.mutable_errormessage()->CopyFrom(SvPb::convertMessageVectorToProtobuf(pTask->getErrorMessages()));
			}

			cmdResponse.set_hresult(S_FALSE);
		}
	}
	else
	{
		cmdResponse.mutable_errormessage()->CopyFrom(SvPb::createErrorMessages(request.objectid(), SvStl::SourceFileParams(StdMessageParams), SvStl::Tid_UnexpectedError));
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;

}

SvPb::InspectionCmdResponse clearDataExternalTool(SvPb::ClearDataExternalToolRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	auto* pExternalToolTask = dynamic_cast<SvOi::IExternalToolTask*> (pObject);

	if (nullptr != pExternalToolTask)
	{
		cmdResponse.set_hresult(pExternalToolTask->ClearData());
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}
SvPb::InspectionCmdResponse setAllAttributesExternalTool(SvPb::SetAllAttributesExternalToolRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	auto* pExternalToolTask = dynamic_cast<SvOi::IExternalToolTask*> (pObject);

	if (nullptr != pExternalToolTask)
	{
		pExternalToolTask->SetAllAttributes();
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getInputValuesDefinitionExternalTool(SvPb::GetInputValuesDefinitionExternalToolRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	auto* pExternalToolTask = dynamic_cast<SvOi::IExternalToolTask*> (pObject);

	if (nullptr != pExternalToolTask)
	{
		SvPb::GetInputValuesDefinitionExternalToolResponse* pResponse = cmdResponse.mutable_getinputvaluesdefinitionexternaltoolresponse();
		Log_Assert(pResponse != nullptr);

		pResponse->set_numinputvalues(pExternalToolTask->getExternalToolDataAdmin().getNumInputValues());

		auto inputDefinitions = pExternalToolTask->getExternalToolDataAdmin().getInputValuesDefinition();
		for (auto& item : inputDefinitions)
		{
			auto* inputEntry = pResponse->add_inputvaluesdefinition();
			inputEntry->set_vt(item->getVt());
			inputEntry->set_linkedvalueindex(item->getLinkedValueIndex());
			inputEntry->set_type(item->getType());
			inputEntry->set_groupname(item->getGroup());
			inputEntry->set_displayname(item->getDisplayName());
			inputEntry->set_helptext(item->getHelpText());
			ConvertVariantToProtobuf(item->getDefaultValue(), inputEntry->mutable_defaultvalue());
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getPropTreeStateExternalTool(SvPb::GetPropTreeStateExternalToolRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	auto* pExternalToolTask = dynamic_cast<SvOi::IExternalToolTask*> (pObject);

	if (nullptr != pExternalToolTask)
	{
		auto* pResponse = cmdResponse.mutable_getproptreestateexternaltoolresponse();
		Log_Assert(pResponse != nullptr);

		auto propTreeState = pExternalToolTask->getExternalToolDataAdmin().getPropTreeState();

		for (auto prop : propTreeState)
		{
			auto* pEntry = pResponse->add_proptreestate();
			pEntry->set_name(prop.first);
			pEntry->set_state(prop.second);
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}

	return cmdResponse;
}

SvPb::InspectionCmdResponse setPropTreeStateExternalTool(SvPb::SetPropTreeStateExternalToolRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	auto* pExternalToolTask = dynamic_cast<SvOi::IExternalToolTask*> (pObject);

	if (nullptr != pExternalToolTask)
	{
		std::map<std::string, bool> propTreeState;
		for (const auto& item : request.proptreestate())
		{
			propTreeState.emplace(item.name(), item.state());
		}

		pExternalToolTask->getExternalToolDataAdmin().setPropTreeState(propTreeState);
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse validateValueParameterExternalTool(SvPb::ValidateValueParameterExternalToolRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.taskobjectid());
	auto* pExternalToolTask = dynamic_cast<SvOi::IExternalToolTask*> (pObject);

	if (nullptr != pExternalToolTask)
	{
		SvStl::MessageContainerVector messageContainers;

		_variant_t value;
		SvPb::ConvertProtobufToVariant(request.newvalue(), value);
		auto ediType = static_cast<SvPb::ExDllInterfaceType>(request.exdllinterfacetype());
		auto hr = pExternalToolTask->validateValueParameter(request.taskobjectid(), request.index(), value, ediType);
		cmdResponse.set_hresult(hr);

		if (S_OK != hr)
		{
			auto dllMessageString = pExternalToolTask->getDllMessageString(hr);

			SvDef::StringVector msgList;
			if (dllMessageString.empty())
			{
				dllMessageString = SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_ExternalDllSomethingWrongWithInputValue);
			}
			msgList.push_back(dllMessageString);
			SvStl::MessageContainer msg;
			msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ExternalDllError, msgList, SvStl::SourceFileParams(StdMessageParams), request.taskobjectid());
			messageContainers.push_back(msg);
			cmdResponse.mutable_errormessage()->CopyFrom(SvPb::convertMessageVectorToProtobuf(messageContainers));
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getDllMessageString(SvPb::GetDllMessageStringRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	auto* pExternalToolTask = dynamic_cast<SvOi::IExternalToolTask*> (pObject);

	if (nullptr != pExternalToolTask)
	{
		auto* pResponse = cmdResponse.mutable_getdllmessagestringresponse();
		Log_Assert(pResponse != nullptr);
		pResponse->set_errormessage(pExternalToolTask->getDllMessageString(request.hresulterror()));
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getResultValuesDefinitionExternalTool(SvPb::GetResultValuesDefinitionExternalToolRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	auto* pExternalToolTask = dynamic_cast<SvOi::IExternalToolTask*> (pObject);

	if (nullptr != pExternalToolTask)
	{
		auto* pResponse = cmdResponse.mutable_getresultvaluesdefinitionexternaltoolresponse();
		Log_Assert(pResponse != nullptr);

		pResponse->set_numresultvalues(pExternalToolTask->getExternalToolDataAdmin().getNumResultValues());

		auto resultDefinitions = pExternalToolTask->getExternalToolDataAdmin().getResultValuesDefinition();
		for (auto& item : resultDefinitions)
		{
			auto* resultEntry = pResponse->add_resultvaluesdefinition();
			resultEntry->set_vt(item->getVT());
			resultEntry->set_groupname(item->getGroup());
			resultEntry->set_displayname(item->getDisplayName());
			resultEntry->set_helptext(item->getHelpText());
			resultEntry->set_usedisplaynames(item->UseDisplayNames());
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}


SvPb::InspectionCmdResponse getTableResultsExternalTool(SvPb::GetTableResultsExternalToolRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	auto* pExternalToolTask = dynamic_cast<SvOi::IExternalToolTask*> (pObject);

	if (nullptr != pExternalToolTask)
	{
		auto* pResponse = cmdResponse.mutable_gettableresultsexternaltoolresponse();
		Log_Assert(pResponse != nullptr);

		*pResponse = pExternalToolTask->getExternalToolDataAdmin().getTableResults();
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getResultRangeObject(SvPb::GetResultRangeObjectRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	auto* pExternalToolTask = dynamic_cast<SvOi::IExternalToolTask*> (pObject);

	if (nullptr != pExternalToolTask)
	{
		auto* pResponse = cmdResponse.mutable_getresultrangeobjectresponse();
		Log_Assert(pResponse != nullptr);

		auto* iObjectClass = pExternalToolTask->getResultRangeObjectAtIndex(request.index());
		if (nullptr != iObjectClass)
		{
			pResponse->set_classid(iObjectClass->GetClassID());
			pResponse->set_objectid(iObjectClass->getObjectId());
		}
		else
		{
			pResponse->set_classid(SvPb::ClassIdEnum::NoObjectClassId);
			pResponse->set_objectid(0);
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getImageInfoExternalTool(SvPb::GetImageInfoExternalToolRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	auto* pExternalToolTask = dynamic_cast<SvOi::IExternalToolTask*> (pObject);

	if (nullptr != pExternalToolTask)
	{
		auto* pResponse = cmdResponse.mutable_getimageinfoexternaltoolresponse();
		Log_Assert(pResponse != nullptr);

		*pResponse = pExternalToolTask->getImageInfoList();
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse setandSortEmbeddedValues(SvPb::SetAndSortEmbeddedValueRequest request)
{
	SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject*>(SvOi::getObject(request.ownerid()));
	if (nullptr != pObject)
	{
		return pObject->setAndSortEmbeddedValues(request);
	}

	SvPb::InspectionCmdResponse cmdResponse;
	cmdResponse.set_hresult(E_POINTER);
	return cmdResponse;
}

SvPb::InspectionCmdResponse getInvalidDependencies(SvPb::GetInvalidDependenciesRequest request)
{
	auto* pTool = dynamic_cast<SvOi::ITool*>(SvOi::getObject(request.objectid()));
	if (nullptr != pTool)
	{
		return pTool->getInvalidDependencies();
	}

	SvPb::InspectionCmdResponse cmdResponse;
	cmdResponse.set_hresult(E_POINTER);
	return cmdResponse;
}

SvPb::InspectionCmdResponse getObjectName(SvPb::GetObjectNameRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	if (SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid()); nullptr != pObject)
	{
		auto* pResponse = cmdResponse.mutable_getobjectnameresponse();
		switch (request.type_case())
		{
			case SvPb::GetObjectNameRequest::kBeforeType:
				pResponse->set_name(pObject->GetObjectNameBeforeObjectType(request.beforetype()));
				break;
			case SvPb::GetObjectNameRequest::kToType:
				pResponse->set_name(pObject->GetObjectNameToObjectType(request.totype()));
				break;
			default:
				pResponse->set_name(pObject->GetName());
				break;
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse setObjectComment(SvPb::SetObjectCommentRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	if (SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid()); nullptr != pObject)
	{
		pObject->setComment(request.comment());
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}


SvPb::InspectionCmdResponse addParameterAndUseIt(SvPb::AddParameterAndUseItRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	if (auto* pObject = dynamic_cast<SvOi::IParameterTask*>(SvOi::getObject(request.parametertaskid())); nullptr != pObject)
	{
		cmdResponse = pObject->addParameterAndUseIt(request);
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse checkParameterNames(SvPb::CheckParameterNamesRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	if (auto* pObject = dynamic_cast<SvOi::IParameterTask*>(SvOi::getObject(request.parametertaskid())); nullptr != pObject)
	{
		cmdResponse = pObject->checkParameterNames(request);
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getDependencyRequest(SvPb::GetDependencyRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;
	std::set<uint32_t> setOfIdObjectDepends;
	std::transform(request.idsofobjectsdependedon().begin(), request.idsofobjectsdependedon().end(),
		std::inserter(setOfIdObjectDepends, setOfIdObjectDepends.begin()), [](google::protobuf::uint32 id) -> uint32_t {return id;});
	LPCTSTR filename = (request.filename().empty()) ? nullptr : request.filename().c_str();
	auto* pResponse = cmdResponse.mutable_getdependencyresponse();
	if (nullptr != pResponse)
	{
		if (request.dependecytype() == SvPb::DependecyTypeEnum::Tool)
		{
			SvOl::DependencyManager::Instance().updateVertexIndex();
			*pResponse = SvOl::DependencyManager::Instance().getDependencyList(setOfIdObjectDepends, request.objecttype(), request.tooldependecytype(), request.alldependecies(), filename);
		}
		else if (request.dependecytype() == SvPb::DependecyTypeEnum::Object)
		{
			SvOl::DependencyManager::Instance().updateVertexIndex();
			*pResponse = SvOl::DependencyManager::Instance().getObjectDependency(setOfIdObjectDepends, request.tooldependecytype());
		}
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getToolDepth(SvPb::GetToolDepthRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	if (auto* pObject = dynamic_cast<SvOi::ITaskObjectListClass*>(SvOi::getObject(request.objectid())); nullptr != pObject)
	{
		if (request.goupward())
		{
			cmdResponse.mutable_gettooldepthresponse()->set_depthupward(pObject->getToolDepth(true));
		}
		else
		{
			cmdResponse.mutable_gettooldepthresponse()->set_depthdownward(pObject->getToolDepth(false));
		}
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getModuleList()
{
	return SvOi::getModuleList();
}

SvPb::InspectionCmdResponse deleteModule(SvPb::DeleteModuleRequest request)
{
	return SvOi::deleteModule(SVGUID {request.guid()});
}

SvPb::InspectionCmdResponse checkNewModuleName(SvPb::CheckNewModuleNameRequest request)
{
	return SvOi::checkNewModuleName(request.name());
}

SvPb::InspectionCmdResponse convertGroupToModuleTool(SvPb::ConvertGroupToModuleToolRequest request)
{
	return SvOi::convertGroupToModuleTool(request.grouptoolid(), request.modulename());
}

SvPb::InspectionCmdResponse convertModuleToGroupTool(SvPb::ConvertModuleToGroupToolRequest request)
{
	return SvOi::convertModuleToGroupTool(request.moduletoolid());
}

SvPb::InspectionCmdResponse renameModule(SvPb::RenameModuleRequest request)
{
	return SvOi::renameModule(SVGUID {request.guid()}, request.name());
}

SvPb::InspectionCmdResponse importModule(SvPb::ImportModuleRequest request)
{
	return SvOi::importModule(request.name(), request.datastring());
}

SvPb::InspectionCmdResponse exportModule(SvPb::ExportModuleRequest request)
{
	return SvOi::exportModule(SVGUID {request.guid()});
}

SvPb::InspectionCmdResponse fixInputsAndGetList(SvPb::FixInputsAndGetInputListRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;
	auto* pFixedListResponse = cmdResponse.mutable_fixinputsandgetinputlistresponse();
	std::vector<SvPb::FixedInputData> fixedDataVector;
	for (auto toolId : request.taskids())
	{
		SVObjectClass* pToolObject(SVObjectManagerClass::Instance().GetObject(toolId));
		if (nullptr != pToolObject)
		{
			if (request.complete_grouptool_inputs_only())
			{
				if (auto* pGroupTool = dynamic_cast<SvTo::GroupTool*>(pToolObject); nullptr != pGroupTool)
				{
					pGroupTool->fixAndReturnAllGroupInputs(std::back_inserter(fixedDataVector));
				}
				else
				{
					Log_Assert(false);
				}
			}
			else
			{
				pToolObject->fixInvalidInputs(std::back_inserter(fixedDataVector));
			}
		}
	}
	if (false == fixedDataVector.empty())
	{
		pFixedListResponse->mutable_list()->Add(fixedDataVector.begin(), fixedDataVector.end());

		for (auto toolId : request.taskids())
		{
			auto* pTool = SVObjectManagerClass::Instance().GetObject(toolId);
			if (nullptr != pTool)
			{
				pTool->resetAllObjects();
			}
		}
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse resetAllCounter(SvPb::ResetAllCounterRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;
	auto pIObject = SvOi::getObject(request.inspectionid());
	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*>(pIObject);
	if (nullptr != pInspection)
	{
		HRESULT hres {S_OK};
		pInspection->resetCounterDirectly();
		if (request.resetallobject())
		{
			hres = pIObject->resetAllObjects();
		}
		if (hres == S_OK && request.dorunonce() && (SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST)) == false)
		{
			hres = pInspection->RunOnce();
		}
		cmdResponse.set_hresult(hres);
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getSourceResultImageList(SvPb::GetSourceResultImageListRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;


	SVObjectClass* pTaskObject {SVObjectManagerClass::Instance().GetObject(request.objectid())};
	if (nullptr != pTaskObject)
	{
		uint32_t taskObjectID {pTaskObject->getObjectId()};
		auto* pSourceResultImageList = cmdResponse.mutable_getsourceresultimagelistresponse();
		switch (pTaskObject->GetObjectSubType())
		{
			case SvPb::SVObjectSubTypeEnum::SVColorToolObjectType:
			{
				addColorToolImages(taskObjectID, pSourceResultImageList);
				break;
			}
			case SvPb::SVObjectSubTypeEnum::SVExternalToolObjectType:
			{
				constexpr int cMaxExternalToolImages {4};
				for (int i = 0; i < cMaxExternalToolImages; ++i)
				{
					addTaskInputImage(taskObjectID, SvPb::ImageInputEId + i, pSourceResultImageList);
					addTaskOutputImage(taskObjectID, SvPb::OutputImageEId + i, pSourceResultImageList);
				}
				break;
			}
			case SvPb::SVObjectSubTypeEnum::SVToolImageObjectType:
			{
				addTaskInputImage(taskObjectID, SvPb::ImageArithmeticAImageInputEId, pSourceResultImageList);
				addTaskInputImage(taskObjectID, SvPb::ImageArithmeticBImageInputEId, pSourceResultImageList);
				addTaskOutputImage(taskObjectID, SvPb::OutputImageEId, pSourceResultImageList);
				break;
			}
			case SvPb::SVObjectSubTypeEnum::SVRingBufferToolObjectType:
			{
				addTaskInputImage(taskObjectID, SvPb::ImageInputEId, pSourceResultImageList);
				for (int i = 0; i < SvDef::cRingBufferNumberOutputImages; i++)
				{
					addTaskOutputImage(taskObjectID, SvPb::OutputImageEId + i, pSourceResultImageList);
				}
				break;
			}
			default:
			{
				addTaskInputImage(taskObjectID, SvPb::ImageInputEId, pSourceResultImageList);
				addTaskOutputImage(taskObjectID, SvPb::OutputImageEId, pSourceResultImageList);
				break;
			}
		}
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse startEditModule(SvPb::StartEditModuleRequest request)
{
	return SvOi::startEditModule(request.moduletoolid());
}

SvPb::InspectionCmdResponse saveEditModule(SvPb::SaveEditModuleRequest request)
{
	return SvOi::saveEditModule(request.moduletoolid(), request.changetext());
}

SvPb::InspectionCmdResponse cancelEditModule(SvPb::CancelEditModuleRequest request)
{
	return SvOi::cancelEditModule(request.moduletoolid());
}
} //namespace SvCmd