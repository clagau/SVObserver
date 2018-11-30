//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTransformationTool
//* .File Name       : $Workfile:   SVTransformationTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:27:14  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

class SVTransformationToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVTransformationToolClass );

public:
	SVTransformationToolClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTRANSFORMATIONTOOL );
	virtual ~SVTransformationToolClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual SVTaskObjectClass* GetObjectAtPoint( const SVPoint<double>& rPoint ) override;
	virtual bool DoesObjectHaveExtents() const override;

	virtual SVStringValueObjectClass* GetInputImageNames() override;

protected:
	virtual bool isInputImage(const SVGUID& rImageGuid) const override { return false; };
	virtual HRESULT UpdateImageWithExtent() override;

private:
	// NOTE:
	//	Must not be virtual and should be private, otherwise if another class
	//	is derived from this class, but the constructor of this class wants
	//	to call init(), the wrong init() ( the init of the derived class ) 
	//	will be called.
	void init();

#pragma region Member Variables
private:
	SVStringValueObjectClass m_SourceImageNames;
#pragma endregion Member Variables
};

