//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "FilesystemUtilities/FileHelperContainer.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVOLibrary/SVLut.h"
#include "CameraLibrary/SVDeviceParamCollection.h"
#pragma endregion Includes

namespace SvIe
{

struct SVConfigurationAcquisitionDeviceInfoStruct
{
public:
	SVConfigurationAcquisitionDeviceInfoStruct();
	~SVConfigurationAcquisitionDeviceInfoStruct();

	SvFs::FileNameContainer msvFiles;
	SVLightReference msvLightArray;
	SVLut mLut;
	SVDeviceParamCollection mDeviceParams;

};

} //namespace SvIe
