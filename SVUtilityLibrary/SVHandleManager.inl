//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHandleManager.inl
//* .File Name       : $Workfile:   SVHandleManager.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:22:12  $
//******************************************************************************
#include "SVHandleManager.h"

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVHandleManager.inl_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:22:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Jun 2009 07:54:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

