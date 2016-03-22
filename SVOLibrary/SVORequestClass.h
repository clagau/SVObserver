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

//Moved to precompiled header: #include <comdef.h>
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVOResponseClass.h"

/*
This object holds the request and response information to a event transaction.
*/
class SVORequestClass
{
public:
	SVORequestClass();
	SVORequestClass(const SVORequestClass &rRequest);

	virtual ~SVORequestClass();

	const SVORequestClass &operator=(const SVORequestClass &rRequest);

	virtual HRESULT Reset();

	virtual bool IsValid() const;
	virtual bool IsComplete() const;

	virtual HRESULT GetIsValid( bool &rbIsValid ) const;
	virtual HRESULT SetIsValid( bool bIsValid );

	virtual HRESULT GetIsComplete( bool &rbIsComplete );
	virtual HRESULT SetIsComplete( bool bIsComplete );

	virtual HRESULT GetOwner( void **pvOwner );
	virtual HRESULT SetOwner( void *pvOwner );

	virtual HRESULT GetParent( void **pvParent );
	virtual HRESULT SetParent( void *pvParent );

	virtual HRESULT GetExtraData( void **ppvResponse );
	virtual HRESULT SetExtraData( void *pResponse );

	virtual HRESULT GetStartTick( SVClock::SVTimeStamp& p_rTick );
	virtual HRESULT SetStartTick( const SVClock::SVTimeStamp& p_rTick );

	virtual HRESULT GetResponse( SVOResponseClass& p_rResponse );
	virtual HRESULT SetResponse( SVOResponseClass p_rResponse );

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
	SVClock::SVTimeStamp m_StartTick;

	//This attribute holds the response data associated with this request.
	SVOResponseClass mResponse;

};

