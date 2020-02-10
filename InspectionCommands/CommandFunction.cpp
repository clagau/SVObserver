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
#include "ObjectInterfaces\IEnumerateValueObject.h"
#include "ObjectInterfaces\IEquation.h"
#include "ObjectInterfaces\IFormulaController.h"
#include "ObjectInterfaces\ISVFilter.h"
#include "ObjectInterfaces\IInspectionProcess.h"
#include "ObjectInterfaces\IMask.h"
#include "ObjectInterfaces\IRootObject.h"
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\ITaskObject.h"
#include "ObjectInterfaces\ITaskObjectListClass.h"
#include "ObjectInterfaces\ITool.h"
#include "ObjectInterfaces\IToolSizeAdjustTask.h"
#include "ObjectInterfaces\IObjectAppClass.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\IPatternAnalyzer.h"
#include "Definitions\ObjectDefines.h"
#include "Definitions\TextDefineSvDef.h"
#include "SVStatusLibrary\ErrorNumbers.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVMatroxLibrary\SVMatroxSimpleEnums.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary\StringHelper.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVObjectLibrary\SVObjectBuilder.h"
#include "SVObjectLibrary\SVObjectClass.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVProtoBuf\ConverterHelper.h"
#include "CommandInternalHelper.h"
#include "ObjectSelectorFilter.h"
#pragma endregion Includes

namespace SvCmd
{
InspectionCmdResult InspectionRunOnce(SvPb::InspectionRunOnceRequest request)
{
	InspectionCmdResult result;

	SVGUID InspectionID = SvPb::GetGuidFromProtoBytes(request.inspectionid());

	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess *>(SvOi::getObject(InspectionID));
	if (nullptr != pInspection)
	{
		result.m_hResult = pInspection->RunOnce();
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult DestroyChildObject(SvPb::DestroyChildRequest request)
{
	InspectionCmdResult result;

	SvOi::ITaskObjectListClass *pTaskObjectList = dynamic_cast<SvOi::ITaskObjectListClass*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.taskobjectlistid())));
	SvOi::ITaskObject *pObject = dynamic_cast<SvOi::ITaskObject*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (nullptr != pTaskObjectList && nullptr != pObject)
	{
		DWORD flag = 0;
		switch (request.flag())
		{
			case SvPb::DestroyChildRequest::Flag_SetDefaultInputs:
				flag = SvDef::SVMFSetDefaultInputs;
				break;
			case SvPb::DestroyChildRequest::Flag_ResetInspection:
				flag = SvDef::SVMFResetInspection;
				break;
			case SvPb::DestroyChildRequest::Flag_SetDefaultInputs_And_ResetInspection:
				flag = SvDef::SVMFSetDefaultInputs | SvDef::SVMFResetInspection;
				break;
			default:
				flag = 0;
				break;
		}
		pTaskObjectList->DestroyChild(*pObject, flag);
	}
	else
	{
		result.m_hResult = SvStl::Err_10024_DestroyChildObject_InvalidParameter;
	}
	return result;
}

InspectionCmdResult GetMessageList(SvPb::GetMessageListRequest request)
{
	InspectionCmdResult result;

	SvOi::ITaskObject* pTask = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (pTask)
	{
		SvPb::GetMessageListResponse* pResponse = result.m_response.mutable_getmessagelistresponse();
		pResponse->mutable_messages()->CopyFrom(setMessageContainerToMessagePB(pTask->getErrorMessages()));
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult ResetObject(SvPb::ResetObjectRequest request)
{
	InspectionCmdResult result;

	SvOi::IObjectClass* pObject = SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid()));

	if (nullptr != pObject)
	{
		SvStl::MessageContainerVector messages;
		result.m_hResult = pObject->resetAllObjects(&messages) ? S_OK : E_FAIL;
		SvPb::ResetObjectResponse* pResponse = result.m_response.mutable_resetobjectresponse();
		pResponse->mutable_messages()->CopyFrom(setMessageContainerToMessagePB(messages));
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult CreateModel(SvPb::CreateModelRequest request)
{
	InspectionCmdResult result;

	SvStl::MessageContainerVector messages;
	SVGUID patternID = SvPb::GetGuidFromProtoBytes(request.patternanalyzerid());
	SvOi::IPatternAnalyzer* pPatAnalyzer = dynamic_cast<SvOi::IPatternAnalyzer*>(SvOi::getObject(patternID));

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
			result.m_hResult = SVMatroxBufferInterface::Export(milBuffer, FileName, FileFormatID);
			if (S_OK != result.m_hResult)
			{
				result.m_hResult = E_FAIL;
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, patternID);
				messages.push_back(Msg);
			}
		}
		else
		{
			result.m_hResult = E_FAIL;
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, patternID);
			messages.push_back(Msg);
		}
	}
	else
	{
		result.m_hResult = E_FAIL;
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, patternID);
		messages.push_back(Msg);
	}

	SvPb::CreateModelResponse* pResponse = result.m_response.mutable_createmodelresponse();
	pResponse->mutable_messages()->CopyFrom(setMessageContainerToMessagePB(messages));

	return result;
}

InspectionCmdResult getObjectParameters(SvPb::GetObjectParametersRequest request)
{
	InspectionCmdResult result;

	SvOi::IObjectClass* pObject = SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid()));
	if (pObject)
	{
		SvPb::GetObjectParametersResponse* pResponse = result.m_response.mutable_getobjectparametersresponse();
		pResponse->set_name(pObject->GetName());
		pResponse->set_allowedattributes(pObject->ObjectAttributesAllowed());
		pResponse->mutable_typeinfo()->set_objecttype(pObject->GetObjectType());
		pResponse->mutable_typeinfo()->set_subtype(pObject->GetObjectSubType());
		SvOi::ITaskObject* pTask = dynamic_cast<SvOi::ITaskObject*>(pObject);
		if (nullptr != pTask)
		{
			pResponse->set_isvalid(pTask->isErrorMessageEmpty());
		}
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult GetEquation(SvPb::GetEquationRequest request)
{
	InspectionCmdResult result;

	SvOi::IEquation* pEquation = dynamic_cast<SvOi::IEquation *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (pEquation)
	{
		SvPb::GetEquationResponse* pResponse = result.m_response.mutable_getequationresponse();
		pResponse->set_equationtext(pEquation->GetEquationText());
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult ValidateAndSetEquation(SvPb::ValidateAndSetEquationRequest request)
{
	InspectionCmdResult result;
	result.m_hResult =  E_FAIL;

	SvOi::IEquation* pEquation = dynamic_cast<SvOi::IEquation *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (pEquation)
	{
		std::string oldString;
		//save old string
		oldString = pEquation->GetEquationText();
		pEquation->SetEquationText(request.equationtext());
		SvStl::MessageContainerVector messages;
		SvOi::EquationTestResult testResult = pEquation->Test(&messages);
		SvPb::ValidateAndSetEquationResponse* pResponse = result.m_response.mutable_validateandsetequationresponse();
		pResponse->mutable_messages()->CopyFrom(setMessageContainerToMessagePB(messages));
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
		result.m_hResult = S_OK;
	}
	return result;
}

InspectionCmdResult getObjectsForMonitorList(SvPb::GetObjectsForMonitorListRequest request)
{
	InspectionCmdResult result;
	result.m_hResult = E_FAIL;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (nullptr != pTool)
	{
		SvStl::MessageContainerVector messages;
		SvOi::ParametersForML paramList = pTool->getParameterForMonitorList(messages);
		SvPb::GetObjectsForMonitorListResponse* pResponse = result.m_response.mutable_getobjectsformonitorlistresponse();
		pResponse->mutable_messages()->CopyFrom(setMessageContainerToMessagePB(messages));
		for (auto& item : paramList)
		{
			auto* pEntry = pResponse->add_list();
			pEntry->set_objectname(item.first.c_str());
			SvPb::SetGuidInProtoBytes(pEntry->mutable_objectid(), item.second);
		}
		result.m_hResult = S_OK;
	}

	return result;
}

InspectionCmdResult MoveObject(SvPb::MoveObjectRequest request)
{
	InspectionCmdResult result;

	SvOi::IObjectClass* pObject = SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.parentid()));
	if (nullptr != pObject)
	{
		switch (request.listmode())
		{
			case SvPb::MoveObjectRequest::TaskObjectList:
			{
				SvOi::ITaskObjectListClass* pTaskObject = dynamic_cast<SvOi::ITaskObjectListClass*>(pObject);
				if (nullptr != pTaskObject)
				{
					pTaskObject->moveTaskObject(SvPb::GetGuidFromProtoBytes(request.objectid()), SvPb::GetGuidFromProtoBytes(request.movepreid()));
				}
				else
				{
					result.m_hResult = E_FAIL;
				}
				break;
			}
			case SvPb::MoveObjectRequest::FriendList:
				pObject->moveFriendObject(SvPb::GetGuidFromProtoBytes(request.objectid()), SvPb::GetGuidFromProtoBytes(request.movepreid()));
				break;
			default:
				result.m_hResult = E_FAIL;
				break;
		}
	}
	else
	{
		result.m_hResult = E_FAIL;
	}

	return result;
}

InspectionCmdResult GetTaskObjectsList(SvPb::TaskObjectListRequest request)
{
	InspectionCmdResult result;

	SvOi::ITaskObjectListClass* pTaskObjectList = dynamic_cast<SvOi::ITaskObjectListClass*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.taskobjectid())));
	if (nullptr != pTaskObjectList)
	{
		SvPb::TaskObjectListResponse* pResponse = result.m_response.mutable_taskobjectlistresponse(); 
		pTaskObjectList->GetTaskObjectListInfo(*pResponse);
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult getImage(SvPb::GetImageRequest request)
{
	InspectionCmdResult result;
	SvOi::SVImageBufferHandlePtr data;

	SvPb::GetImageResponse* pResponse = result.m_response.mutable_getimageresponse();
	if (0 < request.imageid().size())
	{
		SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.imageid())));
		if (pImage)
		{
			data = pImage->getImageData();
		}
	}
	else if (!request.imagename().empty() && 0 < request.parentid().size())
	{
		SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.parentid())));
		if (nullptr != pObject)
		{
			if (!pObject->getSpecialImage(request.imagename(), data))
			{
				SvStl::MessageContainerVector messages;
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams));
				messages.push_back(Msg);
				pResponse->mutable_messages()->CopyFrom(setMessageContainerToMessagePB(messages));
				result.m_hResult= E_FAIL;
				return result;
			}
		}
	}

	SvOi::SVImageBufferHandleInterface* pImageData = data.get();
	if (nullptr != pImageData && !pImageData->empty())
	{
		std::string ImageBuffer;
		SVBitmapInfo bitmapInfo;
		result.m_hResult = SVMatroxBufferInterface::CopyBufferToFileDIB(ImageBuffer, bitmapInfo, pImageData->GetBuffer());
		pResponse->mutable_imagedata()->mutable_rgbdata()->swap(ImageBuffer);
		pResponse->mutable_imagedata()->set_width(std::abs(bitmapInfo.GetWidth()));
		pResponse->mutable_imagedata()->set_height(std::abs(bitmapInfo.GetHeight()));
	}
	else
	{
		SvStl::MessageContainerVector messages;
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams));
		messages.push_back(Msg);
		pResponse->mutable_messages()->CopyFrom(setMessageContainerToMessagePB(messages));
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult areAuxiliaryExtentsAvailable(SvPb::AreAuxiliaryExtentsAvailableRequest request)
{
	InspectionCmdResult result;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (pTool)
	{
		SvPb::AreAuxiliaryExtentsAvailableResponse* pResponse = result.m_response.mutable_areauxiliaryextentsavailableresponse();
		pResponse->set_areavailable(pTool->areAuxExtentsAvailable());
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult getAvailableAuxImages(SvPb::GetAvailableAuxImagesRequest request)
{
	InspectionCmdResult result;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*> (SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (pTool)
	{
		SvPb::GetAvailableAuxImagesResponse*pResponse = result.m_response.mutable_getavailableauximagesresponse();
		auto list = pTool->getAvailableAuxSourceImages();
		for (auto& item : list)
		{
			auto* pEntry = pResponse->add_list();
			pEntry->set_objectname(item.first.c_str());
			SvPb::SetGuidInProtoBytes(pEntry->mutable_objectid(), item.second);
		}
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult getAuxImageObject(SvPb::GetAuxImageObjectRequest request)
{
	InspectionCmdResult result;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (pTool)
	{
		auto sourceImage = pTool->getAuxSourceImage();
		SvPb::GetAuxImageObjectResponse* pResponse = result.m_response.mutable_getauximageobjectresponse();
		pResponse->mutable_auxobject()->set_objectname(sourceImage.first.c_str());
		SvPb::SetGuidInProtoBytes(pResponse->mutable_auxobject()->mutable_objectid(), sourceImage.second);
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult setAuxImageObject(SvPb::SetAuxImageObjectRequest request)
{
	InspectionCmdResult result;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*> (SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (pTool)
	{
		result.m_hResult = pTool->setAuxSourceImage(SvPb::GetGuidFromProtoBytes(request.sourceimageid()));
		if (S_OK != result.m_hResult)
		{
			SvStl::MessageContainerVector messages;
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams));
			messages.push_back(Msg);
			SvPb::SetAuxImageObjectResponse* pResponse = result.m_response.mutable_setauximageobjectresponse();
			pResponse->mutable_messages()->CopyFrom(setMessageContainerToMessagePB(messages));
		}
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult getInputs(SvPb::GetInputsRequest request)
{
	InspectionCmdResult result;

	SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (nullptr != pTaskObject)
	{
		SvDef::SVObjectTypeInfoStruct typeInfo {request.typeinfo().objecttype(), request.typeinfo().subtype(), SvPb::GetGuidFromProtoBytes(request.typeinfo().embeddedid())};
		SvUl::InputNameGuidPairList list;
		if (SvPb::SVImageObjectType == typeInfo.ObjectType)
		{
			pTaskObject->GetInputImages(list, request.maxrequested());
		}
		else
		{
			pTaskObject->GetInputs(list, typeInfo, request.objecttypetoinclude(), request.shouldexcludefirstobjectname());
		}
		SvPb::GetInputsResponse* pResponse = result.m_response.mutable_getinputsresponse();
		for (auto& item : list)
		{
			auto* pEntry = pResponse->add_list();
			pEntry->set_inputname(item.first.c_str());
			pEntry->set_objectname(item.second.first);
			SvPb::SetGuidInProtoBytes(pEntry->mutable_objectid(), item.second.second);
		}
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult connectToObject(SvPb::ConnectToObjectRequest request)
{
	InspectionCmdResult result;

	SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (pObject)
	{
		result.m_hResult = pObject->ConnectToObject(request.inputname(), SvPb::GetGuidFromProtoBytes(request.newconnectedid()), request.objecttype());
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult saveImage(SvPb::SaveImageRequest request)
{
	InspectionCmdResult result;

	SvOi::ISVImage * pObject = dynamic_cast<SvOi::ISVImage *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (pObject)
	{
		result.m_hResult = pObject->Save(request.imagename());
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult setObjectName(SvPb::SetObjectNameRequest request)
{
	InspectionCmdResult result;

	SvOi::IObjectClass* pObject = SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid()));
	if (pObject)
	{
		pObject->SetName(request.objectname().c_str());
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult getAvailableObjects(SvPb::GetAvailableObjectsRequest request)
{
	InspectionCmdResult result;

	SvDef::SVObjectTypeInfoStruct typeInfo {request.typeinfo().objecttype(), request.typeinfo().subtype(), SvPb::GetGuidFromProtoBytes(request.typeinfo().embeddedid())};
	SvPb::SVObjectTypeEnum objectTypeToInclude = request.objecttypetoinclude();
	SVGUID instanceId = SvPb::GetGuidFromProtoBytes(request.objectid());
	SVGetObjectDequeByTypeVisitor visitor(typeInfo);
	SvOi::visitElements(visitor, instanceId);

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
					switch (typeInfo.ObjectType)
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
					SvPb::GetAvailableObjectsResponse* pResponse = result.m_response.mutable_getavailableobjectsresponse();
					if (!name.empty())
					{
						auto* pEntry = pResponse->add_list();
						pEntry->set_objectname(name.c_str());
						SvPb::SetGuidInProtoBytes(pEntry->mutable_objectid(), pObject->GetUniqueObjectID());
					}
				}
			}
			if (bStop)
			{
				return result;
			}
		}
	}
	else
	{
		result.m_hResult = E_NOINTERFACE;
	}
	return result;
}

InspectionCmdResult getSpecialImageList(SvPb::GetSpecialImageListRequest request)
{
	InspectionCmdResult result;

	const SvOi::ITaskObject* pObject = dynamic_cast<const SvOi::ITaskObject*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.taskobjectid())));
	if (nullptr != pObject)
	{
		SvDef::StringVector specialImageList = pObject->getSpecialImageList();
		SvPb::GetSpecialImageListResponse* pResponse = result.m_response.mutable_getspecialimagelistresponse();
		for (auto tmp : specialImageList)
		{
			pResponse->add_specialimagenames(tmp);
		}
	}
	else
	{
		result.m_hResult = E_FAIL;
	}
	return result;
}

InspectionCmdResult exportMask(SvPb::ExportMaskRequest request)
{
	InspectionCmdResult result;

	SvOi::IMask* pObject = dynamic_cast<SvOi::IMask *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (pObject)
	{
		result.m_hResult = pObject->Export(request.filename());
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult importMask(SvPb::ImportMaskRequest request)
{
	InspectionCmdResult result;

	SvOi::IMask* pObject = dynamic_cast<SvOi::IMask *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (pObject)
	{
		result.m_hResult = pObject->Import(request.filename());
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult getObjectId(SvPb::GetObjectIdRequest request)
{
	InspectionCmdResult result;

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
			SvDef::SVObjectTypeInfoStruct typeInfo {rInfoStruct.objecttype(), rInfoStruct.subtype(), SvPb::GetGuidFromProtoBytes(rInfoStruct.embeddedid())};
			pObject = SvOi::FindObject(SvPb::GetGuidFromProtoBytes(request.info().ownerid()) , typeInfo);
			break;
		}
		default:break;
	}
	if (pObject)
	{
		SvPb::GetObjectIdResponse* pResponse = result.m_response.mutable_getobjectidresponse();
		SvPb::SetGuidInProtoBytes(pResponse->mutable_objectid(), pObject->GetUniqueObjectID());
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult constructAndInsert(SvPb::ConstructAndInsertRequest request)
{
	InspectionCmdResult result;

	SVGUID ownerId = SvPb::GetGuidFromProtoBytes(request.ownerid());

	SvOi::IObjectClass* pObject = nullptr;
	switch (request.message_case())
	{
		case SvPb::ConstructAndInsertRequest::kFriend:
		{
			pObject = SVObjectBuilder::CreateFriendObject(request.classid(), GUID_NULL, request.friend_().name(), ownerId, SvPb::GetGuidFromProtoBytes(request.friend_().preguid()));
			break;
		}
		case SvPb::ConstructAndInsertRequest::kTaskObjectPos:
		case SvPb::ConstructAndInsertRequest::kTaskObjectAfterId:
		{
			SvOi::ITaskObjectListClass* pTaskObjectList = dynamic_cast<SvOi::ITaskObjectListClass*>(SvOi::getObject(ownerId));
			SvOi::IObjectAppClass* pObjectApp = dynamic_cast<SvOi::IObjectAppClass*>(pTaskObjectList);

			SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*>(SvOi::ConstructObject(request.classid()));
			pObject = dynamic_cast<SvOi::IObjectClass*>(pTaskObject);

			if (nullptr != pTaskObjectList && nullptr != pTaskObject && nullptr != pObjectApp && nullptr != pObject)
			{
				if (SvPb::ConstructAndInsertRequest::kTaskObjectPos == request.message_case())
				{
					pTaskObjectList->InsertAt(request.taskobjectpos(), *pTaskObject);
				}
				else
				{
					pTaskObjectList->InsertAfter(SvPb::GetGuidFromProtoBytes(request.taskobjectafterid()), *pTaskObject);
				}

				// And last - Create (initialize) it
				if (!pObject->is_Created())
				{
					// And finally try to create the child object...
					if (!pObjectApp->CreateChildObject(*pObject, SvDef::SVMFResetObject))
					{
						result.m_hResult = SvStl::Err_10021_InsertTaskObject_CreateObjectFailed;

						// Remove it from the Tool TaskObjectList ( Destruct it )
						const SVGUID& rObjectID = pObject->GetUniqueObjectID();
						if (GUID_NULL != rObjectID)
						{
							pTaskObjectList->Delete(rObjectID.ToGUID());
						}
						else
						{
							delete pTaskObject;
						}
						pObject = nullptr;
					}
					else
					{
						SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*>(pObject);
						if (nullptr != pTool)
						{
							pTool->finishAddTool();
						}
					}
				}
			}
			break;
		}
		default:break;
	}

	if (S_OK == result.m_hResult)
	{
		if (nullptr != pObject)
		{
			SvPb::ConstructAndInsertResponse* pResponse = result.m_response.mutable_constructandinsertresponse();
			SvPb::SetGuidInProtoBytes(pResponse->mutable_objectid(), pObject->GetUniqueObjectID());
			pResponse->set_name(pObject->GetName());
		}
		else
		{
			result.m_hResult = E_FAIL;
		}
	}
	return result;
}

InspectionCmdResult getCreatableObjects(SvPb::GetCreatableObjectsRequest request)
{
	InspectionCmdResult result;

	SvOi::IObjectClass* pObject = SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid()));
	if (nullptr != pObject)
	{
		SvPb::GetCreatableObjectsResponse* pResponse = result.m_response.mutable_getcreatableobjectsresponse();
		SvDef::SVObjectTypeInfoStruct typeInfo {request.typeinfo().objecttype(), request.typeinfo().subtype(), SvPb::GetGuidFromProtoBytes(request.typeinfo().embeddedid())};
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
		result.m_hResult = E_NOINTERFACE;
	}
	return result;
}

InspectionCmdResult shouldInspectionReset(SvPb::ShouldInspectionResetRequest request)
{
	InspectionCmdResult result;

	SvOi::ISVFilter* pFilter = dynamic_cast<SvOi::ISVFilter*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (nullptr != pFilter)
	{
		SvPb::ShouldInspectionResetResponse* pResponse = result.m_response.mutable_shouldinspectionresetresponse();
		pResponse->set_shouldreset(pFilter->shouldResetInspection());
	}
	else
	{
		result.m_hResult = SvStl::Err_10025_ShouldResetInspection_InvalidParameter;
	}
	return result;
}

InspectionCmdResult getPPQName(SvPb::GetPPQNameRequest request)
{
	InspectionCmdResult result;
	result.m_hResult = E_POINTER;

	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.inspectionid())));
	if (pInspection)
	{
		SvOi::IObjectClass* pPPQ = pInspection->GetPPQInterface();
		if (nullptr != pPPQ)
		{
			SvPb::GetPPQNameResponse* pResponse = result.m_response.mutable_getppqnameresponse();
			pResponse->set_ppqname(pPPQ->GetName());
			result.m_hResult = S_OK;
		}
	}
	return result;
}

InspectionCmdResult getValueObjectEnums(SvPb::GetValueObjectEnumsRequest request)
{
	InspectionCmdResult result;

	SvOi::IEnumerateValueObject* pValueObject = dynamic_cast<SvOi::IEnumerateValueObject*> (SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (pValueObject)
	{
		SvPb::GetValueObjectEnumsResponse* pResponse = result.m_response.mutable_getvalueobjectenumsresponse();
		for (const auto pairValue : pValueObject->GetEnumVector())
		{
			auto* pEntry = pResponse->add_list();
			pEntry->set_name(pairValue.first);
			pEntry->set_value(pairValue.second);
		}
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult getEmbeddedValues(SvPb::GetEmbeddedValuesRequest request)
{
	InspectionCmdResult result;

	SvOi::ITaskObject* pTaskObj = dynamic_cast<SvOi::ITaskObject*> (SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	SVGuidVector EmbeddedVector = pTaskObj->getEmbeddedList();

	SvPb::GetEmbeddedValuesResponse* pResponse = result.m_response.mutable_getembeddedvaluesresponse();
	for (auto const& rEntry : EmbeddedVector)
	{
		SvOi::IObjectClass* pObject = SvOi::getObject(rEntry);
		SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject);
		if (nullptr != pValueObject)
		{
			_variant_t DefaultValue {pValueObject->getDefaultValue()};
			_variant_t Value;
			HRESULT tmpHr = pValueObject->getValue(Value);
			if (S_OK == tmpHr)
			{
				auto* pElement = pResponse->add_list();
				SvPb::SetGuidInProtoBytes(pElement->mutable_objectid(), rEntry);
				SvPb::SetGuidInProtoBytes(pElement->mutable_embeddedid(), pObject->GetEmbeddedID());
				ConvertVariantToProtobuf(Value, pElement->mutable_value());
				ConvertVariantToProtobuf(DefaultValue, pElement->mutable_defaultvalue());
			}
			else if (E_BOUNDS == tmpHr || SVMSG_SVO_105_CIRCULAR_REFERENCE == tmpHr)
			{
				Value.Clear();
				auto* pElement = pResponse->add_list();
				SvPb::SetGuidInProtoBytes(pElement->mutable_objectid(), rEntry);
				SvPb::SetGuidInProtoBytes(pElement->mutable_embeddedid(), pObject->GetEmbeddedID());
				ConvertVariantToProtobuf(Value, pElement->mutable_value());
				ConvertVariantToProtobuf(DefaultValue, pElement->mutable_defaultvalue());
			}
			else
			{
				result.m_hResult = tmpHr;
			}
		}
	}

	return result;
}

InspectionCmdResult setEmbeddedValues(SvPb::SetEmbeddedValuesRequest request)
{
	InspectionCmdResult result;

	SvOi::SetValueStructVector ChangedValues;
	SvOi::SetValueStructVector ChangedDefaultValues;
	for (const auto& rItem : request.list())
	{
		SvOi::IObjectClass* pObject = SvOi::getObject(SvPb::GetGuidFromProtoBytes(rItem.values().objectid()));
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
			result.m_hResult = E_POINTER;
		}
	}

	SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*> (SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (nullptr != pTaskObject)
	{
		SvStl::MessageContainerVector messages = pTaskObject->validateAndSetEmbeddedValues(ChangedValues, true);
		if (0 == messages.size())
		{
			messages = pTaskObject->setEmbeddedDefaultValues(ChangedDefaultValues);
		}

		SvPb::SetEmbeddedValuesResponse* pResponse = result.m_response.mutable_setembeddedvaluesresponse();
		pResponse->mutable_messages()->CopyFrom(setMessageContainerToMessagePB(messages));
		if (0 != messages.size())
		{
			result.m_hResult = E_FAIL;
		}
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult getOutputRectangle(SvPb::GetOutputRectangleRequest request)
{
	InspectionCmdResult result;

	SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.imageid())));
	if (pImage)
	{
		SvPb::GetOutputRectangleResponse* pResponse = result.m_response.mutable_getoutputrectangleresponse();
		auto rect = pImage->GetOutputRectangle();
		pResponse->set_left(rect.left);
		pResponse->set_top(rect.top);
		pResponse->set_right(rect.right);
		pResponse->set_bottom(rect.bottom);
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult getToolSizeAdjustParameter(SvPb::GetToolSizeAdjustParameterRequest request)
{
	InspectionCmdResult result;

	auto pObject = SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid()));
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
		result = getAvailableObjects(getObjectsRequest);
		const SvPb::GetAvailableObjectsResponse& rObjectResponse = result.m_response.getavailableobjectsresponse();
		if (S_OK == result.m_hResult && 0 < rObjectResponse.list_size())
		{
			pTask = dynamic_cast<SvOi::IToolSizeAdjustTask*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rObjectResponse.list(0).objectid())));
		}
	}

	SvPb::GetToolSizeAdjustParameterResponse* pResponse = result.m_response.mutable_gettoolsizeadjustparameterresponse();
	if (nullptr != pTask)
	{
		pResponse->set_isfullsizeallowed(pTask->IsFullSizeAllowed());
		pResponse->set_isadjustsizeallowed(pTask->IsAdjustSizeAllowed());
		pResponse->set_isadjustpositionallowed(pTask->IsAdjustPositionAllowed());
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	if(nullptr != pTool)
	{
		pResponse->set_enableautosize(pTool->getAutoSizeEnabled());
		pResponse->set_canresizetoparent(pTool->canResizeToParent());
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult getExtentParameter(SvPb::GetExtentParameterRequest request)
{
	InspectionCmdResult result;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*> (SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	SvPb::GetExtentParameterResponse* pResponse = result.m_response.mutable_getextentparameterresponse();
	auto* pParameter = pResponse->mutable_parameters();
	if (nullptr != pTool && nullptr != pParameter)
	{
		SvPb::SVExtentTranslationEnum translationType = SvPb::SVExtentTranslationUnknown;
		if (request.shouldfromparent())
		{
			result.m_hResult = pTool->getParentExtentProperties(*pParameter, translationType);
		}
		else
		{
			pTool->getExtentProperties(*pParameter, translationType);
		}
		pResponse->set_translationtype(translationType);
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult setExtentParameter(SvPb::SetExtentParameterRequest request)
{
	InspectionCmdResult result;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	SvPb::SetExtentParameterResponse* pResponse	= result.m_response.mutable_setextentparameterresponse();
	auto* pParameter = pResponse->mutable_parameters();
	if (nullptr != pTool && nullptr != pParameter)
	{
		switch (request.message_case())
		{
			case SvPb::SetExtentParameterRequest::kUpdateFromOutputSpace:
				result.m_hResult = pTool->updateExtentFromOutputSpace(request.updatefromoutputspace().action(), request.updatefromoutputspace().dx(), request.updatefromoutputspace().dy());
				break;
			case SvPb::SetExtentParameterRequest::kSetProperty:
				result.m_hResult = pTool->setExtentProperty(request.setproperty().propertyflag(), request.setproperty().value());
				break;
			case SvPb::SetExtentParameterRequest::kExtentList:
				result.m_hResult = pTool->setExtentList(request.extentlist().extentlist());
				break;
			case SvPb::SetExtentParameterRequest::kSetToParent:
				result.m_hResult = pTool->setExtentToParent();
				break;
			default:
				result.m_hResult = E_POINTER;
				break;
		}
		SvPb::SVExtentTranslationEnum translationType = SvPb::SVExtentTranslationUnknown;
		pTool->getExtentProperties(*pParameter, translationType);
		pResponse->set_translationtype(translationType);
	}
	else
	{
		result.m_hResult = E_POINTER;
	}

	return result;
}

InspectionCmdResult isAllowedLocation(SvPb::IsAllowedLocationRequest request)
{
	InspectionCmdResult result;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (nullptr != pTool)
	{
		SvPb::IsAllowedLocationResponse* pResponse = result.m_response.mutable_isallowedlocationresponse();
		pResponse->set_isallowed(pTool->isAllowedLocation(request.location(), request.direction()));
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult propagateSizeAndPosition(SvPb::PropagateSizeAndPositionRequest request)
{
	InspectionCmdResult result;

	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (nullptr != pInspection)
	{
		pInspection->propagateSizeAndPosition();
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult usePropagateSizeAndPosition(SvPb::UsePropagateSizeAndPositionRequest request)
{
	InspectionCmdResult result;

	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid())));
	if (nullptr != pInspection)
	{
		SvPb::UsePropagateSizeAndPositionResponse* pResponse = result.m_response.mutable_usepropagatesizeandpositionresponse();
		pResponse->set_isused(pInspection->usePropagateSizeAndPosition());
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult getOverlayStruct(SvPb::GetOverlayStructRequest request)
{
	InspectionCmdResult result;

	SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.imageid())));
	if (nullptr != pImage)
	{
		try
		{
			SvPb::OverlayDesc overlay = pImage->getOverlayStruct();
			SvPb::GetOverlayStructResponse* pResponse = result.m_response.mutable_getoverlaystructresponse();
			SvPb::OverlayDesc* pOverlay = pResponse->mutable_overlays();
			*pOverlay = overlay;
		}
		catch (const SvStl::MessageContainer& rExp)
		{
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
			Exception.setMessage(rExp.getMessage());
			result.m_hResult = E_FAIL;
		}
		catch (...)
		{
			result.m_hResult = E_FAIL;
		}
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

InspectionCmdResult getObjectSelectorItems(SvPb::GetObjectSelectorItemsRequest request)
{
	InspectionCmdResult result;

	SvPb::GetObjectSelectorItemsResponse* pResponse = result.m_response.mutable_getobjectselectoritemsresponse();
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
		SvPb::convertVectorToTree(itemVector, pResponse->mutable_tree());
	}

	return result;
}

std::vector<SvPb::TreeItem> getSelectorList(SvPb::GetObjectSelectorItemsRequest request, SvPb::ObjectSelectorType selectorType)
{
	std::vector<SvPb::TreeItem> result;

	GUID inspectionID = SvPb::GetGuidFromProtoBytes(request.inspectionid());
	GUID instanceID = SvPb::GetGuidFromProtoBytes(request.instanceid());

	if (SvPb::ObjectSelectorType::ppqItems == selectorType)
	{
		SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (SvOi::getObject(inspectionID));
		if (nullptr != pInspection)
		{
			result = pInspection->GetPPQSelectorList(request.attribute());
			return result;
		}
	}

	if (SvPb::ObjectSelectorType::cameraObject == selectorType)
	{
		SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (SvOi::getObject(inspectionID));
		if (nullptr != pInspection)
		{
			result = pInspection->GetCameraSelectorList(request.attribute());
			return result;
		}
	}

	if (SvPb::ObjectSelectorType::tableObjects == selectorType)
	{

		const GUID& rGuid = (GUID_NULL != instanceID) ? instanceID : inspectionID;
		SvOi::IObjectClass* pObject = SvOi::getObject(rGuid);
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

	const GUID& rGuid = (GUID_NULL != instanceID) ? instanceID : inspectionID;
	SvOi::IObjectClass* pObject = SvOi::getObject(rGuid);
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

InspectionCmdResult setDefaultInputsRequest(SvPb::SetDefaultInputsRequest request)
{
	InspectionCmdResult result;

	SvOi::IObjectClass* pObject = SvOi::getObject(SvPb::GetGuidFromProtoBytes(request.objectid()));
	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (pObject);

	if (nullptr != pInspection)
	{
		pInspection->SetDefaultInputs();
		result.m_hResult =  S_OK;
	}
	else
	{
		result.m_hResult = E_POINTER;
	}
	return result;
}

} //namespace SvCmd