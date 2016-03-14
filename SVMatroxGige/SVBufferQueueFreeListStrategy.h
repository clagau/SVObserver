//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBufferQueueFreeListStrategy
//* .File Name       : $Workfile:   SVBufferQueueFreeListStrategy.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:36:36  $
//******************************************************************************
#ifndef SVBUFFERQUEUEFREELISTSTRATEGY_H
#define SVBUFFERQUEUEFREELISTSTRATEGY_H

#include "SVOLibrary/SVQueueObject.h"

class SVBufferQueueFreeListStrategy
{
	SVQueueObject m_freeList;

public:
	bool Init();
	bool Destroy();
	bool SetInUse(unsigned long& handle);
	bool SetAvailable(long handle);
};

#endif

