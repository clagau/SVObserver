//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVColorTool
//* .File Name       : $Workfile:   SVColorTool.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   26 Jun 2014 17:26:46  $
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVImageClass.h"
#include "SVTool.h"
#pragma endregion Includes

class SVColorToolClass : public SVToolClass
{
	SV_DECLARE_CLASS( SVColorToolClass );

public:
	SVColorToolClass( BOOL BCreateDefaultTaskList = FALSE, SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVCOLORTOOL );

	virtual ~SVColorToolClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* PCreateStructure ) override;
	virtual BOOL CloseObject() override;

	virtual HRESULT ResetObject() override;

	virtual bool DoesObjectHaveExtents() const override;
	virtual SVStaticStringValueObjectClass* GetInputImageNames() override;

	SVImageClass* GetRGBImage();
	SVImageClass* GetHSIImage();

	SVBoolValueObjectClass* GetConvertToHSIVariable();

protected:
	virtual HRESULT UpdateImageWithExtent( unsigned long p_Index ) override;
	virtual BOOL onRun( SVRunStatusClass& RRunStatus ) override;
	virtual DWORD_PTR processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext ) override;

	BOOL createBandChildLayer( SVImageClass& p_rOutputImage, SVImageClass* p_pInputImage, long p_BandLink );

	// Embeddeds
	SVRGBImageClass band0Image;
	SVRGBImageClass band1Image;
	SVRGBImageClass band2Image;

	SVBoolValueObjectClass convertToHSI;
	// String value object for Source Image Names
	SVStaticStringValueObjectClass m_svSourceImageNames;

private:
	void init();
	HRESULT CollectInputImageNames();
};

