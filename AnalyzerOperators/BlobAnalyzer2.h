//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file BlobAnalyzer2.h
/// This is the class for the Blob Analyzer 2
//******************************************************************************


#pragma once
#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "BlobFeatureTask.h"
#include "ImageAnalyzer.h"
#include "ObjectInterfaces/IBlobAnalyzer2.h"
#include "SVValueObjectLibrary/DoubleSortValueObject.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{
class SVLongResult;
class TableObject;
}

namespace SvAo
{
class BlobFeatureList;
class BlobDrawTask;

/////////////////////////////////////////////////////////////////////////////
//
//
class BlobAnalyzer2 : public ImageAnalyzer, public SvOi::IBlobAnalyzer2
{
	SV_DECLARE_CLASS

public:
	BlobAnalyzer2(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVBLOBANALYZER2);

	virtual ~BlobAnalyzer2();

#pragma region IBlobAnalyzer2
	virtual SvPb::InspectionCmdResponse getFeaturesData() const override;
	virtual SvPb::InspectionCmdResponse setFeatures(SvPb::SetFeaturesRequest request) override;
#pragma endregion IBlobAnalyzer2

	virtual bool CloseObject() override;

	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages = nullptr) override;

	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	/// When the user right clicks inside a tool figure, checks whether it is on a blob. 
	/// If it finds a blob at that point, gets the index of the blob and returns true.
	///	Called from SVImageView::OnContextMenu
	virtual bool IsPtOverResult( const POINT& rPoint ) override;
	virtual	SvDef::StringVector getAnalyzerResult() override;

	const std::set<MIL_ID>& getFeatureGroups() const { return m_featureGroups; };

protected:
	virtual bool Run(SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages = nullptr) override;
	virtual void addOverlayGroups(SVExtentMultiLineStructVector& rMultiLineArray, const SVImageExtentClass& rImageExtents) const override;
	virtual void addOverlayGroups(SvPb::Overlay& rOverlay) const override;

private:
	void init();
	bool updateBlobFeatures(SvStl::MessageContainerVector* pErrorMessages = nullptr);
	void allocateBlobNumberResult();

	SvVol::SVLongValueObjectClass       m_maxBlobDataArraySize;
	SvVol::SVLongValueObjectClass       m_numberOfBlobsFound;
	SvOp::SVLongResult* m_pResultBlob = nullptr;

	int                          m_nBlobIndex;


	MIL_ID			m_BlobContextID = M_NULL;
	MIL_ID			m_ResultBufferID = M_NULL;

	SvVol::SVBoolValueObjectClass       m_isGrayImageValue;
	SvOl::InputObject m_grayImageInput;
	SvVol::SVEnumerateValueObjectClass  m_colorBlobEnumValue;
	SvVol::SVEnumerateValueObjectClass  m_connectivityEnumValue;

	SvOp::TableObject* m_pResultTable;
	std::array<SvVol::DoubleSortValuePtr, 4> m_ResultColumnForOverlayArray{ nullptr };
	std::vector<BlobExcludeData> m_excludeDataVec;
	BlobFeatureList* m_pBlobFeatureList{ nullptr };
	BlobDrawTask* m_pDrawTask{ nullptr };
	std::set<MIL_ID> m_featureGroups;
};

} //namespace SvAo
