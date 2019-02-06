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
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVTimerLibrary/SVClock.h"
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
	SVCameraInfoStruct();
	SVCameraInfoStruct( const SVCameraInfoStruct &rRhs );
	virtual ~SVCameraInfoStruct();
	const SVCameraInfoStruct &operator=( const SVCameraInfoStruct &rRhs );
#pragma endregion Constructor

#pragma region Public Methods
public:
	HRESULT Assign( const SVCameraInfoStruct& rCameraInfo );
	HRESULT Assign( SvTl::SVTimeStamp p_StartFrameTS, SvTl::SVTimeStamp p_EndFrameTS, const SvTrc::IImagePtr pImage );

	void Reset();
	void ClearInfo();
	void ClearCameraInfo();

	const SvTrc::IImagePtr GetNextImage();
	const void setCamera( const SVGUID& rCameraGuid, NextImageHandleFunctor NextImageHandleFunctor );
	const SVGUID& getCameraGuid() const { return m_CameraGuid; };
	const SvTrc::IImagePtr getImage() const { return m_pImage; };
	bool setImage(SvTrc::IImagePtr pImage);

	SvTl::SVTimeStamp m_StartFrameTimeStamp;
	SvTl::SVTimeStamp m_EndFrameTimeStamp;
	SvTl::SVTimeStamp m_CallbackTimeStamp;
#pragma endregion Public Methods

protected:
#pragma region Private Methods
private:
	void ClearIndexes();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	NextImageHandleFunctor m_NextImageFunctor;
	SVGUID m_CameraGuid;
	SvTrc::IImagePtr m_pImage;
#pragma endregion Member Variables
};

typedef std::vector<SVCameraInfoStruct> SVCameraInfoStructVector;
typedef std::map<SVGUID, SVCameraInfoStruct> SVGuidSVCameraInfoStructMap;

} //namespace SvIe
