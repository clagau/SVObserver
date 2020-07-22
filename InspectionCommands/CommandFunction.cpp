//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file CommandFunction.cpp
/// All Rights Reserved
//*****************************************************************************
/// This file contains the intern commands to the inspection.
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "CommandFunction.h"
#include "CommandExternalHelper.h"
#include "ObjectInterfaces\IBarCode.h"
#include "ObjectInterfaces\IEnumerateValueObject.h"
#include "ObjectInterfaces\IEquation.h"
#include "ObjectInterfaces\IFormulaController.h"
#include "ObjectInterfaces\ISVFilter.h"
#include "ObjectInterfaces\IInspectionProcess.h"
#include "ObjectInterfaces\IMask.h"
#include "ObjectInterfaces\IObjectClass.h"
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
#include "Definitions\ObjectDefines.h"
#include "Definitions\TextDefineSvDef.h"
#include "SVStatusLibrary\ErrorNumbers.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVMatroxLibrary\SVMatroxSimpleEnums.h"
#include "SVMatroxLibrary\SVMatroxPatternInterface.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary\StringHelper.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVObjectLibrary\SVObjectBuilder.h"
#include "SVObjectLibrary\SVObjectClass.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVProtoBuf\ConverterHelper.h"
#include "CommandInternalHelper.h"
#include "ObjectSelectorFilter.h"
#include "ObjectInterfaces\ISVLinearAnalyzer.h"

#include <atltypes.h>

#pragma endregion Includes

namespace SvCmd
{
SvPb::StandardResponse canDeleteObject(const SvOi::IObjectClass* const pObject)
{
	/// Do not need to check for nullptr from function parameters
	SvPb::StandardResponse response;
	
	const SvOi::IInspectionProcess* pInspection = dynamic_cast<const SvOi::IInspectionProcess*> (pObject->GetAncestorInterface(SvPb::SVInspectionObjectType));
	if (nullptr != pInspection && pInspection->IsColorCamera())
	{
		const SvOi::IObjectClass* const pParent = SvOi::getObject(pObject->GetParentID());
		///Color inspection must have first tool as color tool
		if (nullptr != pParent && SvPb::SVToolSetObjectType == pParent->GetObjectType())
		{
			const SvOi::ITaskObjectListClass* const pParentTaskObjectList = dynamic_cast<const SvOi::ITaskObjectListClass* const> (pParent);
			if (nullptr != pParentTaskObjectList)
			{
				SvPb::TaskObjectListResponse taskObjListResponse;
				pParentTaskObjectList->GetTaskObjectListInfo(taskObjListResponse);
				int taskObjectSize = taskObjListResponse.taskobjectinfos_size();
				if (0 < taskObjectSize)
				{
					///If first tool is color tool then can only be deleted if second is also color tool
					if(pObject->getObjectId() == taskObjListResponse.taskobjectinfos(0).taskobjectid())
					{
						if(1 < taskObjectSize && SvPb::SVColorToolObjectType != taskObjListResponse.taskobjectinfos(1).objectsubtype())
						{
							SvStl::MessageContainer message(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ColorToolMustBeFirstMessage, SvStl::SourceFileParams(StdMessageParams));
							SvPb::setMessageToMessagePB(message, response.mutable_errormessages()->add_messages());
							response.set_hresult(E_FAIL);
						}
					}
				}
			}
		}
	}
	return response;
}

SvPb::StandardResponse canCreateObject(const SvOi::IObjectClass* const pParent, const SvOi::IObjectClass* const pObject, uint32_t taskObjectBeforeID)
{
	/// Do not need to check for nullptr from function parameters
	SvPb::StandardResponse response;
	const SvOi::IInspectionProcess* pInspection = dynamic_cast<const SvOi::IInspectionProcess*> (pParent->GetAncestorInterface(SvPb::SVInspectionObjectType));
	if (nullptr != pInspection && pInspection->IsColorCamera())
	{
		///Color inspection must have first tool as color tool 
		if (SvPb::SVToolSetObjectType == pParent->GetObjectType())
		{
			const SvOi::ITaskObjectListClass* const pParentTaskObjectList = dynamic_cast<const SvOi::ITaskObjectListClass* const> (pParent);
			if (nullptr != pParentTaskObjectList)
			{
				SvPb::TaskObjectListResponse taskObjListResponse;
				pParentTaskObjectList->GetTaskObjectListInfo(taskObjListResponse);
				int taskObjectSize = taskObjListResponse.taskobjectinfos_size();
				bool canCreate {true};
				///If first inserted tool then tool must be color
				if (0 == taskObjectSize)
				{
					canCreate = SvPb::SVColorToolObjectType == pObject->GetObjectSubType();
				}
				else
				{
					///We are only interested to see if the new task object is to be inserted in the first position
					if (taskObjectBeforeID == taskObjListResponse.taskobjectinfos(0).taskobjectid())
					{
						canCreate = SvPb::SVColorToolObjectType == pObject->GetObjectSubType();
					}
				}
				if (false == canCreate)
				{
					SvStl::MessageContainer message(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ColorToolMustBeFirstMessage, SvStl::SourceFileParams(StdMessageParams));
					SvPb::setMessageToMessagePB(message, response.mutable_errormessages()->add_messages());
					response.set_hresult(E_FAIL);
				}
			}
		}
	}
	return response;
}

SvPb::InspectionCmdResponse InspectionRunOnce(SvPb::InspectionRunOnceRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;
	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess *>(SvOi::getObject(request.inspectionid()));
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
		SvPb::StandardResponse stdResponse = canDeleteObject(pObject);
		if(S_OK != stdResponse.hresult())
		{
			*cmdResponse.mutable_standardresponse() = stdResponse;
			cmdResponse.set_hresult(stdResponse.hresult());
			return cmdResponse;
		}
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
		auto* pTaskObjectList = dynamic_cast<SvOi::ITaskObjectListClass*> (SvOi::getObject(pObject->GetParentID()));
		auto* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (pObject);
		if(nullptr != pTaskObject && nullptr != pTaskObjectList)
		{
			pTaskObjectList->DestroyChild(*pTaskObject, flag);
		}
	}
	else
	{
		cmdResponse.set_hresult(SvStl::Err_10024_DestroyChildObject_InvalidParameter);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse GetMessageList(SvPb::GetMessageListRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ITaskObject* pTask = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(request.objectid()));
	if (pTask)
	{
		SvPb::StandardResponse* pResponse = cmdResponse.mutable_standardresponse();
		pResponse->mutable_errormessages()->CopyFrom(SvPb::setMessageVectorToMessagePB(pTask->getErrorMessages()));
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
		SvStl::MessageContainerVector messages;
		HRESULT result = pObject->resetAllObjects(&messages) ? S_OK : E_FAIL;
		cmdResponse.set_hresult(result); 
		SvPb::StandardResponse* pResponse = cmdResponse.mutable_standardresponse();
		pResponse->mutable_errormessages()->CopyFrom(SvPb::setMessageVectorToMessagePB(messages));
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
			TCHAR FileExtension[_MAX_EXT];
			_tsplitpath(FileName.c_str(), nullptr, nullptr, nullptr, FileExtension);
			// Now save the Model Image buffer to a file
			SVMatroxFileTypeEnum FileFormatID = SVFileMIL; // Set as default.
			if (0 == SvUl::CompareNoCase(std::string(FileExtension), std::string(_T(".bmp"))))
			{
				FileFormatID = SVFileBitmap;
			}
			if (0 == SvUl::CompareNoCase(std::string(FileExtension), std::string(_T(".tif"))))
			{
				FileFormatID = SVFileTiff;
			}

			SVMatroxBuffer milBuffer = imageHandle->GetBuffer();
			HRESULT result = SVMatroxBufferInterface::Export(milBuffer, FileName, FileFormatID);
			if (S_OK != result)
			{
				cmdResponse.set_hresult(E_FAIL);
				message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, request.patternanalyzerid());
			}
		}
		else
		{
			cmdResponse.set_hresult(E_FAIL);
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, request.patternanalyzerid());
		}
	}
	else
	{
		cmdResponse.set_hresult(E_FAIL);
		message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, request.patternanalyzerid());
	}

	if(S_OK != cmdResponse.hresult())
	{
		SvPb::StandardResponse* pResponse = cmdResponse.mutable_standardresponse();
		SvPb::setMessageToMessagePB(message, pResponse->mutable_errormessages()->add_messages());
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getObjectParameters(SvPb::GetObjectParametersRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	if(nullptr != pObject)
	{
		SvPb::GetObjectParametersResponse* pObjParamResponse = cmdResponse.mutable_getobjectparametersresponse();
		pObjParamResponse->set_name(pObject->GetName());
		pObjParamResponse->set_allowedattributes(pObject->ObjectAttributesAllowed());
		pObjParamResponse->mutable_typeinfo()->set_objecttype(pObject->GetObjectType());
		pObjParamResponse->mutable_typeinfo()->set_subtype(pObject->GetObjectSubType());
		SvOi::ITaskObject* pTask = dynamic_cast<SvOi::ITaskObject*>(pObject);
		// cppcheck-suppress knownConditionTrueFalse //dynamic_cast can change to nullptr
		if (nullptr != pTask)
		{
			pObjParamResponse->set_isvalid(pTask->isErrorMessageEmpty());
		}
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

	SvOi::IEquation* pEquation = dynamic_cast<SvOi::IEquation *> (SvOi::getObject(request.objectid()));
	if (nullptr != pEquation)
	{
		std::string oldString;
		//save old string
		oldString = pEquation->GetEquationText();
		pEquation->SetEquationText(request.equationtext());
		SvStl::MessageContainerVector messages;
		SvOi::EquationTestResult testResult = pEquation->Test(&messages);
		SvPb::ValidateAndSetEquationResponse* pResponse = cmdResponse.mutable_validateandsetequationresponse();
		pResponse->mutable_messages()->CopyFrom(SvPb::setMessageVectorToMessagePB(messages));
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

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool *>(SvOi::getObject(request.objectid()));
	if (nullptr != pTool)
	{
		SvStl::MessageContainerVector messages;
		SvOi::ParametersForML paramList = pTool->getParameterForMonitorList(messages);
		SvPb::GetObjectsForMonitorListResponse* pResponse = cmdResponse.mutable_getobjectsformonitorlistresponse();
		pResponse->mutable_messages()->CopyFrom(SvPb::setMessageVectorToMessagePB(messages));
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
				pObject->moveFriendObject(request.objectid(), request.movepreid());
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
		SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage*>(SvOi::getObject(request.imageid()));
		if (pImage)
		{
			data = pImage->getImageData();
		}
	}
	else if (!request.imagename().empty() && SvDef::InvalidObjectId != request.parentid())
	{
		SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject*>(SvOi::getObject(request.parentid()));
		if (nullptr != pObject)
		{
			if (!pObject->getSpecialImage(request.imagename(), data))
			{
				SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams));
				SvPb::setMessageToMessagePB(message, pResponse->mutable_messages()->add_messages());
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
		SvPb::setMessageToMessagePB(message, pResponse->mutable_messages()->add_messages());
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse areAuxiliaryExtentsAvailable(SvPb::AreAuxiliaryExtentsAvailableRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool *>(SvOi::getObject(request.objectid()));
	if(nullptr != pTool)
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

SvPb::InspectionCmdResponse getAvailableAuxImages(SvPb::GetAvailableAuxImagesRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*> (SvOi::getObject(request.objectid()));
	if (nullptr != pTool)
	{
		SvPb::GetAvailableAuxImagesResponse* pResponse = cmdResponse.mutable_getavailableauximagesresponse();
		auto list = pTool->getAvailableAuxSourceImages();
		for (auto& item : list)
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

SvPb::InspectionCmdResponse getAuxImageObject(SvPb::GetAuxImageObjectRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool *>(SvOi::getObject(request.objectid()));
	if(nullptr != pTool)
	{
		auto sourceImage = pTool->getAuxSourceImage();
		SvPb::GetAuxImageObjectResponse* pResponse = cmdResponse.mutable_getauximageobjectresponse();
		pResponse->mutable_auxobject()->set_objectname(sourceImage.first.c_str());
		pResponse->mutable_auxobject()->set_objectid(sourceImage.second);
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse setAuxImageObject(SvPb::SetAuxImageObjectRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*> (SvOi::getObject(request.objectid()));
	if (nullptr != pTool)
	{
		HRESULT result = pTool->setAuxSourceImage(request.sourceimageid());
		if (S_OK != result)
		{
			cmdResponse.set_hresult(result);
			SvStl::MessageContainer message(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams));
			SvPb::StandardResponse* pResponse = cmdResponse.mutable_standardresponse();
			SvPb::setMessageToMessagePB(message, pResponse->mutable_errormessages()->add_messages());
		}
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
		SvUl::InputNameObjectIdPairList list;
		if (SvPb::SVImageObjectType == typeInfo.m_ObjectType)
		{
			pTaskObject->GetInputImages(list, request.maxrequested());
		}
		else
		{
			pTaskObject->GetInputs(list, typeInfo, request.objecttypetoinclude(), request.shouldexcludefirstobjectname());
		}
		SvPb::GetInputsResponse* pResponse = cmdResponse.mutable_getinputsresponse();
		for (auto& item : list)
		{
			auto* pEntry = pResponse->add_list();
			pEntry->set_inputname(item.first.c_str());
			pEntry->set_objectname(item.second.first);
			pEntry->set_objectid(item.second.second);
		}
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

	SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject*> (SvOi::getObject(request.objectid()));
	if(nullptr != pObject)
	{
		result.set_hresult(pObject->ConnectToObject(request.inputname(), request.newconnectedid(), request.objecttype()));
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

	SvOi::ISVImage * pObject = dynamic_cast<SvOi::ISVImage *>(SvOi::getObject(request.objectid()));
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

	SvOi::IObjectClass* pObject = SvOi::getObject(request.objectid());
	if (nullptr != pObject)
	{
		pObject->SetName(request.objectname().c_str());
	}
	else
	{
		cmdResponse.set_hresult(E_POINTER);
	}
	return cmdResponse;
}

SvPb::InspectionCmdResponse getAvailableObjects(SvPb::GetAvailableObjectsRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvDef::SVObjectTypeInfoStruct typeInfo {request.typeinfo().objecttype(), request.typeinfo().subtype(), request.typeinfo().embeddedid()};
	SvPb::SVObjectTypeEnum objectTypeToInclude = request.objecttypetoinclude();
	SVGetObjectDequeByTypeVisitor visitor(typeInfo);
	SvOi::visitElements(visitor, request.objectid());

	IsAllowedFunc isAllowed = getAllowedFunc(request);
	if (isAllowed)// required, even if it does nothing...
	{
		bool bStop = false;
		for(const auto* pObject: visitor.GetObjects())
		{
			if (nullptr != pObject)
			{

				if (isAllowed(pObject, bStop))
				{
					std::string name;
					switch (typeInfo.m_ObjectType)
					{
						case SvPb::SVImageObjectType:
						{
							const SvOi::ISVImage* pImage = dynamic_cast<const SvOi::ISVImage*>(pObject);
							if (pImage)
							{
								name = pImage->getDisplayedName();
							}
						}
						break;
						default:
						{
							if (SvPb::SVNotSetObjectType == objectTypeToInclude)
							{
								name = pObject->GetName();
							}
							else
							{
								if (request.shouldexcludefirstobjectname())
								{
									name = pObject->GetObjectNameBeforeObjectType(objectTypeToInclude);
								}
								else
								{
									name = pObject->GetObjectNameToObjectType(objectTypeToInclude);
								}
							}
						}
						break;
					}
					SvPb::GetAvailableObjectsResponse* pResponse = cmdResponse.mutable_getavailableobjectsresponse();
					if (!name.empty())
					{
						auto* pEntry = pResponse->add_list();
						pEntry->set_objectname(name.c_str());
						pEntry->set_objectid(pObject->getObjectId());
					}
				}
			}
			if (bStop)
			{
				return cmdResponse;
			}
		}
	}
	else
	{
		cmdResponse.set_hresult(E_NOINTERFACE);
	}
	return cmdResponse;
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

	SvOi::IMask* pObject = dynamic_cast<SvOi::IMask *>(SvOi::getObject(request.objectid()));
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

	SvOi::IMask* pObject = dynamic_cast<SvOi::IMask *>(SvOi::getObject(request.objectid()));
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
			pObject = SvOi::FindObject(request.info().ownerid() , typeInfo);
			break;
		}
		default:break;
	}
	if (pObject)
	{
		SvPb::GetObjectIdResponse* pResponse = cmdResponse.mutable_getobjectidresponse();
		pResponse->set_objectid(pObject->getObjectId());
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

	SvOi::IObjectClass* pObject{nullptr};
	switch (request.message_case())
	{
		case SvPb::CreateObjectRequest::kFriend:
		{
			pObject = SVObjectBuilder::CreateFriendObject(request.classid(), SvDef::InvalidObjectId, request.friend_().name(), request.ownerid(), request.friend_().preid());
			break;
		}
		case SvPb::CreateObjectRequest::kTaskObjectInsertBeforeId:
		{
			SvOi::IObjectClass* pParent = SvOi::getObject(request.ownerid());
			SvOi::ITaskObjectListClass* pParentTaskObjectList = dynamic_cast<SvOi::ITaskObjectListClass*>(pParent);
			SvOi::IObjectAppClass* pObjectApp = dynamic_cast<SvOi::IObjectAppClass*>(pParentTaskObjectList);

			pObject = SvOi::ConstructObject(request.classid());
			SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (pObject);

			if (nullptr != pParentTaskObjectList && nullptr != pTaskObject && nullptr != pObjectApp && nullptr != pObject)
			{
				SvPb::StandardResponse stdResponse = canCreateObject(pParent, pObject, request.taskobjectinsertbeforeid());

				if(S_OK != stdResponse.hresult())
				{
					///Delete the object
					delete pTaskObject;
					pObject = nullptr;
					*cmdResponse.mutable_standardresponse() = stdResponse;
					cmdResponse.set_hresult(stdResponse.hresult());
				}
				else
				{
					pParentTaskObjectList->InsertBefore(request.taskobjectinsertbeforeid(), *pTaskObject);

					// And last - Create (initialize) it
					if (!pObject->is_Created())
					{
						// And finally try to create the child object...
						if (!pObjectApp->CreateChildObject(*pObject, SvDef::SVMFResetObject))
						{
							cmdResponse.set_hresult(SvStl::Err_10021_InsertTaskObject_CreateObjectFailed);

							// Remove it from the Tool TaskObjectList ( Destruct it )
							uint32_t objectID = pObject->getObjectId();
							if (SvDef::InvalidObjectId != objectID)
							{
								pParentTaskObjectList->Delete(objectID);
							}
							else
							{
								delete pTaskObject;
							}
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
				}
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

	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess *>(SvOi::getObject(request.inspectionid()));
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
	auto EmbeddedVector = pTaskObj->getEmbeddedList();

	SvPb::GetEmbeddedValuesResponse* pResponse = cmdResponse.mutable_getembeddedvaluesresponse();
	for (auto const& rEntry : EmbeddedVector)
	{
		SvOi::IObjectClass* pObject = SvOi::getObject(rEntry);
		SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject);
		if (nullptr != pValueObject)
		{
			_variant_t DefaultValue {pValueObject->getDefaultValue()};
			_variant_t Value;
			HRESULT result = pValueObject->getValue(Value);
			if (S_OK == result)
			{
				auto* pElement = pResponse->add_list();
				pElement->set_objectid(rEntry);
				pElement->set_embeddedid(pObject->GetEmbeddedID());
				ConvertVariantToProtobuf(Value, pElement->mutable_value());
				ConvertVariantToProtobuf(DefaultValue, pElement->mutable_defaultvalue());
			}
			else if (E_BOUNDS == result || SVMSG_SVO_105_CIRCULAR_REFERENCE == result)
			{
				Value.Clear();
				auto* pElement = pResponse->add_list();
				pElement->set_objectid(rEntry);
				pElement->set_embeddedid(pObject->GetEmbeddedID());
				ConvertVariantToProtobuf(Value, pElement->mutable_value());
				ConvertVariantToProtobuf(DefaultValue, pElement->mutable_defaultvalue());
			}
			else
			{
				cmdResponse.set_hresult(result);
			}
		}
	}

	return cmdResponse;
}

SvPb::InspectionCmdResponse setEmbeddedValues(SvPb::SetEmbeddedValuesRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::SetValueStructVector ChangedValues;
	SvOi::SetValueStructVector ChangedDefaultValues;
	for (const auto& rItem : request.list())
	{
		SvOi::IObjectClass* pObject = SvOi::getObject(rItem.values().objectid());
		SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject);
		if (nullptr != pValueObject)
		{
			if (rItem.ismodified())
			{
				_variant_t value;
				ConvertProtobufToVariant(rItem.values().value(), value);
				ChangedValues.push_back(SvOi::SetValueStruct {pValueObject, value, rItem.arrayindex()});
			}
			if (rItem.isdefaultmodified())
			{
				// Default value has no array index
				_variant_t value;
				ConvertProtobufToVariant(rItem.values().defaultvalue(), value);
				ChangedDefaultValues.push_back(SvOi::SetValueStruct {pValueObject, value, -1});
			}
		}
		else
		{
			cmdResponse.set_hresult(E_POINTER);
		}
	}

	SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (SvOi::getObject(request.objectid()));
	if (nullptr != pTaskObject)
	{
		SvStl::MessageContainerVector messages = pTaskObject->validateAndSetEmbeddedValues(ChangedValues, true);
		if (0 == messages.size())
		{
			messages = pTaskObject->setEmbeddedDefaultValues(ChangedDefaultValues);
		}

		SvPb::StandardResponse* pResponse = cmdResponse.mutable_standardresponse();
		if (0 != messages.size())
		{
			pResponse->mutable_errormessages()->CopyFrom(SvPb::setMessageVectorToMessagePB(messages));
			cmdResponse.set_hresult(E_FAIL);
		}
	}
	else
	{
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

		getObjectsRequest.set_objectid(request.objectid());
		getObjectsRequest.mutable_typeinfo()->set_objecttype(SvPb::SVToolSizeAdjustTaskType);
		getObjectsRequest.mutable_typeinfo()->set_subtype(SvPb::SVNotSetSubObjectType);
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
	if(nullptr != pTool)
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

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*> (SvOi::getObject(request.objectid()));
	SvPb::GetExtentParameterResponse* pResponse = cmdResponse.mutable_getextentparameterresponse();
	auto* pParameter = pResponse->mutable_parameters();
	if (nullptr != pTool && nullptr != pParameter)
	{
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
		HRESULT result{S_OK};
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
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
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
	for (int i = 0; i < request.types_size(); i++)
	{
		std::vector<SvPb::TreeItem> itemVector;
		switch (request.types(i))
		{
			case SvPb::ObjectSelectorType::globalConstantItems:
			{

				itemVector = SvOi::getRootChildSelectorList(_T(""), request.attribute());
				break;
			}

			case SvPb::ObjectSelectorType::tableObjects:
			case SvPb::ObjectSelectorType::ppqItems:
			case SvPb::ObjectSelectorType::toolsetItems:
			case SvPb::ObjectSelectorType::cameraObject:
			{
				itemVector = getSelectorList(request, request.types(i));
				break;
			}

			default:
			{
				break;
			}
		}
		resultItemVector.insert(resultItemVector.end(), itemVector.begin(), itemVector.end());
	}
	*pResponse->mutable_tree()->mutable_children() = {resultItemVector.begin(), resultItemVector.end()};

	return cmdResponse;
}

std::vector<SvPb::TreeItem> getSelectorList(SvPb::GetObjectSelectorItemsRequest request, SvPb::ObjectSelectorType selectorType)
{
	std::vector<SvPb::TreeItem> result;

	if (SvPb::ObjectSelectorType::ppqItems == selectorType)
	{
		SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (SvOi::getObject(request.inspectionid()));
		if (nullptr != pInspection)
		{
			result = pInspection->GetPPQSelectorList(request.attribute());
			return result;
		}
	}

	if (SvPb::ObjectSelectorType::cameraObject == selectorType)
	{
		SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (SvOi::getObject(request.inspectionid()));
		if (nullptr != pInspection)
		{
			result = pInspection->GetCameraSelectorList(request.attribute());
			return result;
		}
	}

	if (SvPb::ObjectSelectorType::tableObjects == selectorType)
	{

		uint32_t id = (SvDef::InvalidObjectId != request.instanceid()) ? request.instanceid() : request.inspectionid();
		SvOi::IObjectClass* pObject = SvOi::getObject(id);
		SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (pObject);
		SvOi::ITaskObject* pTaskObject(nullptr);
		if (pInspection)
		{
			pTaskObject = pInspection->GetToolSetInterface();
		}
	
		if (nullptr != pTaskObject)
		{
		
			IsObjectInfoAllowed pFunc = SvCmd::TableObjectSelector();
			pTaskObject->GetObjectSelectorList(pFunc, result);
			return result;
		}
	}

	uint32_t id = (SvDef::InvalidObjectId != request.instanceid()) ? request.instanceid() : request.inspectionid();
	SvOi::IObjectClass* pObject = SvOi::getObject(id);
	if (nullptr != pObject)
	{
		SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (pObject);

		// When using the RangeSelectorFilter, the InstanceId is for the Range or Tool owning the Range
		// which is needed to get the name for exclusion in filtering, so get the Toolset as well 
		if (SvPb::SelectorFilter::rangeValue == request.filter())
		{
			pTaskObject = dynamic_cast<SvOi::ITaskObject *>(pObject->GetAncestorInterface(SvPb::SVToolSetObjectType));
		}

		if (nullptr == pTaskObject)
		{
			SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (pObject);
			if (nullptr != pInspection)
			{
				pTaskObject = pInspection->GetToolSetInterface();
			}
		}

		//If instance ID is set to the inspection ID then we need the name including the inspection name
		SvPb::SVObjectTypeEnum 	objectTypeToName{SvPb::SVNotSetObjectType};
		if(request.inspectionid() == request.instanceid())
		{
			objectTypeToName = SvPb::SVInspectionObjectType;
		}

		if (nullptr != pTaskObject)
		{
			std::string name;
			pObject->GetCompleteNameToType(SvPb::SVToolObjectType, name);
			IsObjectInfoAllowed pFunc = getObjectSelectorFilterFunc(request, name);
			if(nullptr != pFunc)
			{
				result = pTaskObject->GetSelectorList(pFunc, request.attribute(), request.wholearray(), objectTypeToName);
			}
		}
	}
	return result;
}

SvPb::InspectionCmdResponse getNormalizerValues(SvPb::GetNormalizerValuesRequest request)
{
	SvPb::InspectionCmdResponse cmdResponse;

	SvOi::ISVLinearAnalyzer* pAnalyzer = dynamic_cast<SvOi::ISVLinearAnalyzer*>(SvOi::getObject(request.linearanalyzerid()));

	if(nullptr != pAnalyzer )
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

	if (nullptr != pImage )
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


	auto rect = RECT{ 0 };
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
	{ }
	
		
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


} //namespace SvCmd