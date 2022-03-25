//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEventHandler
//* .File Name       : $Workfile:   SVEventHandler.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:25:58  $
//******************************************************************************

template <typename T>
SVEventHandler<T>::SVEventHandler()
{
}

template <typename T>
SVEventHandler<T>::SVEventHandler(const SVEventHandler& rHandler)
{
	m_handler = rHandler.m_handler;
}

template <typename T>
SVEventHandler<T>& SVEventHandler<T>::operator=(const SVEventHandler& rHandler)
{
	m_handler = rHandler.m_handler;
	return *this;
}

template <typename T>
SVEventHandler<T>& SVEventHandler<T>::operator=(const SVHandlerFunc& func)
{
	m_handler = func;
	return *this;
}

template <typename T>
HRESULT SVEventHandler<T>::Fire(unsigned long handle)
{
	return m_handler(handle);
}

