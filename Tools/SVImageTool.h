//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolImage
//* .File Name       : $Workfile:   SVToolImage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 08:29:14  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#include "SVValueObjectLibrary\SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary\SVLongValueObjectClass.h"
#include "SVValueObjectLibrary\SVPointValueObjectClass.h"
#include "SVValueObjectLibrary\SVStringValueObjectClass.h"
#pragma region Includes

namespace SvTo
{

class SVImageToolClass : public SVToolClass
{
	SV_DECLARE_CLASS

public:
	SVImageToolClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGETOOL );
	virtual ~SVImageToolClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	

	virtual SVToolClass* GetObjectAtPoint(const SVPoint<double>& rPoint) override;
	virtual bool DoesObjectHaveExtents() const override;
	virtual HRESULT SetImageExtent( const SVImageExtentClass& rImageExtent) override;
	virtual HRESULT SetImageExtentToParent() override;

	virtual bool SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual SvVol::SVStringValueObjectClass* GetInputImageNames() override;

	virtual void addOverlays(const SvIe::SVImageClass* pImage, SvPb::OverlayDesc& rOverlay) const override;

	virtual std::vector<std::string> getToolAdjustNameList() const override;

protected:
	HRESULT UpdateTranslation();

private:
	void init();
	void  BuildEmbeddedObjectList();
	
	SvVol::SVLongValueObjectClass m_outputOperator;
	// String value object for Source Image Names
	SvVol::SVStringValueObjectClass m_SourceImageNames;
	SvVol::LinkedValue	m_LinkedGain; //!  Linked value for Gain
	SvVol::LinkedValue	m_LinkedOffset; //!  Linked value for Offset
	SvVol::SVBoolValueObjectClass m_IsGainOffsetEnabled; //!  Is gain and offset enabled
	SvVol::SVBoolValueObjectClass m_UseLut; //!  Use LUT table
};

} //namespace SvTo
