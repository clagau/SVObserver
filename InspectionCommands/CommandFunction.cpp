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
#include "ObjectInterfaces\IEquation.h"
#include "ObjectInterfaces\IFormulaController.h"
#include "ObjectInterfaces\ISVFilter.h"
#include "ObjectInterfaces\IInspectionProcess.h"
#include "ObjectInterfaces\IMask.h"
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\ITaskObject.h"
#include "ObjectInterfaces\ITaskObjectListClass.h"
#include "ObjectInterfaces\ITool.h"
#include "ObjectInterfaces\IObjectAppClass.h"
#include "ObjectInterfaces\IObjectManager.h"
#include "ObjectInterfaces\IPatternAnalyzer.h"
#include "Definitions\ObjectDefines.h"
#include "Definitions\TextDefineSvDef.h"
#include "SVStatusLibrary\ErrorNumbers.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVMatroxLibrary\SVMatroxSimpleEnums.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary\StringHelper.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVObjectLibrary\SVObjectBuilder.h"
#include "SVObjectLibrary\SVObjectClass.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVProtoBuf\ConverterHelper.h"
#include "CommandInternalHelper.h"
#pragma endregion Includes

namespace SvCmd
{
HRESULT InspectionRunOnce(const SvPb::InspectionRunOnceRequest &rMessage)
{
	HRESULT hr = S_OK;

	SVGUID InspectionID = SvPb::GetGuidFromProtoBytes(rMessage.inspectionid());

	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess *>(SvOi::getObject(InspectionID));
	if (nullptr != pInspection)
	{
		hr = pInspection->RunOnce();
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT DestroyChildObject(const SvPb::DestroyChildRequest& rMessage)
{
	HRESULT hr = S_OK;

	SvOi::ITaskObjectListClass *pTaskObjectList = dynamic_cast<SvOi::ITaskObjectListClass*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rMessage.taskobjectlistid())));
	SvOi::ITaskObject *pObject = dynamic_cast<SvOi::ITaskObject*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rMessage.objectid())));
	if (nullptr != pTaskObjectList && nullptr != pObject)
	{
		DWORD flag = 0;
		switch (rMessage.flag())
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
		hr = SvStl::Err_10024_DestroyChildObject_InvalidParameter;
	}
	return hr;
}

HRESULT GetMessageList(const SvPb::GetMessageListRequest& rRequestMessage, SvPb::GetMessageListResponse& rResponseMessage)
{
	HRESULT hr = S_OK;

	SvOi::ITaskObject* pTask = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequestMessage.objectid())));
	if (pTask)
	{
		rResponseMessage.mutable_messages()->CopyFrom(setMessageContainerToMessagePB(pTask->getErrorMessages()));
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT ResetObject(const SvPb::ResetObjectRequest& rRequestMessage, SvPb::ResetObjectResponse* pResponseMessage)
{
	HRESULT hr = S_OK;
	SvOi::IObjectClass* pObject = SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequestMessage.objectid()));

	if (nullptr != pObject)
	{
		SvStl::MessageContainerVector messages;
		hr = pObject->resetAllObjects(&messages) ? S_OK : E_FAIL;
		if (nullptr != pResponseMessage)
		{
			pResponseMessage->mutable_messages()->CopyFrom(setMessageContainerToMessagePB(messages));
		}
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT CreateModel(const SvPb::CreateModelRequest& rRequestMessage, SvPb::CreateModelResponse& rResponseMessage)
{
	HRESULT hr = S_OK;
	SvStl::MessageContainerVector messages;
	SVGUID patternID = SvPb::GetGuidFromProtoBytes(rRequestMessage.patternanalyzerid());
	SvOi::IPatternAnalyzer* pPatAnalyzer = dynamic_cast<SvOi::IPatternAnalyzer*>(SvOi::getObject(patternID));

	if (nullptr != pPatAnalyzer && pPatAnalyzer->UpdateModelFromInputImage(rRequestMessage.posx(), rRequestMessage.posy(), rRequestMessage.modelwidth(), rRequestMessage.modelheight()))
	{
		SvOi::SVImageBufferHandlePtr imageHandle;
		SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*>(pPatAnalyzer);
		if (nullptr != pTaskObject && pTaskObject->getSpecialImage(SvDef::PatternModelImageName, imageHandle) && !imageHandle->empty())
		{
			std::string FileName = rRequestMessage.filename();
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
			hr = SVMatroxBufferInterface::Export(milBuffer, FileName, FileFormatID);
			if (S_OK != hr)
			{
				hr = E_FAIL;
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, patternID);
				messages.push_back(Msg);
			}
		}
		else
		{
			hr = E_FAIL;
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, patternID);
			messages.push_back(Msg);
		}
	}
	else
	{
		hr = E_FAIL;
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams), 0, patternID);
		messages.push_back(Msg);
	}

	rResponseMessage.mutable_messages()->CopyFrom(setMessageContainerToMessagePB(messages));

	return hr;
}

HRESULT getObjectParameters(const SvPb::GetObjectParametersRequest& rRequest, SvPb::GetObjectParametersResponse& rResponse)
{
	HRESULT hr = S_OK;

	SvOi::IObjectClass* pObject = SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.objectid()));
	if (pObject)
	{
		rResponse.set_isvalid(pObject->IsValid());
		rResponse.set_name(pObject->GetName());
		rResponse.set_allowedattributes(pObject->ObjectAttributesAllowed());
		rResponse.mutable_typeinfo()->set_objecttype(pObject->GetObjectType());
		rResponse.mutable_typeinfo()->set_subtype(pObject->GetObjectSubType());
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT GetEquation(const SvPb::GetEquationRequest& rRequestMessage, SvPb::GetEquationResponse& rResponseMessage)
{
	HRESULT hr = S_OK;

	SvOi::IEquation* pEquation = dynamic_cast<SvOi::IEquation *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequestMessage.objectid())));
	if (pEquation)
	{
		rResponseMessage.set_equationtext(pEquation->GetEquationText());
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT ValidateAndSetEquation(const SvPb::ValidateAndSetEquationRequest& rRequestMessage, SvPb::ValidateAndSetEquationResponse& rResponseMessage)
{
	HRESULT hr = E_FAIL;

	SvOi::IEquation* pEquation = dynamic_cast<SvOi::IEquation *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequestMessage.objectid())));
	if (pEquation)
	{
		std::string oldString;
		//save old string
		oldString = pEquation->GetEquationText();
		pEquation->SetEquationText(rRequestMessage.equationtext());
		SvStl::MessageContainerVector messages;
		SvOi::EquationTestResult testResult = pEquation->Test(&messages);
		rResponseMessage.mutable_messages()->CopyFrom(setMessageContainerToMessagePB(messages));
		int retValue = 0;
		if (testResult.bPassed)
		{// set result and set return value to successful
			rResponseMessage.set_result(pEquation->GetYACCResult());
			retValue = SvOi::IFormulaController::validateSuccessful;
		}
		else
		{  // set return value to position of failed
			retValue = testResult.iPositionFailed;
		}
		rResponseMessage.set_validatestatus(retValue);
		if (!rRequestMessage.bsetvalue() || SvOi::IFormulaController::validateSuccessful != retValue)
		{
			//reset old string
			pEquation->SetEquationText(oldString);
		}
		hr = S_OK;
	}
	return hr;
}

HRESULT getObjectsForMonitorList(const SvPb::GetObjectsForMonitorListRequest& rRequestMessage, SvPb::GetObjectsForMonitorListResponse& rResponseMessage)
{
	HRESULT hr = E_FAIL;
	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequestMessage.objectid())));
	if (nullptr != pTool)
	{
		SvStl::MessageContainerVector messages;
		SvOi::ParametersForML paramList = pTool->getParameterForMonitorList(messages);
		rResponseMessage.mutable_messages()->CopyFrom(setMessageContainerToMessagePB(messages));
		for (auto& item : paramList)
		{
			auto* pEntry = rResponseMessage.add_list();
			pEntry->set_objectname(item.first.c_str());
			SvPb::SetGuidInProtoBytes(pEntry->mutable_objectid(), item.second);
		}
		hr = S_OK;
	}

	return hr;
}

HRESULT MoveObject(const SvPb::MoveObjectRequest& rRequestMessage)
{
	SvOi::IObjectClass* pObject = SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequestMessage.parentid()));
	if (nullptr != pObject)
	{
		switch (rRequestMessage.listmode())
		{
			case SvPb::MoveObjectRequest::TaskObjectList:
			{
				SvOi::ITaskObjectListClass* pTaskObject = dynamic_cast<SvOi::ITaskObjectListClass*>(pObject);
				if (nullptr != pTaskObject)
				{
					pTaskObject->moveTaskObject(SvPb::GetGuidFromProtoBytes(rRequestMessage.objectid()), SvPb::GetGuidFromProtoBytes(rRequestMessage.movepreid()));
				}
				else
				{
					return E_FAIL;
				}
				break;
			}
			case SvPb::MoveObjectRequest::FriendList:
				pObject->moveFriendObject(SvPb::GetGuidFromProtoBytes(rRequestMessage.objectid()), SvPb::GetGuidFromProtoBytes(rRequestMessage.movepreid()));
				break;
			default:
				return E_FAIL;
		}
		
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

HRESULT GetTaskObjectsList(const SvPb::TaskObjectListRequest& rRequest, SvPb::TaskObjectListResponse &rResponse)
{
	SvOi::ITaskObjectListClass* pTaskObjectList = dynamic_cast<SvOi::ITaskObjectListClass*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.taskobjectid())));
	if (nullptr == pTaskObjectList)
	{
		return E_FAIL;
	}
	pTaskObjectList->GetTaskObjectListInfo(rResponse);
	return S_OK;
}

HRESULT getImage(const SvPb::GetImageRequest& rRequest, SvPb::GetImageResponse& rResponse)
{
	HRESULT hr = S_OK;
	SvOi::SVImageBufferHandlePtr data;

	if (0 < rRequest.imageid().size())
	{
		SvOi::ISVImage* pImage = dynamic_cast<SvOi::ISVImage*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.imageid())));
		if (pImage)
		{
			data = pImage->getImageData();
		}
	}
	else if (!rRequest.imagename().empty() && 0 < rRequest.parentid().size())
	{
		SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.parentid())));
		if (nullptr != pObject)
		{
			if (!pObject->getSpecialImage(rRequest.imagename(), data))
			{
				SvStl::MessageContainerVector messages;
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams));
				messages.push_back(Msg);
				rResponse.mutable_messages()->CopyFrom(setMessageContainerToMessagePB(messages));
				return E_FAIL;
			}
		}
	}

	SvOi::SVImageBufferHandleInterface* pImageData = data.get();
	if (nullptr != pImageData && !pImageData->empty())
	{
		std::string ImageBuffer;
		SVBitmapInfo bitmapInfo;
		hr = SVMatroxBufferInterface::CopyBufferToFileDIB(ImageBuffer, bitmapInfo, pImageData->GetBuffer());
		rResponse.mutable_imagedata()->mutable_rgbdata()->swap(ImageBuffer);
		rResponse.mutable_imagedata()->set_width(std::abs(bitmapInfo.GetWidth()));
		rResponse.mutable_imagedata()->set_height(std::abs(bitmapInfo.GetHeight()));
	}
	else
	{
		SvStl::MessageContainerVector messages;
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams));
		messages.push_back(Msg);
		rResponse.mutable_messages()->CopyFrom(setMessageContainerToMessagePB(messages));
		hr = E_POINTER;
	}
	return hr;
}

HRESULT areAuxiliaryExtentsAvailable(const SvPb::AreAuxiliaryExtentsAvailableRequest& rRequest, SvPb::AreAuxiliaryExtentsAvailableResponse& rResponse)
{
	HRESULT hr = S_OK;
	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.objectid())));
	if (pTool)
	{
		rResponse.set_areavailable(pTool->areAuxExtentsAvailable());
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT getAvailableAuxImages(const SvPb::GetAvailableAuxImagesRequest& rRequest, SvPb::GetAvailableAuxImagesResponse& rResponse)
{
	HRESULT hr = S_OK;
	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.objectid())));
	if (pTool)
	{
		auto list = pTool->getAvailableAuxSourceImages();
		for (auto& item : list)
		{
			auto* pEntry = rResponse.add_list();
			pEntry->set_objectname(item.first.c_str());
			SvPb::SetGuidInProtoBytes(pEntry->mutable_objectid(), item.second);
		}
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT getAuxImageObject(const SvPb::GetAuxImageObjectRequest& rRequest, SvPb::GetAuxImageObjectResponse& rResponse)
{
	HRESULT hr = S_OK;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.objectid())));
	if (pTool)
	{
		auto sourceImage = pTool->getAuxSourceImage();
		rResponse.mutable_auxobject()->set_objectname(sourceImage.first.c_str());
		SvPb::SetGuidInProtoBytes(rResponse.mutable_auxobject()->mutable_objectid(), sourceImage.second);
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT setAuxImageObject(const SvPb::SetAuxImageObjectRequest& rRequest, SvPb::SetAuxImageObjectResponse& rResponse)
{
	HRESULT hr = S_OK;

	SvOi::ITool* pTool = dynamic_cast<SvOi::ITool *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.objectid())));
	if (pTool)
	{
		hr = pTool->setAuxSourceImage(SvPb::GetGuidFromProtoBytes(rRequest.sourceimageid()));
		if (S_OK != hr)
		{
			SvStl::MessageContainerVector messages;
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_PatAllocModelFailed, SvStl::SourceFileParams(StdMessageParams));
			messages.push_back(Msg);
			rResponse.mutable_messages()->CopyFrom(setMessageContainerToMessagePB(messages));
		}
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT getInputs(const SvPb::GetInputsRequest& rRequest, SvPb::GetInputsResponse& rResponse)
{
	HRESULT hr = S_OK;

	SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.objectid())));
	if (nullptr != pTaskObject)
	{
		SvDef::SVObjectTypeInfoStruct typeInfo {static_cast<SvDef::SVObjectTypeEnum>(rRequest.typeinfo().objecttype()), static_cast<SvDef::SVObjectSubTypeEnum>(rRequest.typeinfo().subtype()), SvPb::GetGuidFromProtoBytes(rRequest.typeinfo().embeddedid())};
		SvUl::InputNameGuidPairList list;
		if (SvDef::SVImageObjectType == typeInfo.ObjectType)
		{
			pTaskObject->GetConnectedImages(list, rRequest.maxrequested());
		}
		else
		{
			pTaskObject->GetInputs(list, typeInfo, static_cast<SvDef::SVObjectTypeEnum>(rRequest.objecttypetoinclude()), rRequest.shouldexcludefirstobjectname());
		}
		for (auto& item : list)
		{
			auto* pEntry = rResponse.add_list();
			pEntry->set_inputname(item.first.c_str());
			pEntry->set_objectname(item.second.first);
			SvPb::SetGuidInProtoBytes(pEntry->mutable_objectid(), item.second.second);
		}
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT connectToObject(const SvPb::ConnectToObjectRequest& rRequest)
{
	HRESULT hr = S_OK;

	SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.objectid())));
	if (pObject)
	{
		hr = pObject->ConnectToObject(rRequest.inputname(), SvPb::GetGuidFromProtoBytes(rRequest.newconnectedid()), static_cast<SvDef::SVObjectTypeEnum>(rRequest.objecttype()));
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT saveImage(const SvPb::SaveImageRequest& rRequest)
{
	HRESULT hr = S_OK;

	SvOi::ISVImage * pObject = dynamic_cast<SvOi::ISVImage *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.objectid())));
	if (pObject)
	{
		hr = pObject->Save(rRequest.imagename());
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT setObjectName(const SvPb::SetObjectNameRequest& rRequest)
{
	HRESULT hr = S_OK;

	SvOi::IObjectClass* pObject = SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.objectid()));
	if (pObject)
	{
		pObject->SetName(rRequest.objectname().c_str());
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT getAvailableObjects(const SvPb::GetAvailableObjectsRequest& rRequest, SvPb::GetAvailableObjectsResponse& rResponse)
{
	HRESULT hr = S_OK;
	SvDef::SVObjectTypeInfoStruct typeInfo {static_cast<SvDef::SVObjectTypeEnum>(rRequest.typeinfo().objecttype()), static_cast<SvDef::SVObjectSubTypeEnum>(rRequest.typeinfo().subtype()), SvPb::GetGuidFromProtoBytes(rRequest.typeinfo().embeddedid())};
	SvDef::SVObjectTypeEnum objectTypeToInclude = static_cast<SvDef::SVObjectTypeEnum>(rRequest.objecttypetoinclude());
	SVGUID instanceId = SvPb::GetGuidFromProtoBytes(rRequest.objectid());
	SVGetObjectDequeByTypeVisitor visitor(typeInfo);
	SvOi::visitElements(visitor, instanceId);

	IsAllowedFunc isAllowed = getAllowedFunc(rRequest);
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
						case SvDef::SVImageObjectType:
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
							if (SvDef::SVNotSetObjectType == objectTypeToInclude)
							{
								name = pObject->GetName();
							}
							else
							{
								if (rRequest.shouldexcludefirstobjectname())
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
					if (!name.empty())
					{
						auto* pEntry = rResponse.add_list();
						pEntry->set_objectname(name.c_str());
						SvPb::SetGuidInProtoBytes(pEntry->mutable_objectid(), pObject->GetUniqueObjectID());
					}
				}
			}
			if (bStop)
			{
				return hr;
			}
		}
	}
	else
	{
		hr = E_NOINTERFACE;
	}
	return hr;
}

HRESULT getSpecialImageList(const SvPb::GetSpecialImageListRequest& rRequest, SvPb::GetSpecialImageListResponse& rResponse)
{
	HRESULT hr = S_OK;
	const SvOi::ITaskObject* pObject = dynamic_cast<const SvOi::ITaskObject*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.taskobjectid())));
	if (nullptr != pObject)
	{
		SvDef::StringVector specialImageList = pObject->getSpecialImageList();
		for (auto tmp : specialImageList)
		{
			rResponse.add_specialimagenames(tmp);
		}
	}
	else
	{
		hr = E_FAIL;
	}
	return hr;
}

HRESULT exportMask(const SvPb::ExportMaskRequest& rRequest)
{
	HRESULT hr = S_OK;

	SvOi::IMask* pObject = dynamic_cast<SvOi::IMask *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.objectid())));
	if (pObject)
	{
		hr = pObject->Export(rRequest.filename());
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT importMask(const SvPb::ImportMaskRequest& rRequest)
{
	HRESULT hr = S_OK;

	SvOi::IMask* pObject = dynamic_cast<SvOi::IMask *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.objectid())));
	if (pObject)
	{
		hr = pObject->Import(rRequest.filename());
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT getObjectId(const SvPb::GetObjectIdRequest& rRequest, SvPb::GetObjectIdResponse& rResponse)
{
	HRESULT hr = S_OK;

	SvOi::IObjectClass* pObject = nullptr;
	switch (rRequest.message_case())
	{
		case SvPb::GetObjectIdRequest::kName:
		{
			pObject = SvOi::getObjectByDottedName(rRequest.name());
			break;
		}
		case SvPb::GetObjectIdRequest::kInfo:
		{
			auto& rInfoStruct = rRequest.info().infostruct();
			SvDef::SVObjectTypeInfoStruct typeInfo {static_cast<SvDef::SVObjectTypeEnum>(rInfoStruct.objecttype()), static_cast<SvDef::SVObjectSubTypeEnum>(rInfoStruct.subtype()), SvPb::GetGuidFromProtoBytes(rInfoStruct.embeddedid())};
			pObject = SvOi::FindObject(SvPb::GetGuidFromProtoBytes(rRequest.info().ownerid()) , typeInfo);
			break;
		}
		default:break;
	}
	if (pObject)
	{
		SvPb::SetGuidInProtoBytes(rResponse.mutable_objectid(), pObject->GetUniqueObjectID());
	}
	else
	{
		hr = E_POINTER;
	}
	return hr;
}

HRESULT constructAndInsert(const SvPb::ConstructAndInsertRequest& rRequest, SvPb::ConstructAndInsertResponse& rResponse)
{
	HRESULT hr = S_OK;

	SVGUID ownerId = SvPb::GetGuidFromProtoBytes(rRequest.ownerid());
	SVGUID classId = SvPb::GetGuidFromProtoBytes(rRequest.classid());

	SvOi::IObjectClass* pObject = nullptr;
	switch (rRequest.message_case())
	{
		case SvPb::ConstructAndInsertRequest::kFriend:
		{
			pObject = SVObjectBuilder::CreateFriendObject(classId, GUID_NULL, rRequest.friend_().name(), ownerId, SvPb::GetGuidFromProtoBytes(rRequest.friend_().preguid()));
			break;
		}
		case SvPb::ConstructAndInsertRequest::kTaskObjectPos:
		case SvPb::ConstructAndInsertRequest::kTaskObjectAfterId:
		{
			SvOi::ITaskObjectListClass* pTaskObjectList = dynamic_cast<SvOi::ITaskObjectListClass*>(SvOi::getObject(ownerId));
			SvOi::IObjectAppClass* pObjectApp = dynamic_cast<SvOi::IObjectAppClass*>(pTaskObjectList);

			SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*>(SvOi::ConstructObject(classId));
			pObject = dynamic_cast<SvOi::IObjectClass*>(pTaskObject);

			if (nullptr != pTaskObjectList && nullptr != pTaskObject && nullptr != pObjectApp && nullptr != pObject)
			{
				if (SvPb::ConstructAndInsertRequest::kTaskObjectPos == rRequest.message_case())
				{
					pTaskObjectList->InsertAt(rRequest.taskobjectpos(), *pTaskObject);
				}
				else
				{
					pTaskObjectList->InsertAfter(SvPb::GetGuidFromProtoBytes(rRequest.taskobjectafterid()), *pTaskObject);
				}

				// And last - Create (initialize) it
				if (!pObject->is_Created())
				{
					// And finally try to create the child object...
					if (!pObjectApp->CreateChildObject(*pObject, SvDef::SVMFResetObject))
					{
						hr = SvStl::Err_10021_InsertTaskObject_CreateObjectFailed;

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

	if (S_OK == hr)
	{
		if (nullptr != pObject)
		{
			SvPb::SetGuidInProtoBytes(rResponse.mutable_objectid(), pObject->GetUniqueObjectID());
			rResponse.set_name(pObject->GetName());
		}
		else
		{
			hr = E_FAIL;
		}
	}
	return hr;
}

HRESULT getCreatableObjects(const SvPb::GetCreatableObjectsRequest& rRequest, SvPb::GetCreatableObjectsResponse& rResponse)
{
	HRESULT hr = S_OK;

	SvOi::IObjectClass* pObject = SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.objectid()));
	if (nullptr != pObject)
	{
		SvDef::SVObjectTypeInfoStruct typeInfo {static_cast<SvDef::SVObjectTypeEnum>(rRequest.typeinfo().objecttype()), static_cast<SvDef::SVObjectSubTypeEnum>(rRequest.typeinfo().subtype()), SvPb::GetGuidFromProtoBytes(rRequest.typeinfo().embeddedid())};
		auto list = pObject->GetCreatableObjects(typeInfo);
		for (auto& item : list)
		{
			auto* pEntry = rResponse.add_list();
			pEntry->set_objectname(item.first.c_str());
			SvPb::SetGuidInProtoBytes(pEntry->mutable_objectid(), item.second);
		}
	}
	else
	{
		hr = E_NOINTERFACE;
	}
	return hr;
}

HRESULT shouldInspectionReset(const SvPb::ShouldInspectionResetRequest& rRequest, SvPb::ShouldInspectionResetResponse& rResponse)
{
	HRESULT hr = S_OK;

	SvOi::ISVFilter* pFilter = dynamic_cast<SvOi::ISVFilter*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.objectid())));
	if (nullptr != pFilter)
	{
		rResponse.set_shouldreset(pFilter->shouldResetInspection());
	}
	else
	{
		hr = SvStl::Err_10025_ShouldResetInspection_InvalidParameter;
	}
	return hr;
}

HRESULT getPPQName(const SvPb::GetPPQNameRequest& rRequest, SvPb::GetPPQNameResponse& rResponse)
{
	HRESULT hr = E_POINTER;

	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequest.inspectionid())));
	if (pInspection)
	{
		SvOi::IObjectClass* pPPQ = pInspection->GetPPQInterface();
		if (pPPQ)
		{
			rResponse.set_ppqname(pPPQ->GetName());
			hr = S_OK;
		}
	}
	return hr;
}
} //namespace SvCmd