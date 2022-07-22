//*****************************************************************************
/// \copyright (c) 2016,2016 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file SVCameraInfoStruct.h
//*****************************************************************************
/// The camera information structure which is used to pass camera relevant 
/// data to other modules
/// 
//******************************************************************************
#pragma once

#pragma region Includes
#include "Definitions/ObjectDefines.h"
#include "ObjectInterfaces/ITRCImage.h"
#include "SVOLibrary/CameraInfo.h"
#pragma endregion Includes

namespace SvIe
{

#pragma region Declarations
using NextImageHandleFunctor = std::function<SvOi::ITRCImagePtr(void)>;
#pragma endregion Declarations

struct SVCameraInfoStruct 
{
#pragma region Constructor
public:
	SVCameraInfoStruct() = default;
	SVCameraInfoStruct(const SVCameraInfoStruct &rRhs) = default;
	virtual ~SVCameraInfoStruct() = default;

	SVCameraInfoStruct &operator=(const SVCameraInfoStruct& rRhs) = default;
	SVCameraInfoStruct &operator=(SVCameraInfoStruct&& rRhs) = default;
#pragma endregion Constructor

#pragma region Public Methods
public:
	void Assign(const CameraInfo& rCameraInfo);

	void ClearInfo(bool clearImage = false);

	const SvOi::ITRCImagePtr GetNextImage();
	const void setCamera(uint32_t cameraId, NextImageHandleFunctor NextImageHandleFunctor);
	uint32_t getCameraId() const { return m_CameraId; };
	const SvOi::ITRCImagePtr getImage() const { return m_cameraInfo.m_pImage; };
	bool setImage(SvOi::ITRCImagePtr pImage);
	const CameraInfo& getCameraInfo() const { return m_cameraInfo; }

#pragma endregion Public Methods

#pragma region Private Methods
private:
#pragma endregion Private Methods

#pragma region Member Variables
private:
	NextImageHandleFunctor m_NextImageFunctor;
	uint32_t m_CameraId{ SvDef::InvalidObjectId };

	CameraInfo m_cameraInfo;
#pragma endregion Member Variables
};

typedef std::map<uint32_t, SVCameraInfoStruct> SVObjectIdSVCameraInfoStructMap;

} //namespace SvIe
