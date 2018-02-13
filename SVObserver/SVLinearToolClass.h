// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVLinearToolClass.h
// * .File Name       : $Workfile:   SVLinearToolClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   09 Dec 2014 09:47:26  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "InspectionEngine/SVTool.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#pragma endregion Includes

#pragma region Declarations
const SvOi::NameValueVector cOrientationEnums{ {_T("Horizontal"), 0}, {_T("Vertical"), 90}};
#pragma endregion Declarations

class SVLinearToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVLinearToolClass );

public:
#pragma region Constructor
	SVLinearToolClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLINEARTOOL );
	virtual ~SVLinearToolClass();
#pragma endregion Constructor

#pragma region Public Methods
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;

	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT SetImageExtentToParent() override;
	virtual SVTaskObjectClass *GetObjectAtPoint( const SVExtentPointStruct &p_rsvPoint ) override;
	virtual bool DoesObjectHaveExtents() const override;

	bool GetRotation();

	//************************************
	//! return Flag with enabled Autosize
	//! \returns  EnableSize  | EnablePosition
	//************************************
	virtual EAutoSize GetAutoSizeEnabled() override;

	virtual SVStringValueObjectClass* GetInputImageNames() override;
#pragma endregion Public Methods

protected:
#pragma region Protected Methods
#pragma endregion Protected Methods

	SVDoubleValueObjectClass m_svRotationAngle;
	SVDoubleValueObjectClass m_svRotationPointX;
	SVDoubleValueObjectClass m_svRotationPointY;

private:
#pragma region Private Methods
	void init();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	SVStringValueObjectClass m_SourceImageNames;
	//Embedded Object : Profile Orientation
	SVEnumerateValueObjectClass m_voProfileOrientation;
	SVBoolValueObjectClass m_voUseProfileRotation;
#pragma endregion Member Variables
};

