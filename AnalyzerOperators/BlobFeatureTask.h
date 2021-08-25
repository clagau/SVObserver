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
#include "SVValueObjectLibrary/DoubleSortValueObject.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#pragma endregion Includes

#pragma region Declarations
namespace SvOp
{
	class IndexEquation;
}
#pragma endregion Declarations

namespace SvAo
{
	struct BlobExcludeData
	{
		BlobExcludeData(const std::string& name, const SvVol::LinkedValue& rLowerBoundValue, const SvVol::LinkedValue& rUpperBoundValue, MIL_ID featureType = M_NULL, bool isEnabled = false, bool isInner = true)
			: m_name (name)
			, m_rLowerBoundValue(rLowerBoundValue)
			, m_rUpperBoundValue(rUpperBoundValue)
			, m_featureType(featureType)
			, m_isEnabled(isEnabled)
			, m_isInner(isInner) {};

		std::string m_name;
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
		const std::array<SvVol::LinkedValue, RangeEnum::ER_COUNT>& m_rRangeValues;
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
		SV_DECLARE_CLASS

#pragma region Constructor
	public:
		BlobFeatureTask(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_BLOB_FEATURE_TASK);
		virtual ~BlobFeatureTask() = default;
		 
#pragma endregion Constructor

#pragma region Public Methods
	public:
		virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;

		void evalRange(SvIe::RunStatus& rRunStatus);

		/// Add the position of the blobs which should be excluded. Check only for custom feature because Mil-feature is done already by MIL.
		/// \param rSortContainer [in,out]
		/// \returns void
		void addExcludeBlobs(std::set<int>& rExcludeSet);

		long getFeatureType() const;
		bool isCustomFeature() const;
		uint32_t getEquationId() const;
		void setFeatureType(long type, bool isCustomFeature);

		/// Set the feature data to the task.
		/// \param rData [in] The data in a protobuf message
		/// \returns int The return value is the data-id of the first error. If the return value 0 the set was successfully. 
		int setFeatureData(const SvPb::FeatureData& rData); 

		BlobExcludeData getExcludeData() const;
		BlobRangeData getRangeData() const;

		SortEnum getSortEnum() const;

		void fillValues(MIL_ID resultBufferId);

		void setValueObject(SvVol::DoubleSortValuePtr pValue);
		SvVol::DoubleSortValuePtr getValueObject() const { return m_pValue; };
#pragma endregion Public Methods

#pragma region Protected Methods
	protected:
#pragma endregion Protected Methods

#pragma region Private Methods
	private:
		void updateEquation();
#pragma endregion Private Methods

#pragma region Member Variables
	private:
		SvVol::SVLongValueObjectClass  m_featureTypeValue;
		SvVol::SVBoolValueObjectClass m_isCustomFeature;
		SvVol::SVEnumerateValueObjectClass  m_sortEnumValue;
		SvVol::SVBoolValueObjectClass m_isExcludeValue;
		SvVol::SVBoolValueObjectClass m_isInnerExcludeValue;
		SvVol::LinkedValue m_ExcludeLowerBoundValue;
		SvVol::LinkedValue m_ExcludeUpperBoundValue;
		SvVol::SVBoolValueObjectClass m_isRangeValue;
		std::array<SvVol::LinkedValue, RangeEnum::ER_COUNT> m_RangeValues;
		SvVol::DoubleSortValuePtr m_pValue;
		SvOp::IndexEquation* m_pEquation = nullptr;
#pragma endregion Member Variables
	};
}