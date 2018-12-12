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
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
SVCameraInfoStruct::SVCameraInfoStruct() :
 m_CameraGuid( GUID_NULL )
,m_StartFrameTimeStamp( 0 )
,m_EndFrameTimeStamp( 0 )
,m_CallbackTimeStamp( 0 )
,m_pImage(nullptr)
{
}

SVCameraInfoStruct::SVCameraInfoStruct( const SVCameraInfoStruct &rRhs ) :
m_CameraGuid( GUID_NULL )
,m_StartFrameTimeStamp( 0 )
,m_EndFrameTimeStamp( 0 )
,m_CallbackTimeStamp( 0 )
,m_pImage(nullptr)
{
	Assign( rRhs );
}

SVCameraInfoStruct::~SVCameraInfoStruct()
{
	Reset();
}

const SVCameraInfoStruct &SVCameraInfoStruct::operator=( const SVCameraInfoStruct &rRhs )
{
	if( this != &rRhs )
	{
		m_CameraGuid = rRhs.getCameraGuid();
		m_NextImageFunctor = rRhs.m_NextImageFunctor;
		m_StartFrameTimeStamp = rRhs.m_StartFrameTimeStamp;
		m_EndFrameTimeStamp = rRhs.m_EndFrameTimeStamp;
		m_CallbackTimeStamp = rRhs.m_CallbackTimeStamp;
		m_pImage = rRhs.m_pImage;
	}

	return *this;
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT SVCameraInfoStruct::Assign( const SVCameraInfoStruct &rCameraInfo )
{
	HRESULT l_Status = S_OK;

	if( this != &rCameraInfo )
	{
		m_CameraGuid = rCameraInfo.getCameraGuid();
		m_NextImageFunctor = rCameraInfo.m_NextImageFunctor;
		m_StartFrameTimeStamp = rCameraInfo.m_StartFrameTimeStamp;
		m_EndFrameTimeStamp = rCameraInfo.m_EndFrameTimeStamp;
		m_CallbackTimeStamp = rCameraInfo.m_CallbackTimeStamp;
		m_pImage = rCameraInfo.m_pImage;
	}

	return l_Status;
}

HRESULT SVCameraInfoStruct::Assign( SvTl::SVTimeStamp p_StartFrameTS, SvTl::SVTimeStamp p_EndFrameTS, const SvTrc::IImagePtr pImage)
{
	HRESULT l_Status = S_OK;

	m_StartFrameTimeStamp = p_StartFrameTS;
	m_EndFrameTimeStamp = p_EndFrameTS;

	m_CallbackTimeStamp = SvTl::GetTimeStamp();
	m_pImage = pImage;

	return l_Status;
}


void SVCameraInfoStruct::Reset()
{
	m_CameraGuid = GUID_NULL;
	m_StartFrameTimeStamp = 0;
	m_EndFrameTimeStamp = 0;
	//! For Debugging Only
	m_CallbackTimeStamp	= 0;

	ClearIndexes();
}// end Reset

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

const SvTrc::IImagePtr SVCameraInfoStruct::GetNextImage()
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

const void SVCameraInfoStruct::setCamera( const SVGUID& rCameraGuid, NextImageHandleFunctor NextImageHandleFunctor )
{
	m_CameraGuid = rCameraGuid;
	m_NextImageFunctor = NextImageHandleFunctor;
}

bool SVCameraInfoStruct::setImage(SvTrc::IImagePtr pImage) 
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
