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
#pragma endregion Includes

#pragma region Declarations
typedef boost::function<HRESULT (SVDataManagerHandle&, SVDataManagerLockTypeEnum)> NextImageHandleIndexFunctor;
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
	HRESULT Assign( const SVCameraInfoStruct& rCameraInfo, SVDataManagerLockTypeEnum LockType );
	HRESULT Assign( SvTl::SVTimeStamp p_StartFrameTS, SvTl::SVTimeStamp p_EndFrameTS, const SVDataManagerHandle& p_rIndexHandle, SVDataManagerLockTypeEnum p_LockType );

	void Reset();
	void ClearInfo();
	void ClearCameraInfo();

	HRESULT GetNextAvailableIndexes( SVDataManagerLockTypeEnum LockType );
	long GetIndex() const;
	const SVDataManagerHandle& GetSourceImageDMIndexHandle() const;
	const void setCamera( const SVGUID& rCameraGuid, NextImageHandleIndexFunctor NextImageHandleIndex );
	const SVGUID& getCameraGuid() const { return m_CameraGuid; };

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
	NextImageHandleIndexFunctor m_NextImageHandleIndex;
	SVDataManagerHandle m_SourceImageDMIndexHandle;
	SVGUID m_CameraGuid;
#pragma endregion Member Variables
};

typedef std::vector<SVCameraInfoStruct> SVCameraInfoStructVector;
typedef std::map<SVGUID, SVCameraInfoStruct> SVGuidSVCameraInfoStructMap;
