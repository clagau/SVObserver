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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SV1394CameraFileLibrary\SVDCamISGDriver.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:36:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Nov 2009 18:39:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated drivers to fix the software trigger functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 14:57:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   15 Jul 2004 09:19:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  436
 * SCR Title:  Update ISG camera handler
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented: StrobePulseDuration, InputTriggerSelect, PixelCorrectionEnable, NumCameraBuffers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   31 Oct 2003 08:22:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CanSoftwareTrigger and various ISG temp register settings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Oct 2003 09:29:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added code to compensate for column intensity variation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   14 Oct 2003 09:19:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated for LightWise camera
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Oct 2003 10:37:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixes & updates
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Sep 2003 11:50:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   split MHPAS and LightWise to support different register locations
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Sep 2003 14:56:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Lut code - this is not complete as of yet.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Sep 2003 09:01:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   general changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Aug 2003 10:11:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jul 2003 08:44:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/