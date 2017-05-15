// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVORequestClass
// * .File Name       : $Workfile:   SVORequestClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:28:12  $
// ******************************************************************************

#include "stdafx.h"
#include "SVORequestClass.h"

/*
This constructor initializes the local attributes and calls the Reset method.
*/
SVORequestClass::SVORequestClass()
{
	mpvOwner = nullptr;
	mpvParent = nullptr;
	mpvExtraData = nullptr;

	Reset();
}

/*
This constructor initializes the local attributes, calls the Reset method, and calls the assignment operator.
*/
SVORequestClass::SVORequestClass(const SVORequestClass &rRequest)
{
	mpvOwner = nullptr;
	mpvParent = nullptr;
	mpvExtraData = nullptr;

	Reset();

	*this = rRequest;
}

/*
This destructor is a placeholder and performs no operations.
*/
SVORequestClass::~SVORequestClass()
{
}

/*
This operator assigns the data from the provided object into this object.
*/
const SVORequestClass &SVORequestClass::operator=(const SVORequestClass &rRequest)
{
	mbIsValid = rRequest.mbIsValid;
	mbIsComplete = rRequest.mbIsComplete;

	mpvOwner = rRequest.mpvOwner;
	mpvParent = rRequest.mpvParent;
	mpvExtraData = rRequest.mpvExtraData;

	m_StartTick = rRequest.m_StartTick;

	mResponse = rRequest.mResponse;

	mDMHandle.Assign( rRequest.mDMHandle, rRequest.mDMHandle.GetLockType() );

	return *this;
}

/*
This method sets all the local attributes to the desired defaults.
*/
HRESULT SVORequestClass::Reset()
{
	HRESULT hrOk = S_OK;

	mbIsValid = false;
	mbIsComplete = false;

	m_StartTick = SvTl::GetTimeStamp();

	if ( S_OK != mResponse.Reset() )
	{
		hrOk = S_FALSE;
	}

	mDMHandle.clear();

	return hrOk;
}

/*
This method returns the current validity state of this object.
*/
bool SVORequestClass::IsValid() const
{
	return mbIsValid;
}

/*
This method returns the current completion state of this object.
*/
bool SVORequestClass::IsComplete() const
{
	return mbIsComplete;
}

/*
This method returns the state of the validity attribute.
*/
HRESULT SVORequestClass::GetIsValid( bool &rbIsValid ) const
{
	HRESULT hrOk = S_OK;

	rbIsValid = mbIsValid;

	return hrOk;
}

/*
This method sets the state of the validity attribute.
*/
HRESULT SVORequestClass::SetIsValid( bool bIsValid )
{
	HRESULT hrOk = S_OK;

	mbIsValid = bIsValid;

	return hrOk;
}

/*
This method returns the state of the completion attribute.
*/
HRESULT SVORequestClass::GetIsComplete( bool &rbIsComplete )
{
	HRESULT hrOk = S_OK;

	rbIsComplete = mbIsComplete;

	return hrOk;
}

/*
This method sets the state of the completion attribute.
*/
HRESULT SVORequestClass::SetIsComplete( bool bIsComplete )
{
	HRESULT hrOk = S_OK;

	mbIsComplete = bIsComplete;

	return hrOk;
}

/*
This method returns the pointer of the owner object of this object.
*/
HRESULT SVORequestClass::GetOwner( void **ppvOwner )
{
	HRESULT hrOk = S_OK;

	*ppvOwner = mpvOwner;

	return hrOk;
}

/*
This method sets the pointer of the owner object of this object.
*/
HRESULT SVORequestClass::SetOwner( void *pvOwner )
{
	HRESULT hrOk = S_OK;

	mpvOwner = pvOwner;

	return hrOk;
}

/*
This method returns the pointer of the parent object of this object.
*/
HRESULT SVORequestClass::GetParent( void **ppvParent )
{
	HRESULT hrOk = S_OK;

	*ppvParent = mpvParent;

	return hrOk;
}

/*
This method sets the pointer of the parent object of this object.
*/
HRESULT SVORequestClass::SetParent( void *pvParent )
{
	HRESULT hrOk = S_OK;

	mpvParent = pvParent;

	return hrOk;
}

/*
This method returns the pointer of the context data of this object.
*/
HRESULT SVORequestClass::GetExtraData( void **ppvExtraData )
{
	HRESULT hrOk = S_OK;

	*ppvExtraData = mpvExtraData;

	return hrOk;
}

/*
This method sets the pointer of the context data of this object.
*/
HRESULT SVORequestClass::SetExtraData( void *pvExtraData )
{
	HRESULT hrOk = S_OK;

	mpvExtraData = pvExtraData;

	return hrOk;
}

/*
This method returns the start time stamp of this object.
*/
HRESULT SVORequestClass::GetStartTick( SvTl::SVTimeStamp& p_rTick )
{
	HRESULT hrOk = S_OK;

	p_rTick = m_StartTick;

	return hrOk;
}

/*
This method sets the start time stamp of this object.
*/
HRESULT SVORequestClass::SetStartTick( const SvTl::SVTimeStamp& p_rTick )
{
	HRESULT hrOk = S_OK;

	m_StartTick = p_rTick;

	return hrOk;
}

/*
This method returns the start response of this object.
*/
HRESULT SVORequestClass::GetResponse( SVOResponseClass& p_rResponse )
{
	HRESULT hrOk = S_OK;

	p_rResponse = mResponse;

	return hrOk;
}

/*
This method sets the start response of this object.
*/
HRESULT SVORequestClass::SetResponse( SVOResponseClass p_Response )
{
	HRESULT hrOk = S_OK;

	mResponse = p_Response;

	mDMHandle.Assign( mResponse.mDMHandle, mResponse.mDMHandle.GetLockType() );

	SetIsValid( mResponse.IsValid() && -1 < mDMHandle.GetIndex() );

	return hrOk;
}

