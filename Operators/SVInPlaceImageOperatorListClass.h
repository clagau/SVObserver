//******************************************************************************
/// COPYRIGHT (c) 2016 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
//******************************************************************************
/// Class for the InplaceImageOperator List
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUnaryImageOperatorList.h"
#pragma endregion Includes

namespace SvOp
{

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVInPlaceImageOperatorListClass 
// -----------------------------------------------------------------------------
// .Description : Makes in place processing on input image. Has NO Output Image!
//				:
//				: All operators are doing inplace processing on input image:
//				: Input Image  ---> Input Image
//				:
//				: Input Image must be a physical image type !!!
//				: Validation fails, if input image is not physical.
//				:
//				: Is doing an inplace processing on the input image, that
//				: means another output image is not needed. This is useful,
//				: if wanting to apply image operators to an existing
//				: image object ( to his owner TaskObject ).
// -----------------------------------------------------------------------------
// .Special : Threshold Operator, if he things he is the first one, but 
//			: thresholding is deactivated, he is doing a copy (input to input)
////////////////////////////////////////////////////////////////////////////////
class SVInPlaceImageOperatorListClass : public SVUnaryImageOperatorList 
{
	SV_DECLARE_CLASS

public:
	SVInPlaceImageOperatorListClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVINPLACEIMAGEOPERATORLIST );
	virtual ~SVInPlaceImageOperatorListClass();

	virtual bool isInputImage(uint32_t) const override { return false; };

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;
	virtual bool Run( SvIe::RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

	virtual const SvIe::SVImageClass* getOutputImage(bool bRunMode = false) const override { return getInputImage(bRunMode); };
	virtual const SvIe::SVImageClass* getReferenceImage(bool bRunMode = false) const override { return getInputImage(bRunMode); };

private:
	void init();
};

} //namespace SvOp
