//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeBufferManager
//* .File Name       : $Workfile:   SVMatroxGigeBufferManager.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:39:22  $
//******************************************************************************

#include "SVMatroxGigeBufferManager.h"

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::SVMatroxGigeBufferManager()
{
}

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::~SVMatroxGigeBufferManager()
{
}

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
bool SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::IsValidBufferHandle(unsigned long handle) const
{
	return ( handle >= 0 && handle < m_Buffers.size() );
}

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
bool SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::IsValidBuffer(unsigned long handle) const
{
	bool bValid = IsValidBufferHandle(handle);
	if (bValid)
	{
		bValid = m_Buffers[handle].IsValid();
	}
	return bValid;
}

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
bool SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::Init()
{
	return m_bufferStrategy.Init();
}

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
bool SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::Destroy()
{
	return m_bufferStrategy.Destroy();
}

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
HRESULT SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::CreateBuffer(const SVMatroxSystem& rSystem, unsigned long p_ulWidth, unsigned long p_ulHeight, unsigned long p_ulFormat, unsigned long& rHandle)
{
	BufferType l_Buffer;
	HRESULT l_hr = m_creator.Create(rSystem, p_ulWidth, p_ulHeight, p_ulFormat, l_Buffer);
	
	if (l_hr == S_OK)
	{
		// if the index requested already exists - destroy it
		if (IsValidBufferHandle(rHandle))
		{
			l_hr = DestroyBuffer(rHandle);
		}
		else
		{
			m_bufferStrategy.SetInUse(rHandle);
/*
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
					}
				}
			}
*/
		}

		if (l_hr == S_OK)
		{
			if (IsValidBufferHandle(rHandle))
			{
				m_Buffers[rHandle] = l_Buffer;
			}
			else
			{
				m_Buffers.push_back( l_Buffer );
				rHandle = m_Buffers.size() - 1;
			}
		}
	}

	return l_hr;
}

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
HRESULT SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::DestroyBuffers()
{
	HRESULT l_hr = S_OK;

	for (long i = static_cast<long>(m_Buffers.size()) - 1; i >= 0; i--)
	{
		if (DestroyBuffer( i ) != S_OK)
		{
			l_hr = S_FALSE;
		}
	}
	m_Buffers.clear();

	return l_hr;
}

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
HRESULT SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::DestroyBuffer(unsigned long handle)
{
	HRESULT l_hr = S_OK;

	if (IsValidBuffer(handle))
	{
		// remove from list
		l_hr = m_creator.Destroy(m_Buffers[ handle ]);

		m_bufferStrategy.SetAvailable(handle);
/*
		//
		if (m_freeList.IsCreated())
		{
			m_freeList.AddTail( (void *)handle);
		}
*/
	}
	return l_hr;
}

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
HRESULT SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::Acquire(unsigned long handle)
{
	HRESULT l_hr = S_FALSE;

	if ( IsValidBufferHandle(handle) )
	{
		m_Buffers[handle].m_bIsLocked = true;
		l_hr = S_OK;
		TRACE( "SVMatroxGigeBufferManager::Acquire( %d )\n", handle );
	}
	else
	{
		TRACE( "SVMatroxGigeBufferManager::Acquire( %d ) Invalid Handle\n", handle );
	}
	return l_hr;
}

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
HRESULT SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::Release(unsigned long handle)
{
	HRESULT l_hr = S_FALSE;

	if ( IsValidBufferHandle(handle) )
	{
		m_Buffers[handle].m_bIsLocked = false;
		l_hr = S_OK;
		TRACE( "SVMatroxGigeBufferManager::Release( %d )\n", handle );
	}
	else
	{
		TRACE( "SVMatroxGigeBufferManager::Release( %d ) Invalid Handle\n", handle );
	}
	return l_hr;
}

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
HRESULT SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::IsInUse(unsigned long handle, bool& bInUse) const
{
	HRESULT l_hr = S_FALSE;
	if ( IsValidBufferHandle(handle) )
	{
		bInUse = m_Buffers[handle].m_bIsLocked;
		l_hr = S_OK;
		TRACE( "SVMatroxGigeBufferManager::IsInUse( %d, %s )\n", handle, (bInUse) ? "TRUE" : "FALSE" );
	}
	else
	{
		TRACE( "SVMatroxGigeBufferManager::IsInUse( %d ) Invalid Handle\n", handle );
	}
	return l_hr;
}

// This methods assumes that IsValidBufferHandle has been called prior and that it was successful
template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
SVMatroxGigeBufferStruct& SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::Get(unsigned long handle)
{
	return m_Buffers[handle];
}

// This methods assumes that IsValidBufferHandle has been called prior and that it was successful
template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
const SVMatroxGigeBufferStruct& SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::Get(unsigned long handle) const
{
	return m_Buffers[handle];
}

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
HRESULT SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::BufferGetWidth( unsigned long handle, unsigned long& rWidth ) const
{
	HRESULT hr = S_FALSE;

	if ( IsValidBufferHandle(handle) && IsValidBuffer(handle) )
	{
		rWidth = m_Buffers[handle].m_ulWidth;
		hr = S_OK;
	}
	return hr;
}

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
HRESULT SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::BufferGetHeight( unsigned long handle, unsigned long& rHeight ) const
{
	HRESULT hr = S_FALSE;

	if ( IsValidBufferHandle(handle) && IsValidBuffer(handle) )
	{
		rHeight = m_Buffers[handle].m_ulHeight;
		hr = S_OK;
	}
	return hr;
}

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
HRESULT SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::BufferGetFormat( unsigned long handle, unsigned long& rFormat ) const
{
	HRESULT hr = S_FALSE;

	if ( IsValidBufferHandle(handle) && IsValidBuffer(handle) )
	{
		rFormat = m_Buffers[handle].m_ulFormat;
		hr = S_OK;
	}
	return hr;
}

template <typename BufferCreator, typename BufferStrategy, typename BufferCollection>
HRESULT SVMatroxGigeBufferManager<BufferCreator, BufferStrategy, BufferCollection>::BufferGetAddress( unsigned long handle, unsigned long& rAddress) const
{
	HRESULT hr = S_FALSE;

	if ( IsValidBufferHandle(handle) && IsValidBuffer(handle) )
	{
		rAddress = m_Buffers[handle].m_ulAddress;
		hr = S_OK;
	}
	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeBufferManager.inl_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:39:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 May 2010 14:03:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include file due to changes in the data manager include path update.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:18:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
