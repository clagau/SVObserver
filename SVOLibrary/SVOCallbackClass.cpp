// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCallbackClass
// * .File Name       : $Workfile:   SVOCallbackClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:53:54  $
// ******************************************************************************

#include "stdafx.h"
#include "SVOCallbackClass.h"

/*
This constructor initializes the attributes to the desired default values.
*/
SVOCallbackClass::SVOCallbackClass()
{
	mpCallback = NULL;
  mpvOwner = NULL;
  mpvCaller = NULL;

	m_lRefCount = 0;

	mRequestQueue.Create();
	mResponseQueue.Create();
}

/*
This constructor initializes the attributes of this object from the provided object's values.
*/
SVOCallbackClass::SVOCallbackClass(const SVOCallbackClass& rData)
{
	mpCallback = NULL;
  mpvOwner = NULL;
  mpvCaller = NULL;

	m_lRefCount = 0;

	mRequestQueue.Create();
	mResponseQueue.Create();

	*this = rData;
}

/*
This destructor clears and destroys the two internal queues of this objects.
*/
SVOCallbackClass::~SVOCallbackClass()
{
	mRequestQueue.Destroy();
	mResponseQueue.Destroy();
}

/*
This operator copies the attributes of the provided object's values to this object.
*/
const SVOCallbackClass& SVOCallbackClass::operator=(const SVOCallbackClass& rData)
{
	mpCallback = rData.mpCallback;
  mpvOwner = rData.mpvOwner;
  mpvCaller = rData.mpvCaller;

	m_lRefCount = rData.m_lRefCount;

	mRequestQueue.RemoveAll();
	mResponseQueue.RemoveAll();

	return *this;	
}

