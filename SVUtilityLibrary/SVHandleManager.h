//******************************************************************************
//* COPYRIGHT (c) 2009 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHandleManager.h
//* .File Name       : $Workfile:   SVHandleManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:21:56  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <bitset>
#pragma endregion Includes

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
