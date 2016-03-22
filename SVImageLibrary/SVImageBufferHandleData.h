//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageBufferHandleData
//* .File Name       : $Workfile:   SVImageBufferHandleData.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:44:28  $
//******************************************************************************

#pragma once

struct SVImageBufferHandleStruct;

class SVImageBufferHandleData  
{
public:
	SVImageBufferHandleData() {}
	virtual ~SVImageBufferHandleData() {}

	virtual HRESULT SetHandleData( const SVImageBufferHandleStruct& p_rObject ) = 0;
};

