//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobAnalyzer
//* .File Name       : $Workfile:   SVBlobAnalyzer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   18 Nov 2014 05:07:48  $
//******************************************************************************

#pragma once
#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVImageAnalyzerClass.h"
#include "SVBlobAnalyzerResultDlg.h"
#include "TableObject.h"
#include "ObjectInterfaces/IBlobAnalyzer.h"
#include "SVValueObjectLibrary/DoubleSortValueObject.h"
#include "SVLibrary/SVTemplate.h"
#pragma endregion Includes

class SVLongResultClass;

/////////////////////////////////////////////////////////////////////////////
//
// Define a record for the constant attributes of a SVBlobFeature record.
//
struct SVBlobFeatureConstant
{
	SVBlobSelectionEnum  MILFeatureDef;
	GUID&   rEmbeddedID;
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
class SVBlobAnalyzerClass : public SVImageAnalyzerClass, public SvOi::IBlobAnalyzer
{
	SV_DECLARE_CLASS( SVBlobAnalyzerClass );

	friend class SVSetupDialogManager;

public:
	SVBlobAnalyzerClass(SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVBLOBANALYZER);

	virtual ~SVBlobAnalyzerClass();

#pragma region IBlobAnalyzer
	virtual SvOi::NameValueVector getFeatureList(bool isSelected) const override;
	virtual SvOi::IObjectClass* getResultObject(int FeatureIndex) override;
	virtual SvOi::IObjectClass* getResultBlob() override;
#pragma endregion IBlobAnalyzer

	virtual bool CloseObject() override;

	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

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
	SVLongResultClass* GetBlobResultObject();

	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	virtual bool IsPtOverResult( const POINT& rPoint ) override;
	virtual	void DisplayAnalyzerResult() override;

	SVLongResultClass*           m_pResultBlob;
	SVStringValueObjectClass     m_PersistantFeaturesEnabled;

	SVDoubleValueObjectClass     m_Value[SvOi::SV_NUMBER_OF_BLOB_FEATURES];
	GUID                         m_guidResults[SvOi::SV_NUMBER_OF_BLOB_FEATURES];

/*- msvPersistantNbrOfBlobs ---------------------------------------------------*/
/*- This value is for persistant storage only.  The "working" counterpart is   */
/*- msvlNbrOfBlobs. -----------------------------------------------------------*/
	SVLongValueObjectClass       m_lvoBlobSampleSize;//msvPersistantNbrOfBlobs;
	SVLongValueObjectClass       m_lvoMaxBlobDataArraySize;

	long                         m_lBlobSampleSize;//msvlNbrOfBlobs;
	long                         m_lMaxBlobDataArraySize;

/* msvSortFeature -------------------------------------------------------------*/
/* MIL value from the msvlMILFeatureDef array. This value determines which      */
/* feature should be sorted on. -----------------------------------------------*/
	SVLongValueObjectClass       m_SortFeature;
	SVBoolValueObjectClass       m_SortAscending;
// Exclude the Failed blobs from the result list.
	SVBoolValueObjectClass       m_bExcludeFailed;

	SVLongValueObjectClass       m_lvoNumberOfBlobsFound;
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

	SVMatroxIdentifier			m_BlobContextID = M_NULL;
	SVMatroxIdentifier			m_ResultBufferID = M_NULL;

	SVEnumerateValueObjectClass  m_colorBlobEnumValue;

	//new value objects for Fill Blob option
	SVBoolValueObjectClass       m_bvoFillBlobs;
	SVEnumerateValueObjectClass  m_evoBlobFillColor;
	SVEnumerateValueObjectClass  m_evoBlobType;

	TableObject* m_pResultTable;
	DoubleSortValuePtr m_ResultTableColumnValueObjects[SvOi::SV_NUMBER_OF_BLOB_FEATURES];

protected:
	virtual HRESULT onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructVector& p_rMultiLineArray ) override;

/*- MapQuickSort () ----------------------------------------------------------*/
/*- This should not be called directly, but through SortBlobs () -------------*/

	void MapQuickSort (double* aSortArray, long* alSortMap, long alBeginning, long alEnd, bool abAscending);


	virtual void addDefaultInputObjects( SvOl::SVInputInfoListClass* PInputListToFill = nullptr ) override;

private:
	void init();
	void CreateArray();
	
	std::string m_PreviousFeaturesEnabled
		;
	static const int m_defaultResultNumberOfBlobsLowFail = 0;
	static const int m_defaultResultNumberOfBlobsLowWarn = 0;
	static const int m_defaultResultNumberOfBlobsHighFail = 9999;
	static const int m_defaultResultNumberOfBlobsHighWarn = 9999;
};

