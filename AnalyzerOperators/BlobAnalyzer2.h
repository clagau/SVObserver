//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobAnalyzer2.h
/// This is the class for the Blob Analyzer 2
//******************************************************************************


#pragma once
#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "BlobFeatureTask.h"
#include "SVImageAnalyzerClass.h"
#include "ObjectInterfaces/IBlobAnalyzer2.h"
#include "SVValueObjectLibrary/DoubleSortValueObject.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVByteValueObjectClass.h"
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
enum SV_BLOB_FILL_TYPE_ENUM
{
	SV_BLOB_FILL_ALL       = 0x8000,
	SV_BLOB_FILL_INCLUDED  = 0x2000,
	SV_BLOB_FILL_EXCLUDED  = 0x1000
};

// Add String for SVEnumerateValueObjectClass
const LPCSTR g_strBlobFillTypeEnums = _T( "All=32768,Included=8192,Excluded=4096" );

/////////////////////////////////////////////////////////////////////////////
//
//
class BlobAnalyzer2 : public SVImageAnalyzerClass, public SvOi::IBlobAnalyzer2
{
	SV_DECLARE_CLASS(BlobAnalyzer2);

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

	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	/// When the user right clicks inside a tool figure, checks whether it is on a blob. 
	/// If it finds a blob at that point, gets the index of the blob and returns true.
	///	Called from SVImageView::OnContextMenu
	virtual bool IsPtOverResult( const POINT& rPoint ) override;
	virtual	SvDef::StringVector getAnalyzerResult() override;

protected:
	virtual HRESULT onCollectOverlays(SvIe::SVImageClass* pImage, SVExtentMultiLineStructVector& rMultiLineArray ) override;
	virtual void addOverlayGroups(const SvIe::SVImageClass* pImage, SvPb::Overlay& rOverlay) const override;

private:
	void init();
	bool updateBlobFeatures(SvStl::MessageContainerVector* pErrorMessages = nullptr);
	void fillFeatureTaskVec();
	void addedMissingNeededFeatureTask();
	std::set<MIL_ID> updateTableDefinition();
	void allocateBlobNumberResult();

	/// Check if range failed and return the state.
	/// \returns std::pair<bool, bool> First value is if range failed and second if range warned.
	std::pair<bool, bool> checkRange();


	SvVol::SVLongValueObjectClass       m_maxBlobDataArraySize;
	SvVol::SVLongValueObjectClass       m_numberOfBlobsFound;
	SvOp::SVLongResult* m_pResultBlob;

	int                          m_nBlobIndex;


	MIL_ID			m_BlobContextID = M_NULL;
	MIL_ID			m_ResultBufferID = M_NULL;

	SvVol::SVBoolValueObjectClass       m_isGrayImageValue;
	SvOl::SVInObjectInfoStruct m_grayImageInfo;
	SvVol::SVEnumerateValueObjectClass  m_colorBlobEnumValue;
	SvVol::SVEnumerateValueObjectClass  m_connectivityEnumValue;

	//value objects for Fill Blob option
	SvVol::SVBoolValueObjectClass       m_isFillBlobs;
	SvVol::SVByteValueObjectClass  m_blobFillColor;
	SvVol::SVEnumerateValueObjectClass  m_evoBlobType;

	SvOp::TableObject* m_pResultTable;
	std::vector<std::pair<MIL_ID /*FeatureType*/, SvVol::DoubleSortValuePtr>> m_ResultTableColumnVector;
	std::array<SvVol::DoubleSortValuePtr, 4> m_ResultColumnForOverlayArray{ nullptr };
	std::vector<BlobFeatureTask*> m_featureTaskVec;
	std::vector<BlobExcludeData> m_excludeDataVec;
	std::vector<std::pair<SvVol::DoubleSortValuePtr, BlobRangeData>> m_rangeDataVec;
};

} //namespace SvAo
