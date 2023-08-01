//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BlobAnalyzer
//* .File Name       : $Workfile:   BlobAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   18 Nov 2014 05:07:48  $
//******************************************************************************

#pragma once
#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "Definitions/SVMatroxSimpleEnums.h"
#include "ImageAnalyzer.h"
#include "ObjectInterfaces/IBlobAnalyzer.h"
#include "SVValueObjectLibrary/DoubleSortValueObject.h"
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#include "SVValueObjectLibrary/SVLongValueObjectClass.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#include "SVLibrary/SVTemplate.h"
#pragma endregion Includes

namespace SvOp
{
class SVLongResult;
class TableObject;
}

namespace SvAo
{

/////////////////////////////////////////////////////////////////////////////
//
// Define a record for the constant attributes of a SVBlobFeature record.
//
struct SVBlobFeatureConstant
{
	SVBlobSelectionEnum  MILFeatureDef;
	SvPb::EmbeddedIdEnum   embeddedID;
	int     NewStringResourceID;
};

enum SV_BLOB_COLOR_ENUM
{
	SV_BLOB_BLACK    = 0,
	SV_BLOB_WHITE    = 1	
};// end SV_BLOB_COLOR_ENUM

// Add String for SVEnumerateValueObjectClass
const LPCSTR g_strBlobColorEnums = 
	_T( "Black=0,White=1" );

enum SV_BLOB_FILL_COLOR_ENUM
{
	SV_BLOB_FILL_BLACK    = 0x0000,	// 0000 0000 0000 0000
	SV_BLOB_FILL_WHITE    = 0x00FF	// 0000 0000 1111 1111
};// end SV_BLOB_FILL_COLOR_ENUM

// Add String for SVEnumerateValueObjectClass
const LPCSTR g_strBlobFillColorEnums = 
                _T( "Black=0,White=255" );

enum SV_BLOB_FILL_TYPE_ENUM
{
	SV_BLOB_FILL_ALL       = 0x8000,
	SV_BLOB_FILL_INCLUDED  = 0x2000,
	SV_BLOB_FILL_EXCLUDED  = 0x1000
};

// Add String for SVEnumerateValueObjectClass
const LPCSTR g_strBlobFillTypeEnums = 
                _T( "All=32768,Included=8192,Excluded=4096" );

/////////////////////////////////////////////////////////////////////////////
//
//
class BlobAnalyzer : public ImageAnalyzer, public SvOi::IBlobAnalyzer
{
	SV_DECLARE_CLASS

	friend class SVSetupDialogManager;

public:
	BlobAnalyzer(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVBLOBANALYZER);

	virtual ~BlobAnalyzer();

#pragma region IBlobAnalyzer
	virtual SvDef::NameValueVector getFeatureList(bool isSelected) const override;
	virtual SvOi::IObjectClass* getResultObject(int FeatureIndex) override;
	virtual SvOi::IObjectClass* getResultBlob() override;
#pragma endregion IBlobAnalyzer

	virtual bool CloseObject() override;

	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual bool ResetObject(SvStl::MessageContainerVector* pErrorMessages = nullptr) override;

	DWORD BuildFeatureListID ();
	void UpdateBlobFeatures();
	void EnableFeature(int FeatureIndex);
	DWORD AllocateBlobResult();

/*- SortBlobs () --------------------------------------------------------------*/
/*- This will create a sorted mapping of the items in the msvDblValueArray.    */
/*- The sort will be by the feature indicated in asvlSortFeature and the map   */
/*- will be returned as asvlSortMap. No array data will be moved. If we choose */
/*- to permit sorting on multiple features, it will just mean tracking         */
/*- multiple sort maps. -------------------------------------------------------*/
	void SortBlobs (long asvlSortFeature, long* alSortMap, long p_lArraySize );

	DWORD AllocateResult(int FeatureIndex);
	void FreeResult(int FeatureIndex);
	void RebuildResultObjectArray();

/*- GetResultObject () --------------------------------------------------------*/
/*- Each enabled feature is associated with its own result. This function      */
/*- will hand back the associated with the feature which is referenced by      */ 
/*- aFeatureIndex. ------------------------------------------------------------*/
	SvOp::SVLongResult* GetBlobResultObject();

	virtual bool onRun( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	virtual bool IsPtOverResult( const POINT& rPoint ) override;
	virtual	SvDef::StringVector getAnalyzerResult() override;
	virtual void addParameterForMonitorList(SvStl::MessageContainerVector& rMessages, std::back_insert_iterator<SvOi::ParametersForML> inserter) const override;

	SvOp::SVLongResult*           m_pResultBlob;
	SvVol::SVStringValueObjectClass     m_PersistantFeaturesEnabled;

	SvVol::SVDoubleValueObjectClass     m_Value[SvOi::SV_NUMBER_OF_BLOB_FEATURES];
	uint32_t                         m_ResultIds[SvOi::SV_NUMBER_OF_BLOB_FEATURES];

/*- msvPersistantNbrOfBlobs ---------------------------------------------------*/
/*- This value is for persistant storage only.  The "working" counterpart is   */
/*- msvlNbrOfBlobs. -----------------------------------------------------------*/
	SvVol::SVLongValueObjectClass       m_lvoBlobSampleSize;//msvPersistantNbrOfBlobs;
	SvVol::SVLongValueObjectClass       m_lvoMaxBlobDataArraySize;

	long                         m_lBlobSampleSize;//msvlNbrOfBlobs;
	long                         m_lMaxBlobDataArraySize;

/* msvSortFeature -------------------------------------------------------------*/
/* MIL value from the msvlMILFeatureDef array. This value determines which      */
/* feature should be sorted on. -----------------------------------------------*/
	SvVol::SVLongValueObjectClass       m_SortFeature;
	SvVol::SVBoolValueObjectClass       m_SortAscending;
// Exclude the Failed blobs from the result list.
	SvVol::SVBoolValueObjectClass       m_bExcludeFailed;

	SvVol::SVLongValueObjectClass       m_lvoNumberOfBlobsFound;
	long                         m_lNumberOfBlobsFound;
	long                         m_lNumberOfBlobsToProcess;
	int                          m_nBlobIndex;

	vector2d<double>             m_vec2dBlobResults;

/*- msvlSortMap ---------------------------------------------------------------*/
/*- Set by SortBlobs () during each onRun (), this value keeps the mapping of  */
/*- the blob sort order as determined by SortBlobs ().  All features can then  */
/*- be extracted based on the sort without needing to move large quantities of */
/*- data. ---------------------------------------------------------------------*/
	std::vector<long> m_SortVector;

	__int64			m_BlobContextID = M_NULL;
	__int64			m_ResultBufferID = M_NULL;

	SvVol::SVEnumerateValueObjectClass  m_colorBlobEnumValue;

	//new value objects for Fill Blob option
	SvVol::SVBoolValueObjectClass       m_bvoFillBlobs;
	SvVol::SVEnumerateValueObjectClass  m_evoBlobFillColor;
	SvVol::SVEnumerateValueObjectClass  m_evoBlobType;

	SvOp::TableObject* m_pResultTable;
	SvVol::DoubleSortValuePtr m_ResultTableColumnValueObjects[SvOi::SV_NUMBER_OF_BLOB_FEATURES];

protected:
	virtual void addOverlayGroups(SVExtentMultiLineStructVector& rMultiLineArray, const SVImageExtentClass& rImageExtents) const override;
	virtual void addOverlayGroups(SvPb::Overlay& rOverlay) const override;

/*- MapQuickSort () ----------------------------------------------------------*/
/*- This should not be called directly, but through SortBlobs () -------------*/

	void MapQuickSort (double* aSortArray, long* alSortMap, long alBeginning, long alEnd, bool abAscending);

private:
	void init();
	void CreateArray();
	void fillResultId(int featurePos);
	
	std::string m_PreviousFeaturesEnabled;
	static const int m_defaultResultNumberOfBlobsLowFail = 0;
	static const int m_defaultResultNumberOfBlobsLowWarn = 0;
	static const int m_defaultResultNumberOfBlobsHighFail = 9999;
	static const int m_defaultResultNumberOfBlobsHighWarn = 9999;
};

} //namespace SvAo
