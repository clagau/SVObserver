//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPerspectiveToolClass
//* .File Name       : $Workfile:   SVPerspectiveTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   09 Dec 2014 09:47:26  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVTool.h"
#include "SVObjectLibrary/InputObject.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

namespace SvTo
{

class SVPerspectiveToolClass : public SVToolClass
{
	SV_DECLARE_CLASS

public:
	SVPerspectiveToolClass( SVObjectClass *p_pOwner = nullptr, int p_iStringResourceID = IDS_CLASSNAME_SVPERSPECTIVETOOL );
	virtual ~SVPerspectiveToolClass();

	virtual bool allowExtensionCopy() const override;
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual SvPb::SVExtentLocationPropertyEnum getLocationPropertyAtPoint( const SVPoint<double>& rPoint ) const override;
	virtual bool DoesObjectHaveExtents() const override;
	virtual HRESULT SetImageExtent( const SVImageExtentClass& rImageExtent) override;

	virtual void addOverlays(SvPb::OverlayDesc& rOverlay) const override;

	virtual std::vector<std::string> getToolAdjustNameList() const override;

	enum WarpType
	{
		WarpTypeInvalid = 0,
		WarpTypeHorizontal = 1,
		WarpTypeVertical = 2,
	};


public:
	virtual SvVol::SVStringValueObjectClass* GetInputImageNames() override;

protected:
	HRESULT UpdateOutputImageExtents();
	virtual bool isInputImage(uint32_t imageId) const override;
	virtual bool onRun( SvIe::RunStatus &p_rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr ) override;

private:
	void LocalInitialize();

	HRESULT CreateLUT();
	void DestroyLUT();
	
	SvOl::InputObject m_InputImage;

	SvIe::SVImageClass m_OutputImage;
	SvVol::SVEnumerateValueObjectClass m_svWarpType;
	SvVol::SVDoubleValueObjectClass m_svXOffset;
	SvVol::SVDoubleValueObjectClass m_svYOffset;
	SvVol::SVEnumerateValueObjectClass m_svInterpolationMode;

	SVMatroxBuffer m_LutX;
	SVMatroxBuffer m_LutY;
	
private:
	// String value object for Source Image Names
	SvVol::SVStringValueObjectClass m_SourceImageNames;
};

} //namespace SvTo
