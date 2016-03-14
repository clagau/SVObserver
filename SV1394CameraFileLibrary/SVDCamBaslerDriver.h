//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamBaslerDriver
//* .File Name       : $Workfile:   SVDCamBaslerDriver.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:33:00  $
//******************************************************************************

#ifndef SVDCAMBASLERDRIVER_H
#define SVDCAMBASLERDRIVER_H

#include "SVDCamStandardDriver.h"

class SVDCamBaslerDriver : public SVDCamStandardDriver
{
public:
	SVDCamBaslerDriver();
	virtual ~SVDCamBaslerDriver();

	virtual HRESULT SetParameters( const SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT SetParameter( const SVDeviceParamWrapper& rw );

	virtual HRESULT GetSoftwareTriggerRegisterAndValue(unsigned long& ulRegister, unsigned long& ulValue);

	static SVDCamDriver* CreateNew();

protected:

	enum SVISG1394CameraRegistersEnum
	{
		Basler_BaseAddress_AdvFeatures = 0xF2F00000,
		Basler_TEST_IMAGE = Basler_BaseAddress_AdvFeatures | 0x98,
	};

	enum SVBasler1394CameraRegisterValuesEnum
	{
		Basler_TEST_IMAGE_TestImageBits = DCAMBIT15 | DCAMBIT16 | DCAMBIT17,
		Basler_TEST_IMAGE_Off  = 0,
		Basler_TEST_IMAGE_Image1 = 1,
		Basler_TEST_IMAGE_Image2 = 2,
		Basler_TEST_IMAGE_Image3 = 3,
	};

	virtual unsigned long GetDCAMTriggerOnOff() const;

};

#endif

