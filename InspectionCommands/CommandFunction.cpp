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
#include "CommandFunctionHelper.h"
#include "ObjectInterfaces\IEquation.h"
#include "ObjectInterfaces\IFormulaController.h"
#include "ObjectInterfaces\IInspectionProcess.h"
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\ITaskObject.h"
#include "ObjectInterfaces\ITaskObjectListClass.h"
#include "ObjectInterfaces\ITool.h"
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
#pragma endregion Includes

namespace SvCmd
{
HRESULT InspectionRunOnce(const SvPb::InspectionRunOnceRequest &rMessage)
{
	HRESULT hr = S_OK;

	SVGUID InspectionID = SvPb::GetGuidFromProtoBytes(rMessage.inspectionid());
	SVGUID ToolID = SvPb::GetGuidFromProtoBytes(rMessage.taskid());

	SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess *>(SvOi::getObject(InspectionID));
	if (nullptr != pInspection)
	{
		SvOi::ITaskObject* pTool(nullptr);
		if (GUID_NULL != ToolID)
		{
			pTool = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(ToolID));
		}
		hr = pInspection->RunOnce(pTool);
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

HRESULT IsValid(const SvPb::IsValidRequest& rRequestMessage, SvPb::IsValidResponse& rResponseMessage)
{
	HRESULT hr = S_OK;

	SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequestMessage.objectid())));
	if (pObject)
	{
		rResponseMessage.set_isvalid(pObject->IsObjectValid());
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

HRESULT MoveTaskObject(const SvPb::MoveTaskObjectRequest& rRequestMessage)
{
	SvOi::ITaskObjectListClass* pObject = dynamic_cast<SvOi::ITaskObjectListClass*>(SvOi::getObject(SvPb::GetGuidFromProtoBytes(rRequestMessage.parentid())));
	if (nullptr != pObject)
	{
		pObject->moveTaskObject(SvPb::GetGuidFromProtoBytes(rRequestMessage.taskobjectid()), SvPb::GetGuidFromProtoBytes(rRequestMessage.movepreid()));
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
	SvOi::ObjectInfoVector  m_ObjectInfoVector;
	pTaskObjectList->GetTaskObjectListInfo(rResponse);
	return S_OK;
}
} //namespace SvCmd