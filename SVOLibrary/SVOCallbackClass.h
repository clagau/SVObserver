// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOCallbackClass
// * .File Name       : $Workfile:   SVOCallbackClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:54:10  $
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <memory>
#include "SVQueueObject.h"
#pragma endregion Includes


typedef HRESULT ( CALLBACK *SVOCallbackPtr )( void *,   /* Owner */
                                              void *,   /* Caller */ 
                                              void * ); /* Data */ 

/*
This class is used to identify the callback requesters and holds the data to be requested and returned.
*/
class SVOCallbackClass  
{
public:
	SVOCallbackClass();
	SVOCallbackClass(const SVOCallbackClass& rData);
	virtual ~SVOCallbackClass();

	const SVOCallbackClass& operator=(const SVOCallbackClass& rData);

	//This attribute holds the function to call when data is available to return.
	SVOCallbackPtr mpCallback;
	//This attribute holds a pointer to the owner/receipent of the callback.
	void *mpvOwner;
	//This attribute holds a pointer to the caller of the callback function.
	void *mpvCaller;

	//This attribute holds the reference count of this object.
	long m_lRefCount;

};

typedef std::shared_ptr<SVOCallbackClass> SVOCallbackClassPtr;
typedef SVTQueueObject<SVOCallbackClassPtr> SVCallbackClassPtrQueue;

