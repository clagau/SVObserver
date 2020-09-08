//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file CameraInfo.h
/// \brief Is the structure containig the camera information
//******************************************************************************

#pragma once

#pragma region Includes
#include "TriggerRecordController/IImage.h"
#pragma endregion Includes

struct CameraInfo
{
	CameraInfo() {}
	CameraInfo(const CameraInfo& rRhs)
	{
		*this = rRhs;
	}

	CameraInfo& operator=(const CameraInfo& rRhs)
	{
		m_pImage = rRhs.m_pImage;
		m_startFrameTime = rRhs.m_startFrameTime;
		m_endFrameTime = rRhs.m_endFrameTime;
		return *this;
	}

	void reset()
	{ 
		m_pImage.reset();
	}

	double m_startFrameTime{ 0.0 };
	double m_endFrameTime{ 0.0 };

	SvTrc::IImagePtr m_pImage{nullptr};
};

