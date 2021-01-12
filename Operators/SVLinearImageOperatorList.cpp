//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLinearImageOperatorList
//* .File Name       : $Workfile:   SVLinearImageOperatorList.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Aug 2013 10:08:52  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVLinearImageOperatorList.h"
#include "SVUnaryImageOperatorClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxImageInterface.h"
#include "SVMatroxLibrary/SVMatroxImageRotateStruct.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVStatusLibrary/RunStatus.h"
#include "Tools/SVLinearToolClass.h"
#include "Tools/SVTool.h"
#include "Operators/SVResultDouble.h"
#include "Operators/SVRange.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVLinearImageOperatorList, SvPb::LinearImageOperatorListClassId)


SVLinearImageOperatorList::SVLinearImageOperatorList(SVObjectClass* POwner, int StringResourceID)
	:SVStdImageOperatorListClass(POwner, StringResourceID)
{
	init();
}

SVLinearImageOperatorList::~SVLinearImageOperatorList()
{
	SVLinearImageOperatorList::CloseObject();
}

bool SVLinearImageOperatorList::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool l_bOk = SVStdImageOperatorListClass::CreateObject(rCreateStructure);

	SvIe::SVDataBufferInfoClass& rBufferInfo = m_svProfileResultData.GetDataBufferInfo();
	rBufferInfo.Type = SvIe::SVDataBufferInfoClass::SVProjectResult;

	ResetLogicalROIImage();

	l_bOk &= S_OK == UpdateLineExtentData();
	m_svLinearData.SetObjectAttributesAllowed(SvPb::viewable | SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);

	m_MaxProfile.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_MinProfile.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_ProfileDelta.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	
	if (false == areResultsAllocated())
	{
		if (allocateResults() & SvDef::svErrorCondition)
		{
			l_bOk = false; // Some error has occurred.
		}
		else
		{
			setDefaultRange(m_Results[ResultIndex::MinProfileIdx]);
			setDefaultRange(m_Results[ResultIndex::MaxProfileIdx]);
			setDefaultRange(m_Results[ResultIndex::ProfileDeltaIdx]);
		}
	}

	return l_bOk;
}

bool SVLinearImageOperatorList::CloseObject()
{
	bool l_bOk = m_svProfileResultData.CloseObject();

	l_bOk &= SVStdImageOperatorListClass::CloseObject();

	return l_bOk;
}

bool SVLinearImageOperatorList::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);
	ResetLogicalROIImage();

	m_inputUseRotationAngle.validateInput();
	m_inputProfileOrientation.validateInput();

	CollectInputImageNames();

	if (S_OK != UpdateLineExtentData())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateLineExtentDataFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	SvOi::ITool* pTool = GetToolInterface();
	if (nullptr != pTool)
	{
		pTool->UpdateImageWithExtent();
	}

	return Result;
}

bool SVLinearImageOperatorList::Run(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	clearRunErrorMessages();

	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(GetTool());
	bool result = nullptr != pTool;
	const SVImageExtentClass&  rImageExtent = result ? pTool->GetImageExtent() : SVImageExtentClass{};

	BOOL UseRotation = true;
	result = result && (S_OK == getUseRotationAngle(UseRotation));

	if (!result)
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		m_RunErrorMessages.push_back(Msg);
	}

	SvIe::SVImageClass* pInputImage = getInputImage(true);
	if (nullptr == pInputImage)
	{
		// Signal something is wrong...
		result = false;
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		m_RunErrorMessages.push_back(Msg);
	}

	SvOi::ITRCImagePtr pOutputBuffer = nullptr;

	if (result)
	{
		if (UseRotation) 
		{
			SvOi::SVImageBufferHandlePtr input;
			if (nullptr == m_replaceSourceImage)
			{
				SvOi::ITRCImagePtr pInputBuffer = (nullptr != pInputImage) ? pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord.get()) : nullptr;
				input = (nullptr != pInputBuffer) ? pInputBuffer->getHandle() : nullptr;
			}
			else
			{
				input = createTmpSourceImage();
			}
			pOutputBuffer = m_OutputImage.getImageToWrite(rRunStatus.m_triggerRecord);
			result = RunLocalRotation(rRunStatus, input, pOutputBuffer, rImageExtent);
		}
		else
		{
			SvOi::SVImageBufferHandlePtr input;
			if (nullptr == m_replaceSourceImage)
			{
				SvOi::ITRCImagePtr pInputImageBuffer = m_LogicalROIImage.getImageReadOnly(rRunStatus.m_triggerRecord.get());
				input = (nullptr != pInputImageBuffer) ? pInputImageBuffer->getHandle() : nullptr;
			}
			else
			{
				input = createTmpSourceImage();
			}
			result = RunLocal(rRunStatus, input, m_OutputImage);
			pOutputBuffer = m_OutputImage.getImageReadOnly(rRunStatus.m_triggerRecord.get());
		}
	}

	if (result)
	{
		const SvIe::SVDataBufferHandleStruct& rDataBufferHandle = m_svProfileResultData.GetDataBufferHandle();

		long ProjectAngle(0L);

		if (0 == m_aulLineData.size() || S_OK != getInputProfileOrientation(ProjectAngle))
		{
			result = false;
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			m_RunErrorMessages.push_back(Msg);
		}

		if (result)
		{
			double l_dProjectHeight = 0.0;
			SVMatroxBuffer buffer;
			if (nullptr != pOutputBuffer && !pOutputBuffer->isEmpty())
			{
				buffer = pOutputBuffer->getHandle()->GetBuffer();
			}
			SVMatroxImageInterface::Project(rDataBufferHandle.milResult, buffer, static_cast<double> (ProjectAngle));

			SVMatroxImageInterface::GetResult(rDataBufferHandle.milResult, m_aulLineData);

			if (0 == ProjectAngle)
			{
				rImageExtent.GetExtentProperty(SvPb::SVExtentPropertyHeight, l_dProjectHeight);
			}
			else
			{
				rImageExtent.GetExtentProperty(SvPb::SVExtentPropertyWidth, l_dProjectHeight);
			}

			double dMin(9999999.0);
			double dMax(0.0);
			if (l_dProjectHeight == 0.0 || l_dProjectHeight == 1.0)
			{
				for (unsigned long i = 0; i < m_ulLineLength; i++)
				{
					double l_dValue = m_aulLineData[i];

					dMin = std::min(l_dValue, dMin);
					dMax = std::max(l_dValue, dMax);

					m_svArray.at(i) = l_dValue;
				}
			}
			else
			{
				for (unsigned long i = 0; i < m_ulLineLength; i++)
				{
					double l_dValue = m_aulLineData[i] / l_dProjectHeight;

					dMin = std::min(l_dValue, dMin);
					dMax = std::max(l_dValue, dMax);

					m_svArray.at(i) = l_dValue;
				}
			}

			result = (S_OK == m_MinProfile.SetValue(dMin)) && result;
			result = (S_OK == m_MaxProfile.SetValue(dMax)) && result;
			result = (S_OK == m_ProfileDelta.SetValue(dMax-dMin)) && result;
			
			result = (S_OK == m_svLinearData.SetArrayValues(m_svArray)) && result;

			assert(result);

			if (!result)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				m_RunErrorMessages.push_back(Msg);
			}

			//update Result objects explicitely
			RunStatus ResultRunStatus;
			ResultRunStatus.m_UpdateCounters = rRunStatus.m_UpdateCounters;
			ResultRunStatus.m_triggerRecord = std::move(rRunStatus.m_triggerRecord);
			for (SvOp::SVDoubleResult* pResult : m_Results)
			{
				if (nullptr != pResult)
				{
					ResultRunStatus.ResetRunStateAndToolSetTimes();
					pResult->Run(ResultRunStatus);
					rRunStatus.updateState(ResultRunStatus);
				}
			}
			rRunStatus.m_triggerRecord = std::move(ResultRunStatus.m_triggerRecord);
		}
	}

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}

	if (!result)
	{
		// Something was wrong...
		rRunStatus.SetInvalid();
	}

	// Get Status Color...
	DWORD dwValue = rRunStatus.GetStatusColor();
	m_statusColor.SetValue(dwValue);

	// Get Status...
	dwValue = rRunStatus.GetState();
	m_statusTag.SetValue(dwValue);

	return result;
}

HRESULT SVLinearImageOperatorList::getUseRotationAngle(BOOL& rUseRotationAngle)
{
	HRESULT Result(E_FAIL);

	if (m_inputUseRotationAngle.IsConnected() && nullptr != m_inputUseRotationAngle.GetInputObjectInfo().getObject())
	{
		double Value(0.0);
		Result = m_inputUseRotationAngle.GetInputObjectInfo().getObject()->getValue(Value);
		rUseRotationAngle = 0.0 < Value ? true : false;
	}

	return Result;
}

HRESULT SVLinearImageOperatorList::getInputProfileOrientation(long& rProfileOrientation)
{
	HRESULT Result(E_FAIL);

	if (m_inputProfileOrientation.IsConnected() && nullptr != m_inputProfileOrientation.GetInputObjectInfo().getObject())
	{
		double Value(0.0);
		Result = m_inputProfileOrientation.GetInputObjectInfo().getObject()->getValue(Value);
		rProfileOrientation = static_cast<long> (Value);
	}

	return Result;
}

void SVLinearImageOperatorList::init()
{
	// Identify our output type
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVUnaryImageOperatorListObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVLinearImageOperatorListObjectType;

	m_inputProfileOrientation.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVEnumValueObjectType, SvPb::ProfileOrientationEId);
	registerInputObject(&m_inputProfileOrientation, _T("LinearImageOperatorListProfileOrientation"), SvPb::ProfileOrientationInputEId);
	m_inputProfileOrientation.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	m_inputUseRotationAngle.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVBoolValueObjectType, SvPb::LinearToolUseRotationEId);
	registerInputObject(&m_inputUseRotationAngle, _T("LinearImageOperatorListUseRotationAngle"), SvPb::UseRoationAngleInputEId);
	m_inputUseRotationAngle.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);;

	m_svLinearData.SetLegacyVectorObjectCompatibility();

	RegisterEmbeddedObject(&m_svLinearData, SvPb::LinearDataClassEId, IDS_CLASSNAME_SVLINEARDATACLASS, false, SvOi::SVResetItemNone);
	m_svLinearData.setSaveValueFlag(false);
	RegisterEmbeddedObject(&m_MinProfile, SvPb::LinearThresholdMinEId, IDS_OBJECTNAME_LINEAR_PROFILE_MINVALUE, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_MaxProfile, SvPb::LinearThresholdMaxEId, IDS_OBJECTNAME_LINEAR_PROFILE_MAXVALUE, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_ProfileDelta, SvPb::LinearThresholdDeltaEId, IDS_OBJECTNAME_LINEAR_PROFILE_DELTA, false, SvOi::SVResetItemNone);
	

	m_MinProfile.SetDefaultValue(0.0);
	m_MinProfile.setSaveValueFlag(false);
	m_MaxProfile.SetDefaultValue(0.0);
	m_MaxProfile.setSaveValueFlag(false);
	m_ProfileDelta.SetDefaultValue(0.0);
	m_ProfileDelta.setSaveValueFlag(false);

	m_ulLineLength = 0;
}

HRESULT SVLinearImageOperatorList::UpdateLineExtentData()
{
	HRESULT l_hrOk = S_FALSE;

	SVImageExtentClass l_svExtents;

	long projAngle(0);

	RECT l_oRect;

	// This is the new Absolute Extent of the Image
	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (GetTool());
	if (nullptr != pTool &&
		S_OK == pTool->GetImageExtent().GetOutputRectangle(l_oRect) &&
		(S_OK == getInputProfileOrientation(projAngle)))
	{
		if (0 == projAngle)
		{
			m_ulLineLength = l_oRect.right;
		}
		else if (90 == projAngle)
		{
			m_ulLineLength = l_oRect.bottom;
		}

		if (0 < m_ulLineLength)
		{
			m_svArray.resize(m_ulLineLength);

			m_aulLineData.resize(m_ulLineLength);

			if (m_svProfileResultData.Resize(m_ulLineLength))
			{
				l_hrOk = S_OK;
			}

			m_svLinearData.SetArraySize(m_ulLineLength);
		}
	}

	return l_hrOk;
}

void SVLinearImageOperatorList::ResetLogicalROIImage()
{
	BOOL UseRotation = TRUE;
	if ((S_OK == getUseRotationAngle(UseRotation)) && !UseRotation)
	{
		//if Rotation don't used, add logical ROI image to embeddedList (It will be used.)
		auto Iter = std::find(m_embeddedList.begin(), m_embeddedList.end(), &m_LogicalROIImage);
		if (m_embeddedList.end() == Iter)
		{
			AddEmbeddedObject(&m_LogicalROIImage);
		}
	}
	else
	{
		//if Rotation used, remove logical ROI image from embeddedList to avoid the reset. (It will not used in this case.)
		auto Iter = std::find(m_embeddedList.begin(), m_embeddedList.end(), &m_LogicalROIImage);
		if (m_embeddedList.end() != Iter)
		{
			m_embeddedList.erase(Iter);
		}
	}
}

bool SVLinearImageOperatorList::RunLocalRotation(RunStatus &rRunStatus, SvOi::SVImageBufferHandlePtr input, SvOi::ITRCImagePtr pOutputBuffer, const SVImageExtentClass& rImageExtent)
{
	bool childUpdateCounters = rRunStatus.m_UpdateCounters;

	// Run yourself...
	bool bRetVal = onRun(rRunStatus, &m_RunErrorMessages);

	if (bRetVal)
	{
		if (nullptr != input && !input->empty() && nullptr != pOutputBuffer && !pOutputBuffer->isEmpty())
		{
			POINT l_oInPoint;
			POINT l_oOutPoint;
			double dRotationAngle;

			rImageExtent.GetExtentProperty(SvPb::SVExtentPropertyRotationAngle, dRotationAngle);
			rImageExtent.GetExtentProperty(SvPb::SVExtentPropertyPositionPoint, l_oInPoint);
			rImageExtent.GetExtentProperty(SvPb::SVExtentPropertyOutputPositionPoint, l_oOutPoint);

			SVMatroxImageRotateStruct l_Rotate(input->GetBuffer());
			l_Rotate.m_dAngle = dRotationAngle;
			l_Rotate.m_dSrcCenX = l_oInPoint.x;
			l_Rotate.m_dSrcCenY = l_oInPoint.y;
			l_Rotate.m_dDstCenX = l_oOutPoint.x;
			l_Rotate.m_dDstCenY = l_oOutPoint.y;
			l_Rotate.m_eInterpolation = SVNearestNeighOverScanClear;
			SVMatroxImageInterface::Rotate(pOutputBuffer->getHandle()->GetBuffer(), l_Rotate);
		}

		RunStatus ChildRunStatus;
		ChildRunStatus.m_UpdateCounters = childUpdateCounters;
		ChildRunStatus.m_triggerRecord = std::move(rRunStatus.m_triggerRecord);
		// Run children...
		for (int i = 0; i < GetSize(); i++)
		{
			ChildRunStatus.ResetRunStateAndToolSetTimes();

			SVUnaryImageOperatorClass*  pOperator = dynamic_cast<SVUnaryImageOperatorClass *>(GetAt(i));
			if (nullptr != pOperator && nullptr != pOutputBuffer && !pOutputBuffer->isEmpty())
			{
				pOperator->Run(false, pOutputBuffer->getHandle(), pOutputBuffer->getHandle(), ChildRunStatus);
			}
			else if (nullptr != dynamic_cast<SvOp::SVResult*>(GetAt(i)))
			{
				//There also SVResult objects at this level, they have to be ignored for now!
				continue;
			}
			else
			{
				bRetVal = false;
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				m_RunErrorMessages.push_back(Msg);
			}

			rRunStatus.updateState(ChildRunStatus);
		}
		rRunStatus.m_triggerRecord = std::move(ChildRunStatus.m_triggerRecord);
	}
	return bRetVal;
}

DWORD SVLinearImageOperatorList::allocateResults()
{
	DWORD LastError(0);

	for (std::size_t i = 0; i < m_Results.size(); ++i)
	{
		auto* pSrcObject = getObjectByResultIndex(static_cast<int>(i));
		if (nullptr != pSrcObject)
		{
			m_Results[i] = constructDoubleResultFromObject(*pSrcObject);
		}
		else
		{
			SvStl::MessageManager  Ex(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Ex.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_50000_InvalidObject, getObjectId());
			LastError = static_cast<DWORD> (-SvStl::Err_50000_InvalidObject);
			break;
		}
		Add(m_Results[i]);

		// Ensure this Object's inputs get connected
		if (false == m_Results[i]->connectAllInputs())
		{
			SvStl::MessageManager MesMan(SvStl::MsgType::Log);
			MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams),
				SvStl::Err_50001_CreateResultObjectFailed, getObjectId());
			LastError = static_cast<DWORD> (-SvStl::Err_50001_CreateResultObjectFailed);
			break;
		}

		// And last - Create (initialize) it
		if (!initializeResult(m_Results[i]))
		{
			SvStl::MessageManager MesMan(SvStl::MsgType::Log);
			MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_UnexpectedError, SvStl::SourceFileParams(StdMessageParams), 
				SvStl::Err_50002_ErrorConnectingInputs, getObjectId());
			LastError = static_cast<DWORD> (-SvStl::Err_50002_ErrorConnectingInputs);
			break;
		}

	}
	
	return LastError;
}

bool SVLinearImageOperatorList::initializeResult(SvOp::SVDoubleResult* pResult)
{
	bool bRet = false;
	if (!pResult->IsCreated())
	{
		// And finally try to create the child object...
		if (!CreateChildObject(pResult))
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_LinearImageOperatorList_ResultCreateFailed, SvStl::SourceFileParams(StdMessageParams), 
				SvStl::Err_50001_CreateResultObjectFailed, getObjectId());

			// Remove it from the TaskObjectList ( Destruct it )
			uint32_t objectID = pResult->getObjectId();
			if (SvDef::InvalidObjectId != objectID)
			{
				Delete(objectID);
			}
			else
			{
				delete pResult;
			}
			bRet = false;
		}
		else
		{
			bRet = true;
		}
	}
	else
	{
		bRet = true;
	}

	return bRet;
}

bool SVLinearImageOperatorList::setDefaultRange(SvOp::SVDoubleResult* pResult)
{
	bool bRet = false;
	SvOp::SVRange* pRange = pResult->GetResultRange();
	if (nullptr == pRange)
	{

		SvStl::MessageManager MesMan(SvStl::MsgType::Log);
		MesMan.setMessage(SVMSG_SVO_103_REPLACE_ERROR_TRAP, SvStl::Tid_LinearImageOperatorList_GetResultRangeFailed, SvStl::SourceFileParams(StdMessageParams), 
			SvStl::Err_50003_GetResultRangeFailed, getObjectId());
	}
	else
	{
		pRange->setLowValues(m_defaultDoubleResultLowFail, m_defaultDoubleResultLowWarn);
		pRange->setHighValues(m_defaultDoubleResultHighFail, m_defaultDoubleResultHighWarn);
		bRet = true;
	}

	return bRet;
}


SvOp::SVDoubleResult* SVLinearImageOperatorList::constructDoubleResultFromObject(const SvVol::SVDoubleValueObjectClass& dValueObject)
{
	SvIe::SVClassInfoStruct       resultClassInfo;
	SvDef::SVObjectTypeInfoStruct  interfaceInfo;

	interfaceInfo.m_EmbeddedID = dValueObject.GetEmbeddedID();
	resultClassInfo.m_DesiredInputVector.push_back(interfaceInfo);

	resultClassInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVResultObjectType;
	resultClassInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVResultDoubleObjectType;
	resultClassInfo.m_ClassId = SvPb::DoubleResultClassId;
	resultClassInfo.m_ClassName = SvUl::LoadStdString(IDS_OBJECTNAME_RESULT);
	std::string Title = dValueObject.GetName();
	resultClassInfo.m_ClassName += " " + Title;
	
	// Construct the result class
	return dynamic_cast<SvOp::SVDoubleResult*>(resultClassInfo.Construct());
}

SvVol::SVDoubleValueObjectClass* SVLinearImageOperatorList::getObjectByResultIndex(int i)
{
	SvVol::SVDoubleValueObjectClass* pObject = nullptr;
	switch (i)
	{
	case ResultIndex::MinProfileIdx:
		pObject = &m_MinProfile;
		break;
	case ResultIndex::MaxProfileIdx:
		pObject = &m_MaxProfile;
		break;
	case ResultIndex::ProfileDeltaIdx:
		pObject = &m_ProfileDelta;
		break;
	default:
		pObject = nullptr;
		break;
	}
	return pObject;
}

bool SVLinearImageOperatorList::areResultsAllocated()
{
	if (nullptr == m_Results[ResultIndex::MinProfileIdx])
	{
		m_Results[ResultIndex::MinProfileIdx] = getResultObject(&m_MinProfile);
	}
	if (nullptr == m_Results[ResultIndex::MaxProfileIdx])
	{
		m_Results[ResultIndex::MaxProfileIdx] = getResultObject(&m_MaxProfile);
	}
	if (nullptr == m_Results[ResultIndex::ProfileDeltaIdx])
	{
		m_Results[ResultIndex::ProfileDeltaIdx] = getResultObject(&m_ProfileDelta);
	}

	if (nullptr != m_Results[ResultIndex::MinProfileIdx] && 
		nullptr != m_Results[ResultIndex::MaxProfileIdx] &&
		nullptr != m_Results[ResultIndex::ProfileDeltaIdx])
	{
		return true;
	}

	return false;
}

SvOp::SVDoubleResult* SVLinearImageOperatorList::getResultObject(SvVol::SVDoubleValueObjectClass* pSrcObject)
{
	SvOp::SVDoubleResult* pResult = nullptr;
	SvDef::SVObjectTypeInfoStruct info{ SvPb::SVResultObjectType, SvPb::SVResultDoubleObjectType };
	std::vector<SvOi::IObjectClass*> list;
	fillObjectList(std::back_inserter(list), info);

	for (const auto pObject : list)
	{
		pResult = dynamic_cast<SvOp::SVDoubleResult*>(pObject);
		if (nullptr != pResult)
		{
			const auto& resultInputList = pResult->GetPrivateInputList();
			if (pSrcObject == resultInputList[0]->GetInputObjectInfo().getObject())
			{
				break;
			}
			else
			{
				pResult = nullptr;
			}
		}
	}

	return pResult;
}

} //namespace SvOp
