// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOResponseClass
// * .File Name       : $Workfile:   SVOResponseClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:28:18  $
// ******************************************************************************

#include "stdafx.h"
#include "SVOResponseClass.h"

/*
This constructor initializes the local attributes and calls the Reset method.
*/
SVOResponseClass::SVOResponseClass()
{
	mpvOwner = nullptr;

	Reset();
}

/*
This constructor initializes the local attributes, calls the Reset method, and calls the assignment operator.
*/
SVOResponseClass::SVOResponseClass(const SVOResponseClass &rResponse)
{
	mpvOwner = nullptr;

	Reset();

	*this = rResponse;
}

/*
This destructor is a placeholder and performs no operations.
*/
SVOResponseClass::~SVOResponseClass()
{
}

/*
This method returns the current validity state of this object.
*/
bool SVOResponseClass::IsValid() const
{
	return mbIsValid;
}

/*
This method returns the current completion state of this object.
*/
bool SVOResponseClass::IsComplete() const
{
	return mbIsComplete;
}

/*
This operator assigns the data from the provided object into this object.
*/
const SVOResponseClass &SVOResponseClass::operator=(const SVOResponseClass &rResponse)
{
	mbIsValid = rResponse.mbIsValid;
	mbIsComplete = rResponse.mbIsComplete;
	mDMHandle.Assign( rResponse.mDMHandle, rResponse.mDMHandle.GetLockType() );
	mpvOwner = rResponse.mpvOwner;
	mExtraData = rResponse.mExtraData;
	m_StartTick = rResponse.m_StartTick;
	m_EndTick = rResponse.m_EndTick;

	return *this;
}

/*
This method sets all the local attributes to the desired defaults.
*/
HRESULT SVOResponseClass::Reset()
{
	HRESULT hrOk = S_OK;

	mbIsValid = FALSE;
	mbIsComplete = FALSE;
	mDMHandle.clear();
	mExtraData.swap(boost::any());

	m_StartTick = SvTl::GetTimeStamp();
	m_EndTick = SvTl::GetTimeStamp();

	return hrOk;
}

/*
This method returns the pointer of the owner object of this object.
*/
HRESULT SVOResponseClass::GetOwner( void **ppvOwner )
{
	HRESULT hrOk = S_OK;

	*ppvOwner = mpvOwner;

	return hrOk;
}

/*
This method sets the pointer of the owner object of this object.
*/
HRESULT SVOResponseClass::SetOwner( void *pvOwner )
{
	HRESULT hrOk = S_OK;

	mpvOwner = pvOwner;

	return hrOk;
}

/*
This method returns the start time stamp of this object.
*/
HRESULT SVOResponseClass::GetStartTick( SvTl::SVTimeStamp& p_rTick )
{
	HRESULT hrOk = S_OK;

	p_rTick = m_StartTick;

	return hrOk;
}

/*
This method sets the start time stamp of this object.
*/
HRESULT SVOResponseClass::SetStartTick( const SvTl::SVTimeStamp& p_rTick )
{
	HRESULT hrOk = S_OK;

	m_StartTick = p_rTick;

	return hrOk;
}

/*
This method returns the end time stamp of this object.
*/
HRESULT SVOResponseClass::GetEndTick( SvTl::SVTimeStamp& p_rTick )
{
	HRESULT hrOk = S_OK;

	p_rTick = m_EndTick;

	return hrOk;
}

/*
This method sets the end time stamp of this object.
*/
HRESULT SVOResponseClass::SetEndTick( const SvTl::SVTimeStamp& p_rTick )
{
	HRESULT hrOk = S_OK;

	m_EndTick = p_rTick;

	return hrOk;
}

/*
This method returns the state of the validity attribute.
*/
HRESULT SVOResponseClass::GetIsValid( bool &rbIsValid ) const
{
	HRESULT hrOk = S_OK;

	rbIsValid = mbIsValid;

	return hrOk;
}

/*
This method sets the state of the validity attribute.
*/
HRESULT SVOResponseClass::SetIsValid( bool bIsValid )
{
	HRESULT hrOk = S_OK;

	mbIsValid = bIsValid;

	return hrOk;
}

/*
This method returns the state of the completion attribute.
*/
HRESULT SVOResponseClass::GetIsComplete( bool &rbIsComplete )
{
	HRESULT hrOk = S_OK;

	rbIsComplete = mbIsComplete;

	return hrOk;
}

/*
This method sets the state of the completion attribute.
*/
HRESULT SVOResponseClass::SetIsComplete( bool bIsComplete )
{
	HRESULT hrOk = S_OK;

	mbIsComplete = bIsComplete;

	return hrOk;
}

/*
This method returns the context data of this object.
*/
HRESULT SVOResponseClass::GetExtraData( boost::any& p_rExtraData )
{
	HRESULT hrOk = S_OK;

	p_rExtraData = mExtraData;

	return hrOk;
}

/*
This method sets the context data of this object.
*/
HRESULT SVOResponseClass::SetExtraData( const boost::any& p_rExtraData )
{
	HRESULT hrOk = S_OK;

	mExtraData = p_rExtraData;

	return hrOk;
}

