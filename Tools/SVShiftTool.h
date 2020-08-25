//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVShiftTool
//* .File Name       : $Workfile:   SVShiftTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   09 Dec 2014 09:52:44  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVValueObjectLibrary/SVBoolValueObjectClass.h"
#include "SVValueObjectLibrary/SVDoubleValueObjectClass.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#include "SVValueObjectLibrary/SVStringValueObjectClass.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVTool.h"
#pragma endregion Includes

namespace SvTo
{

#pragma region Declarations
enum SV_SHIFT_ENUM
{
	SV_SHIFT_NONE		= 0,
	SV_SHIFT_ABSOLUTE	= 1,
	SV_SHIFT_REFERENCE	= 2
};

#pragma endregion Declarations

class SVShiftTool : public SVToolClass
{
	SV_DECLARE_CLASS( SVShiftTool );

public:
#pragma region Constructor
	SVShiftTool( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVSHIFTTOOL );
	virtual ~SVShiftTool();
#pragma endregion Constructor

#pragma region Public Methods
	virtual bool CreateObject( const SVObjectLevelCreateStruct& rCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT SetImageExtentToParent() override;
	virtual SvVol::SVStringValueObjectClass* GetInputImageNames() override;

	virtual SVToolClass* GetObjectAtPoint(const SVPoint<double>& rPoint) override;
	virtual bool DoesObjectHaveExtents() const override;
	//************************************
	//! return Flag with enabled Autosize
	//! \returns  EnableSize  | EnablePosition
	//************************************
	virtual SvPb::EAutoSize GetAutoSizeEnabled() const override;

	//These need to be public so that SVShiftToolUtility can access the values
	SvVol::SVDoubleValueObjectClass* GetTranslationXInput(bool bRunMode = false) const;
	SvVol::SVDoubleValueObjectClass* GetTranslationYInput(bool bRunMode = false) const;

	virtual void addOverlays(const SvIe::SVImageClass* pImage, SvPb::OverlayDesc& rOverlay) const override;

#pragma endregion Public Methods

protected:
#pragma region Protected Methods
	virtual bool isInputImage(uint32_t imageId) const override;
	virtual bool onRun( RunStatus &p_rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
#pragma endregion Protected Methods

#pragma region Member Variables
	SvOl::SVInObjectInfoStruct m_ImageInput;
	SvOl::SVInObjectInfoStruct m_TranslationYInput;
	SvOl::SVInObjectInfoStruct m_TranslationXInput;

	SvVol::SVStringValueObjectClass m_SourceImageName;
	SvVol::SVLongValueObjectClass m_TranslationX;
	SvVol::SVLongValueObjectClass m_TranslationY;
	SvVol::SVDoubleValueObjectClass m_LearnedTranslationX;
	SvVol::SVDoubleValueObjectClass m_LearnedTranslationY;
	SvVol::SVDoubleValueObjectClass m_DisplacementX;
	SvVol::SVDoubleValueObjectClass m_DisplacementY;
	SvVol::SVDoubleValueObjectClass m_LeftResult;
	SvVol::SVDoubleValueObjectClass m_TopResult;
	SvIe::SVImageClass m_OutputImage;
	SvVol::SVBoolValueObjectClass m_EnableSourceImageExtents; // this value object is obsolete (perhaps it can be removed at some future date)

public:
	SvVol::SVEnumerateValueObjectClass m_evoShiftMode; // @WARNING:  bad practice making members public
#pragma endregion Member Variables

private:
#pragma region Private Methods
	void LocalInitialize();
	void LocalDestroy();
	void SetAttributeData();
	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;
#pragma endregion Private Methods
};

} //namespace SvTo
