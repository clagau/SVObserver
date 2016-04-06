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
#include "SVImageClass.h"

#include "SVValueObjectImpl.h"

#include "SVDataBuffer.h"
#pragma endregion Includes

class SVToolClass;
class SVIPDoc;

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
// .History
//	 Date		Author		Comment                                       
//  :20.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVUnaryImageOperatorListClass : public SVTaskObjectListClass 
{
public:

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

	friend class SVUnaryImageOperatorClass;

public:
	SVUnaryImageOperatorListClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr , int StringResourceID = IDS_CLASSNAME_SVUNARYIMAGEOPERATORLIST );
	virtual ~SVUnaryImageOperatorListClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );

	SVImageClass* getInputImage();
	virtual BOOL OnValidate();

private:
	void init();

protected:
	SVInObjectInfoStruct		inputImageObjectInfo;
	SVSmartHandlePointer		m_milTmpImageObjectInfo1;
	SVSmartHandlePointer		m_milTmpImageObjectInfo2;
};

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVStdImageOperatorListClass  
// -----------------------------------------------------------------------------
// .Description : Provides his own output image.
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
// .Special : Threshold Operator, if he things he is the first one, but 
//			: thresholding is deactivated, he is doing a copy (input to output)
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVStdImageOperatorListClass : public SVUnaryImageOperatorListClass 
{
	SV_DECLARE_CLASS( SVStdImageOperatorListClass )
//******************************************************************************
// Friend Declaration(s):
//******************************************************************************
	friend class SVUnaryImageOperatorClass;

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVStdImageOperatorListClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVSTDIMAGEOPERATORLIST );
	virtual ~SVStdImageOperatorListClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual BOOL Run( SVRunStatusClass& RRunStatus );

	virtual BOOL OnValidate();

protected:
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );

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

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVInPlaceImageOperatorListClass 
// -----------------------------------------------------------------------------
// .Description : Makes in place processing on input image. Has NO Output Image!
//				:
//				: All operators are doing in place processing on input image:
//				: Input Image  ---> Input Image
//				:
//				: Input Image must be a physical image type !!!
//				: Validation fails, if input image is not physical.
//				:
//				: Is doing an in place processing on the input image, that
//				: means we don't need another output image. This is useful,
//				: if we want to just apply image operators to an existing
//				: image object ( to his owner TaskObject ).
// -----------------------------------------------------------------------------
// .Special : Threshold Operator, if he things he is the first one, but 
//			: thresholding is deactivated, he is doing a copy (input to input)
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :20.03.2000 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
class SVInPlaceImageOperatorListClass : public SVUnaryImageOperatorListClass 
{
	SV_DECLARE_CLASS( SVInPlaceImageOperatorListClass )

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVInPlaceImageOperatorListClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVINPLACEIMAGEOPERATORLIST );
	virtual ~SVInPlaceImageOperatorListClass();

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );

	virtual BOOL OnValidate();
	virtual BOOL Run( SVRunStatusClass& RRunStatus );

private:
	void init();
};
