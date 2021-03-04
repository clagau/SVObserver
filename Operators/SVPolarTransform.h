//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPolarTransform
//* .File Name       : $Workfile:   SVPolarTransform.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 11:56:12  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "InspectionEngine/SVTaskObject.h"
#include "SVObjectLibrary/InputObject.h"
#pragma endregion Includes

namespace SvOp
{

class SVPolarTransform : public SvIe::SVTaskObjectClass
{
	SV_DECLARE_CLASS

public:
	SVPolarTransform( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVPOLARTRANSFORM );
	virtual ~SVPolarTransform();

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

protected:
	// Inputs
	SvOl::InputObject m_inputCenterXResult;
	SvOl::InputObject m_inputCenterYResult;
	
	SvOl::InputObject m_inputStartRadiusResult;
	SvOl::InputObject m_inputEndRadiusResult;

	SvOl::InputObject m_inputStartAngleResult;
	SvOl::InputObject m_inputEndAngleResult;
};

} //namespace SvOp
