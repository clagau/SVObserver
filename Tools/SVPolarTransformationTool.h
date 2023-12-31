//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPolarTransformation
//* .File Name       : $Workfile:   SVPolarTransformationTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   09 Dec 2014 09:47:26  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

namespace SvTo
{

class SVPolarTransformationToolClass : public SVToolClass
{
	SV_DECLARE_CLASS

public:
	SVPolarTransformationToolClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVPOLARTRANSFORMATIONTOOL );
	virtual ~SVPolarTransformationToolClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual SvPb::SVExtentLocationPropertyEnum getLocationPropertyAtPoint( const SVPoint<double>& rPoint ) const override;
	virtual bool DoesObjectHaveExtents() const override;

	virtual HRESULT SetImageExtent( const SVImageExtentClass& rImageExtent) override;

	virtual SvVol::SVStringValueObjectClass* GetInputImageNames() override;

	virtual void addOverlays(SvPb::OverlayDesc& rOverlay) const override;

	virtual std::vector<std::string> getToolAdjustNameList() const override;

	SvVol::SVEnumerateValueObjectClass m_svAngularMethod;

private:
	// NOTE:
	//	Must not be virtual and should be private, otherwise if another class
	//	is derived from this class, but the constructor of this class wants
	//	to call init(), the wrong init() ( the init of the derived class ) 
	//	will be called.
	void init();

#pragma region Member Variables
private:
	SvVol::SVStringValueObjectClass m_SourceImageNames;
#pragma endregion Member Variables
};

} //namespace SvTo
