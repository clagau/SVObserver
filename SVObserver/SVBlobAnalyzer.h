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

enum { SV_MAX_NUMBER_OF_BLOBS = 100 };

enum SVBlobFeatureEnum
{
	SV_AREA = 0,            // 0
	SV_BOXX_MAX,            // 1
	SV_BOXX_MIN,            // 2
	SV_BOXY_MAX,            // 3
	SV_BOXY_MIN,            // 4
	SV_BREADTH,             // 5
	SV_CENTEROFGRAVITY_X,   // 6
	SV_CENTEROFGRAVITY_Y,   // 7
	SV_CONVEX_PERIMETER,    // 8
	SV_FERET_ELONGATION,    // 9
	SV_FERETMAX_ANGLE,      // 10
	SV_FERETMAX_DIAMETER,   // 11
	SV_FERETMEAN_DIAMETER,  // 12
	SV_FERETMIN_ANGLE,      // 13
	SV_FERETMIN_DIAMETER,   // 14
	SV_FERET_X,             // 15
	SV_FERET_Y,             // 16
	SV_FIRSTPOINT_X,        // 17
	SV_FIRSTPOINT_Y,        // 18
	SV_LABEL,               // 19
	SV_LENGTH,              // 20
	SV_NBROF_HOLES,         // 21
	SV_PERIMETER,           // 22
	SV_ROUGHNESS,           // 23
//- SV_SUM_PIXEL - This setting does not work on binary images, and is
//- therefore being removed from available features at this time.  The 
//- selection must still be available in the list in order to support
//- previous configurations, but will not appear in the list of available
//- features.  This is being enforced within 
//- SVBlobAnalyzeFeatureListBoxClass::Init ().
	SV_SUM_PIXEL,           // 24  // Will not work with thresholded images i.e.
	                               // binary images vs. grey scale images.
	//
	// v3.1 Added Blob Features
	//
	SV_COMPACTNESS,         // 25
	SV_NBR_RUNS,            // 26
	SV_XMINAT_YMIN,         // 27
	SV_XMAXAT_YMAX,         // 28
	SV_YMINAT_XMAX,         // 29
	SV_YMAXAT_XMIN,         // 30
	SV_ELONGATION,          // 31
	SV_INTERCEPT_0,         // 32
	SV_INTERCEPT_45,        // 33
	SV_INTERCEPT_90,        // 34
	SV_INTERCEPT_135,       // 35
	SV_MOMENT_X0Y1,         // 36
	SV_MOMENT_X1Y0,         // 37
	SV_MOMENT_X1Y1,         // 38
	SV_MOMENT_X0Y2,         // 39
	SV_MOMENT_X2Y0,         // 40
	SV_CENTRAL_X0Y2,        // 41
	SV_CENTRAL_X2Y0,        // 42
	SV_CENTRAL_X1Y1,        // 43
	SV_AXISPRINCIPAL_ANGLE, // 44
	SV_AXISSECONDARY_ANGLE, // 45
	SV_EULER_NBR,           // 46
	SV_CENTER_X_SOURCE,     // 47
	SV_CENTER_Y_SOURCE,		// 48

	SV_TOPOF_LIST
};

enum { SV_NUMBER_OF_BLOB_FEATURES  = SV_TOPOF_LIST };

/////////////////////////////////////////////////////////////////////////////
//
// Define a record for the constant attributes of a SVBlobFeature record.
//
struct SVBlobFeatureConstants
{
	SVBlobSelectionEnum  MILFeatureDef;
	GUID*   pEmbeddedID;
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

#pragma region IEnumerateValueObject
	virtual SvOi::NameValueList getFeatureList(bool isSelected) const override;
#pragma endregion IEnumerateValueObject

	virtual bool CloseObject() override;

	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	DWORD BuildFeatureListID ();
	DWORD EnableFeature (SVBlobFeatureEnum aIndex);
	DWORD DisableFeature (SVBlobFeatureEnum aIndex);
	DWORD AllocateBlobResult ();

/*- SortBlobs () --------------------------------------------------------------*/
/*- This will create a sorted mapping of the items in the msvDblValueArray.    */
/*- The sort will be by the feature indicated in asvlSortFeature and the map   */
/*- will be returned as asvlSortMap. No array data will be moved. If we choose */
/*- to permit sorting on multiple features, it will just mean tracking         */
/*- multiple sort maps. -------------------------------------------------------*/
	void SortBlobs (long asvlSortFeature, long* alSortMap, long p_lArraySize );

	DWORD AllocateResult (SVBlobFeatureEnum aFeatureIndex);
	DWORD FreeResult (SVBlobFeatureEnum aFeatureIndex);
	void RebuildResultObjectArray();

/*- GetResultObject () --------------------------------------------------------*/
/*- Each enabled feature is associated with its own result. This function      */
/*- will hand back the associated with the feature which is referenced by      */ 
/*- aFeatureIndex. ------------------------------------------------------------*/
	SvOi::IObjectClass* GetResultObject(SVBlobFeatureEnum aFeatureIndex);
	SVLongResultClass* GetBlobResultObject();

	virtual bool onRun( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	virtual bool IsPtOverResult( const POINT& rPoint ) override;
	virtual	void DisplayAnalyzerResult() override;

	SVLongResultClass*           m_pResultBlob;

/*- ENABLED FEATURES ----------------------------------------------------------*/
/*- Since the std::string member of the SVStringValueObject can not be directly    */
/*- accessed, the msvszFeaturesEnabled string acts at a workspace for          */
/*- manipulating the enabled features configuration.                           */
/*- msvPersistantFeaturesEnabled is required to preserve the values during     */
/*- scripting. ----------------------------------------------------------------*/
	SVStringValueObjectClass     m_PersistantFeaturesEnabled;
	TCHAR                        m_FeaturesEnabled [SV_NUMBER_OF_BLOB_FEATURES + 1]; // Null Terminated
/*- End of ENABLED FEATURES. --------------------------------------------------*/

	SVDoubleValueObjectClass     m_Value [SV_NUMBER_OF_BLOB_FEATURES];
	GUID                         m_guidResults[ SV_NUMBER_OF_BLOB_FEATURES ];

/*- msvPersistantNbrOfBlobs ---------------------------------------------------*/
/*- This value is for persistant storage only.  The "working" counterpart is   */
/*- msvlNbrOfBlobs. -----------------------------------------------------------*/
	SVLongValueObjectClass       m_lvoBlobSampleSize;//msvPersistantNbrOfBlobs;
	SVLongValueObjectClass       m_lvoMaxBlobDataArraySize;

	long                         m_lBlobSampleSize;//msvlNbrOfBlobs;
	long                         m_lMaxBlobDataArraySize;

/* msvSortFeature -------------------------------------------------------------*/
/* MIL value from the msvlMILFeatureDef array. This value determins which      */
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

	DWORD                        m_DefaultAttributes; 

	SVMatroxIdentifier			m_BlobContextID = M_NULL;
	SVMatroxIdentifier			m_ResultBufferID = M_NULL;

	SVEnumerateValueObjectClass  m_colorBlobEnumValue;

	//new value objects for Fill Blob option
	SVBoolValueObjectClass       m_bvoFillBlobs;
	SVEnumerateValueObjectClass  m_evoBlobFillColor;
	SVEnumerateValueObjectClass  m_evoBlobType;

	TableObject* m_pResultTable;
	DoubleSortValueObject* m_ResultTableColumnValueObjects[SV_NUMBER_OF_BLOB_FEATURES];

protected:
	virtual HRESULT onCollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructVector& p_rMultiLineArray ) override;

/*- MapQuickSort () ----------------------------------------------------------*/
/*- This should not be called directly, but through SortBlobs () -------------*/

	void MapQuickSort (double* aSortArray, long* alSortMap, long alBeginning, long alEnd, bool abAscending);


	virtual void addDefaultInputObjects( SVInputInfoListClass* PInputListToFill = nullptr ) override;

private:
	void init();
	void CreateArray();

	static const int m_defaultResultNumberOfBlobsLowFail = 0;
	static const int m_defaultResultNumberOfBlobsLowWarn = 0;
	static const int m_defaultResultNumberOfBlobsHighFail = 9999;
	static const int m_defaultResultNumberOfBlobsHighWarn = 9999;
};

