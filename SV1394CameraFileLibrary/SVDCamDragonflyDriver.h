//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamDragonflyDriver
//* .File Name       : $Workfile:   SVDCamDragonflyDriver.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:36:36  $
//******************************************************************************

#ifndef SVDCAMDRAGONFLYDRIVER_H
#define SVDCAMDRAGONFLYDRIVER_H

#include "SVDCamStandardDriver.h"

class SVDCamDragonflyDriver : public SVDCamStandardDriver
{
public:
	SVDCamDragonflyDriver();
	virtual ~SVDCamDragonflyDriver();

	virtual HRESULT SetParameters( const SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT SetParameter( const SVDeviceParamWrapper& rw );

	virtual HRESULT GetSoftwareTriggerRegisterAndValue(unsigned long& ulRegister, unsigned long& ulValue);

	static SVDCamDriver* CreateNew();

protected:
	enum SVDragonflyCameraRegistersEnum
	{
		Dragonfly_SOFT_ASYNC_TRIGGER = 0x102C,
		Dragonfly_GPIO_CONTROL    = 0x1100,
		Dragonfly_GPIO_XTRA       = 0x1104,
		Dragonfly_SHUTTER_DELAY   = 0x1108,
		Dragonfly_GPIO_CTRL_PIN_0 = 0x1110,
		Dragonfly_GPIO_XTRA_PIN_0 = 0x1114,
		Dragonfly_GPIO_CTRL_PIN_1 = 0x1120,
		Dragonfly_GPIO_XTRA_PIN_1 = 0x1124,
		Dragonfly_GPIO_CTRL_PIN_2 = 0x1130,
		Dragonfly_GPIO_XTRA_PIN_2 = 0x1134,
	};

	enum SVDragonflyCameraRegisterValuesEnum
	{
		Dragonfly_GPIO_XTRA_StrobeStart_Bit  = DCAMBIT0,
		Dragonfly_GPIO_XTRA_TriggerQueue_Bit = DCAMBIT1,
		Dragonfly_GPIO_XTRA_StrobeMultiplier_Bits = DCAM_BYTE3,
		Dragonfly_SHUTTER_DELAY_ShutterDelay_Bits = DCAM_HIWORD,
		Dragonfly_GPIO_Pin_Mode_Bits = DCAMBITS_12_15,
		Dragonfly_GPIO_Pin_Mode_Input = 0,
		Dragonfly_GPIO_Pin_Mode_Output = 1,
		Dragonfly_GPIO_Pin_Mode_AsyncTrigger = 2,
		Dragonfly_GPIO_Pin_Mode_Strobe = 3,
	};

	union FloatLongConvert
	{
		ULONG ulValue;
		float fValue;
	};

	ULONG m_ulGPIO_XTRA;

};

#endif

