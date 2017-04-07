//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for the StdImageOperator List
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUnaryImageOperatorList.h"
#include "SVImageClass.h"
#pragma endregion Includes

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVStdImageOperatorListClass  
// -----------------------------------------------------------------------------
// .Description : Provides its own output image.
//				:
//				: First operator in the list is processing
//				: Input Image  ---> Output Image
//				: 
//				: All other succeeding operators are doing in place processing 
//				: on output image
//				: Output Image ---> Output Image
//				:
//				: Replaces the old SVUnaryImageOperatorListClass and 
//				: for compatibility to older versions it inherits the classID
//				: of the old SVUnaryImageOperatorListClass.
// -----------------------------------------------------------------------------
// .Special : Threshold Operator, if it thinks it is the first one, but 
//			: thresholding is deactivated, it is doing a copy (input to output)
////////////////////////////////////////////////////////////////////////////////
class SVStdImageOperatorListClass : public SVUnaryImageOperatorListClass 
{
	SV_DECLARE_CLASS( SVStdImageOperatorListClass )

	friend class SVUnaryImageOperatorClass;

public:
	SVStdImageOperatorListClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVSTDIMAGEOPERATORLIST );
	virtual ~SVStdImageOperatorListClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual bool Run( SVRunStatusClass& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

protected:

	SVImageClass *getOutputImage();

	SVImageClass outputImageObject;	// Embedded

private:
	void init();

	//************************************
	//! Copy a image from one buffer to another buffer
	//! \param input [in] Input buffer
	//! \param output [in,out] Output buffer, has to be allocated and the same size as input.
	//! \returns bool If copy was successfully.
	//************************************
	bool copyBuffer( const SVSmartHandlePointer input, SVSmartHandlePointer output );

public:
	HRESULT CollectInputImageNames();
};
