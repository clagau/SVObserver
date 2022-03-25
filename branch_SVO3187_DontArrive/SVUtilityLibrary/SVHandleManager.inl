//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHandleManager.inl
//* .File Name       : $Workfile:   SVHandleManager.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:22:12  $
//******************************************************************************

template<long NumBits, typename HandleType, HandleType InvalidHandle>
SVHandleManager<NumBits, HandleType, InvalidHandle>::SVHandleManager()
{
	clear();
}

template<long NumBits, typename HandleType, HandleType InvalidHandle>
SVHandleManager<NumBits, HandleType, InvalidHandle>::~SVHandleManager()
{
	clear();
}

template<long NumBits, typename HandleType, HandleType InvalidHandle>
HandleType SVHandleManager<NumBits, HandleType, InvalidHandle>::AcquireHandle()
{
	HandleType handle = InvalidHandle;

	// get first bit not set
	for (size_t i = 0;i < m_handleList.size();i++)
	{
		if (!m_handleList.test(i))
		{
			m_handleList.set(i);
			// Handle is one based
			handle = static_cast<HandleType>(i + 1);
			break;
		}
	}
	return handle;
}

template<long NumBits, typename HandleType, HandleType InvalidHandle>
void SVHandleManager<NumBits, HandleType, InvalidHandle>::ReleaseHandle(HandleType handle)
{
	// Handle is one based
	m_handleList.reset(handle - 1);
}

template<long NumBits, typename HandleType, HandleType InvalidHandle>
bool SVHandleManager<NumBits, HandleType, InvalidHandle>::IsValidHandle(HandleType handle) const
{
	// Handle is one based
	return (handle - 1 >= 0 && handle - 1 < NumBits);
}

template<long NumBits, typename HandleType, HandleType InvalidHandle>
bool SVHandleManager<NumBits, HandleType, InvalidHandle>::IsHandleAcquired(HandleType handle) const
{
	// Handle is one based
	return m_handleList.test(handle - 1);
}

template<long NumBits, typename HandleType, HandleType InvalidHandle>
void SVHandleManager<NumBits, HandleType, InvalidHandle>::clear()
{
	m_handleList.reset();
}

