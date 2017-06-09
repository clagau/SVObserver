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

#include "SVMatroxEnums.h"
#include "SVMatroxStatusInformation.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxPatResult.h"
#include "SVMatroxPatternModel.h"
#include "SVMatroxApplicationInterface.h"


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
Set function uses the Matrox MpatSetSearchParameter. This function sets search parameters for the SVMatroxPatternModel.  After a call to this function the PreprocessModel should be called.
GetResult function uses the Matrox MpatGetResult. This function retrieves the result(s) of the specified type from a specified pattern matching result buffer.
*/
class SVMatroxPatternInterface
{
public:
	SVMatroxPatternInterface();

	virtual ~SVMatroxPatternInterface();

	// This function replaces MpatAllocModel
	static HRESULT Create( SVMatroxPatternModel& p_rModel, SVMatroxBuffer& p_rSrcImageId, 
		long p_lOffX, long p_lOffY, long p_lSizeX, long p_lSizeY, long p_lType=SVPatModelTypeNormalized );
	
	// This function replaces MpatAllocResult
	static HRESULT Create( SVMatroxPatResult& p_rResultId, long& p_rlNbrEntries );
	
	// This function replaces MpatFree
	static HRESULT Destroy( SVMatroxPatternModel& p_rId );
	static HRESULT Destroy( SVMatroxPatResult& p_rId );

	// This function replaces MpatCopy
	// Do Not use with Circular Overscan to try and restore the Image from the Model 
	static HRESULT PatCopy( const SVMatroxBuffer& p_rDestImageId, const SVMatroxPatternModel& p_rSourceModelId );

	// This function replaces MpatPreprocModel
	static HRESULT PreProcModel( const SVMatroxPatternModel& p_rModelId, const SVMatroxBuffer& p_rImageId);

	// This function replaces MpatFindModel
	static HRESULT Execute( const SVMatroxPatResult& p_rResultId, const SVMatroxBuffer& p_rSourceImageId, const SVMatroxPatternModel& p_rModelId );

	// Get Functions for model setup (MpatInquire)
	static HRESULT Get( const SVMatroxPatternModel& p_rModelId, const SVPatternTypeEnum p_eType, double& p_rdValue ) ;
	static HRESULT Get( const SVMatroxPatternModel& p_rModelId, const SVPatternTypeEnum p_eType, long& p_rlValue ) ;
	
	// Results... (MpatGetResult)
	static HRESULT GetResult( const SVMatroxPatResult& p_rResultId, const SVPatternResultEnum p_eType, double* p_pdValue ) ;

	// (MpatGetNumber)
	static HRESULT GetNumber( const SVMatroxPatResult& p_rModelId, long& p_rlValue ) ;

	// Set Functions for model setup, general Search parameters (MpatSetSearchParameter)
	static HRESULT Set( const SVMatroxPatternModel& p_rResultId, const SVPatternTypeEnum p_eType, const double& p_rdValue ) ;
	static HRESULT Set( const SVMatroxPatternModel& p_rResultId, const SVPatternTypeEnum p_eType, const long& p_rlValue ) ;
	
	// Set Functions for model setup, advanced Search parameters (MpatSetSearchParameter)
	static HRESULT Set( const SVMatroxPatternModel& p_rResultId, const SVPatternSearchParameterTypeEnum p_eType, const double& p_rdValue ) ;
	static HRESULT Set( const SVMatroxPatternModel& p_rResultId, const SVPatternSearchParameterTypeEnum p_eType, const long& p_rlValue ) ;

	// (MpatSetCenter)
	static HRESULT SetCenter( const SVMatroxPatternModel& p_rModelId, long p_lXOffset, long p_lYOffset);

	/// Set the "don't care" pixels in a model. 
	/// \param rDontCareImageId [in] Id of the Don't care image
	/// \param rModelId [in] ID of the model.
	/// \returns HRESULT
	static HRESULT SetDontCare( const SVMatroxBuffer& rDontCareImageId, const SVMatroxPatternModel& rModelId );

	// (MpatSetNumber)
	static HRESULT SetNumber( const SVMatroxPatternModel& p_rModelId, long p_lNumber );

	// (MpatSetPosition)
	static HRESULT SetPosition( const SVMatroxPatternModel& p_rModelId, long p_lXOffset, long p_lYOffset, long p_lSizeX, long p_lSizeY );
	
	// (MpatSetAcceptance)
	static HRESULT SetAcceptance( const SVMatroxPatternModel& p_rModelId, const double p_dValue );

	// (MpatSetCertainty)
	static HRESULT SetCertainty( const SVMatroxPatternModel& p_rModelId, const double p_dValue );
	
	// (MpatSetAccuracy)
	static HRESULT SetAccuracy( const SVMatroxPatternModel& p_rModelId, const long p_lValue );

	// (MpatSetSpeed)
	static HRESULT SetSpeed( const SVMatroxPatternModel& p_rModelId, const long p_lValue );
	
	// (MpatSetAngle)
	static HRESULT SetAngle( const SVMatroxPatternModel& p_rModelId, SVPatternTypeEnum p_eType, double p_dValue );
	
	// (MpatSetAngle - M_SEARCH_ANGLE_MODE [M_ENABLE/M_DISABLE])
	static HRESULT SetAngle( const SVMatroxPatternModel& p_rModelId, bool p_bEnable );

	// helpers for Circular Overscan
	static RECT CalculateOverscanOuterRect( const POINT& pos, const SIZE& size );
	static RECT CalculateOverscanInnerRect( const POINT& pos, const SIZE& size );

private:
	static long ConvertModelType2MatroxType( long p_lType );
	static long Convert2MatroxType( SVPatternTypeEnum p_eType );
	static long Convert2MatroxType( SVPatternSearchParameterTypeEnum p_eType );
	static long Convert2MatroxType( SVPatternResultEnum p_eType );
};

