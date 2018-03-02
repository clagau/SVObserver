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
HRESULT InspectionRunOnce(const SvPB::InspectionRunOnceRequest &rMessage)
{
	HRESULT hr = S_OK;

	SVGUID InspectionID = SvPB::GetGuidFromProtoBytes(rMessage.inspectionid());
	SVGUID ToolID = SvPB::GetGuidFromProtoBytes(rMessage.taskid());

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

HRESULT DestroyChildObject(const SvPB::DestroyChildRequest& rMessage)
{
	HRESULT hr = S_OK;

	SvOi::ITaskObjectListClass *pTaskObjectList = dynamic_cast<SvOi::ITaskObjectListClass*>(SvOi::getObject(SvPB::GetGuidFromProtoBytes(rMessage.taskobjectlistid())));
	SvOi::ITaskObject *pObject = dynamic_cast<SvOi::ITaskObject*>(SvOi::getObject(SvPB::GetGuidFromProtoBytes(rMessage.objectid())));
	if (nullptr != pTaskObjectList && nullptr != pObject)
	{
		DWORD flag = 0;
		switch (rMessage.flag())
		{
			case SvPB::DestroyChildRequest::Flag_SetDefaultInputs:
				flag = SvDef::SVMFSetDefaultInputs;
				break;
			case SvPB::DestroyChildRequest::Flag_ResetInspection:
				flag = SvDef::SVMFResetInspection;
				break;
			case SvPB::DestroyChildRequest::Flag_SetDefaultInputs_And_ResetInspection:
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

HRESULT GetMessageList(const SvPB::GetMessageListRequest& rRequestMessage, SvPB::GetMessageListResponse& rResponseMessage)
{
	HRESULT hr = S_OK;

	SvOi::ITaskObject* pTask = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(SvPB::GetGuidFromProtoBytes(rRequestMessage.objectid())));
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

HRESULT ResetObject(const SvPB::ResetObjectRequest& rRequestMessage, SvPB::ResetObjectResponse* pResponseMessage)
{
	HRESULT hr = S_OK;
	SvOi::IObjectClass* pObject = SvOi::getObject(SvPB::GetGuidFromProtoBytes(rRequestMessage.objectid()));

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

HRESULT CreateModel(const SvPB::CreateModelRequest& rRequestMessage, SvPB::CreateModelResponse& rResponseMessage)
{
	HRESULT hr = S_OK;
	SvStl::MessageContainerVector messages;
	SVGUID patternID = SvPB::GetGuidFromProtoBytes(rRequestMessage.patternanalyzerid());
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

HRESULT IsValid(const SvPB::IsValidRequest& rRequestMessage, SvPB::IsValidResponse& rResponseMessage)
{
	HRESULT hr = S_OK;

	SvOi::ITaskObject* pObject = dynamic_cast<SvOi::ITaskObject *>(SvOi::getObject(SvPB::GetGuidFromProtoBytes(rRequestMessage.objectid())));
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

HRESULT GetEquation(const SvPB::GetEquationRequest& rRequestMessage, SvPB::GetEquationResponse& rResponseMessage)
{
	HRESULT hr = S_OK;

	SvOi::IEquation* pEquation = dynamic_cast<SvOi::IEquation *>(SvOi::getObject(SvPB::GetGuidFromProtoBytes(rRequestMessage.objectid())));
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

HRESULT ValidateAndSetEquation(const SvPB::ValidateAndSetEquationRequest& rRequestMessage, SvPB::ValidateAndSetEquationResponse& rResponseMessage)
{
	HRESULT hr = S_OK;

	SvOi::IEquation* pEquation = dynamic_cast<SvOi::IEquation *>(SvOi::getObject(SvPB::GetGuidFromProtoBytes(rRequestMessage.objectid())));
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
	}
	return hr;
}
} //namespace SvCmd