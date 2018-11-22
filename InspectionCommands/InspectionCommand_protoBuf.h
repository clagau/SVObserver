//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file InspectionCommand_protoBuf.h
/// All Rights Reserved
//*****************************************************************************
/// This is the Command for Executing a InspectionCommand.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf/InspectionCommands.h"
#include "CommandFunction.h"
#pragma endregion Includes


#pragma region Declarations
//! Declaration is in SVUtilityLibrary\SVGUID.h
class SVGUID;
#pragma endregion Declarations

namespace SvCmd
{
class InspectionCommands_protoBuf
{
public:
	InspectionCommands_protoBuf(const InspectionCommands_protoBuf&) = delete;
	InspectionCommands_protoBuf& operator=(const InspectionCommands_protoBuf&) = delete;
	InspectionCommands_protoBuf(SvPb::InspectionCmdMsgs* pRequest, SvPb::InspectionCmdMsgs* pResponse)
		: m_pRequest(pRequest), m_pResponse(pResponse)
	{
	};

	virtual ~InspectionCommands_protoBuf() {};

	HRESULT Execute()
	{
		HRESULT hr = E_POINTER;
		if (!m_pRequest)
		{
			return hr;
		}
		switch (m_pRequest->message_case())
		{
			case SvPb::InspectionCmdMsgs::kInspectionRunOnceRequest:
				hr = InspectionRunOnce(m_pRequest->inspectionrunoncerequest());
				break;
			case SvPb::InspectionCmdMsgs::kDestroyChildRequest:
				hr = DestroyChildObject(m_pRequest->destroychildrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetMessageListRequest:
				hr = GetMessageList(m_pRequest->getmessagelistrequest(), *(m_pResponse->mutable_getmessagelistresponse()));
				break;
			case SvPb::InspectionCmdMsgs::kResetObjectRequest:
				hr = ResetObject(m_pRequest->resetobjectrequest(), m_pResponse->mutable_resetobjectresponse());
				break;
			case SvPb::InspectionCmdMsgs::kCreateModelRequest:
				hr = CreateModel(m_pRequest->createmodelrequest(), *m_pResponse->mutable_createmodelresponse());
				break;
			case SvPb::InspectionCmdMsgs::kGetObjectParametersRequest:
				hr = getObjectParameters(m_pRequest->getobjectparametersrequest(), *m_pResponse->mutable_getobjectparametersresponse());
				break;
			case SvPb::InspectionCmdMsgs::kGetEquationRequest:
				hr = GetEquation(m_pRequest->getequationrequest(), *m_pResponse->mutable_getequationresponse());
				break;
			case SvPb::InspectionCmdMsgs::kValidateAndSetEquationRequest:
				hr = ValidateAndSetEquation(m_pRequest->validateandsetequationrequest(), *m_pResponse->mutable_validateandsetequationresponse());
				break;
			case SvPb::InspectionCmdMsgs::kGetObjectsForMonitorListRequest:
				hr = getObjectsForMonitorList(m_pRequest->getobjectsformonitorlistrequest(), *m_pResponse->mutable_getobjectsformonitorlistresponse());
				break;
			case SvPb::InspectionCmdMsgs::kMoveObjectRequest:
				hr = MoveObject(m_pRequest->moveobjectrequest());
				break;
			case SvPb::InspectionCmdMsgs::kTaskObjectListRequest:
				hr = GetTaskObjectsList(m_pRequest->taskobjectlistrequest(), *m_pResponse->mutable_taskobjectlistresponse());
				break;
			case SvPb::InspectionCmdMsgs::kGetImageRequest:
				hr = getImage(m_pRequest->getimagerequest(), *m_pResponse->mutable_getimageresponse());
				break;
			case SvPb::InspectionCmdMsgs::kAreAuxiliaryExtentsAvailableRequest:
				hr = areAuxiliaryExtentsAvailable(m_pRequest->areauxiliaryextentsavailablerequest(), *m_pResponse->mutable_areauxiliaryextentsavailableresponse());
				break;
			case SvPb::InspectionCmdMsgs::kGetAvailableAuxImagesRequest:
				hr = getAvailableAuxImages(m_pRequest->getavailableauximagesrequest(), *m_pResponse->mutable_getavailableauximagesresponse());
				break;
			case SvPb::InspectionCmdMsgs::kGetAuxImageObjectRequest:
				hr = getAuxImageObject(m_pRequest->getauximageobjectrequest(), *m_pResponse->mutable_getauximageobjectresponse());
				break;
			case SvPb::InspectionCmdMsgs::kSetAuxImageObjectRequest:
				hr = setAuxImageObject(m_pRequest->setauximageobjectrequest(), *m_pResponse->mutable_setauximageobjectresponse());
				break;
			case SvPb::InspectionCmdMsgs::kGetInputsRequest:
				hr = getInputs(m_pRequest->getinputsrequest(), *m_pResponse->mutable_getinputsresponse());
				break;
			case SvPb::InspectionCmdMsgs::kConnectToObjectRequest:
				hr = connectToObject(m_pRequest->connecttoobjectrequest());
				break;
			case SvPb::InspectionCmdMsgs::kSaveImageRequest:
				hr = saveImage(m_pRequest->saveimagerequest());
				break;
			case SvPb::InspectionCmdMsgs::kSetObjectNameRequest:
				hr = setObjectName(m_pRequest->setobjectnamerequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetAvailableObjectsRequest:
				hr = getAvailableObjects(m_pRequest->getavailableobjectsrequest(), *m_pResponse->mutable_getavailableobjectsresponse());
				break;
			case SvPb::InspectionCmdMsgs::kGetSpecialImageListRequest:
				hr = getSpecialImageList(m_pRequest->getspecialimagelistrequest(), *m_pResponse->mutable_getspecialimagelistresponse());
				break;
			case SvPb::InspectionCmdMsgs::kExportMaskRequest:
				hr = exportMask(m_pRequest->exportmaskrequest());
				break;
			case SvPb::InspectionCmdMsgs::kImportMaskRequest:
				hr = importMask(m_pRequest->importmaskrequest());
				break;
			case SvPb::InspectionCmdMsgs::kGetObjectIdRequest:
				hr = getObjectId(m_pRequest->getobjectidrequest(), *m_pResponse->mutable_getobjectidresponse());
				break;
			case SvPb::InspectionCmdMsgs::kConstructAndInsertRequest:
				hr = constructAndInsert(m_pRequest->constructandinsertrequest(), *m_pResponse->mutable_constructandinsertresponse());
				break;
			case SvPb::InspectionCmdMsgs::kGetCreatableObjectsRequest:
				hr = getCreatableObjects(m_pRequest->getcreatableobjectsrequest(), *m_pResponse->mutable_getcreatableobjectsresponse());
				break;
			case SvPb::InspectionCmdMsgs::kShouldInspectionResetRequest:
				hr = shouldInspectionReset(m_pRequest->shouldinspectionresetrequest(), *m_pResponse->mutable_shouldinspectionresetresponse());
				break;
			case SvPb::InspectionCmdMsgs::kGetPPQNameRequest:
				hr = getPPQName(m_pRequest->getppqnamerequest(), *m_pResponse->mutable_getppqnameresponse());
				break;
			default:;
		}

		return hr;
	}

	bool empty() const
	{
		return true;
	}

private:
	SvPb::InspectionCmdMsgs* m_pRequest;
	SvPb::InspectionCmdMsgs* m_pResponse;
};
typedef std::shared_ptr<InspectionCommands_protoBuf> InspectionCommands_protoBufPtr;
} //namespace SvCmd

