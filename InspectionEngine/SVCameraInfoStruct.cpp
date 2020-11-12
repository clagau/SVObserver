//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file SVCameraInfoStruct.cpp
//*****************************************************************************
/// The camera information structure which is used to pass camera relevant 
/// data to other modules
/// 
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVCameraInfoStruct.h"
#include "SVTimerLibrary/SVClock.h"
#pragma endregion Includes

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Public Methods
HRESULT SVCameraInfoStruct::Assign( double p_StartFrameTS, double p_EndFrameTS, const SvOi::ITRCImagePtr pImage)
{
	HRESULT l_Status = S_OK;

	m_StartFrameTimeStamp = p_StartFrameTS;
	m_EndFrameTimeStamp = p_EndFrameTS;

	m_CallbackTimeStamp = SvTl::GetTimeStamp();
	m_pImage = pImage;

	return l_Status;
}


void SVCameraInfoStruct::ClearInfo()
{
	m_StartFrameTimeStamp = 0;
	m_EndFrameTimeStamp = 0;
	//! For Debugging Only
	m_CallbackTimeStamp	= 0;

	ClearIndexes();
}

void SVCameraInfoStruct::ClearCameraInfo()
{
	m_StartFrameTimeStamp = 0;
	m_EndFrameTimeStamp = 0;

	ClearIndexes();
}

const SvOi::ITRCImagePtr SVCameraInfoStruct::GetNextImage()
{
	if (!m_NextImageFunctor.empty())
	{
		m_pImage = m_NextImageFunctor();
	}
	else
	{
		m_pImage = nullptr;
	}
	return m_pImage;
}

const void SVCameraInfoStruct::setCamera(uint32_t cameraId, NextImageHandleFunctor NextImageHandleFunctor)
{
	m_CameraId = cameraId;
	m_NextImageFunctor = NextImageHandleFunctor;
}

bool SVCameraInfoStruct::setImage(SvOi::ITRCImagePtr pImage) 
{
	if (nullptr == pImage || pImage->isValid())
	{
		m_pImage = pImage;
		return true;
	}
	return false;
};
#pragma endregion Public Methods

#pragma region Private Methods
void SVCameraInfoStruct::ClearIndexes()
{
	m_pImage = nullptr;
}
#pragma endregion Private Methods

} //namespace SvIe
