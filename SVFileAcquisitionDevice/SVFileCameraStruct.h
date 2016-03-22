//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileCamera
//* .File Name       : $Workfile:   SVFileCameraStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:28:42  $
//******************************************************************************

#pragma once

#include "SVUtilityLibrary/SVString.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"
#include "SVOMFCLibrary/SVFileAcquisitionLoadingModeEnum.h"

struct SVFileCameraStruct
{
	SVFileAcquisitonLoadingModeEnum mode;
	SVString directory;
	SVString fileName;
	SIZE imageSize;
	SVImageFormatEnum imageFormat; // bit depth basically

	SVFileCameraStruct()
	: mode(ContinuousMode), imageFormat(SVImageFormatMono8)
	{
		imageSize.cx = 640;
		imageSize.cy = 480;
	}
};

