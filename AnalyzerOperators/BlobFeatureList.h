//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobFeatureList.h
/// This is the class for the Blob Analyzer 2 - Feature List
//******************************************************************************


#pragma once
#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "InspectionEngine/SVTaskObjectList.h"
#include "SVValueObjectLibrary/DoubleSortValueObject.h"
#include "BlobFeatureTask.h"
#pragma endregion Includes


namespace SvOp
{
	class SVLongResult;
	class TableObject;
}

namespace SvAo
{
	class BlobFeatureList : public SvIe::SVTaskObjectListClass
	{
		SV_DECLARE_CLASS

	public:
		BlobFeatureList(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_BLOBFEATURELIST);

		virtual ~BlobFeatureList();

		SvPb::InspectionCmdResponse getFeaturesData() const;
		SvPb::InspectionCmdResponse setFeatures(SvPb::SetFeaturesRequest request, bool isGrayValid);

		virtual bool CloseObject() override;

		virtual bool onRun(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

		SvDef::StringVector getAnalyzerResult(int blobIndex) const;

		void updateTableDefinition(long maxArraySize, std::array<SvVol::DoubleSortValuePtr, 4>& resultColumnForOverlayArray, std::set<MIL_ID>& rRequiredFeatureGroup);
		bool updateBlobFeatures(SvStl::MessageContainerVector* pErrorMessages, std::back_insert_iterator<std::vector<BlobExcludeData>> excludeDataVecInserter);

		void setSortControls(MIL_ID blobContextId) const;

		void setNumberOfBlobsObject(SvVol::SVLongValueObjectClass* pNumberOfBlobsObject) { m_pNumberOfBlobsObject = pNumberOfBlobsObject; };
		void setResultTable(SvOp::TableObject* pResultTable) { m_pResultTable = pResultTable; };
		void setResultBufferId(MIL_ID resultBufferID) { m_ResultBufferID = resultBufferID; };

	protected:
		virtual bool Run(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

	private:
		void init();
		void fillFeatureTaskVec();
		void addedMissingNeededFeatureTask();

		void moveFeatureTasks(size_t posFrom, size_t posTo);

		bool rangeCheck(SvStl::MessageContainerVector* pErrorMessages, const BlobFeatureTask* pFeatureTask, const SvVol::LinkedValue& rHigherValue, const SvVol::LinkedValue& rLowerValue, SvStl::MessageTextEnum higherId, SvStl::MessageTextEnum lowerId);

		MIL_ID			m_ResultBufferID = M_NULL;

		SvVol::SVLongValueObjectClass* m_pNumberOfBlobsObject = nullptr;
		SvOp::TableObject* m_pResultTable = nullptr;
		std::vector<BlobFeatureTask*> m_featureTaskVec;
		std::vector<BlobFeatureTask*> m_customFeatureTaskExcludeVec;
	};

} //namespace SvAo
