//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeHandleList
//* .File Name       : $Workfile:   SVMatroxGigeHandleList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:44:24  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <utility>
#include "SVUtilityLibrary/SVHandleManager.h"

template <int MaxHandles, typename HandleType, HandleType InvalidHandle, typename value_type>
class SVMatroxGigeHandleList
{
	typedef std::map<HandleType, value_type> Collection;
	typedef SVHandleManager<MaxHandles, HandleType, InvalidHandle> HandleManager;

private:
	Collection m_list;
	HandleManager m_handleMgr;
	value_type m_defaultValue;

public:
	typedef typename value_type value_type;
	typedef typename HandleManager::HandleType HandleType;
	typedef typename Collection::iterator iterator;
	typedef typename Collection::const_iterator const_iterator;
	
	HRESULT Add(value_type& rValue);
	HRESULT Remove(HandleType handle);

	value_type& Get(HandleType handle, HRESULT& status);
	const value_type& Get(HandleType handle, HRESULT& status) const;

	bool IsValidHandle(HandleType handle) const;
	bool IsValidComponentHandle(HandleType handle, HandleType componentHandle) const;

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;

	size_t size() const;
	void clear();
};

#include "SVMatroxGigeHandleList.inl"

