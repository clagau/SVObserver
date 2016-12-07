//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalTool
//* .File Name       : $Workfile:   SVExternalTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:21:46  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVTool.h"
#pragma endregion Includes

class SVExternalTool : public SVToolClass
{
	SV_DECLARE_CLASS( SVExternalTool );

public:
	SVExternalTool( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SV_EXTERNAL_TOOL );
	virtual ~SVExternalTool();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;

	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint ) override;

	virtual HRESULT GetImageExtent( SVImageExtentClass &p_rsvImageExtent ) override;
	virtual HRESULT GetImageExtent( unsigned long p_ulIndex, SVImageExtentClass &p_rsvImageExtent ) override;

	virtual SVStaticStringValueObjectClass* GetInputImageNames() override;

protected:
	virtual HRESULT UpdateImageWithExtent( unsigned long p_Index ) override;

	SVBoolValueObjectClass m_svPerformTranslation;

private:
	void Initialize();

#pragma region Member Variables
private:
	// String value object for Source Image Names
	SVStaticStringValueObjectClass m_svSourceImageNames;
#pragma endregion Member Variables
};

