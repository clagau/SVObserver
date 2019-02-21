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
#include "SVObjectLibrary/SVClsIds.h"
#include "SVObjectLibrary/SVInObjectInfoStruct.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "Tools/SVLinearToolClass.h"
#include "Tools/SVTool.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVLinearImageOperatorListClass, SVLinearImageOperatorListClassGuid)


SVLinearImageOperatorListClass::SVLinearImageOperatorListClass(SVObjectClass* POwner, int StringResourceID)
	:SVStdImageOperatorListClass(POwner, StringResourceID)
{
	init();
}

SVLinearImageOperatorListClass::~SVLinearImageOperatorListClass()
{
	CloseObject();
}

bool SVLinearImageOperatorListClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool l_bOk = SVStdImageOperatorListClass::CreateObject(rCreateStructure);

	SvIe::SVDataBufferInfoClass& rBufferInfo = m_svProfileResultData.GetDataBufferInfo();
	rBufferInfo.Type = SvIe::SVDataBufferInfoClass::SVProjectResult;

	ResetLogicalROIImage();

	l_bOk &= S_OK == UpdateLineExtentData();
	m_svLinearData.SetObjectAttributesAllowed(SvPb::viewable | SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);

	m_svMaxThreshold.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);
	m_svMinThreshold.SetObjectAttributesAllowed(SvPb::printable, SvOi::SetAttributeType::RemoveAttribute);


	return l_bOk;
}

bool SVLinearImageOperatorListClass::CloseObject()
{
	bool l_bOk = m_svProfileResultData.CloseObject();

	l_bOk &= SVStdImageOperatorListClass::CloseObject();

	return l_bOk;
}

bool SVLinearImageOperatorListClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);
	ResetLogicalROIImage();

	SvOl::SVInObjectInfoStructPtrVector InputList
	{
		&inputUseRotationAngle,
		&inputProfileOrientation
	};

	SvOl::ValidateInputList(InputList);

	CollectInputImageNames();

	if (S_OK != UpdateLineExtentData())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_UpdateLineExtentDataFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
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

bool SVLinearImageOperatorListClass::Run(SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	clearRunErrorMessages();

	SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(GetTool());
	bool result = nullptr != pTool;
	const SVImageExtentClass&  rImageExtent = result ? pTool->GetImageExtent() : SVImageExtentClass{};

	BOOL UseRotation = true;
	result = result && (S_OK == getUseRotationAngle(UseRotation));

	if (!result)
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
		m_RunErrorMessages.push_back(Msg);
	}

	SvIe::SVImageClass* pInputImage = getInputImage(true);
	if (nullptr == pInputImage)
	{
		// Signal something is wrong...
		result = false;
		SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
		m_RunErrorMessages.push_back(Msg);
	}

	SvTrc::IImagePtr pOutputBuffer = m_OutputImage.getImageToWrite(rRunStatus.m_triggerRecord);

	if (result)
	{
		if (UseRotation)
		{
			SvTrc::IImagePtr pInputBuffer = (nullptr != pInputImage) ? pInputImage->getImageReadOnly(rRunStatus.m_triggerRecord) : nullptr;
			result = RunLocalRotation(rRunStatus, pInputBuffer, pOutputBuffer, rImageExtent);
		}
		else
		{
			SvTrc::IImagePtr pInputImageBuffer = m_LogicalROIImage.getImageReadOnly(rRunStatus.m_triggerRecord);
			result = RunLocal(rRunStatus, pInputImageBuffer, pOutputBuffer);
		}
	}

	if (result)
	{
		SvIe::SVDataBufferInfoClass& rDataBufferInfo = m_svProfileResultData.GetDataBufferInfo();

		long ProjectAngle(0L);

		if (0 == m_aulLineData.size() || S_OK != getInputProfileOrientation(ProjectAngle))
		{
			result = false;
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			m_RunErrorMessages.push_back(Msg);
		}

		double dMin(9999999.0);
		double dMax(0.0);

		if (result)
		{
			double l_dProjectHeight = 0.0;
			SVMatroxBuffer buffer;
			if (nullptr != pOutputBuffer && !pOutputBuffer->isEmpty())
			{
				buffer = pOutputBuffer->getHandle()->GetBuffer();
			}
			SVMatroxImageInterface::Project(rDataBufferInfo.HBuffer.milResult, buffer, static_cast<double> (ProjectAngle));

			SVMatroxImageInterface::GetResult(rDataBufferInfo.HBuffer.milResult, m_aulLineData);

			if (0 == ProjectAngle)
			{
				rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyHeight, l_dProjectHeight);
			}
			else
			{
				rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyWidth, l_dProjectHeight);
			}

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

			result = (S_OK == m_svMinThreshold.SetValue(dMin)) && result;
			result = (S_OK == m_svMaxThreshold.SetValue(dMax)) && result;

			result = (S_OK == m_svLinearData.SetArrayValues(m_svArray)) && result;
			assert(result);
			if (!result)
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SetValueFailed, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				m_RunErrorMessages.push_back(Msg);
			}
		}
	}

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}

	if (!result)
	{
		// Something was wrong...

		SetInvalid();
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

HRESULT SVLinearImageOperatorListClass::getUseRotationAngle(BOOL& rUseRotationAngle)
{
	HRESULT Result(E_FAIL);

	if (inputUseRotationAngle.IsConnected() && nullptr != inputUseRotationAngle.GetInputObjectInfo().getObject())
	{
		double Value(0.0);
		Result = inputUseRotationAngle.GetInputObjectInfo().getObject()->getValue(Value);
		rUseRotationAngle = 0.0 < Value ? true : false;
	}

	return Result;
}

HRESULT SVLinearImageOperatorListClass::getInputProfileOrientation(long& rProfileOrientation)
{
	HRESULT Result(E_FAIL);

	if (inputProfileOrientation.IsConnected() && nullptr != inputProfileOrientation.GetInputObjectInfo().getObject())
	{
		double Value(0.0);
		Result = inputProfileOrientation.GetInputObjectInfo().getObject()->getValue(Value);
		rProfileOrientation = static_cast<long> (Value);
	}

	return Result;
}

void SVLinearImageOperatorListClass::init()
{
	// Identify our output type
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVUnaryImageOperatorListObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVLinearImageOperatorListObjectType;

	inputProfileOrientation.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVEnumValueObjectType, SVProfileOrientationGuid);
	inputProfileOrientation.SetObject(GetObjectInfo());
	RegisterInputObject(&inputProfileOrientation, _T("LinearImageOperatorListProfileOrientation"));

	inputUseRotationAngle.SetInputObjectType(SvPb::SVValueObjectType, SvPb::SVBoolValueObjectType, SVLinearToolUseRotationGuid);
	inputUseRotationAngle.SetObject(GetObjectInfo());
	RegisterInputObject(&inputUseRotationAngle, _T("LinearImageOperatorListUseRotationAngle"));

	m_svLinearData.SetLegacyVectorObjectCompatibility();

	RegisterEmbeddedObject(&m_svLinearData, SVLinearDataClassGuid, IDS_CLASSNAME_SVLINEARDATACLASS, false, SvOi::SVResetItemNone);
	m_svLinearData.setSaveValueFlag(false);
	RegisterEmbeddedObject(&m_svMinThreshold, SVLinearThresholdMinObjectGuid, IDS_OBJECTNAME_LINEAR_THRESHOLD_MINVALUE, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_svMaxThreshold, SVLinearThresholdMaxObjectGuid, IDS_OBJECTNAME_LINEAR_THRESHOLD_MAXVALUE, false, SvOi::SVResetItemNone);

	m_svMinThreshold.SetDefaultValue(0.0);
	m_svMinThreshold.setSaveValueFlag(false);
	m_svMaxThreshold.SetDefaultValue(0.0);
	m_svMaxThreshold.setSaveValueFlag(false);

	m_ulLineLength = 0;

	// Set default inputs and outputs
	addDefaultInputObjects();
}

HRESULT SVLinearImageOperatorListClass::UpdateLineExtentData()
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

void SVLinearImageOperatorListClass::ResetLogicalROIImage()
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

bool SVLinearImageOperatorListClass::RunLocalRotation(SVRunStatusClass &rRunStatus, SvTrc::IImagePtr pInputBuffer, SvTrc::IImagePtr pOutputBuffer, const SVImageExtentClass& rImageExtent)
{
	SVRunStatusClass ChildRunStatus;
	ChildRunStatus.m_lResultDataIndex = rRunStatus.m_lResultDataIndex;
	ChildRunStatus.m_triggerRecord = rRunStatus.m_triggerRecord;
	ChildRunStatus.m_UpdateCounters = rRunStatus.m_UpdateCounters;

	// Run yourself...
	bool bRetVal = onRun(rRunStatus, &m_RunErrorMessages);

	if (bRetVal)
	{
		if (nullptr != pInputBuffer && !pInputBuffer->isEmpty() && nullptr != pOutputBuffer && !pOutputBuffer->isEmpty())
		{
			POINT l_oInPoint;
			POINT l_oOutPoint;
			double dRotationAngle;

			rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyRotationAngle, dRotationAngle);
			rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyPositionPoint, l_oInPoint);
			rImageExtent.GetExtentProperty(SvDef::SVExtentPropertyOutputPositionPoint, l_oOutPoint);

			SVMatroxImageRotateStruct l_Rotate(pInputBuffer->getHandle()->GetBuffer());
			l_Rotate.m_dAngle = dRotationAngle;
			l_Rotate.m_dSrcCenX = l_oInPoint.x;
			l_Rotate.m_dSrcCenY = l_oInPoint.y;
			l_Rotate.m_dDstCenX = l_oOutPoint.x;
			l_Rotate.m_dDstCenY = l_oOutPoint.y;
			l_Rotate.m_eInterpolation = SVNearestNeighOverScanClear;
			SVMatroxImageInterface::Rotate(pOutputBuffer->getHandle()->GetBuffer(), l_Rotate);
		}


		// Run children...
		for (int i = 0; i < GetSize(); i++)
		{
			ChildRunStatus.ResetRunStateAndToolSetTimes();

			SVUnaryImageOperatorClass*  pOperator = dynamic_cast<SVUnaryImageOperatorClass *>(GetAt(i));
			if (nullptr != pOperator && nullptr != pOutputBuffer && !pOutputBuffer->isEmpty())
			{
				pOperator->Run(false, pOutputBuffer->getHandle(), pOutputBuffer->getHandle(), ChildRunStatus);
			}
			else
			{
				bRetVal = false;
				SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				m_RunErrorMessages.push_back(Msg);
			}

			// Update our Run Status
			if (ChildRunStatus.IsDisabled()) { rRunStatus.SetDisabled(); }

			if (ChildRunStatus.IsDisabledByCondition())	{ rRunStatus.SetDisabledByCondition();}

			if (ChildRunStatus.IsWarned()) { rRunStatus.SetWarned();}

			if (ChildRunStatus.IsFailed()) {rRunStatus.SetFailed();}

			if (ChildRunStatus.IsPassed()) {rRunStatus.SetPassed();}

			if (ChildRunStatus.IsCriticalFailure())	{rRunStatus.SetCriticalFailure();}
		}
	}
	return bRetVal;
}

} //namespace SvOp