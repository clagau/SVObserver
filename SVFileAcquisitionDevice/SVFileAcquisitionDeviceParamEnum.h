//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionDeviceParamEnum
//* .File Name       : $Workfile:   SVFileAcquisitionDeviceParamEnum.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:28:52  $
//******************************************************************************

#pragma once

enum SVFileAcquisitionDeviceParamEnum
{
	SVFileAcquisitionParameterFilename,
	SVFileAcquisitionParameterDirectory,
	SVFileAcquisitionParameterLoadingMode,
	SVFileAcquisitionParameterImageWidth,
	SVFileAcquisitionParameterImageHeight,
	SVFileAcquisitionParameterImageFormat,
	SVFileAcquisitionParameterMaxPreloadFileNumber,
	SVFileAcquisitionParameterPreloadTimeDelay,
	SVFileAcquisitionParameterSize
};

constexpr const char* SVFileAcquisitionParameterNames[SVFileAcquisitionParameterSize] =
{
	_T("Filename"),
	_T("Directory"),
	_T("File Loading Mode"),
	_T("Image Width"),
	_T("Image Height"),
	_T("Image Format"),
	_T("Max Preload FileNumber"),
	_T("PreloadTimeDelay")
};
