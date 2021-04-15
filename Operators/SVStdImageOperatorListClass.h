//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Class for the StdImageOperator List
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUnaryImageOperatorList.h"
#include "InspectionEngine/SVImageClass.h"
#pragma endregion Includes

namespace SvOp
{

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
//				: Replaces the old SVUnaryImageOperatorList and 
//				: for compatibility to older versions it inherits the classID
//				: of the old SVUnaryImageOperatorList.
// -----------------------------------------------------------------------------
// .Special : Threshold Operator, if it thinks it is the first one, but 
//			: thresholding is deactivated, it is doing a copy (input to output)
////////////////////////////////////////////////////////////////////////////////
class SVStdImageOperatorListClass : public SVUnaryImageOperatorList 
{
	SV_DECLARE_CLASS

	friend class SVUnaryImageOperatorClass;

public:
	SVStdImageOperatorListClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVSTDIMAGEOPERATORLIST );
	virtual ~SVStdImageOperatorListClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual bool Run( RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	virtual const SvIe::SVImageClass* getOutputImage(bool) const override { return &m_OutputImage; };
	virtual const SvIe::SVImageClass* getReferenceImage(bool ) const override { return &m_LogicalROIImage; };

	void setTempSourceImageHandle(SvOi::SVImageBufferHandlePtr replaceSourceImage) { m_replaceSourceImage = replaceSourceImage; };

protected:

	HRESULT CollectInputImageNames();
	bool RunLocal(RunStatus &rRunStatus, SvOi::SVImageBufferHandlePtr input, SvIe::SVImageClass& rOutputImage);
	SvOi::SVImageBufferHandlePtr createTmpSourceImage();
	
private:
	void init();

	//************************************
	//! Copy a image from one buffer to another buffer
	//! \param input [in] Input buffer
	//! \param output [in,out] Output buffer, has to be allocated and the same size as input.
	//! \returns bool If copy was successfully.
	//************************************
	bool copyBuffer( const SvOi::SVImageBufferHandlePtr input, SvOi::SVImageBufferHandlePtr output );

protected:
	SvIe::SVImageClass m_LogicalROIImage; // Embedded
	SvIe::SVImageClass m_OutputImage;	// Embedded
	SvOi::SVImageBufferHandlePtr m_milTmpImageObjectInfo1;
	SvOi::SVImageBufferHandlePtr m_milTmpImageObjectInfo2;

	SvOi::SVImageBufferHandlePtr m_replaceSourceImage; ///< This image handle is set if regressionTest will replace the source image. Than use this.
};

} //namespace SvOp
