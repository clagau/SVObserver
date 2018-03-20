//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file InspectionCommand_protoBuf.h
/// All Rights Reserved
//*****************************************************************************
/// This is the Command for Executing a InspectionCommand.
//******************************************************************************
#pragma once

#pragma region Includes
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

		InspectionCommands_protoBuf(const google::protobuf::Message* pRequest, google::protobuf::Message* pResponse)
			: m_pRequest(pRequest), m_pResponse(pResponse) {};

		virtual ~InspectionCommands_protoBuf() {};

		HRESULT Execute()
		{
			HRESULT hr = E_POINTER;

			if (nullptr != dynamic_cast<const SvPb::InspectionRunOnceRequest*>(m_pRequest))
			{
				hr = InspectionRunOnce(*static_cast<const SvPb::InspectionRunOnceRequest*>(m_pRequest));
			}
			else if (nullptr != dynamic_cast<const SvPb::DestroyChildRequest*>(m_pRequest))
			{
				hr = DestroyChildObject(*static_cast<const SvPb::DestroyChildRequest*>(m_pRequest));
			}
			else if (nullptr != dynamic_cast<const SvPb::GetMessageListRequest*>(m_pRequest) && nullptr != dynamic_cast<SvPb::GetMessageListResponse*>(m_pResponse))
			{
				hr = GetMessageList(*static_cast<const SvPb::GetMessageListRequest*>(m_pRequest), *static_cast<SvPb::GetMessageListResponse*>(m_pResponse));
			}
			else if (nullptr != dynamic_cast<const SvPb::ResetObjectRequest*>(m_pRequest))
			{
				hr = ResetObject(*static_cast<const SvPb::ResetObjectRequest*>(m_pRequest), dynamic_cast<SvPb::ResetObjectResponse*>(m_pResponse));
			}
			else if (nullptr != dynamic_cast<const SvPb::CreateModelRequest*>(m_pRequest) && nullptr != dynamic_cast<SvPb::CreateModelResponse*>(m_pResponse))
			{
				hr = CreateModel(*static_cast<const SvPb::CreateModelRequest*>(m_pRequest), *static_cast<SvPb::CreateModelResponse*>(m_pResponse));
			}
			else if (nullptr != dynamic_cast<const SvPb::IsValidRequest*>(m_pRequest) && nullptr != dynamic_cast<SvPb::IsValidResponse*>(m_pResponse))
			{
				hr = IsValid(*static_cast<const SvPb::IsValidRequest*>(m_pRequest), *static_cast<SvPb::IsValidResponse*>(m_pResponse));
			}
			else if (nullptr != dynamic_cast<const SvPb::GetEquationRequest*>(m_pRequest) && nullptr != dynamic_cast<SvPb::GetEquationResponse*>(m_pResponse))
			{
				hr = GetEquation(*static_cast<const SvPb::GetEquationRequest*>(m_pRequest), *static_cast<SvPb::GetEquationResponse*>(m_pResponse));
			}
			else if (nullptr != dynamic_cast<const SvPb::ValidateAndSetEquationRequest*>(m_pRequest) && nullptr != dynamic_cast<SvPb::ValidateAndSetEquationResponse*>(m_pResponse))
			{
				hr = ValidateAndSetEquation(*static_cast<const SvPb::ValidateAndSetEquationRequest*>(m_pRequest), *static_cast<SvPb::ValidateAndSetEquationResponse*>(m_pResponse));
			}

			return hr;
		}

		bool empty() const
		{
			return true;
		}

	private:
		const google::protobuf::Message* m_pRequest;
		google::protobuf::Message* m_pResponse;
	};
	typedef std::shared_ptr<InspectionCommands_protoBuf> InspectionCommands_protoBufPtr;
} //namespace SvCmd

