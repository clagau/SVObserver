// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxPatternInterface
// * .File Name       : $Workfile:   SVMatroxPatternInterface.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   26 Jun 2014 11:29:22  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVMatroxSimpleEnums.h"
#include "SVMatroxBuffer.h"
#pragma endregion Includes


/**
@SVObjectName Matrox Pattern Interface

@SVObjectOverview The pattern interface class provides functions that support the matrox pattern matching operations.

@SVObjectOperations The Create function uses the Matrox MpatAllocModel. This function allocates a model, using data from the specified area of the model's source image.
The CreateResult function uses the Matrox MpatAllocResult. This function allocates a result buffer with the specified number of entries.
The Destroyfunction uses the Matrox MpatFree. This function deletes the specified pattern matching model or result buffer identifier and releases any memory associated with it. 
PatCopy function uses the Matrox MpatCopy. This function copies the specified model to the specified destination image buffer, starting at the top-left corner of the buffer. Once the model is copied to the destination buffer, it can then be displayed (if the destination buffer is displayable).
SVPatPreProcModel function uses the Matrox MpatPreprocModel. This function preprocesses the specified model. It trains the system to search for the model in the most efficient manner (optionally within a specified typical image). The procedure is potentially quite lengthy (up to several seconds). 
Execute function uses the Matrox MpatFindModel. This function finds occurrences of the specified model in the given image and returns the position of each occurrence. The search is performed using the model's current search parameters.
Get function uses the Matrox MpatInquire. This function returns information about the specified model or result buffer. It is useful to determine, for example, the size of a model that has been restored from disk and its position in the model's source image.
Set function uses the Matrox MpatSetSearchParameter. This function sets search parameters for the SVMatroxIdentifier.  After a call to this function the PreprocessModel should be called.
GetResult function uses the Matrox MpatGetResult. This function retrieves the result(s) of the specified type from a specified pattern matching result buffer.
*/
class SVMatroxPatternInterface
{
public:
	SVMatroxPatternInterface();

	virtual ~SVMatroxPatternInterface();

	// This function replaces MpatAllocModel
	static HRESULT CreateContext( __int64& rModel, SVMatroxBuffer& rSrcImageId, 
		long lOffX, long lOffY, long lSizeX, long lSizeY, long lType=SVPatModelTypeNormalized );
	
	// This function replaces MpatAllocResult
	static HRESULT CreateResult( __int64& rResultId, long& rlNbrEntries );
	
	// This function replaces MpatFree
	static HRESULT DestroyContext( __int64& rId );
	static HRESULT DestroyResult( __int64& rId );

	// This function replaces MpatPreprocModel
	static HRESULT PreProcModel( const __int64& rModelId, const SVMatroxBuffer& rImageId);

	// This function replaces MpatFindModel
	static HRESULT Execute( const __int64& rResultId, const SVMatroxBuffer& rSourceImageId, const __int64& rModelId );

	// Get Functions for model setup (MpatInquire)
	static HRESULT Get( const __int64& rModelId, const SVPatternTypeEnum eType, double& rdValue ) ;
	static HRESULT Get( const __int64& rModelId, const SVPatternTypeEnum eType, long& rlValue ) ;
	
	// Results... (MpatGetResult)
	static HRESULT GetResult( const __int64& rResultId, const SVPatternResultEnum eType, double* pdValue ) ;

	// (MpatGetNumber)
	static HRESULT GetNumber( const __int64& rModelId, long& rlValue ) ;

	// Set Functions for model setup, general Search parameters (MpatSetSearchParameter)
	static HRESULT Set( const __int64& rResultId, const SVPatternTypeEnum eType, const double& rdValue ) ;
	static HRESULT Set( const __int64& rResultId, const SVPatternTypeEnum eType, const long& rlValue ) ;
	
	// Set Functions for model setup, advanced Search parameters (MpatSetSearchParameter)
	static HRESULT Set( const __int64& rResultId, const SVPatternSearchParameterTypeEnum eType, const double& rdValue ) ;
	static HRESULT Set( const __int64& rResultId, const SVPatternSearchParameterTypeEnum eType, const long& rlValue ) ;

	// (MpatSetCenter)
	static HRESULT SetCenter( const __int64& rModelId, long lXOffset, long lYOffset);

	/// Set the "don't care" pixels in a model. 
	/// \param rDontCareImageId [in] Id of the Don't care image
	/// \param rModelId [in] ID of the model.
	/// \returns HRESULT
	static HRESULT SetDontCare( const SVMatroxBuffer& rDontCareImageId, const __int64& rModelId );

	// (MpatSetNumber)
	static HRESULT SetNumber( const __int64& rModelId, long lNumber );

	// (MpatSetPosition)
	static HRESULT SetPosition( const __int64& rModelId, long lXOffset, long lYOffset, long lSizeX, long lSizeY );
	
	// (MpatSetAcceptance)
	static HRESULT SetAcceptance( const __int64& rModelId, const double dValue );

	// (MpatSetCertainty)
	static HRESULT SetCertainty( const __int64& rModelId, const double dValue );
	
	// (MpatSetAccuracy)
	static HRESULT SetAccuracy( const __int64& rModelId, const long lValue );

	// (MpatSetSpeed)
	static HRESULT SetSpeed( const __int64& rModelId, const long lValue );
	
	// (MpatSetAngle)
	static HRESULT SetAngle( const __int64& rModelId, SVPatternTypeEnum eType, double dValue );
	
	// (MpatSetAngle - M_SEARCH_ANGLE_MODE [M_ENABLE/M_DISABLE])
	static HRESULT SetAngle( const __int64& rModelId, bool bEnable );

	// helpers for Circular Overscan
	static RECT CalculateOverscanOuterRect( const POINT& pos, const SIZE& size );
	static RECT CalculateOverscanInnerRect( const POINT& pos, const SIZE& size );

private:
	static long ConvertModelType2MatroxType( long lType );
	static long Convert2MatroxType( SVPatternTypeEnum eType );
	static long Convert2MatroxType( SVPatternSearchParameterTypeEnum eType );
	static long Convert2MatroxType( SVPatternResultEnum eType );
};

