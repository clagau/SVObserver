//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamISGDriver
//* .File Name       : $Workfile:   SVDCamISGDriver.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:36:42  $
//******************************************************************************

#ifndef __SVDCAMISGDRIVER_H__
#define __SVDCAMISGDRIVER_H__
#pragma once

#include "SVDCamStandardDriver.h"
#include <map>

class SVDCamISGDriver : public SVDCamStandardDriver
{
public:
	SVDCamISGDriver();
	virtual ~SVDCamISGDriver();

	virtual HRESULT SetParameters( const SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT SetParameter( const SVDeviceParamWrapper& rw );
	virtual HRESULT SoftwareTrigger();
	virtual HRESULT GoOnline();
	virtual HRESULT GoOffline();

protected:

	void init();	// derived classes must call this at end of constructor
	HRESULT NeutralLutToRawLut( const SVLut& rLut, std::vector<ULONG>& raData );

	std::map<ULONG, ULONG> m_mapRegisterCache;
	//ULONG m_ulCurrentTriggerStrobeMode;

	// These registers are different between the two cameras.
	// set the register location in the constructor of the derived classes.
	ULONG ISG_CNTL1;        // Trigger and strobe control
	ULONG ISG_STAT;         // trigger status
	ULONG ISG_TRGDLY_LO;    // integration delay
	ULONG ISG_TRGDLY_HI;    // integration delay
	ULONG ISG_TRGDLY;		// integration delay
	ULONG ISG_STBADV;       // strobe advance
	ULONG ISG_RTGDLY_LO;    // retrigger delay
	ULONG ISG_RTGDLY_HI;    // retrigger delay
	ULONG ISG_RTGDLY;       // retrigger delay
	ULONG ISG_PWM;          // Pulse Width Mode duty cycle FF=100% 80=50% 00=0.4%
	ULONG ISG_OUTMODE;      // Outmode
	ULONG ISG_VERREG;       // FPGA version register
	ULONG ISG_CLKDIV;       // clock div
	ULONG ISG_INTCNT;       // integration count ( shutter raw ); Lightwise only
	ULONG ISG_STBDLY;       // strobe delay; Lightwise only
	ULONG ISG_VVDLY;        // video valid delay register; Lightwise only??
	ULONG ISG_LUTAD;        // LUT address; Lightwise only
	ULONG ISG_LUTDATA;      // LUT data; Lightwise only
	ULONG ISG_NUM_BUFFERS;  // LightWise only
	ULONG ISG_STBDUR;       // LightWise only

	ULONG ISG_IBIS_DACFINE; // Lightwise only

	// assume that the bit definitions are the same for MHPAS and LightWise for now.
	// Notice that these bit positions do not use the screwy DCAM bits!
	enum SVISG1394CameraRegisterValuesEnum
	{
		ISG_OUTMODE_TestModeBit = BIT2,
		ISG_OUTMODE_TestModeOn  = 1,
		ISG_OUTMODE_TestModeOff = 0,

		ISG_CNTL1_TriggerModeBits = BIT0 | BIT1,
		ISG_CNTL1_TriggerModeLocalTriggerOneShot = 0,
		ISG_CNTL1_TriggerModeLocalTriggerRetriggerable = 1,
		ISG_CNTL1_TriggerModeHostTriggerOneShot = 2,
		ISG_CNTL1_TriggerModeHostTriggerRetriggerable = 3,

		ISG_CNTL1_StrobeModeBits = BIT2 | BIT3,
		ISG_CNTL1_StrobeModeActiveHigh = 0,
		ISG_CNTL1_StrobeModeActiveLow  = 1,
		ISG_CNTL1_StrobeModeAlwaysHigh = 2,
		ISG_CNTL1_StrobeModeAlwaysLow  = 3,

		ISG_CNTL1_HostTrigger_Bit = BIT4,
		ISG_CNTL1_HostTrigger = 1,

		ISG_CNTL1_VideoEnable_Bit = BIT5,
		ISG_CNTL1_TriggerSense_Bit = BIT6,
		ISG_CNTL1_LUT_Enable_Bit = BIT8,

		ISG_CNTL1_OutputFormat_Bits = BIT9 | BIT10 | BIT11 | BIT12,
		ISG_CNTL1_OutputFormat_8Bit = 0,
		ISG_CNTL1_OutputFormat_10_16_Pad_LSB = 1,
		ISG_CNTL1_OutputFormat_10_16_Pad_MSB = 2,
		ISG_CNTL1_OutputFormat_TestPattern = 4,
		ISG_CNTL1_OutputFormat_10bit_NoPad = 8,

		ISG_CNTL1_StrobeDurationEnable_Bit = BIT13,
		ISG_CNTL1_InputTriggerSelect_Bit = BIT14,
		ISG_CNTL1_PixelCorrectionEnable_Bit = BIT15,

	};

};

class SVDCamISGMHPASDriver : public SVDCamISGDriver
{
public:
	SVDCamISGMHPASDriver();
	static SVDCamDriver* CreateNew();
	virtual HRESULT SetParameter( const SVDeviceParamWrapper& rw );
};

class SVDCamISGLightWiseDriver : public SVDCamISGDriver
{
public:
	SVDCamISGLightWiseDriver();
	static SVDCamDriver* CreateNew();
	virtual HRESULT SetParameter( const SVDeviceParamWrapper& rw );
protected:
	void init();
};

#endif	//#ifndef __SVDCAMISGDRIVER_H__

