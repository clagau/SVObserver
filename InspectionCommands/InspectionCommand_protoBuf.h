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
InspectionCommands_protoBuf( SvPb::InspectionCmdMsgs* pRequest, SvPb::InspectionCmdMsgs* pResponse)
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
				hr = hr = CreateModel(m_pRequest->createmodelrequest(), *m_pResponse->mutable_createmodelresponse());
				break;
			case SvPb::InspectionCmdMsgs::kIsValidRequest:
				hr = IsValid(m_pRequest->isvalidrequest(), *m_pResponse->mutable_isvalidresponse());
				break;

			case SvPb::InspectionCmdMsgs::kGetEquationRequest:
				hr = GetEquation(m_pRequest->getequationrequest(), *m_pResponse->mutable_getequationresponse());
				break;
		
			case SvPb::InspectionCmdMsgs::kValidateAndSetEquationRequest:
				hr = ValidateAndSetEquation(m_pRequest->validateandsetequationrequest(),*m_pResponse->mutable_validateandsetequationresponse());
				break;
			case SvPb::InspectionCmdMsgs::kGetObjectsForMonitorListRequest:
				hr = getObjectsForMonitorList(m_pRequest->getobjectsformonitorlistrequest(), *m_pResponse->mutable_getobjectsformonitorlistresponse());
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

