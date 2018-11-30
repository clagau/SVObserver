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
#include "InspectionEngine/SVTool.h"
#pragma region Includes

class SVImageToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVImageToolClass );

public:
	SVImageToolClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVIMAGETOOL );
	virtual ~SVImageToolClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual SVTaskObjectClass* GetObjectAtPoint(const SVPoint<double>& rPoint) override;
	virtual bool DoesObjectHaveExtents() const override;
	virtual HRESULT SetImageExtent( const SVImageExtentClass& rImageExtent ) override;
	virtual HRESULT SetImageExtentToParent() override;

	virtual bool SetDefaultFormulas(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual SVStringValueObjectClass* GetInputImageNames() override;

protected:
	HRESULT UpdateTranslation();

private:
	void init();

	SVBoolValueObjectClass		outputEnableOffsetA;
	SVPointValueObjectClass		outputOffsetAPoint;
	SVBoolValueObjectClass		outputEnableOffsetB;
	SVPointValueObjectClass		outputOffsetBPoint;
	SVLongValueObjectClass		outputOperator;
	// String value object for Source Image Names
	SVStringValueObjectClass m_SourceImageNames;
};

