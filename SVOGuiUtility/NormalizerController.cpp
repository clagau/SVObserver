//*****************************************************************************
// \copyright COPYRIGHT (c) 2015,2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file NormalizerController.cpp
/// Implements the NormalizerController class.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "NormalizerController.h"
#include "InspectionCommands/CommandExternalHelper.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVProtoBuf/ConverterHelper.h"
#pragma endregion Includes

namespace SvOgu
{
NormalizerController::NormalizerController(uint32_t p_inspectionId, uint32_t p_AnalyzerId)
	: m_inspectionId(p_inspectionId), m_analyzerId(p_AnalyzerId), m_normalizerValues()
{}


NormalizerController::NormalizerValues NormalizerController::getValues()
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_getnormalizervaluesrequest();
	pRequest->set_linearanalyzerid(m_analyzerId);

	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getnormalizervaluesresponse())
	{
		auto normalizerResponse = responseCmd.getnormalizervaluesresponse();
		m_normalizerValues.normalBaseSize = normalizerResponse.normalrangebasesize();
		m_normalizerValues.normalMax = normalizerResponse.normalrangemax();
		m_normalizerValues.normalMin = normalizerResponse.normalrangemin();
		m_normalizerValues.normalStep = normalizerResponse.normalrangestepwidth();

		m_normalizerValues.rangeBaseSize = normalizerResponse.realrangebasesize();
		m_normalizerValues.rangeMax = normalizerResponse.realrangemax();
		m_normalizerValues.rangeMin = normalizerResponse.realrangemin();
		m_normalizerValues.rangeStep = normalizerResponse.realrangestepwidth();
	}
	else
	{
		SvStl::MessageManager msg(SvStl::MsgType::Data);
		if (SvStl::MessageContainerVector errorMsgContainer = SvPb::convertProtobufToMessageVector(responseCmd.errormessage()); errorMsgContainer.size() > 0)
		{
			msg.setMessage(errorMsgContainer.at(0).getMessage());
		}
		else
		{
			msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommandError, {std::to_string(hr)}, SvStl::SourceFileParams(StdMessageParams));
		}
		msg.Throw();
	}

	return m_normalizerValues;
}


NormalizerController::NormalizerValues NormalizerController::setRanges(double realMin, double realMax, double realStep, double normalMin, double normalMax, double normalStep)
{
	SvPb::InspectionCmdRequest requestCmd;
	SvPb::InspectionCmdResponse responseCmd;
	auto* pRequest = requestCmd.mutable_setnormalizerrangesrequest();
	pRequest->set_linearanalyzerid(m_analyzerId);
	pRequest->set_normalmax(normalMax);
	pRequest->set_normalmin(normalMin);
	pRequest->set_normalstepwidth(normalStep);
	pRequest->set_realmax(realMax);
	pRequest->set_realmin(realMin);
	pRequest->set_realstepwidth(realStep);

	HRESULT hr = SvCmd::InspectionCommands(m_inspectionId, requestCmd, &responseCmd);
	if (S_OK == hr && responseCmd.has_getnormalizervaluesresponse())
	{
		auto normalizerResponse = responseCmd.getnormalizervaluesresponse();
		m_normalizerValues.normalBaseSize = normalizerResponse.normalrangebasesize();
		m_normalizerValues.normalMax = normalizerResponse.normalrangemax();
		m_normalizerValues.normalMin = normalizerResponse.normalrangemin();
		m_normalizerValues.normalStep = normalizerResponse.normalrangestepwidth();

		m_normalizerValues.rangeBaseSize = normalizerResponse.realrangebasesize();
		m_normalizerValues.rangeMax = normalizerResponse.realrangemax();
		m_normalizerValues.rangeMin = normalizerResponse.realrangemin();
		m_normalizerValues.rangeStep = normalizerResponse.realrangestepwidth();
	}
	else
	{
		SvStl::MessageManager msg(SvStl::MsgType::Data);
		if (SvStl::MessageContainerVector errorMsgContainer = SvPb::convertProtobufToMessageVector(responseCmd.errormessage()); errorMsgContainer.size() > 0)
		{
			msg.setMessage(errorMsgContainer.at(0).getMessage());
		}
		else
		{
			msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UnknownCommandError, {std::to_string(hr)}, SvStl::SourceFileParams(StdMessageParams));
		}
		msg.Throw();
	}
	return m_normalizerValues;
}

double NormalizerController::calcRealValueFromLocalValues(double XN)
{
	return((m_normalizerValues.rangeBaseSize / m_normalizerValues.normalBaseSize) * (XN - m_normalizerValues.normalMin) + m_normalizerValues.rangeMin);
}

double NormalizerController::calcNormalizedValueFromLocalValues(double X)
{
	return((m_normalizerValues.normalBaseSize / m_normalizerValues.rangeBaseSize) * (X - m_normalizerValues.rangeMin) + m_normalizerValues.normalMin);
}

} //namespace SvOgu