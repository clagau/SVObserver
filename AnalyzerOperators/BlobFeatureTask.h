//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobFeatureTask.h
/// Class for the parameter a feature of the blob analyzer.
//******************************************************************************
#pragma once

#pragma region Includes
#include "InspectionEngine/SVTaskObjectList.h"
#include "Definitions/RangeEnum.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations
namespace SvAo
{
	struct BlobExcludeData
	{
		BlobExcludeData(const SvVol::LinkedValue& rLowerBoundValue, const SvVol::LinkedValue& rUpperBoundValue, MIL_ID featureType = M_NULL, bool isEnabled = false, bool isInner = true)
			: m_rLowerBoundValue(rLowerBoundValue)
			, m_rUpperBoundValue(rUpperBoundValue)
			, m_featureType(featureType)
			, m_isEnabled(isEnabled)
			, m_isInner(isInner) {};

		MIL_ID m_featureType = M_NULL;
		bool m_isEnabled = false;
		bool m_isInner = true;
		const SvVol::LinkedValue& m_rLowerBoundValue;
		const SvVol::LinkedValue& m_rUpperBoundValue;
	};

	struct BlobRangeData
	{
		MIL_ID m_featureType = M_NULL;
		bool m_isEnabled = false;
		double m_failHigh = 9999.;
		double m_warnHigh = 9999.;
		double m_warnLow = 0.;
		double m_failLow = 0.;
		std::string m_failHighIndirect = "";
		std::string m_warnHighIndirect = "";
		std::string m_warnLowIndirect = "";
		std::string m_failLowIndirect = "";
	};

	enum class SortEnum
	{
		None = 0,
		Ascent = 1,
		Descent = 2,
	};

	class BlobFeatureTask : public SvIe::SVTaskObjectListClass
		/// \brief Class for the parameter a feature of the blob analyzer.
	{
		SV_DECLARE_CLASS(BlobFeatureTask);

#pragma region Constructor
	public:
		BlobFeatureTask(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_BLOB_FEATURE_TASK);
		virtual ~BlobFeatureTask() = default;
		 
#pragma endregion Constructor

#pragma region Public Methods
	public:
		long getFeatureType() const;
		void setFeatureType(long type);

		/// Set the feature data to the task.
		/// \param rData [in] The data in a protobuf message
		/// \returns int The return value is the data-id of the first error. If the return value 0 the set was successfully. 
		int setFeatureData(const SvPb::FeatureData& rData); 

		BlobExcludeData getExcludeData() const;
		BlobRangeData getRangeData() const;

		SortEnum getSortEnum() const;
#pragma endregion Public Methods

#pragma region Protected Methods
	protected:
#pragma endregion Protected Methods

#pragma region Private Methods
	private:

#pragma endregion Private Methods

#pragma region Member Variables
	private:
		SvVol::SVLongValueObjectClass  m_featureTypeValue;
		SvVol::SVEnumerateValueObjectClass  m_sortEnumValue;
		SvVol::SVBoolValueObjectClass m_isExcludeValue;
		SvVol::SVBoolValueObjectClass m_isInnerExcludeValue;
		SvVol::LinkedValue m_ExcludeLowerBoundValue;
		SvVol::LinkedValue m_ExcludeUpperBoundValue;
		SvVol::SVBoolValueObjectClass m_isRangeValue;
		std::array<SvVol::LinkedValue, RangeEnum::ER_COUNT> m_RangeValues;
#pragma endregion Member Variables
	};
}