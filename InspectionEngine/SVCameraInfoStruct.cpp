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
 m_CameraGuid( SV_GUID_NULL )
,m_SourceImageDMIndexHandle()
,m_StartFrameTimeStamp( 0 )
,m_EndFrameTimeStamp( 0 )
,m_CallbackTimeStamp( 0 )
{
}

SVCameraInfoStruct::SVCameraInfoStruct( const SVCameraInfoStruct &rRhs ) :
m_CameraGuid( SV_GUID_NULL )
,m_SourceImageDMIndexHandle()
,m_StartFrameTimeStamp( 0 )
,m_EndFrameTimeStamp( 0 )
,m_CallbackTimeStamp( 0 )
{
	Assign( rRhs, rRhs.m_SourceImageDMIndexHandle.GetLockType() );
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
		m_NextImageHandleIndex = rRhs.m_NextImageHandleIndex;
		m_StartFrameTimeStamp = rRhs.m_StartFrameTimeStamp;
		m_EndFrameTimeStamp = rRhs.m_EndFrameTimeStamp;
		m_CallbackTimeStamp = rRhs.m_CallbackTimeStamp;

		m_SourceImageDMIndexHandle.Assign( rRhs.m_SourceImageDMIndexHandle, rRhs.m_SourceImageDMIndexHandle.GetLockType() );
	}

	return *this;
}
#pragma endregion Constructor

#pragma region Public Methods
HRESULT SVCameraInfoStruct::Assign( const SVCameraInfoStruct &rCameraInfo, SVDataManagerLockTypeEnum LockType )
{
	HRESULT l_Status = S_OK;

	if( this != &rCameraInfo )
	{
		m_CameraGuid = rCameraInfo.getCameraGuid();
		m_NextImageHandleIndex = rCameraInfo.m_NextImageHandleIndex;
		m_StartFrameTimeStamp = rCameraInfo.m_StartFrameTimeStamp;
		m_EndFrameTimeStamp = rCameraInfo.m_EndFrameTimeStamp;
		m_CallbackTimeStamp = rCameraInfo.m_CallbackTimeStamp;

		l_Status = m_SourceImageDMIndexHandle.Assign( rCameraInfo.m_SourceImageDMIndexHandle, LockType );

		if( m_SourceImageDMIndexHandle.GetIndex() != rCameraInfo.m_SourceImageDMIndexHandle.GetIndex() )
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVCameraInfoStruct::Assign( SvTl::SVTimeStamp p_StartFrameTS, SvTl::SVTimeStamp p_EndFrameTS, const SVDataManagerHandle& p_rIndexHandle, SVDataManagerLockTypeEnum p_LockType )
{
	HRESULT l_Status = S_OK;

	assert( m_SourceImageDMIndexHandle.GetIndex() < 0 || ( 0 <= m_SourceImageDMIndexHandle.GetIndex() && 0 <= p_rIndexHandle.GetIndex() ) );

	m_StartFrameTimeStamp = p_StartFrameTS;
	m_EndFrameTimeStamp = p_EndFrameTS;

	m_CallbackTimeStamp = SvTl::GetTimeStamp();

	l_Status = m_SourceImageDMIndexHandle.Assign( p_rIndexHandle, p_LockType );

	if( m_SourceImageDMIndexHandle.GetIndex() != p_rIndexHandle.GetIndex() )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}


void SVCameraInfoStruct::Reset()
{
	m_CameraGuid = SV_GUID_NULL;
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

HRESULT SVCameraInfoStruct::GetNextAvailableIndexes( SVDataManagerLockTypeEnum LockType )
{
	HRESULT Result( S_OK );

	if( !m_NextImageHandleIndex.empty() )
	{
		Result = m_NextImageHandleIndex( m_SourceImageDMIndexHandle, LockType );
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

long SVCameraInfoStruct::GetIndex() const
{
	return m_SourceImageDMIndexHandle.GetIndex();
}

const SVDataManagerHandle& SVCameraInfoStruct::GetSourceImageDMIndexHandle() const
{
	return m_SourceImageDMIndexHandle;
}

const void SVCameraInfoStruct::setCamera( const SVGUID& rCameraGuid, NextImageHandleIndexFunctor NextImageHandleIndex )
{
	m_CameraGuid = rCameraGuid;
	m_NextImageHandleIndex = NextImageHandleIndex;
}
#pragma endregion Public Methods

#pragma region Private Methods
void SVCameraInfoStruct::ClearIndexes()
{
	m_SourceImageDMIndexHandle.clear();
}
#pragma endregion Private Methods
