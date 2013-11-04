//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVFileAcquisitionDevice\SVEventHandler.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:25:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Sep 2008 07:57:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/