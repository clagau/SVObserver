//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file BlobFeatureTask.cpp
/// Class for the parameter a feature of the blob analyzer.
//******************************************************************************

#include "stdafx.h"
#include "BlobFeatureTask.h"
#include "Operators/IndexEquation.h"
#include "SVProtoBuf/SVO-Enum.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectLibrary.h"
#include "InspectionEngine/RunStatus.h"
#include "SVValueObjectLibrary/DoubleSortValueObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvAo
{
	enum Range_defaults
	{
		lowDef = 0, // Warn/Fail Low Default
		highDef = 9999999 // Warn/Fail High Default
	};
	static const VARTYPE cVarType_Value = VT_R8;

	enum SV_BLOB_COLOR_ENUM
	{
		SV_BLOB_BLACK = 0,
		SV_BLOB_WHITE = 1
	};// end SV_BLOB_COLOR_ENUM

	  // Add String for SVEnumerateValueObjectClass
	const LPCSTR g_strFeatureTypeEnums = _T("None=0,Any=1");


	const LPCSTR g_strSortEnums = _T("None=0,Ascent=1,Descent=2");

	SV_IMPLEMENT_CLASS(BlobFeatureTask, SvPb::BlobFeatureTaskClassId);

	bool setLinkedValue(std::string indirectValue, double value, SvVol::LinkedValue& linkedValue)
	{
		bool result = true;
		if (indirectValue.empty())
		{
			result = linkedValue.setDirectValue(value);
		}
		else
		{
			try
			{
				result = linkedValue.setIndirectValue(indirectValue);
				linkedValue.resetAllObjects();
			}
			catch (...)
			{
				return false;
			}
		}
		return result;
	}

	BlobFeatureTask::BlobFeatureTask(SVObjectClass* POwner, int StringResourceID)
		:SVTaskObjectListClass(POwner, StringResourceID)
	{
		m_ObjectTypeInfo.m_ObjectType = SvPb::BlobFeatureObjectType;

		registerPwfAndSetDefaults(*this);

		RegisterEmbeddedObject(&m_featureTypeValue, SvPb::FeatureTypeEId, IDS_BLOBFEATURE_TYPE, false, SvOi::SVResetItemOwner, false);
		RegisterEmbeddedObject(&m_isCustomFeature, SvPb::IsCustomFeatureEId, IDS_BLOBFEATURE_ISCUSTOMFEATURE, false, SvOi::SVResetItemOwner, false);
		RegisterEmbeddedObject(&m_sortEnumValue, SvPb::SortEnumEId, IDS_BLOBFEATURE_SORT, false, SvOi::SVResetItemOwner, false);
		RegisterEmbeddedObject(&m_isExcludeValue, SvPb::IsExcludeEId, IDS_BLOBFEATURE_ISEXCLUDE, false, SvOi::SVResetItemOwner, false);
		RegisterEmbeddedObject(&m_isInnerExcludeValue, SvPb::IsInnerExcludeEId, IDS_BLOBFEATURE_ISINNER_EXCLDUE, false, SvOi::SVResetItemOwner, false);
		RegisterEmbeddedObject(&m_ExcludeLowerBoundValue, SvPb::ExcludeLowerBoundEId, IDS_BLOBFEATURE_EXCLUDE_LOWER, false, SvOi::SVResetItemOwner, false);
		RegisterEmbeddedObject(&m_ExcludeUpperBoundValue, SvPb::ExcludeUpperBoundEId, IDS_BLOBFEATURE_EXCLUDE_UPPER, false, SvOi::SVResetItemOwner, false);
		
		RegisterEmbeddedObject(&m_isRangeValue, SvPb::IsRangeEId, IDS_BLOBFEATURE_ISRANGE, false, SvOi::SVResetItemOwner, false);
		RegisterEmbeddedObject(&m_RangeValues[RangeEnum::ER_FailHigh], SvPb::RangeClassFailHighEId, IDS_OBJECTNAME_FAIL_HIGH, false, SvOi::SVResetItemNone, false);
		RegisterEmbeddedObject(&m_RangeValues[RangeEnum::ER_WarnHigh], SvPb::RangeClassWarnHighEId, IDS_OBJECTNAME_WARN_HIGH, false, SvOi::SVResetItemNone, false);
		RegisterEmbeddedObject(&m_RangeValues[RangeEnum::ER_FailLow], SvPb::RangeClassFailLowEId, IDS_OBJECTNAME_FAIL_LOW, false, SvOi::SVResetItemNone, false);
		RegisterEmbeddedObject(&m_RangeValues[RangeEnum::ER_WarnLow], SvPb::RangeClassWarnLowEId, IDS_OBJECTNAME_WARN_LOW, false, SvOi::SVResetItemNone, false);
				
		m_featureTypeValue.SetDefaultValue(0, true);
		m_isCustomFeature.SetDefaultValue(BOOL(false), true);

		m_sortEnumValue.SetEnumTypes(g_strSortEnums);
		m_sortEnumValue.SetDefaultValue(static_cast<long>(SortEnum::None), true);
		m_isExcludeValue.SetDefaultValue(BOOL(false), true);
		m_isInnerExcludeValue.SetDefaultValue(BOOL(false), true);
		m_ExcludeLowerBoundValue.SetDefaultValue(static_cast<double>(lowDef), true);
		m_ExcludeUpperBoundValue.SetDefaultValue(static_cast<double>(highDef), true);

		m_isRangeValue.SetDefaultValue(BOOL(false), true);
		_variant_t vtTemp;
		::VariantInit(&vtTemp);
		vtTemp.vt = cVarType_Value;
		vtTemp.dblVal = lowDef;
		m_RangeValues[RangeEnum::ER_FailLow].SetDefaultValue(vtTemp, true);
		m_RangeValues[RangeEnum::ER_WarnLow].SetDefaultValue(vtTemp, true);
		vtTemp.dblVal = highDef;
		m_RangeValues[RangeEnum::ER_FailHigh].SetDefaultValue(vtTemp, true);
		m_RangeValues[RangeEnum::ER_WarnHigh].SetDefaultValue(vtTemp, true);
	}

	bool BlobFeatureTask::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
	{
		bool l_bOk = SVTaskObjectClass::CreateObject(rCreateStructure);

		m_ExcludeLowerBoundValue.setExcludeSameLineageListForObjectSelector({rCreateStructure.m_pTool});
		m_ExcludeUpperBoundValue.setExcludeSameLineageListForObjectSelector({rCreateStructure.m_pTool});
		m_RangeValues[RangeEnum::ER_FailHigh].setExcludeSameLineageListForObjectSelector({rCreateStructure.m_pTool});
		m_RangeValues[RangeEnum::ER_WarnHigh].setExcludeSameLineageListForObjectSelector({rCreateStructure.m_pTool});
		m_RangeValues[RangeEnum::ER_FailLow].setExcludeSameLineageListForObjectSelector({rCreateStructure.m_pTool});
		m_RangeValues[RangeEnum::ER_WarnLow].setExcludeSameLineageListForObjectSelector({rCreateStructure.m_pTool});

		updateEquation();

		return l_bOk;
	}

	void BlobFeatureTask::evalRange(SvIe::RunStatus& rRunStatus)
	{
		//range check
		BOOL isRange = false;
		m_isRangeValue.GetValue(isRange);
		if (isRange)
		{
			double failHigh, warnHigh, warnLow, failLow;
			bool isWarn = false, isFail = false;
			m_RangeValues[RangeEnum::ER_FailHigh].getValue(failHigh);
			m_RangeValues[RangeEnum::ER_WarnHigh].getValue(warnHigh);
			m_RangeValues[RangeEnum::ER_WarnLow].getValue(warnLow);
			m_RangeValues[RangeEnum::ER_FailLow].getValue(failLow);
			std::vector<double> values;
			m_pValue->GetArrayValues(values);
			for (auto value : values)
			{
				if (failHigh < value || failLow > value)
				{
					isFail = true;
					break;
				}
				if (!isWarn && (warnHigh < value || warnLow > value))
				{
					isWarn = true;
				}
			}
			if (isFail)
			{
				rRunStatus.SetFailed();
			}
			else if (isWarn)
			{
				rRunStatus.SetWarned();
			}
			else
			{
				rRunStatus.SetPassed();
			}

			setWarnedFailedStatus(isWarn, isFail);
		}
		setStatus(rRunStatus);
	}

	void BlobFeatureTask::addExcludeBlobs(std::set<int>& rExcludeSet)
	{
		Log_Assert(isCustomFeature());

		BOOL isExclude = false;
		m_isExcludeValue.GetValue(isExclude);
		if (isExclude)
		{
			std::vector<double> values;
			m_pValue->GetArrayValues(values);
			if (0 < values.size())
			{
				BOOL isInner = false;
				m_isInnerExcludeValue.GetValue(isInner);
				double upperBound, lowerBound;
				m_ExcludeLowerBoundValue.getValue(lowerBound);
				m_ExcludeUpperBoundValue.getValue(upperBound);

				for (int i = static_cast<int>(values.size()) - 1; i >= 0; --i)
				{
					if ((upperBound < values[i] || lowerBound > values[i]) ^ isInner)
					{
						rExcludeSet.insert(i);
					}
				}
			}
		}
	}

	long BlobFeatureTask::getFeatureType() const
	{
		long type = 0;
		m_featureTypeValue.GetValue(type);
		return type;
	}

	bool BlobFeatureTask::isCustomFeature() const
	{
		BOOL value = false;
		m_isCustomFeature.GetValue(value);
		return TRUE == value;
	}

	uint32_t BlobFeatureTask::getEquationId() const
	{
		if (nullptr != m_pEquation)
		{
			return m_pEquation->getObjectId();
		}
		else
		{
			return SvDef::InvalidObjectId;
		}
	}

	void BlobFeatureTask::setFeatureType(long type, bool isCustomFeature)
	{
		m_featureTypeValue.SetValue(type);
		m_isCustomFeature.SetValue(isCustomFeature);
		updateEquation();
	}

	int BlobFeatureTask::setFeatureData(const SvPb::FeatureData& rData)
	{
		if (rData.is_custom())
		{
			SetName(rData.name().c_str());
		}
		SortEnum sortEnum = SortEnum::None;
		if (rData.is_sort())
		{
			if (rData.is_ascent())
			{
				sortEnum = SortEnum::Ascent;
			}
			else
			{
				sortEnum = SortEnum::Descent;
			}
		}
		m_sortEnumValue.SetValue(static_cast<long>(sortEnum));
		m_isExcludeValue.SetValue(rData.is_exclude());
		if (rData.is_exclude())
		{
			m_isInnerExcludeValue.SetValue(rData.is_exclude_inner());
			SvStl::MessageContainerVector messageContainers = validateAndSetEmbeddedValues({{SvOi::SetLinkedStruct{&m_ExcludeLowerBoundValue, rData.lower_bound()}}}, true);
			if (0 != messageContainers.size())
			{
				return rData.kLowerBoundFieldNumber;
			}
			messageContainers = validateAndSetEmbeddedValues({{SvOi::SetLinkedStruct{&m_ExcludeUpperBoundValue, rData.upper_bound()}}}, true);
			if (0 != messageContainers.size())
			{
				return rData.kUpperBoundFieldNumber;
			}
		}
		
		m_isRangeValue.SetValue(rData.is_range());
		if (rData.is_range())
		{
			SvStl::MessageContainerVector messageContainers = validateAndSetEmbeddedValues({{SvOi::SetLinkedStruct{&m_RangeValues[RangeEnum::ER_FailHigh], rData.range_fail_high()}}}, true);
			if (0 != messageContainers.size())
			{
				return rData.kRangeFailHighFieldNumber;
			}
			messageContainers = validateAndSetEmbeddedValues({{SvOi::SetLinkedStruct{&m_RangeValues[RangeEnum::ER_WarnHigh], rData.range_warn_high()}}}, true);
			if (0 != messageContainers.size())
			{
				return rData.kRangeWarnHighFieldNumber;
			}
			messageContainers = validateAndSetEmbeddedValues({{SvOi::SetLinkedStruct{&m_RangeValues[RangeEnum::ER_WarnLow], rData.range_warn_low()}}}, true);
			if (0 != messageContainers.size())
			{
				return rData.kRangeWarnLowFieldNumber;
			}
			messageContainers = validateAndSetEmbeddedValues({{SvOi::SetLinkedStruct{&m_RangeValues[RangeEnum::ER_FailLow], rData.range_fail_low()}}}, true);
			if (0 != messageContainers.size())
			{
				return rData.kRangeFailLowFieldNumber;
			}
		}

		return 0;
	}

	BlobExcludeData BlobFeatureTask::getExcludeData() const
	{
		BlobExcludeData data(GetName(), m_ExcludeLowerBoundValue, m_ExcludeUpperBoundValue, getFeatureType());
		BOOL tmpBool = false;
		m_isExcludeValue.GetValue(tmpBool);
		data.m_isEnabled = (TRUE == tmpBool);
		m_isInnerExcludeValue.GetValue(tmpBool);
		data.m_isInner = (TRUE == tmpBool);
		return data;
	}

	BlobRangeData BlobFeatureTask::getRangeData() const
	{
		BOOL tmpBool = false;
		m_isRangeValue.GetValue(tmpBool);
		return {getFeatureType(), (TRUE == tmpBool), m_RangeValues};
	}

	SortEnum BlobFeatureTask::getSortEnum() const
	{
		long value = 0;
		m_sortEnumValue.GetValue(value);
		return static_cast<SortEnum>(value);
	}

	void BlobFeatureTask::fillValues(MIL_ID resultBufferId)
	{
		if (nullptr != m_pValue && 0 < m_pValue->getSortContainerSize())
		{
			if (!isCustomFeature())
			{
				std::vector<double> valueArray;
				valueArray.resize(m_pValue->getSortContainerSize(), 0);
				MblobGetResult(resultBufferId, M_INCLUDED_BLOBS, getFeatureType(), valueArray);
				m_pValue->SetArrayValues(valueArray);
			}
		}
	}

	void BlobFeatureTask::setValueObject(SvVol::DoubleSortValuePtr pValue)
	{ 
		m_pValue = pValue; 
		if (nullptr != m_pEquation)
		{
			m_pEquation->setResultColumn(m_pValue);
		}
	}

	void BlobFeatureTask::updateEquation()
	{
		for (size_t j = 0; j < m_friendList.size(); j++)
		{
			SvOp::IndexEquation* pEquation = dynamic_cast<SvOp::IndexEquation*> (m_friendList[j]);
			if (nullptr != pEquation)
			{
				if (SvDef::CustomFeatureEquationName == std::string(pEquation->GetName()))
				{
					m_pEquation = pEquation;
					break;
				}
			}
		}

		BOOL isCustomFeature;
		m_isCustomFeature.GetValue(isCustomFeature);
		if (isCustomFeature)
		{
			if (nullptr == m_pEquation)
			{
				SvOp::IndexEquation* pObject = dynamic_cast<SvOp::IndexEquation*> (SvOi::ConstructObject(SvPb::IndexEquationId));
				if (nullptr != pObject)
				{
					pObject->SetObjectOwner(this);
					pObject->SetName(SvDef::CustomFeatureEquationName);
					pObject->SetEquationText(SvDef::cIndexKeyword);
					AddFriend(pObject);
					if (CreateChildObject(pObject))
					{
						m_pEquation = pObject;
					}
					else
					{
						SvStl::MessageManager Msg(SvStl::MsgType::Log);
						Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CreateChildBufferFailed, SvStl::SourceFileParams(StdMessageParams), getObjectId());
						Delete(pObject->getObjectId());
					}					
				}
			}
			if (nullptr != m_pEquation)
			{
				m_pEquation->setResultColumn(getValueObject());
			}
		}
		else
		{
			if (nullptr != m_pEquation)
			{
				Delete(m_pEquation->getObjectId());
				m_pEquation = nullptr;
			}
		}
	}
}