// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVORequestClass
// * .File Name       : $Workfile:   SVORequestClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:28:14  $
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVOResponseClass.h"
#pragma endregion Includes

/*
This object holds the request and response information to a event transaction.
*/
class SVORequestClass
{
public:
	SVORequestClass();
	SVORequestClass(const SVORequestClass &rRequest);

	~SVORequestClass();

	const SVORequestClass &operator=(const SVORequestClass &rRequest);

	HRESULT Reset();

	bool IsValid() const;
	bool IsComplete() const;

	HRESULT GetIsValid( bool &rbIsValid ) const;
	HRESULT SetIsValid( bool bIsValid );

	HRESULT GetIsComplete( bool &rbIsComplete );
	HRESULT SetIsComplete( bool bIsComplete );

	HRESULT GetOwner( void **pvOwner );
	HRESULT SetOwner( void *pvOwner );

	HRESULT GetParent( void **pvParent );
	HRESULT SetParent( void *pvParent );

	HRESULT GetExtraData( void **ppvResponse );
	HRESULT SetExtraData( void *pResponse );

	HRESULT GetStartTick( SvTl::SVTimeStamp& p_rTick );
	HRESULT SetStartTick( const SvTl::SVTimeStamp& p_rTick );

	HRESULT GetResponse( SVOResponseClass& p_rResponse );
	HRESULT SetResponse( SVOResponseClass p_rResponse );

	//This attribute holds the Data Manager index of the associated data element.
	SVDataManagerHandle mDMHandle;

private:

	//This attribute holds a pointer to the owner of the request.
	void *mpvOwner;
	//This attribute holds a pointer to the parent of the request. 
	void *mpvParent;
	//This attribute holds a pointer to context data of the request. 
	void *mpvExtraData;

	//This attribute holds the validity state of the request.
	bool mbIsValid;
	//This attribute holds the completion state of the request.
	bool mbIsComplete;

	//This attribute holds the start time stamp the request.
	SvTl::SVTimeStamp m_StartTick;

	//This attribute holds the response data associated with this request.
	SVOResponseClass mResponse;

};

