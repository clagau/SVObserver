//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVTool.h"
#include "SVObjectLibrary/InputObject.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

namespace SvTo
{

class SVCylindricalWarpToolClass : public SVToolClass  
{
	SV_DECLARE_CLASS

public:
	SVCylindricalWarpToolClass( SVObjectClass* p_pOwner = nullptr, int p_iStringResourceID = IDS_CLASSNAME_SVCYLINDRICALTOOL );
	virtual ~SVCylindricalWarpToolClass();

	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool CloseObject() override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual std::vector<std::string> getToolAdjustNameList() const override;

	HRESULT ValidateAngle(double &p_dWarpAngle );

	enum WarpType
	{
		WarpTypeInvalid = 0,
		WarpTypeHorizontal = 1,
		WarpTypeVertical = 2,
	};

	virtual bool DoesObjectHaveExtents() const override;

	virtual SvVol::SVStringValueObjectClass* GetInputImageNames() override;

protected:
	virtual bool onRun( SvIe::RunStatus &p_rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;

private:
	void LocalInitialize();
	HRESULT UpdateOutputImageExtents();

	HRESULT LocalCreate();
	HRESULT LocalDestroy();

	bool CreateLUT();
	HRESULT DestroyLUT();
	
	SvOl::InputObject m_InputImage;

	SvIe::SVImageClass m_OutputImage;
	SvVol::SVEnumerateValueObjectClass m_svWarpType;
	SvVol::SVEnumerateValueObjectClass m_svInterpolationMode;
	SvVol::SVDoubleValueObjectClass m_svWarpAngle;

	SVMatroxBuffer m_LutX;
	SVMatroxBuffer m_LutY;
	// String value object for Source Image Names
	SvVol::SVStringValueObjectClass m_SourceImageNames;
};

} //namespace SvTo
