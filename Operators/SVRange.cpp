//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRange.cpp
//* .File Name       : $Workfile:   SVRange.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.8  $
//* .Check In Date   : $Date:   17 Feb 2015 18:14:06  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVRange.h"
#include "Definitions/StringTypeDef.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVStatusLibrary/RunStatus.h"
#include "Tools/SVTool.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct SVRangeClassCancelData : public SVCancelData	// this does not need to be visible to anyone but this file.
{
	SvVol::LinkedValue FailLow;
	SvVol::LinkedValue FailHigh;
	SvVol::LinkedValue WarnLow;
	SvVol::LinkedValue WarnHigh;
};

const TCHAR* const ToolSetName = _T("Tool Set");
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVRange, SvPb::RangeClassId);

#pragma region Constructor
SVRange::SVRange(SVObjectClass* POwner, int StringResourceID)
	: SVTaskObjectClass(POwner, StringResourceID)
{
	init();
}

SVRange::~SVRange()
{
}
#pragma endregion Constructor

void SVRange::init()
{
	m_bUseOverlays = false;

	// Identify our type in the Output List
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVRangeObjectType;

	// Register Embedded Objects
	RegisterEmbeddedObject(&m_LinkedValues[RangeEnum::ER_FailHigh], SvPb::RangeClassFailHighEId, IDS_OBJECTNAME_FAIL_HIGH, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_LinkedValues[RangeEnum::ER_WarnHigh], SvPb::RangeClassWarnHighEId, IDS_OBJECTNAME_WARN_HIGH, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_LinkedValues[RangeEnum::ER_FailLow], SvPb::RangeClassFailLowEId, IDS_OBJECTNAME_FAIL_LOW, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_LinkedValues[RangeEnum::ER_WarnLow], SvPb::RangeClassWarnLowEId, IDS_OBJECTNAME_WARN_LOW, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_LinkedValues[RangeEnum::ER_FailHigh].getLinkedName(), SvPb::RangeClassFailHighIndirectEId, IDS_OBJECTNAME_FAIL_HIGH_INDIRECT, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject(&m_LinkedValues[RangeEnum::ER_WarnHigh].getLinkedName(), SvPb::RangeClassWarnHighIndirectEId, IDS_OBJECTNAME_WARN_HIGH_INDIRECT, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject(&m_LinkedValues[RangeEnum::ER_FailLow].getLinkedName(), SvPb::RangeClassFailLowIndirectEId, IDS_OBJECTNAME_FAIL_LOW_INDIRECT, false, SvOi::SVResetItemOwner);
	RegisterEmbeddedObject(&m_LinkedValues[RangeEnum::ER_WarnLow].getLinkedName(), SvPb::RangeClassWarnLowIndirectEId, IDS_OBJECTNAME_WARN_LOW_INDIRECT, false, SvOi::SVResetItemOwner);

	// Set Embedded defaults
	_variant_t vtTemp;
	::VariantInit(&vtTemp);
	vtTemp.vt = cVarType_Value;
	vtTemp.dblVal = lowDef;
	m_LinkedValues[RangeEnum::ER_FailLow].SetDefaultValue(vtTemp, true);
	m_LinkedValues[RangeEnum::ER_WarnLow].SetDefaultValue(vtTemp, true);
	vtTemp.dblVal = highDef;
	m_LinkedValues[RangeEnum::ER_FailHigh].SetDefaultValue(vtTemp, true);
	m_LinkedValues[RangeEnum::ER_WarnHigh].SetDefaultValue(vtTemp, true);

	for (int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		m_LinkedValues[i].getLinkedName().SetDefaultValue(_T(""), true);
	}

	// Setup up the input
	m_inputObjectInfo.SetObject(GetObjectInfo());
	RegisterInputObject(&m_inputObjectInfo, _T("RangeValue"));

	// Add Default Inputs and Outputs
	addDefaultInputObjects();
}

bool SVRange::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	m_isCreated = SVTaskObjectClass::CreateObject(rCreateStructure);

	// Set / Reset Printable Flags
	const UINT cAttributes = SvPb::audittrail | SvPb::setableOnline | SvPb::remotelySetable;
	for (int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		m_LinkedValues[i].SetObjectAttributesAllowed(cAttributes, SvOi::SetAttributeType::AddAttribute);
		m_LinkedValues[i].getLinkedName().SetObjectAttributesAllowed(SvPb::audittrail | SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
	}

	return m_isCreated;
}

bool SVRange::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = true;

	SvOl::ValidateInput(m_inputObjectInfo);

	// check if input is valid
	if (!m_inputObjectInfo.IsConnected() || nullptr == m_inputObjectInfo.GetInputObjectInfo().getObject())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorGettingInputs, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	Result = Result && checkLinkedValues(RangeEnum::ERange::ER_FailHigh, RangeEnum::ERange::ER_WarnHigh, pErrorMessages);
	Result = Result && checkLinkedValues(RangeEnum::ERange::ER_FailHigh, RangeEnum::ERange::ER_WarnLow, pErrorMessages);
	Result = Result && checkLinkedValues(RangeEnum::ERange::ER_FailHigh, RangeEnum::ERange::ER_FailLow, pErrorMessages);
	Result = Result && checkLinkedValues(RangeEnum::ERange::ER_WarnHigh, RangeEnum::ERange::ER_WarnLow, pErrorMessages);
	Result = Result && checkLinkedValues(RangeEnum::ERange::ER_WarnHigh, RangeEnum::ERange::ER_FailLow, pErrorMessages);
	Result = Result && checkLinkedValues(RangeEnum::ERange::ER_WarnLow, RangeEnum::ERange::ER_FailLow, pErrorMessages);

	Result = SVTaskObjectClass::ResetObject(pErrorMessages) && Result;
	return Result;
}

void SVRange::addEntriesToMonitorList(std::back_insert_iterator<SvOi::ParametersForML> inserter) const
{
	inserter = SvOi::ParameterPairForML(m_LinkedValues[RangeEnum::ER_FailLow].GetCompleteName(), m_LinkedValues[RangeEnum::ER_FailLow].getObjectId());
	// cppcheck-suppress redundantAssignment symbolName=inserter ; cppCheck doesn't know back_insert_iterator
	inserter = SvOi::ParameterPairForML(m_LinkedValues[RangeEnum::ER_FailHigh].GetCompleteName(), m_LinkedValues[RangeEnum::ER_FailHigh].getObjectId());
	inserter = SvOi::ParameterPairForML(m_LinkedValues[RangeEnum::ER_WarnLow].GetCompleteName(), m_LinkedValues[RangeEnum::ER_WarnLow].getObjectId());
	// cppcheck-suppress unreadVariable symbolName=inserter ; cppCheck doesn't know back_insert_iterator
	// cppcheck-suppress redundantAssignment symbolName=inserter ; cppCheck doesn't know back_insert_iterator
	inserter = SvOi::ParameterPairForML(m_LinkedValues[RangeEnum::ER_WarnHigh].GetCompleteName(), m_LinkedValues[RangeEnum::ER_WarnHigh].getObjectId());
}

bool SVRange::onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector*)
{
	double InputValue, rangeValues[RangeEnum::ER_COUNT];
	getInputValue(InputValue);

	for (int i = 0; i < RangeEnum::ER_COUNT; i++)
	{
		m_LinkedValues[i].getValue(rangeValues[i]);
		
	}

	bool isFailed = (InputValue < rangeValues[RangeEnum::ER_FailLow] || InputValue > rangeValues[RangeEnum::ER_FailHigh]);
	bool isWarned = (!isFailed && (InputValue < rangeValues[RangeEnum::ER_WarnLow] || InputValue > rangeValues[RangeEnum::ER_WarnHigh]));

	if (isFailed)
	{
		rRunStatus.SetFailed();
	}

	if (isWarned)
	{
		rRunStatus.SetWarned();
	}

	if (!isFailed && !isWarned)
	{
		rRunStatus.SetPassed();
	}

	return true;
}

// ISVCancel interface
HRESULT SVRange::GetCancelData(SVCancelData*& rpCancelData)
{
	assert(nullptr == rpCancelData);
	SVRangeClassCancelData* pData = new SVRangeClassCancelData;
	rpCancelData = pData;

	pData->FailHigh = m_LinkedValues[RangeEnum::ER_FailHigh];
	pData->FailLow = m_LinkedValues[RangeEnum::ER_FailLow];
	pData->WarnHigh = m_LinkedValues[RangeEnum::ER_WarnHigh];
	pData->WarnLow = m_LinkedValues[RangeEnum::ER_WarnLow];

	return S_OK;
}

HRESULT SVRange::SetCancelData(SVCancelData* pCancelData)
{
	SVRangeClassCancelData* pData = dynamic_cast<SVRangeClassCancelData*> (pCancelData);
	if (pData)
	{
		m_LinkedValues[RangeEnum::ER_FailHigh] = pData->FailHigh;
		m_LinkedValues[RangeEnum::ER_FailLow] = pData->FailLow;
		m_LinkedValues[RangeEnum::ER_WarnHigh] = pData->WarnHigh;
		m_LinkedValues[RangeEnum::ER_WarnLow] = pData->WarnLow;

		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

HRESULT SVRange::SetValuesForAnObject(uint32_t aimObjectID, SVObjectAttributeClass* pDataObject)
{
	//This method in this class is only for backward compatibility (configuration older then 8.10), to set the indirect values correct to the linkedValues
	SvCl::SVObjectStdStringArrayClass valueStringArray;
	if (SV_STRING_Type == pDataObject->GetType() && pDataObject->GetData(valueStringArray) && 1 == valueStringArray.size() && !valueStringArray[0].empty())
	{
		for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
		{
			SVObjectClass* pObject = *Iter;
			if (nullptr != pObject)
			{
				// check if it's this object
				if (aimObjectID == pObject->getObjectId())
				{
					if (SvPb::RangeClassFailHighIndirectEId == pObject->GetEmbeddedID())
					{
						if (VT_BSTR != m_LinkedValues[RangeEnum::ER_FailHigh].GetType())
						{
							m_LinkedValues[RangeEnum::ER_FailHigh].SetObjectValue(pDataObject);
							return S_OK;
						}
					}
					if (SvPb::RangeClassWarnHighIndirectEId == pObject->GetEmbeddedID())
					{
						if (VT_BSTR != m_LinkedValues[RangeEnum::ER_WarnHigh].GetType())
						{
							m_LinkedValues[RangeEnum::ER_WarnHigh].SetObjectValue(pDataObject);
							return S_OK;
						}
					}
					if (SvPb::RangeClassFailLowIndirectEId == pObject->GetEmbeddedID())
					{
						if (VT_BSTR != m_LinkedValues[RangeEnum::ER_FailLow].GetType())
						{
							m_LinkedValues[RangeEnum::ER_FailLow].SetObjectValue(pDataObject);
							return S_OK;
						}
					}
					if (SvPb::RangeClassWarnLowIndirectEId == pObject->GetEmbeddedID())
					{
						if (VT_BSTR != m_LinkedValues[RangeEnum::ER_WarnLow].GetType())
						{
							m_LinkedValues[RangeEnum::ER_WarnLow].SetObjectValue(pDataObject);
							return S_OK;
						}
					}
					break;
				}
			}
		}
	}

	return __super::SetValuesForAnObject(aimObjectID, pDataObject);
}

void SVRange::setHighValues(double failHigh, double warnHigh)
{
	m_LinkedValues[RangeEnum::ER_FailHigh].SetValue(failHigh);
	m_LinkedValues[RangeEnum::ER_WarnHigh].SetValue(warnHigh);
}

void SVRange::setLowValues(double failLow, double warnLow)
{
	m_LinkedValues[RangeEnum::ER_FailLow].SetValue(failLow);
	m_LinkedValues[RangeEnum::ER_WarnLow].SetValue(warnLow);
}

void SVRange::setDefaultLowValues(double failLow, double warnLow)
{
	m_LinkedValues[RangeEnum::ER_FailLow].SetDefaultValue(failLow);
	m_LinkedValues[RangeEnum::ER_WarnLow].SetDefaultValue(warnLow);
}

bool SVRange::getInputValue(double& rValue)
{
	if (m_inputObjectInfo.IsConnected() && nullptr != m_inputObjectInfo.GetInputObjectInfo().getObject())
	{
		return S_OK == m_inputObjectInfo.GetInputObjectInfo().getObject()->getValue(rValue);
	}
	return false;
}

HRESULT SVRange::getValue(RangeEnum::ERange ra, double &rValue)
{
	switch (ra)
	{
		case RangeEnum::ER_FailHigh:
		case RangeEnum::ER_WarnHigh:
		case RangeEnum::ER_FailLow:
		case RangeEnum::ER_WarnLow:
			m_LinkedValues[ra].getValue(rValue);
			break;
		default:
			return E_FAIL;
	}
	return S_OK;
};

bool SVRange::checkLinkedValues(RangeEnum::ERange firstType, RangeEnum::ERange secondType, SvStl::MessageContainerVector* pErrorMessages)
{
	static std::map<RangeEnum::ERange, SvStl::MessageTextEnum> enumTids
	{
		{RangeEnum::ERange::ER_FailHigh, SvStl::Tid_FailHigh},
		{RangeEnum::ERange::ER_WarnHigh, SvStl::Tid_WarnHigh},
		{RangeEnum::ERange::ER_FailLow, SvStl::Tid_FailLow},
		{RangeEnum::ERange::ER_WarnLow, SvStl::Tid_WarnLow}
	};
	bool Result = true;
	if (!m_LinkedValues[firstType].isIndirectValue() && !m_LinkedValues[secondType].isIndirectValue())
	{
		//check if high greater than low
		double excludeHigh = 0;
		m_LinkedValues[firstType].getValue(excludeHigh);
		double excludeLow = 0;
		m_LinkedValues[secondType].getValue(excludeLow);

		if (excludeHigh < excludeLow)
		{
			Result = false;
			if (nullptr != pErrorMessages)
			{
				SvDef::StringVector messageList;
				messageList.push_back(SvStl::MessageData::convertId2AdditionalText(enumTids[firstType]));
				messageList.push_back(SvStl::MessageData::convertId2AdditionalText(enumTids[secondType]));
				SvStl::MessageContainer message;
				message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_IsLessThan, messageList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				pErrorMessages->push_back(message);
			}
		}
	}
	return Result;
}

} //namespace SvOp
