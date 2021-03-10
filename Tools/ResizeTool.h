//*****************************************************************************
/// \copyright COPYRIGHT (c) 2015, 2021 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Contains the Resize Tool class definition
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTool.h"
#include "SVObjectLibrary/InputObject.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion

namespace SvTo
{

class ResizeTool: public SVToolClass
{
	SV_DECLARE_CLASS

#pragma region Public Methods
public:
	ResizeTool(SVObjectClass* pOwner = nullptr, int stringResourceID = IDS_OBJECTNAME_RESIZETOOL);

	virtual ~ResizeTool(void);

	virtual bool CloseObject() override;
	virtual bool CreateObject(const SVObjectLevelCreateStruct& rCreateStructure) override;

	virtual HRESULT SetImageExtentToParent() override;
	virtual HRESULT SetImageExtent( const SVImageExtentClass& rImageExtent) override;
	
	/// GetObjectAtPoint
	///  Tests to see if the passed in point (usually from a mouse location)
	///  is contained within itself (the ROI).  If so it returns itself (this),
	///  otherwise returns a nullptr.
	virtual SVToolClass* GetObjectAtPoint(const SVPoint<double>& rPoint) override;
	virtual bool DoesObjectHaveExtents() const override;
	virtual std::vector<std::string> getToolAdjustNameList() const override;

	/// GetInputImageNames
	///  Retrieves source image name from m_SourceImageNames.
	virtual SvVol::SVStringValueObjectClass* GetInputImageNames() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	bool AreAllAllScaleFactorValuesValid();
	bool isInterpolationModeValueOK();
	bool isOverscanActiveValueOK();

	SvIe::SVImageClass* getInputImage(bool bRunMode = false) const;

#pragma endregion Public Methods

#pragma region Private Methods
private:

	// Source Image - input
	SvOl::InputObject m_InputImage;
	void LocalInitialize();

	void BuildInputObjectList();
	void BuildEmbeddedObjectList();

	virtual bool isInputImage(uint32_t imageId) const override;

	virtual bool onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	bool ModifyImageExtentByScaleFactors();
#pragma endregion Private Methods

#pragma region Private Members
	SvVol::SVStringValueObjectClass m_SourceImageNames;

	// Output Image - embedded
	SvIe::SVImageClass m_OutputImage;
	SvIe::SVImageClass m_LogicalROIImage;

	SvVol::SVEnumerateValueObjectClass	m_ResizeInterpolationMode;
	SvVol::SVEnumerateValueObjectClass	m_ResizeOverscanActive;

#pragma endregion Private Members
};

} //namespace SvTo
 