//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeHandleList.inl
//* .File Name       : $Workfile:   SVMatroxGigeHandleList.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:44:28  $
//******************************************************************************

#include "SVMatroxGigeHandleList.h"

template <int MaxHandles, typename HandleType, HandleType InvalidHandle, typename value_type>
HRESULT SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::Add(value_type& rValue)
{
	HRESULT hr = S_FALSE;
	
	rValue.m_Handle = m_handleMgr.AcquireHandle();
	if (rValue.m_Handle != InvalidHandle)
	{
		typedef std::pair<Collection::iterator, bool> InsertRc;

		InsertRc rc = m_list.insert(std::make_pair(rValue.m_Handle, rValue));
		if (rc.second)
		{
			hr = S_OK;
		}
	}
	return hr;
}

template <int MaxHandles, typename HandleType, HandleType InvalidHandle, typename value_type>
HRESULT SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::Remove(HandleType handle)
{
	HRESULT hr = S_FALSE;

	Collection::iterator it = m_list.find(handle);
	if (it != m_list.end())
	{
		m_handleMgr.ReleaseHandle(it->first);
		m_list.erase(it);
		hr = S_OK;
	}
	return hr;
}

template <int MaxHandles, typename HandleType, HandleType InvalidHandle, typename value_type>
value_type& SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::Get(HandleType handle, HRESULT& status)
{
	status = S_FALSE;

	Collection::iterator it = m_list.find(handle);
	if (it != m_list.end())
	{
		status = S_OK;
		return it->second;
	}
	//throw std::out_of_range("invalid map<T> key");
	
	return m_defaultValue;
}

template <int MaxHandles, typename HandleType, HandleType InvalidHandle, typename value_type>
const value_type& SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::Get(HandleType handle, HRESULT& status) const
{
	status = S_FALSE;

	Collection::const_iterator it = m_list.find(handle);
	if (it != m_list.end())
	{
		status = S_OK;
		return it->second;
	}
	//throw std::out_of_range("invalid map<T> key");

	return m_defaultValue;
}

template <int MaxHandles, typename HandleType, HandleType InvalidHandle, typename value_type>
bool SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::IsValidHandle(HandleType handle) const
{
	return (m_handleMgr.IsValidHandle(handle) && m_handleMgr.IsHandleAcquired(handle));
}

template <int MaxHandles, typename HandleType, HandleType InvalidHandle, typename value_type>
bool SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::IsValidComponentHandle(HandleType handle, HandleType componentHandle) const
{
	bool bRetVal = false;
	if (IsValidHandle(handle))
	{
		HRESULT hr = S_OK;
		value_type value = Get(handle, hr);
		if (S_OK == hr)
		{
			if (value.IsValidHandle(componentHandle))
			{
				bRetVal = true;
			}
		}
	}
	return bRetVal;
}

template <int MaxHandles, typename HandleType, HandleType InvalidHandle, typename value_type>
typename SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::iterator SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::begin()
{
	return m_list.begin();
}

template <int MaxHandles, typename HandleType, HandleType InvalidHandle, typename value_type>
typename SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::iterator SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::end()
{
	return m_list.end();
}

template <int MaxHandles, typename HandleType, HandleType InvalidHandle, typename value_type>
typename SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::const_iterator SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::begin() const
{
	return m_list.begin();
}

template <int MaxHandles, typename HandleType, HandleType InvalidHandle, typename value_type>
typename SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::const_iterator SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::end() const
{
	return m_list.end();
}

template <int MaxHandles, typename HandleType, HandleType InvalidHandle, typename value_type>
size_t SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::size() const
{
	return m_list.size();
}

template <int MaxHandles, typename HandleType, HandleType InvalidHandle, typename value_type>
void SVMatroxGigeHandleList<MaxHandles, HandleType, InvalidHandle, value_type>::clear()
{
	m_list.clear();
	m_handleMgr.clear();
}

