//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#pragma endregion Includes

class SVPolarTransformationToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVPolarTransformationToolClass );

public:
	SVPolarTransformationToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVPOLARTRANSFORMATIONTOOL );
	virtual ~SVPolarTransformationToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;

	virtual BOOL SetDefaultFormulas() override;

	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint ) override;
	virtual bool DoesObjectHaveExtents() const override;

	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent ) override;
	friend class SVImagePolarTransformClass;

	virtual SVStaticStringValueObjectClass* GetInputImageNames() override;

	SVEnumerateValueObjectClass m_svAngularMethod;

private:
	// NOTE:
	//	Must not be virtual and should be private, otherwise if another class
	//	is derived from this class, but the constructor of this class wants
	//	to call init(), the wrong init() ( the init of the derived class ) 
	//	will be called.
	void init();

#pragma region Member Variables
private:
	SVStaticStringValueObjectClass m_svSourceImageNames;
#pragma endregion Member Variables
};


