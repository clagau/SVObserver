//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCylindricalWarpToolClass
//* .File Name       : $Workfile:   SVCylindricalWarpTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 May 2014 11:13:42  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVOCore/SVImageClass.h"
#include "SVTool.h"
#pragma endregion Includes

const CString CYLINDRICAL_WARP_TYPE_HORIZONTAL = _T("Horizontal Warp");
const CString CYLINDRICAL_WARP_TYPE_VERTICAL = _T("Vertical Warp");

class SVCylindricalWarpToolClass : public SVToolClass  
{
	SV_DECLARE_CLASS( SVCylindricalWarpToolClass );

public:
	SVCylindricalWarpToolClass( BOOL p_bCreateDefaultTaskList = FALSE, 
	                            SVObjectClass* p_pOwner = nullptr, 
	                            int p_iStringResourceID = IDS_CLASSNAME_SVCYLINDRICALTOOL );
	virtual ~SVCylindricalWarpToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* p_pCreateStructure ) override;
	virtual BOOL CloseObject() override;

	virtual HRESULT ResetObject() override;

	virtual BOOL OnValidate() override;

	SVImageClass* GetInputImage();
	HRESULT ValidateAngle(double &p_dWarpAngle );

	enum WarpType
	{
		WarpTypeInvalid = 0,
		WarpTypeHorizontal = 1,
		WarpTypeVertical = 2,
	};

	virtual bool DoesObjectHaveExtents() const override;

	virtual SVStaticStringValueObjectClass* GetInputImageNames() override;

protected:
	virtual BOOL onRun( SVRunStatusClass &p_rRunStatus ) override;

private:
	void LocalInitialize();
	HRESULT UpdateOutputImageExtents();

	HRESULT LocalCreate();
	HRESULT LocalDestroy();

	HRESULT CreateLUT();
	HRESULT DestroyLUT();
	
	SVInObjectInfoStruct m_InputImageObjectInfo;

	SVImageClass m_OutputImage;
	SVEnumerateValueObjectClass m_svWarpType;
	SVEnumerateValueObjectClass m_svInterpolationMode;
	SVDoubleValueObjectClass m_svWarpAngle;

	SVMatroxBuffer m_LutX;
	SVMatroxBuffer m_LutY;
	// String value object for Source Image Names
	SVStaticStringValueObjectClass m_svSourceImageName;
};

