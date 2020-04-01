//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file SVCameraInfoStruct.h
//*****************************************************************************
/// The camera information structure which is used to pass camera relevant 
/// data to other modules
/// 
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <boost/function.hpp>
#include "SVUtilityLibrary/SVGUID.h"
#include "TriggerRecordController/IImage.h"
#pragma endregion Includes

namespace SvIe
{

#pragma region Declarations
typedef boost::function<SvTrc::IImagePtr ()> NextImageHandleFunctor;
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
	HRESULT Assign( double p_StartFrameTS, double p_EndFrameTS, const SvTrc::IImagePtr pImage );

	void ClearInfo();
	void ClearCameraInfo();

	const SvTrc::IImagePtr GetNextImage();
	const void setCamera( const SVGUID& rCameraGuid, NextImageHandleFunctor NextImageHandleFunctor );
	const SVGUID& getCameraGuid() const { return m_CameraGuid; };
	const SvTrc::IImagePtr getImage() const { return m_pImage; };
	bool setImage(SvTrc::IImagePtr pImage);

	double m_StartFrameTimeStamp {0.0};
	double m_EndFrameTimeStamp {0.0};
	double m_CallbackTimeStamp {0.0};
#pragma endregion Public Methods

protected:
#pragma region Private Methods
private:
	void ClearIndexes();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	NextImageHandleFunctor m_NextImageFunctor;
	SVGUID m_CameraGuid {GUID_NULL};
	SvTrc::IImagePtr m_pImage {nullptr};
#pragma endregion Member Variables
};

typedef std::vector<SVCameraInfoStruct> SVCameraInfoStructVector;
typedef std::unordered_map<GUID, SVCameraInfoStruct> SVGuidSVCameraInfoStructMap;

} //namespace SvIe
