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
#pragma endregion Includes

class SVTransformationToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVTransformationToolClass );

public:
	SVTransformationToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVTRANSFORMATIONTOOL );
	virtual ~SVTransformationToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure );
	virtual HRESULT ResetObject();

	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint );
	virtual bool DoesObjectHaveExtents() const override;

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );

	virtual SVStaticStringValueObjectClass* GetInputImageNames() override;

protected:
	virtual HRESULT UpdateImageWithExtent( unsigned long p_Index );

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

