//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHandleManager.h
//* .File Name       : $Workfile:   SVHandleManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:21:56  $
//******************************************************************************
#ifndef SVHANDLEMANAGER_H
#define SVHANDLEMANAGER_H

#include <bitset>

template<long NumBits, typename HandleType, HandleType InvalidHandle>
class SVHandleManager
{
private:
	std::bitset<NumBits> m_handleList;

public:
	typedef HandleType HandleType;

	SVHandleManager();
	~SVHandleManager();

	HandleType AcquireHandle();
	void ReleaseHandle(HandleType handle);

	bool IsValidHandle(HandleType handle) const;
	bool IsHandleAcquired(HandleType handle) const;

	void clear();
};

#include "SVHandleManager.inl"

#endif

