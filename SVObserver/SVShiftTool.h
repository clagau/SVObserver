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
#include "SVOCore/SVImageClass.h"
#include "SVTool.h"
#pragma endregion Includes

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
	SVShiftTool( BOOL BCreateDefaultTaskList = false, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVSHIFTTOOL );
	virtual ~SVShiftTool();
#pragma endregion Constructor

#pragma region Public Methods
	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual bool ResetObject(SvStl::MessageContainerVector *pErrorMessages=nullptr) override;

	virtual HRESULT SetImageExtentToParent( unsigned long p_ulIndex ) override;
	virtual SVStringValueObjectClass* GetInputImageNames() override;

	virtual HRESULT IsInputImage( SVImageClass *p_psvImage ) override;
	virtual SVTaskObjectClass* GetObjectAtPoint( const SVExtentPointStruct &p_rPoint ) override;
	virtual bool DoesObjectHaveExtents() const override;
	//************************************
	//! return Flag with enabled Autosize
	//! \returns  EnableSize  | EnablePosition
	//************************************
	virtual EAutoSize GetAutoSizeEnabled() override;
#pragma endregion Public Methods

protected:
#pragma region Protected Methods
	virtual bool onRun( SVRunStatusClass &p_rRunStatus, SvStl::MessageContainerVector *pErrorMessages=nullptr ) override;
	SVImageClass* GetImageInput() const;
	SVDoubleValueObjectClass* GetTranslationXInput() const;
	SVDoubleValueObjectClass* GetTranslationYInput() const;
#pragma endregion Protected Methods

#pragma region Member Variables
	SVInObjectInfoStruct m_ImageInput;
	SVInObjectInfoStruct m_TranslationYInput;
	SVInObjectInfoStruct m_TranslationXInput;

	SVStringValueObjectClass m_SourceImageName;
	SVLongValueObjectClass m_TranslationX;
	SVLongValueObjectClass m_TranslationY;
	SVDoubleValueObjectClass m_LearnedTranslationX;
	SVDoubleValueObjectClass m_LearnedTranslationY;
	SVDoubleValueObjectClass m_DisplacementX;
	SVDoubleValueObjectClass m_DisplacementY;
	SVDoubleValueObjectClass m_LeftResult;
	SVDoubleValueObjectClass m_TopResult;
	SVImageClass m_OutputImage;
	SVBoolValueObjectClass m_EnableSourceImageExtents; // this value object is obsolete (perhaps it can be removed at some future date)

public:
	SVEnumerateValueObjectClass m_evoShiftMode; // @WARNING:  bad practice making members public
#pragma endregion Member Variables

private:
#pragma region Private Methods
	void LocalInitialize();
	void LocalDestroy();
	void SetAttributeData();
	bool ValidateLocal(SvStl::MessageContainerVector *pErrorMessages=nullptr) const;
#pragma endregion Private Methods
};

