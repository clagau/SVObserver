//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVTaskObjectList.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#pragma endregion Includes

class SVImageClass;

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVUnaryImageOperatorListClass 
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
//				: The #1 replaces the old SVUnaryImageOperatorListClass and 
//				: for compatibility to older versions it inherits the classID
//				: of the old SVUnaryImageOperatorListClass.
////////////////////////////////////////////////////////////////////////////////
class SVUnaryImageOperatorListClass : public SVTaskObjectListClass 
{
public:
	friend class SVUnaryImageOperatorClass;

public:
	SVUnaryImageOperatorListClass( SVObjectClass* POwner = nullptr , int StringResourceID = IDS_CLASSNAME_SVUNARYIMAGEOPERATORLIST );
	virtual ~SVUnaryImageOperatorListClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	SVImageClass* getInputImage() const;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

protected:
	virtual bool isInputImage(const SVGUID& rImageGuid) const override;

private:
	void init();

protected:
	SVInObjectInfoStruct		inputImageObjectInfo;
	SvOi::SVImageBufferHandlePtr		m_milTmpImageObjectInfo1;
	SvOi::SVImageBufferHandlePtr		m_milTmpImageObjectInfo2;
};
