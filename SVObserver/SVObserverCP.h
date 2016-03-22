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
template <class T>
class CProxy_ISVCommandEvents : public IConnectionPointImpl<T, &IID__ISVCommandEvents, CComDynamicUnkArray>
{
public:
	HRESULT Fire_StreamingData(SAFEARRAY * saProcessCount, SAFEARRAY * saNames, SAFEARRAY * saValues)
	{
		HRESULT ret=S_FALSE;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();

		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();

			IUnknown* pTemp = m_vec.GetAt(nConnectionIndex);

			// the following line is needed when this function is called from different thread
			// than what Advise was called
			pTemp = GetUnmarshaledInterface(pTemp);

			CComPtr<IUnknown> sp = pTemp;
			pT->Unlock();
			_ISVCommandEvents* p_ISVCommandEvents = reinterpret_cast<_ISVCommandEvents*>(sp.p);
			if (p_ISVCommandEvents != NULL)
				ret = p_ISVCommandEvents->StreamingData(saProcessCount, saNames, saValues);
		}	return ret;

	}
	HRESULT Fire_TestConnection()
	{
		HRESULT ret;
		T* pT = static_cast<T*>(this);
		int nConnectionIndex;
		int nConnections = m_vec.GetSize();

		for (nConnectionIndex = 0; nConnectionIndex < nConnections; nConnectionIndex++)
		{
			pT->Lock();
			CComPtr<IUnknown> sp = m_vec.GetAt(nConnectionIndex);
			pT->Unlock();
			_ISVCommandEvents* p_ISVCommandEvents = reinterpret_cast<_ISVCommandEvents*>(sp.p);
			if (p_ISVCommandEvents != NULL)
				ret = p_ISVCommandEvents->TestConnection();
		}	return ret;

	}

	STDMETHOD(Advise)( IUnknown* pUnkSink, DWORD* pdwCookie )
	{               
		//add here
		T* pT = static_cast<T*>(this);
		IUnknown* p;
		HRESULT hRes = S_OK;
		if (pUnkSink == NULL || pdwCookie == NULL)
			return E_POINTER;
		IID iid;
		GetConnectionInterface(&iid);
		hRes = pUnkSink->QueryInterface(iid, (void**)&p);
		if (SUCCEEDED(hRes))
		{
			pT->Lock();
			*pdwCookie = m_vec.Add(p);

			StoreMarshalInterface(p);

			hRes = (*pdwCookie != NULL) ? S_OK : CONNECT_E_ADVISELIMIT;
			pT->Unlock();
			if (hRes != S_OK)
				p->Release();
		}
		else if (hRes == E_NOINTERFACE)
			hRes = CONNECT_E_CANNOTCONNECT;
		if (FAILED(hRes))
			*pdwCookie = 0;
		return hRes;
	}


	STDMETHOD(Unadvise) (DWORD dwCookie)
	{
		T* pT = static_cast<T*>(this);
		pT->Lock();
		IUnknown* p = m_vec.GetUnknown(dwCookie);
		HRESULT hRes = m_vec.Remove(dwCookie) ? S_OK : CONNECT_E_NOCONNECTION;
		pT->Unlock();
		if (hRes == S_OK && p != NULL)
			p->Release();
		return hRes;
	}

protected:

#pragma inline_depth (0)

	HRESULT StoreMarshalInterface(IUnknown* pUnk)
	{
		IStream* pStream;
		HRESULT hr;
		hr = CoMarshalInterThreadInterfaceInStream( IID__ISVCommandEvents, pUnk, &pStream);
		if ( hr == S_OK )
		{
			m_mapInterface.SetAt(pUnk, pStream);
			m_mapMarshaled.SetAt(pUnk, false);
		}
		else
		{
			m_mapInterface.SetAt(pUnk, NULL);
		}

		return hr;
	}
	IUnknown* GetUnmarshaledInterface(IUnknown* pUnk)
	{
		IUnknown* pUnmarshal=pUnk;
		IStream* pStream;
		bool bMarshaled;
		HRESULT hr;

		if ( m_mapInterface.Lookup(pUnk, (IUnknown*&) pStream) )
		{
			if ( pStream )
			{
				if ( m_mapMarshaled.Lookup(pUnk, (bool&) bMarshaled) )
				{
					if ( bMarshaled )
					{
						pUnmarshal = reinterpret_cast<IUnknown*> (pStream);
					}
					else
					{
						hr = CoGetInterfaceAndReleaseStream(pStream, IID__ISVCommandEvents, (void**) &pUnmarshal);
						m_mapInterface.SetAt(pUnk, pUnmarshal);
						m_mapMarshaled.SetAt(pUnk, true);
					}
				}
			}
		}

		return pUnmarshal;
	}

	typedef CMap< IUnknown*, IUnknown*, IUnknown*, IUnknown* > CInterfacePointerMap;
	typedef CMap< IUnknown*, IUnknown*, bool, bool > CMarshaledPointerMap;

	CInterfacePointerMap m_mapInterface;
	CMarshaledPointerMap m_mapMarshaled;

};

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

			if( p_ISVCommandObserverEvents != NULL )
			{
				if( p_ISVCommandObserverEvents->IsValidObserver( p_rNotificationType ) == S_OK )
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

		if( l_Status == S_OK )
		{
			IID iid;
			IUnknown* p = NULL;

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

		if( p != NULL )
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
		IStream* pStream = NULL;

		HRESULT hr = CoMarshalInterThreadInterfaceInStream( IID__ISVCommandObserverEvents, pUnk, &pStream );

		if( hr == S_OK )
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
				if( CoGetInterfaceAndReleaseStream( l_StreamIter->second, IID__ISVCommandObserverEvents, (void**) &l_pInterface ) == S_OK )
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

