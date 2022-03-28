//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUnaryImageOperatorList
//* .File Name       : $Workfile:   SVUnaryImageOperatorList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 15:06:24  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "InspectionEngine/SVTaskObjectList.h"
#include "SVObjectLibrary/InputObject.h"
#pragma endregion Includes


namespace SvIe
{
class SVImageClass;
}

namespace SvOp
{

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVUnaryImageOperatorList 
// -----------------------------------------------------------------------------
// .Description : New base class for 
//				: #1 SVStdImageOperatorListClass		and
//              : #2 SVInPlaceImageOperatorListClass. 
//              :
//				; The difference between these new classes is that #1 has his
//				: own image output and is processing from input to output image.
//				:
//				: #2 is doing an in place processing on the input image, that
//				: means we don't need another output image. This is useful,
//				: if we want to just apply image operators to an existing
//				: image object ( to his owner TaskObject ).
//				:
//				: The #1 replaces the old SVUnaryImageOperatorList and 
//				: for compatibility to older versions it inherits the classID
//				: of the old SVUnaryImageOperatorList.
////////////////////////////////////////////////////////////////////////////////
class SVUnaryImageOperatorList : public SvIe::SVTaskObjectListClass
{
public:
	friend class SVUnaryImageOperatorClass;

public:
	SVUnaryImageOperatorList(bool onlyImageBefore = true, SVObjectClass* POwner = nullptr , int StringResourceID = IDS_CLASSNAME_SVUNARYIMAGEOPERATORLIST );
	virtual ~SVUnaryImageOperatorList();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	SvIe::SVImageClass* getInputImage(bool bRunMode = false) const;
	virtual const SvIe::SVImageClass* getOutputImage(bool bRunMode = false) const = 0;
	virtual const SvIe::SVImageClass* getReferenceImage(bool bRunMode = false) const = 0;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

protected:
	virtual bool isInputImage(uint32_t imageId) const override;

private:
	void init(bool onlyImageBefore);

protected:
	SvOl::InputObject m_inputImage;
};

} //namespace SvOp
