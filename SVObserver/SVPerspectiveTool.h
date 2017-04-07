//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVOCore/SVImageClass.h"
#include "SVTool.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

class SVPerspectiveToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVPerspectiveToolClass );

public:
	SVPerspectiveToolClass( BOOL p_bCreateDefaultTaskList = FALSE, 
		                    SVObjectClass *p_pOwner = nullptr, 
												int p_iStringResourceID = IDS_CLASSNAME_SVPERSPECTIVETOOL );
	virtual ~SVPerspectiveToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct *p_pCreateStructure ) override;
	virtual BOOL CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	SVImageClass *GetInputImage();

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage ) override;
	virtual SVTaskObjectClass* GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint ) override;
	virtual bool DoesObjectHaveExtents() const override;
	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent ) override;

	enum WarpType
	{
		WarpTypeInvalid = 0,
		WarpTypeHorizontal = 1,
		WarpTypeVertical = 2,
	};


public:
	virtual SVStringValueObjectClass* GetInputImageNames() override;

protected:
	HRESULT UpdateOutputImageExtents();
	virtual bool onRun( SVRunStatusClass &p_rRunStatus, SvStl::MessageContainerVector *pErrorMessages = nullptr ) override;

private:
	void LocalInitialize();

	HRESULT CreateLUT();
	void DestroyLUT();
	
	SVInObjectInfoStruct m_InputImageObjectInfo;

	SVImageClass m_OutputImage;
	SVEnumerateValueObjectClass m_svWarpType;
	SVDoubleValueObjectClass m_svXOffset;
	SVDoubleValueObjectClass m_svYOffset;
	SVEnumerateValueObjectClass m_svInterpolationMode;

	SVMatroxBuffer m_LutX;
	SVMatroxBuffer m_LutY;
	
private:
	// String value object for Source Image Names
	SVStringValueObjectClass m_SourceImageNames;
};


