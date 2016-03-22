// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBlobInterface
// * .File Name       : $Workfile:   SVMatroxBlobInterface.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   12 Nov 2014 08:24:36  $
// ******************************************************************************

#pragma once

//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <comdef.h>
#include "SVMatroxEnums.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxBlobFeatureList.h"
#include "SVMatroxBlobResult.h"
#include "SVMatroxApplicationInterface.h"

class SVCommandDataHolder;


/**
@SVObjectName Matrox Blob Interface

@SVObjectOverview This class contains interface functions for using the Matrox blob analyzer.

@SVObjectOperations The create function creates a blob feature list. The only parameter is a SVMatroxBlobFeatureList&.
The CreateResult function creates a blob result object. The only parameter is a SVMatroxBlobResult&.
There are two Destroy functions The destroy( SVMatroxBlobResult& )frees the handle from the createResult function. The second Destroy( SVMatroxBlobFeatureList& ) destroys the handle from the create function.
These Set functions select the feature(s) to be calculated by MblobCalculate() when using the specified feature list. These functions include double&, long& and SVMatroxString& types.  Each of these functions take a SVMatroxBlobFeatureList reference, a SVBlobEnum and one of the three value references.
The Get functions include double&, long&, and SVMatroxString& types. Each of these functions take a SVMatroxBlobFeatureList reference, a SVBlobEnum and a value reference;
The GetResult functions retrieve the results for a specified feature for all included blobs, from the blob analysis result buffer. The GetResult functions include double&, long&, and SVMatroxString& types. Each of these functions take a SVMatroxBlobResult& reference, a SVBlobEnum and a value reference;
The BlobFill function draws, in an image, those blobs that meet a specified fill criterion with the specified fill value. It takes the following parameters: const SVMatroxBlobResult&, const SVMatroxBuffer&, SVBlobEnum, const long& color.
The Select function selects blobs that meet a specified criterion. These blobs will be included in or excluded from future operations (calculations or result retrieval), or deleted entirely from the result buffer. Selection criterion can be based on a calculated feature or on the current status of the blobs. Execute() must have been called at least once to call this function. The parameters are const the SVMatroxBlobResult object, a SVBlobEnum, and two double range values.
The BlobSelectFeature function selects the feature(s) to be calculated by Execute function when using the specified feature list.  It takes the following parameters: const SVMatroxBlobResult& p_rResult, SVBlobEnum p_eOperation, const double& p_dCondLow, const double& p_dCondHigh.
*/
class SVMatroxBlobInterface
{
public:
	typedef SVMatroxApplicationInterface::SVStatusCode SVStatusCode;

	struct SVBlob
	{
		POINT centerOfGravity;
		RECT boundingRect;
		POINT center;
		long area;

		SVBlob() 
		{
			centerOfGravity.x = 0;
			centerOfGravity.y = 0;
			area = 0;
			boundingRect.left = 0;
			boundingRect.right = 0;
			boundingRect.top = 0;
			boundingRect.bottom = 0;
			center.x = 0;
			center.y = 0;
		}
	};

	typedef std::deque<SVBlob> SVBlobList;
	typedef std::deque<POINT> SVBlobOffsetList;

	SVMatroxBlobInterface();

	virtual ~SVMatroxBlobInterface();

	// This function replaces MblobAllocFeatureList
	static SVStatusCode Create( SVMatroxBlobFeatureList& p_rFeatureListId );
	// This function replaces MblobAllocResult
	static SVStatusCode Create( SVMatroxBlobResult& p_rResultId );

	// This function replaces MblobFree
	static SVStatusCode Destroy( SVMatroxBlobFeatureList& p_rResultId );
	static SVStatusCode Destroy( SVMatroxBlobResult& p_rResultId );


	// This function replaces MblobCalculate( 
	static SVStatusCode Execute( const SVMatroxBlobResult& p_rResult, const SVMatroxBuffer& p_rIdentId,  const SVMatroxBlobFeatureList& p_rFeatureListId );

	static SVStatusCode Get( const SVMatroxBlobResult& p_rResultId, SVBlobControlEnum p_lControlType, long& p_lControlValue) ;
	static SVStatusCode Get( const SVMatroxBlobResult& p_rResultId, SVBlobControlEnum p_lControlType, double& p_lControlValue) ;
	static SVStatusCode GetNumber( const SVMatroxBlobResult& p_rResultId, long& p_rNumber ) ;
	static SVStatusCode GetResult( const SVMatroxBlobResult& p_rResultId, SVBlobSelectionEnum p_lControlType, double* p_pdResultsArray ) ;
	static SVStatusCode GetResult( const SVMatroxBlobResult& p_rResultId, SVBlobSelectionEnum p_lControlType, long* p_plResultsArray );


	// These functions replace MblobControl
	static SVStatusCode Set( const SVMatroxBlobResult& p_rResultId, SVBlobControlEnum p_lControlType, const long p_lControlValue);
	static SVStatusCode Set( const SVMatroxBlobResult& p_rResultId, SVBlobControlEnum p_lControlType, const double p_lControlValue);

	//
	static SVStatusCode GetForeground( const SVMatroxBlobResult& p_rResultId, bool& rIsDarkForeground);
	static SVStatusCode SetForeground( const SVMatroxBlobResult& p_rResultId, bool isDarkForeground);
	static SVStatusCode GetIdentifier( const SVMatroxBlobResult& p_rResultId, bool& p_bBinary);
	static SVStatusCode SetIdentifier( const SVMatroxBlobResult& p_rResultId, bool p_bBinary);

	// This function replaces MblobFill
	static SVStatusCode BlobFill( const SVMatroxBlobResult& p_rResult, const SVMatroxBuffer& p_rImageID, SVBlobControlEnum p_eCriterion, const long& p_lColor);
	// This function replaces MblobSelect
	static SVStatusCode BlobSelect( const SVMatroxBlobResult& p_rResult, SVBlobOperationEnum p_eOperation, SVBlobSelectionEnum p_eSelection, SVConditionEnum p_eCondition, const double& p_dCondLow, const double& p_dCondHigh);
	// This function replaces MblobSelectFeature
	static SVStatusCode BlobSelectFeature( const SVMatroxBlobFeatureList& p_rFeatureList, SVBlobSelectionEnum p_eFeature);

	static bool Compare(int criteria, const SVBlob& lBlob, const SVBlob& rBlob);

	static HRESULT FindSizedBlobs( SVMatroxBuffer& p_rThresholdImage, bool p_LightBackground, SVBlobList& rList ); 
	static HRESULT NumBlobs( SVMatroxBuffer& p_rThresholdImage, BYTE p_Background, long& p_rBlobCount );

	static HRESULT FillResults( const SVBlobOffsetList& p_rBlobList, SVCommandDataHolder& p_rResults );

	static long CalculateOptimalWidth(const SVBlobList& rBlobList, unsigned short padding);
	static long CalculateOptimalHeight(const SVBlobList& rBlobList, unsigned short padding);

	static HRESULT CreateBlobOffsetResultList(const SVBlobList& results, SVBlobOffsetList& offsetList, const SIZE& size, const POINT& offset, const SIZE& imageSize);
	
private:
	enum SVImageBackgroundEnum
	{
		DARK  = 0,
		LIGHT = 1,
	};

	typedef std::vector<long> SVBlobResultCollection;

	static long Convert2MatroxControlType(SVBlobSelectionEnum p_lControlType) ;
	static long Convert2MatroxControlType(SVBlobControlEnum p_lControlType) ;
	static long Convert2MatroxControlType(SVBlobOperationEnum p_lControlType) ;
	static long Convert2MatroxControlType(SVConditionEnum p_lControlType) ;

	enum SortType
	{
		SortBy_XY		= 1,
		SortBy_YX		= 2,
		SortBy_X		= 4,
		SortBy_Y		= 8,
		SortBy_Area		= 16,
		SortAscending	= 128
	};

	static void Sort(SVBlobList& list, int sortCriteria);

	static bool IsValidSize(int width, int height);
	static bool IsValidDimension(int value);
	static bool IsValidCandidate(const SVBlob& rBlob, const SIZE& size, const POINT& offset);

};

