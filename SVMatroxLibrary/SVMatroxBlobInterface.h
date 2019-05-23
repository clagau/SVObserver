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

#pragma region Includes
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <comdef.h>
#include "SVMatroxSimpleEnums.h"
#pragma endregion Includes

class SVCommandDataHolder;
class SVMatroxBuffer;


/**
@SVObjectName Matrox Blob Interface

@SVObjectOverview This class contains interface functions for using the Matrox blob analyzer.

@SVObjectOperations The create function creates a blob feature list. The only parameter is a SVMatroxIdentifier&.
The CreateResult function creates a blob result object. The only parameter is a SVMatroxIdentifier&.
There are two Destroy functions The destroy( SVMatroxIdentifier& )frees the handle from the createResult function. The second Destroy( SVMatroxIdentifier& ) destroys the handle from the create function.
These Set functions select the feature(s) to be calculated by MblobCalculate() when using the specified feature list. These functions include double&, long& and SVMatroxString& types.  Each of these functions take a SVMatroxIdentifier reference, a SVBlobEnum and one of the three value references.
The Get functions include double&, long&, and SVMatroxString& types. Each of these functions take a SVMatroxIdentifier reference, a SVBlobEnum and a value reference;
The GetResult functions retrieve the results for a specified feature for all included blobs, from the blob analysis result buffer. The GetResult functions include double&, long&, and SVMatroxString& types. Each of these functions take a SVMatroxIdentifier& reference, a SVBlobEnum and a value reference;
The BlobFill function draws, in an image, those blobs that meet a specified fill criterion with the specified fill value. It takes the following parameters: const SVMatroxIdentifier&, const SVMatroxBuffer&, SVBlobEnum, const long& color.
The Select function selects blobs that meet a specified criterion. These blobs will be included in or excluded from future operations (calculations or result retrieval), or deleted entirely from the result buffer. Selection criterion can be based on a calculated feature or on the current status of the blobs. Execute() must have been called at least once to call this function. The parameters are const the SVMatroxIdentifier object, a SVBlobEnum, and two double range values.
The BlobSelectFeature function selects the feature(s) to be calculated by Execute function when using the specified feature list.  It takes the following parameters: const SVMatroxIdentifier& rResult, SVBlobEnum eOperation, const double& dCondLow, const double& dCondHigh.
*/
class SVMatroxBlobInterface
{
public:
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
	static HRESULT CreateContext(__int64& rContextId );
	// This function replaces MblobAllocResult
	static HRESULT CreateResult( __int64& rResultId );

	// This function replaces MblobFree
	static HRESULT DestroyContext(__int64& rContextId );
	static HRESULT DestroyResult( __int64& rResultId );


	// This function replaces MblobCalculate( 
	static HRESULT Execute( const __int64& rResultId, const SVMatroxBuffer& rIdentId,  const __int64& rContextId );

	static HRESULT GetNumber( const __int64& rResultId, long& rNumber ) ;
	static HRESULT GetResult( const __int64& rResultId, SVBlobSelectionEnum lControlType, double* pdResultsArray ) ;
	static HRESULT GetResult( const __int64& rResultId, SVBlobSelectionEnum lControlType, long* plResultsArray );


	// These functions replace MblobControl
	static HRESULT Set( const __int64& rContextId, SVBlobControlEnum lControlType, const long lControlValue);
	static HRESULT Set( const __int64& rContextId, SVBlobControlEnum lControlType, const double lControlValue);

	//
	static HRESULT GetForeground( const __int64& rContextId, bool& rIsDarkForeground);
	static HRESULT SetForeground( const __int64& rContextId, bool isDarkForeground);
	static HRESULT GetIdentifier( const __int64& rContextId, bool& bBinary);
	static HRESULT SetIdentifier( const __int64& rContextId, bool bBinary);

	static HRESULT BlobFill( const __int64& rResultId, const SVMatroxBuffer& rImageID, SVBlobControlEnum eCriterion, const long& lColor);
	// This function replaces MblobSelect
	static HRESULT BlobSelect( const __int64& rResultId, SVBlobOperationEnum eOperation, SVBlobSelectionEnum eSelection, SVConditionEnum eCondition, const double& dCondLow, const double& dCondHigh);
	// This function replaces MblobSelectFeature
	static HRESULT BlobSelectFeature( const __int64& rContextID, std::set<SVBlobSelectionEnum> featureSet);

	static bool Compare(int criteria, const SVBlob& lBlob, const SVBlob& rBlob);

	static HRESULT FindSizedBlobs( SVMatroxBuffer& rThresholdImage, bool LightBackground, SVBlobList& rList ); 
	static HRESULT NumBlobs( SVMatroxBuffer& rThresholdImage, BYTE Background, long& rBlobCount );

	static HRESULT FillResults( const SVBlobOffsetList& rBlobList, SVCommandDataHolder& rResults );

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

