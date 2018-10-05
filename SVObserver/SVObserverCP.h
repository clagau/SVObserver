//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObserverCP
//* .File Name       : $Workfile:   SVObserverCP.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:44:12  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>

/**
@SVObjectName COM Command Events

@SVObjectOverview This object is used for managing the SIAC streaming data events.  This object handles setting up the custom marshaling needed to transfer the data and manage the synchronization events.

@SVObjectOperations This object presents the following operations associated with the streaming events: Fire Streaming Data, Fire Test Connection, Advise, Unadvise, Store Marshal Interface, and Get Unmarshaled Interface.

The Fire Streaming Data operator iterates through the connection list, gets an unmarshaled interface, and calls the Streaming Data operator.

The Fire Test Connection operator iterates through the connection list, gets an unmarshaled interface, and calls the Streaming Data operator.

The Advise operator stores the established marshaled interface.

The Unadvise operator clears the marshaled interface specified by the cookie.

The Store Marshal Interface operator stores the established marshaled interface.

The Get Unmarshaled Interface operator gets the desired unmarshaled interface.

*/
template< class T >
class CProxy_ISVCommandObserverEvents : public IConnectionPointImpl<T, &IID__ISVCommandObserverEvents, CComDynamicUnkArray>
{
public:
	HRESULT Fire_NotifyObservers( const _variant_t& p_rNotificationType, ISVRemoteCommandData* p_pResults )
	{
		HRESULT ret = S_FALSE;

		T* pT = static_cast< T* >( this );

		int nConnectionIndex;
		int nConnections = m_vec.GetSize();

		for( nConnectionIndex = 0; nConnectionIndex < nConnections; ++nConnectionIndex )
		{
			pT->Lock();

			IUnknown* pTemp = m_vec.GetAt( nConnectionIndex );

			// the following line is needed when this function is called from different thread
			// than what Advise was called
			pTemp = GetInterface( pTemp );

			CComPtr<IUnknown> sp = pTemp;

			pT->Unlock();

			_ISVCommandObserverEvents* p_ISVCommandObserverEvents = reinterpret_cast< _ISVCommandObserverEvents* >( sp.p );

			if( nullptr != p_ISVCommandObserverEvents )
			{
				if( S_OK == p_ISVCommandObserverEvents->IsValidObserver( p_rNotificationType ) )
				{
					ret = p_ISVCommandObserverEvents->NotifyObserver( p_pResults );
				}
			}
		}

		return ret;
	}

	STDMETHOD(Advise)( IUnknown* pUnkSink, DWORD* pdwCookie )
	{               
		HRESULT l_Status = IConnectionPointImpl<T, &IID__ISVCommandObserverEvents, CComDynamicUnkArray>::Advise( pUnkSink, pdwCookie );

		if( S_OK == l_Status )
		{
			IID iid;
			IUnknown* p = nullptr;

			GetConnectionInterface( &iid );

			l_Status = pUnkSink->QueryInterface( iid, reinterpret_cast< void** >( &p ) );

			if( SUCCEEDED( l_Status ) )
			{
				T* pT = static_cast< T* >( this );

				pT->Lock();

				MarshalInterface( p );

				p->Release();

				pT->Unlock();

				StoreEventObserver( *pdwCookie, pT );
			}
		}

		return l_Status;
	}

	STDMETHOD(Unadvise) (DWORD dwCookie)
	{
		T* pT = static_cast<T*>(this);

		pT->Lock();

		IUnknown* p = m_vec.GetUnknown( dwCookie );

		if( nullptr != p )
		{
			ReleaseEventObserver( dwCookie, pT );

			m_MarshalInterface.erase( p );
			m_Interface.erase( p );

			p->Release();
		}

		pT->Unlock();

		HRESULT l_Status = IConnectionPointImpl<T, &IID__ISVCommandObserverEvents, CComDynamicUnkArray>::Unadvise( dwCookie );

		return l_Status;
	}

protected:
	virtual HRESULT StoreEventObserver( DWORD dwCookie, CComPtr< T > p_pObserver ) = 0;
	virtual HRESULT ReleaseEventObserver( DWORD dwCookie, CComPtr< T > p_pObserver ) = 0;

#pragma inline_depth (0)

	HRESULT MarshalInterface( IUnknown* pUnk )
	{
		IStream* pStream = nullptr;

		HRESULT hr = CoMarshalInterThreadInterfaceInStream( IID__ISVCommandObserverEvents, pUnk, &pStream );

		if( S_OK == hr )
		{
			m_MarshalInterface[ pUnk ] = pStream;
		}

		return hr;
	}

	IUnknown* GetInterface( IUnknown* pUnk )
	{
		IUnknown* l_pInterface = pUnk;

		SVInterfacePointerMap::iterator l_Iter = m_Interface.find( pUnk );

		if( l_Iter != m_Interface.end() )
		{
			l_pInterface = reinterpret_cast< IUnknown* >( l_Iter->second );
		}
		else
		{
			SVMarshalInterfacePointerMap::iterator l_StreamIter = m_MarshalInterface.find( pUnk );

			if( l_StreamIter != m_MarshalInterface.end() )
			{
				if( S_OK == CoGetInterfaceAndReleaseStream( l_StreamIter->second, IID__ISVCommandObserverEvents, (void**) &l_pInterface ) )
				{
					m_Interface[ pUnk ] = l_pInterface;
				}
			}
		}

		return l_pInterface;
	}

	typedef std::map< IUnknown*, IStream* > SVMarshalInterfacePointerMap;
	typedef std::map< IUnknown*, IUnknown* > SVInterfacePointerMap;

	SVMarshalInterfacePointerMap m_MarshalInterface;
	SVInterfacePointerMap m_Interface;

};

