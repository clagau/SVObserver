// ******************************************************************************
// * COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVODataResponseClass
// * .File Name       : $Workfile:   SVODataResponseClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:28:04  $
// ******************************************************************************

#include "stdafx.h"
#include "SVODataResponseClass.h"
#include "SVTimerLibrary/SVClock.h"

/*
This constructor initializes the local attributes.
*/
SVODataResponseClass::SVODataResponseClass()
: mDMHandle()
, mbIsValid( false )
, mbIsComplete( false )
, mpvOwner( NULL )
, m_StartTick( 0.0 )
, m_EndTick( 0.0 )
, mulExtraData( 0 )
{
	m_StartTick = SVClock::GetTimeStamp();
	m_EndTick = SVClock::GetTimeStamp();
}

/*
This constructor initializes the local attributes.
*/
SVODataResponseClass::SVODataResponseClass(const SVODataResponseClass& rResponse)
: mDMHandle( rResponse.mDMHandle )
, mbIsValid( rResponse.mbIsValid )
, mbIsComplete( rResponse.mbIsComplete )
, mpvOwner( rResponse.mpvOwner )
, m_StartTick( rResponse.m_StartTick )
, m_EndTick( rResponse.m_EndTick )
, mulExtraData( rResponse.mulExtraData )
{
}

/*
This destructor is a placeholder and performs no operations.
*/
SVODataResponseClass::~SVODataResponseClass()
{
}

/*
This method returns the current validity state of this object.
*/
bool SVODataResponseClass::IsValid() const
{
	return mbIsValid;
}

/*
This method returns the current completion state of this object.
*/
bool SVODataResponseClass::IsComplete() const
{
	return mbIsComplete;
}

/*
This method sets all the local attributes to the desired defaults.
*/
HRESULT SVODataResponseClass::Reset()
{
	HRESULT hrOk = S_OK;

	mDMHandle.clear();

	mbIsValid = false;
	mbIsComplete = false;

	m_StartTick = SVClock::GetTimeStamp();
	m_EndTick = SVClock::GetTimeStamp();

	mulExtraData = 0;

	return hrOk;
}

/*
This method returns the pointer of the owner object of this object.
*/
HRESULT SVODataResponseClass::GetOwner( void **ppvOwner ) const
{
	HRESULT hrOk = S_OK;

	*ppvOwner = mpvOwner;

	return hrOk;
}

/*
This method sets the pointer of the owner object of this object.
*/
HRESULT SVODataResponseClass::SetOwner( void *pvOwner )
{
	HRESULT hrOk = S_OK;

	mpvOwner = pvOwner;

	return hrOk;
}

/*
This method returns the start time stamp of this object.
*/
HRESULT SVODataResponseClass::GetStartTick( SVClock::SVTimeStamp& p_rTick ) const
{
	HRESULT hrOk = S_OK;

	p_rTick = m_StartTick;

	return hrOk;
}

/*
This method sets the start time stamp of this object.
*/
HRESULT SVODataResponseClass::SetStartTick( const SVClock::SVTimeStamp& p_rTick )
{
	HRESULT hrOk = S_OK;

	m_StartTick = p_rTick;

	return hrOk;
}

/*
This method returns the end time stamp of this object.
*/
HRESULT SVODataResponseClass::GetEndTick( SVClock::SVTimeStamp& p_rTick ) const
{
	HRESULT hrOk = S_OK;

	p_rTick = m_EndTick;

	return hrOk;
}

/*
This method sets the end time stamp of this object.
*/
HRESULT SVODataResponseClass::SetEndTick( const SVClock::SVTimeStamp& p_rTick )
{
	HRESULT hrOk = S_OK;

	m_EndTick = p_rTick;

	return hrOk;
}

/*
This method returns the state of the validity attribute.
*/
HRESULT SVODataResponseClass::GetIsValid( bool &rbIsValid ) const
{
	HRESULT hrOk = S_OK;

	rbIsValid = mbIsValid;

	return hrOk;
}

/*
This method sets the state of the validity attribute.
*/
HRESULT SVODataResponseClass::SetIsValid( bool bIsValid )
{
	HRESULT hrOk = S_OK;

	mbIsValid = bIsValid;

	return hrOk;
}

/*
This method returns the state of the completion attribute.
*/
HRESULT SVODataResponseClass::GetIsComplete( bool &rbIsComplete ) const
{
	HRESULT hrOk = S_OK;

	rbIsComplete = mbIsComplete;

	return hrOk;
}

/*
This method sets the state of the completion attribute.
*/
HRESULT SVODataResponseClass::SetIsComplete( bool bIsComplete )
{
	HRESULT hrOk = S_OK;

	mbIsComplete = bIsComplete;

	return hrOk;
}

/*
This method returns the context data of this object.
*/
HRESULT SVODataResponseClass::GetExtraData(unsigned long &rulData) const
{
	HRESULT hrOk = S_OK;

	rulData = mulExtraData;

	return hrOk;
}

/*
This method sets the context data of this object.
*/
HRESULT SVODataResponseClass::SetExtraData(unsigned long ulData)
{
	HRESULT hrOk = S_OK;

	mulExtraData = ulData;

	return hrOk;
}

