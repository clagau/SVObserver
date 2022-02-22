//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file CameraInfo.h
/// \brief Is the structure containig the camera information
//******************************************************************************

#pragma once

#pragma region Includes
#include "ObjectInterfaces/ITRCImage.h"
#pragma endregion Includes

enum CameraDataEnum : int
{
	ChunkFrameID = 0,
	ChunkTimeStamp,
	ChunkLineStatusAll,
	CameraDataNumber
};


struct CameraInfo
{
	CameraInfo() = default;

	double m_startFrameTimestamp{ 0.0 };
	double m_endFrameTimestamp{ 0.0 };

	SvOi::ITRCImagePtr m_pImage{nullptr};

	std::array<_variant_t, CameraDataEnum::CameraDataNumber> m_cameraData;
};

