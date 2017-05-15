//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferInterface
//* .File Name       : $Workfile:   SVImageBufferInterface.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   07 May 2013 20:14:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVImageBufferInterface.h"

SVImageBufferInterface::SVImageBufferInterface()
:	m_StartTimeStamp( 0 ),
	m_EndTimeStamp( 0 )
{
}

SVImageBufferInterface::SVImageBufferInterface( const SVImageBufferInterface& p_rObject )
:	m_ImageHandle( p_rObject.m_ImageHandle ),
	m_StartTimeStamp( p_rObject.m_StartTimeStamp ),
	m_EndTimeStamp( p_rObject.m_EndTimeStamp )
{
	m_IndexHandle.Assign( p_rObject.m_IndexHandle, p_rObject.m_IndexHandle.GetLockType() );
}

SVImageBufferInterface::SVImageBufferInterface( SVSmartHandlePointer p_ImageHandle, const SVDataManagerHandle& p_rIndexHandle )
:	m_ImageHandle( p_ImageHandle ),
	m_StartTimeStamp( 0 ),
	m_EndTimeStamp( 0 )
{
	m_IndexHandle.Assign( p_rIndexHandle, p_rIndexHandle.GetLockType() );
}

SVImageBufferInterface::~SVImageBufferInterface()
{
}

const SVImageBufferInterface& SVImageBufferInterface::operator=( const SVImageBufferInterface& p_rObject )
{
	if( this != &p_rObject )
	{
		m_ImageHandle = p_rObject.m_ImageHandle;
		m_IndexHandle.Assign( p_rObject.m_IndexHandle, p_rObject.m_IndexHandle.GetLockType() );
		m_StartTimeStamp = p_rObject.m_StartTimeStamp;
		m_EndTimeStamp = p_rObject.m_EndTimeStamp;
	}

	return *this;
}

void SVImageBufferInterface::clear()
{
	m_ImageHandle.clear();
	m_IndexHandle.clear();
	m_StartTimeStamp = 0;
	m_EndTimeStamp = 0;
}

HRESULT SVImageBufferInterface::Assign( SVSmartHandlePointer p_ImageHandle, const SVDataManagerHandle& p_rIndexHandle )
{
	HRESULT l_Status = S_OK;

	m_StartTimeStamp = 0;
	m_EndTimeStamp = 0;

	m_ImageHandle = p_ImageHandle;

	l_Status = m_IndexHandle.Assign( p_rIndexHandle, p_rIndexHandle.GetLockType() );

	return l_Status;
}

unsigned char* SVImageBufferInterface::GetBufferAddress() const
{
	unsigned char* l_pBuffer = nullptr;

	if( ! m_ImageHandle.empty() )
	{
		l_pBuffer = reinterpret_cast< unsigned char* >( m_ImageHandle->GetBufferAddress() );
	}

	return l_pBuffer;
}

const SvTl::SVTimeStamp& SVImageBufferInterface::GetStartFrameTimeStamp() const
{
	return m_StartTimeStamp;
}

void SVImageBufferInterface::SetStartFrameTimeStamp()
{
	m_StartTimeStamp = SvTl::GetTimeStamp();
}

void SVImageBufferInterface::SetStartFrameTimeStamp( const SvTl::SVTimeStamp& p_rTimeStamp )
{
	m_StartTimeStamp = p_rTimeStamp;
}

const SvTl::SVTimeStamp& SVImageBufferInterface::GetEndFrameTimeStamp() const
{
	return m_EndTimeStamp;
}

void SVImageBufferInterface::SetEndFrameTimeStamp()
{
	m_EndTimeStamp = SvTl::GetTimeStamp();
}

void SVImageBufferInterface::SetEndFrameTimeStamp( const SvTl::SVTimeStamp& p_rTimeStamp )
{
	m_EndTimeStamp = p_rTimeStamp;
}

