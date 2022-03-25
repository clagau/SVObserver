//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
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
void SVCameraInfoStruct::Assign(const CameraInfo& rCameraInfo)
{
	m_cameraInfo = rCameraInfo;
}


void SVCameraInfoStruct::ClearInfo()
{
	m_cameraInfo = {};
}

const SvOi::ITRCImagePtr SVCameraInfoStruct::GetNextImage()
{
	if (nullptr != m_NextImageFunctor)
	{
		m_cameraInfo.m_pImage = m_NextImageFunctor();
	}
	else
	{
		m_cameraInfo.m_pImage.reset();
	}
	return m_cameraInfo.m_pImage;
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
		m_cameraInfo.m_pImage = pImage;
		return true;
	}
	return false;
};
#pragma endregion Public Methods

} //namespace SvIe
