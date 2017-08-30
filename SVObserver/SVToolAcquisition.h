//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAcquisition
//* .File Name       : $Workfile:   SVToolAcquisition.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 13:10:48  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOCore/SVTool.h"
#include "SVMainImageClass.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

class SVImageClass;

class SVAcquisitionToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVAcquisitionToolClass );

public:
	SVAcquisitionToolClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVACQUISITIONTOOL );

	virtual ~SVAcquisitionToolClass();

	virtual bool CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual bool DoesObjectHaveExtents() const override;

	virtual SVStringValueObjectClass* GetInputImageNames( ) override;

protected:
	void init();

	// Embedded Objects:
	SVMainImageClass				mainImageObject;	// Embedded

	// String value object for Source Image Names
	SVStringValueObjectClass m_SourceImageNames;
};

