//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBufferQueueFreeListStrategy
//* .File Name       : $Workfile:   SVBufferQueueFreeListStrategy.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:36:30  $
//******************************************************************************
#include "Stdafx.h"
#include "SVBufferQueueFreeListStrategy.h"

bool SVBufferQueueFreeListStrategy::Init()
{
	return m_freeList.Create() ? true : false;
}

bool SVBufferQueueFreeListStrategy::Destroy()
{
	return m_freeList.Destroy() ? true : false;
}

bool SVBufferQueueFreeListStrategy::SetInUse(unsigned long& rHandle)
{
	bool bRetVal = false;

	if (m_freeList.IsCreated())
	{
		long l_lSize = 0;

		// 
		if (m_freeList.GetSize( l_lSize ) && 0 < l_lSize)
		{
			unsigned long l_ulTemp = 0;

			if (m_freeList.RemoveHead((void **)&l_ulTemp))
			{
				rHandle = l_ulTemp;
				bRetVal = true;
			}
		}
	}
	return bRetVal;
}

bool SVBufferQueueFreeListStrategy::SetAvailable(long handle)
{
	bool bRetVal = false;

	if (m_freeList.IsCreated())
	{
		m_freeList.AddTail( (void *)handle);
		bRetVal = true;
	}
	return bRetVal;
}

