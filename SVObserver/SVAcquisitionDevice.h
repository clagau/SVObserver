//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionDevice.h
//* .File Name       : $Workfile:   SVAcquisitionDevice.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:24:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVFileSystemLibrary/SVFileNameArrayClass.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVImageLibrary/SVLut.h"
#include "CameraLibrary/SVDeviceParamCollection.h"
#pragma endregion Includes

struct SVConfigurationAcquisitionDeviceInfoStruct
{
public:
	SVConfigurationAcquisitionDeviceInfoStruct();
	~SVConfigurationAcquisitionDeviceInfoStruct();

	SVFileNameArrayClass msvFiles;
	SVLightReference msvLightArray;
	SVLut mLut;
	SVDeviceParamCollection mDeviceParams;

};

