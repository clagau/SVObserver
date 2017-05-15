// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOResponseClass
// * .File Name       : $Workfile:   SVOResponseClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 15:28:20  $
// ******************************************************************************

#pragma once

//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <boost/any.hpp>
#include "SVDataManagerLibrary/SVDataManagerHandle.h"
#include "SVTimerLibrary/SVClock.h"

class SVOResponseClass  //@TODO[Arvid] this is similar to SVODataResponseClass: combine or derive?
{
public:
	SVOResponseClass();
	SVOResponseClass(const SVOResponseClass &rResponse);
	~SVOResponseClass();

	const SVOResponseClass &operator=(const SVOResponseClass &rResponse);

	HRESULT Reset();

	bool IsValid() const;
	bool IsComplete() const;

	HRESULT GetIsValid( bool &rbIsValid ) const;
	HRESULT SetIsValid( bool bIsValid );

	HRESULT GetIsComplete( bool &rbIsComplete );
	HRESULT SetIsComplete( bool bIsComplete );

	HRESULT GetOwner( void **pvOwner );
	HRESULT SetOwner( void *pvOwner );

	HRESULT GetStartTick( SvTl::SVTimeStamp& p_rTick );
	HRESULT SetStartTick( const SvTl::SVTimeStamp& p_rTick );

	HRESULT GetEndTick( SvTl::SVTimeStamp& p_rTick );
	HRESULT SetEndTick( const SvTl::SVTimeStamp& p_rTick );

	HRESULT GetExtraData( boost::any& p_rExtraData );
	HRESULT SetExtraData( const boost::any& p_rExtraData );

	//This attribute holds the Data Manager index of the associated data element.
	SVDataManagerHandle mDMHandle;

private:
	//This attribute holds a pointer to the owner of the response.
	bool mbIsValid;
	//This attribute holds a pointer to the parent of the response. 
	bool mbIsComplete;

	//This attribute holds a pointer to the owner of the response.
	void *mpvOwner;

	//This attribute holds the start time stamp the response.
	SvTl::SVTimeStamp m_StartTick;
	//This attribute holds the end time stamp the response.
	SvTl::SVTimeStamp m_EndTick;

	//This attribute holds context data of the response. 
	boost::any mExtraData;  //used by SVCameraTriggerClass::TriggerCallback() only

};

