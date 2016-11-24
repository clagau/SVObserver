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
#pragma endregion Includes

const CString PERSPECTIVE_WARP_TYPE_HORIZONTAL = _T("Horizontal Warp");
const CString PERSPECTIVE_WARP_TYPE_VERTICAL = _T("Vertical Warp");

class SVPerspectiveToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVPerspectiveToolClass );

public:
	SVPerspectiveToolClass( BOOL p_bCreateDefaultTaskList = FALSE, 
		                    SVObjectClass *p_pOwner = nullptr, 
												int p_iStringResourceID = IDS_CLASSNAME_SVPERSPECTIVETOOL );
	virtual ~SVPerspectiveToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct *p_pCreateStructure );
	virtual BOOL CloseObject();

	virtual HRESULT ResetObject();

	virtual BOOL OnValidate();

	SVImageClass *GetInputImage();

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage );
	virtual SVTaskObjectClass* GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint );
	virtual bool DoesObjectHaveExtents() const override;
	virtual HRESULT SetImageExtent( unsigned long p_ulIndex, SVImageExtentClass p_svImageExtent );

	enum WarpType
	{
		WarpTypeInvalid = 0,
		WarpTypeHorizontal = 1,
		WarpTypeVertical = 2,
	};


public:
	virtual SVStaticStringValueObjectClass* GetInputImageNames() override;

protected:
	HRESULT UpdateOutputImageExtents();
	virtual BOOL onRun( SVRunStatusClass &p_rRunStatus );

private:
	void LocalInitialize();

	HRESULT LocalDestroy();

	HRESULT CreateLUT();
	HRESULT DestroyLUT();
	
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
	SVStaticStringValueObjectClass m_svSourceImageName;
};


