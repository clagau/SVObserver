//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Jim Brown
//*****************************************************************************

/// Classes and functions to ....
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTool.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#pragma endregion

namespace SvTo
{

class ResizeTool: public SVToolClass
{
	SV_DECLARE_CLASS (ResizeTool);

#pragma region Public Methods
public:
	ResizeTool(SVObjectClass* pOwner = nullptr, int stringResourceID = IDS_OBJECTNAME_RESIZETOOL);

	virtual ~ResizeTool(void);

	virtual bool CloseObject() override;
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;

	virtual HRESULT SetImageExtentToParent() override;
	virtual HRESULT SetImageExtent( const SVImageExtentClass& rImageExtent ) override;
	HRESULT SetImageExtentWithoutScaleFactors(const SVImageExtentClass& rImageExtent);
	
	/// GetObjectAtPoint
	///  Tests to see if the passed in point (usually from a mouse location)
	///  is contained within itself (the ROI).  If so it returns itself (this),
	///  otherwise returns a nullptr.
	virtual SVToolClass* GetObjectAtPoint(const SVPoint<double>& rPoint) override;
	virtual bool DoesObjectHaveExtents() const override;
	
	/// GetInputImageNames
	///  Retrieves source image name from m_SourceImageNames.
	virtual SvVol::SVStringValueObjectClass* GetInputImageNames() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	bool AreAllAllScaleFactorValuesValid();
	bool isInterpolationModeValueOK ();

	SvIe::SVImageClass* getInputImage(bool bRunMode = false) const;

#pragma endregion Public Methods

#pragma region Private Methods
private:

	// Source Image - input
	SvOl::SVInObjectInfoStruct m_InputImageObjectInfo;
	void LocalInitialize();

	void BuildInputObjectList();
	void BuildEmbeddedObjectList();

	virtual bool isInputImage(uint32_t imageId) const override;

	virtual bool onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	bool allScalefactorsHaveDefaultValues();
	bool ModifyImageExtentByScaleFactors();

#pragma endregion Private Methods

#pragma region Private Members
	SvVol::SVStringValueObjectClass m_SourceImageNames;

	// Output Image - embedded
	SvIe::SVImageClass m_OutputImage;
	SvIe::SVImageClass m_LogicalROIImage;

	SvVol::SVEnumerateValueObjectClass	m_ResizeInterpolationMode;

	SvVol::LinkedValue m_contentWidthScaleFactor;
	SvVol::LinkedValue m_contentHeightScaleFactor;
	SvVol::LinkedValue m_formatWidthScaleFactor;
	SvVol::LinkedValue m_formatHeightScaleFactor;

#pragma endregion Private Members
};

} //namespace SvTo
 