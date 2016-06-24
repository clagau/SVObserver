//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocket
//* .File Name       : $Workfile:   SVSocket.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   28 Oct 2014 10:46:28  $
//******************************************************************************

#pragma warning(push)
#pragma warning(disable: 4482)

namespace Seidenader { namespace SVSocketLibrary
{
	inline std::string addr2s(const sockaddr_in & addr)
	{
		std::ostringstream os;
		os << static_cast< short >( addr.sin_addr.S_un.S_un_b.s_b1 ) << '.' <<
			static_cast< short >( addr.sin_addr.S_un.S_un_b.s_b2 ) << '.' <<
			static_cast< short >( addr.sin_addr.S_un.S_un_b.s_b3 ) << '.' <<
			static_cast< short >( addr.sin_addr.S_un.S_un_b.s_b4 ) << ':' <<
			addr.sin_port;
		return os.str();
	}

	inline double frequency()
	{
		LARGE_INTEGER li;
		::QueryPerformanceFrequency(&li);
		return double(li.QuadPart);
	}

	inline double currentTime()
	{
		LARGE_INTEGER li;
		static double freq = frequency();
		::QueryPerformanceCounter(&li);
		return double(li.QuadPart) / freq;
	}

	template<typename API>
	inline Err SVSocket<API>::Create()
	{
		Err error = SVSocketError::Success;

		if (m_socket == InvalidSock)
		{
			m_socket = API::socket(AF_INET, Traits<API>::sokType, Traits<API>::proto);

			int namelen = sizeof(sockaddr_in);

			if (m_socket == InvalidSock)
			{
				error = SVSocketError::GetLastSocketError();
			}
			else
			{
				Traits<API>::CreateOptions(m_socket);
				DWORD no = 0;
				API::setsockopt(m_socket, IPPROTO_IP, IP_RECEIVE_BROADCAST, &no, sizeof(DWORD)); // do not receive broadcasts
				m_hasOwner = true;
			}
		}
		else
		{
			// already created
			error = SVSocketError::AlreadyCreated;
		}
		return error;
	}

	template<typename API>
	inline void SVSocket<API>::Destroy()
	{
		if (m_socket != InvalidSock)
		{
			int rc = 0;
			if (IsConnected())
			{
				rc = SetLingerOption(false, 0);
				if (rc == SOCKET_ERROR)
				{
					Err error = SVSocketError::GetLastSocketError();
				}
				rc = API::shutdown(m_socket, SD_BOTH);
				if (rc == SOCKET_ERROR)
				{
					Err error = SVSocketError::GetLastSocketError();
				}
			}
			rc = API::close(m_socket);
			if (rc == SOCKET_ERROR)
			{
				Err error = SVSocketError::GetLastSocketError();
			}
			m_socket = InvalidSock;
			memset(&m_peer, 0, sizeof(sockaddr_in));
			memset(&m_addr, 0, sizeof(sockaddr_in));
		}
	}

	template<typename API>
	inline std::string SVSocket<API>::state()const
	{
		return std::string( m_hasOwner ? " owner" : "" ) + ( IsValidSocket() ? "" : " invalid" );
	}

	template<typename API>
	inline std::string SVSocket<API>::Log(const std::string & msg, bool full) const
	{
	//#ifdef LOG_ENABLED
		static int tick = 0;
		static double start = currentTime();
		char buff[256];

		#pragma warning (push)
		#pragma warning (disable: 4996)
		if (full)
		{
			// id  tick msg class
			_snprintf(buff, 255, "%4d >> %6d %-32s %s addr: %s, peer: %s time: %4.4f\n", 
				m_sockNum, ++tick, msg.c_str(), 
				("Socket<" + Traits<API>::ApiName() + '>' + state()).c_str(),
				addr2s(m_addr).c_str(), addr2s(m_peer).c_str(), currentTime() - start);
		}
		else
		{
			_snprintf(buff, 255, "%4d >> %6d %-32s time: %4.4f\n", 
				m_sockNum, ++tick, msg.c_str(), currentTime() - start);
		}
		#pragma warning (pop)

#if defined (TRACE_THEM_ALL) || defined (TRACE_OTHER)
		::OutputDebugStringA(buff);
#endif
		return buff;
	//#endif
	}

	template<typename API> // is this really a good idea?
	inline SVSocket<API>::operator Socket_t() const
	{
		return m_socket;
	}

	template<typename API>
	inline void SVSocket<API>::Attach(Socket_t socket, const sockaddr_in& addr)
	{
		if (socket != m_socket)
		{
			Destroy();
			m_socket = socket;
			m_addr = addr;
		}
	}

	template<typename API>
	inline typename SVSocket<API>::Socket_t SVSocket<API>::Release()
	{
		Socket_t socket = m_socket;
		Destroy();
		return socket;
	}

	template<typename API>
	inline Err SVSocket<API>::Bind(const char* hostAddr, unsigned short portNo)
	{
		std::string msg = "bind: ";
		Err error = SVSocketError::Success;
		if (IsValidSocket())
		{
			m_peer.sin_family = AF_INET;
			m_peer.sin_addr.s_addr = inet_addr(hostAddr);
			m_peer.sin_port = htons(portNo);
			if (API::bind(m_socket, reinterpret_cast< sockaddr* >( &m_peer ), sizeof(sockaddr_in)) == SOCKET_ERROR)
			{
				error = SVSocketError::GetLastSocketError();
			}
		}
		else
		{
			error = SVSocketError::NotASocket;
		}
		Log(msg + SVSocketError::GetErrorText(error), true);
			
		return error;
	}

	template<typename API>
	inline ULONG SVSocket<API>::GetAddr() const
	{
		return m_addr.sin_addr.S_un.S_addr;
	}

	template<typename API>
	inline Err SVSocket<API>::SetNonBlocking()
	{
		SVSocketError::ErrorEnum error = SVSocketError::Success;
		int nCurFlags = 1;
		if ( ioctlsocket( m_socket, FIONBIO, reinterpret_cast< ULONG* >( &nCurFlags ) ) == SOCKET_ERROR )
		{
			error = SVSocketError::GetLastSocketError();
		}
		return error;
	}

	template<typename API>
	inline Err SVSocket<API>::SetBlocking()
	{
		SVSocketError::ErrorEnum error = SVSocketError::Success;
		int nCurFlags = 0;
		if ( ioctlsocket(m_socket, FIONBIO, reinterpret_cast< ULONG* >( &nCurFlags ) ) == SOCKET_ERROR )
		{
			error = SVSocketError::GetLastSocketError();
		}
		return error;
	}

	template <typename API>
	inline Err SVSocket<API>::GetMaxDgramSize(size_t & sz)
	{
		int optLen = sizeof(size_t);
		if ( API::getsockopt( m_socket, SOL_SOCKET, SO_MAX_MSG_SIZE, reinterpret_cast< char* >( &sz ), &optLen ) == SOCKET_ERROR )
		{
			return SVSocketError::GetLastSocketError();
		}
		else
		{
			return SVSocketError::Success;
		}
	}

	template<typename API>
	inline Err SVSocket<API>::SetKeepAliveOption(bool bEnable)
	{
		Err error = SVSocketError::Success;

		BOOL bOptVal = TRUE;
		int bOptLen = sizeof(BOOL);
		int iOptVal;
		int iOptLen = sizeof(int);

		if ( ( API::getsockopt( m_socket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast< char* >( &iOptVal ), &iOptLen ) == SOCKET_ERROR ) 
			|| ( API::setsockopt( m_socket, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast< char* >( &bOptVal ), bOptLen ) == SOCKET_ERROR ) )
		{
			error = SVSocketError::GetLastSocketError();
		}
		return error;
	}

	template<typename API>
	inline Err SVSocket<API>::SetKeepAliveValues(int interval, int timeVal)
	{
		Err error = SVSocketError::Success;
		if ( API::setKeepAliveValues( m_socket, interval, timeVal ) == SOCKET_ERROR )
		{
			error = SVSocketError::GetLastSocketError();
		}
		return error;
	}
		
	template<typename API>
	inline Err SVSocket<API>::SetBufferSize(int sz)
	{
		Err error = SVSocketError::Success;

		if ( API::setsockopt( m_socket, SOL_SOCKET, SO_SNDBUF, reinterpret_cast< void* >( &sz ), sizeof(sz) ) == SOCKET_ERROR
			|| API::setsockopt( m_socket, SOL_SOCKET, SO_RCVBUF, reinterpret_cast< void* >( &sz ), sizeof(sz) ) == SOCKET_ERROR )
		{
			error = SVSocketError::GetLastSocketError();
		}
		return error;
	}

	template<typename API>
	inline Err SVSocket<API>::SetLingerOption(bool bEnable, unsigned short nTime)
	{
		Err error = SVSocketError::Success;

		LINGER linger;
		linger.l_onoff = bEnable;
		linger.l_linger = nTime;

		if ( API::setsockopt( m_socket, SOL_SOCKET, SO_LINGER, reinterpret_cast< const char* >( &linger ), sizeof(linger) ) == SOCKET_ERROR )
		{
			error = SVSocketError::GetLastSocketError();
		}
		return error;
	}

	template<typename API>
	inline Err SVSocket<API>::SetOptionReuseAddr()
	{
		Err error = SVSocketError::Success;
		int nReuse  = TRUE;
		if ( API::setsockopt( m_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast< const char* >( &nReuse ), sizeof(int) ) == SOCKET_ERROR )
		{
			error = SVSocketError::GetLastSocketError();
		}
		return error;
	}

	template<typename API>
	inline Err SVSocket<API>::SetReadTimeout(u_int tout)
	{
		Err error = SVSocketError::Success;
			
		if ( API::setsockopt( m_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast< const char* >( &tout ), sizeof(u_int) ) == SOCKET_ERROR )
		{
			error = SVSocketError::GetLastSocketError();
		}
		return error;
	}

	template<typename API>
	inline Err SVSocket<API>::DisableDelay()
	{
		Err error = SVSocketError::Success;
		DWORD enabled = FALSE;

		if ( Traits<API>::proto == IPPROTO_TCP && 
			API::setsockopt( m_socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast< const char* >( &enabled ), sizeof(enabled) ) == SOCKET_ERROR )
		{
			error = SVSocketError::GetLastSocketError();
		}
		return error;
	}

	template<typename API>
	inline Err SVSocket<API>::EnableDelay()
	{
		Err error = SVSocketError::Success;
		DWORD enabled = TRUE;
		if ( Traits<API>::proto == IPPROTO_TCP && 
			API::setsockopt( m_socket, IPPROTO_TCP, TCP_NODELAY, static_cast< const char* >( &enabled ), sizeof(enabled) ) == SOCKET_ERROR )
		{
			error = SVSocketError::GetLastSocketError();
		}
		return error;
	}

	template<typename API>
	inline Err SVSocket<API>::Write(const std::string& data, bool hasHeader)
	{
		return Write( reinterpret_cast< const unsigned char* >( data.c_str() ), static_cast< int >( data.length() ), hasHeader);
	}

	template<>
	inline Err SVSocket<TcpApi>::Write(const u_char * data, size_t len, bool)
	{
		// Currently, we can only write 2 billion bytes at a time, but Write takes 
		// len as a size_t because we want to allow for future functionality the 
		// possibility to write larger amounts.

		// htonl needs u_long
		u_long dsz = htonl( static_cast< u_long >( len ) );

		// send needs length as an int
		int err = TcpApi::send(m_socket, reinterpret_cast< char* >( &dsz ), static_cast< int >( sizeof( u_long ) ), 0);
		if (SOCKET_ERROR != err)
		{
			err = TcpApi::send( m_socket, 
				( char* )data, // no _cast can convert from const u_char* to char*
				static_cast< int >( len ), // send needs length as an int
				0 );
		}
		return (SOCKET_ERROR == err) ? SVSocketError::TranslateError(err) : SVSocketError::Success;
	}

	template<typename API>
	char * SVSocket<API>::dupWithHeader(const u_char * src, std::vector<char> & vec, size_t len) const
	{
		static uint32 id = 0;
		vec.resize(len + sizeof(header));
		memcpy(&vec[sizeof(header)], reinterpret_cast<const char *>(src), len);
		header & head = *reinterpret_cast<header *>(&vec[0]);
		bool singlePacket = len + sizeof(header) <= m_buff_sz;
		// header uses uint32 and uint16 regardless of platform.
		head = header( ++id, 
			static_cast< uint32 >( singlePacket ? len : ( m_buff_sz - sizeof( header ) ) ), 
			static_cast< uint32 >( len ), 
			static_cast< uint16 >( singlePacket ? 1 : 1 + ( len / ( m_buff_sz - sizeof( header ) ) ) ), 
			0 );
		return vec.data();
	}

	template<typename API>
	inline Err SVSocket<API>::Write(const unsigned char* buffer, size_t len, bool hasHeader)
	{
		// Currently, we can only write 2 billion bytes at a time, but Write takes 
		// len as a size_t because we want to allow for future functionality the 
		// possibility to write larger amounts.

		Err error = SVSocketError::Success;
		if (len > 0)
		{
			std::vector<char> vec;
			char * buff = hasHeader ? dupWithHeader( buffer, vec, len ) : ( char* )buffer; // no _cast can convert from const u_char* to char*
			int l_Error;
			if (hasHeader && Traits<API>::proto == IPPROTO_UDP) // only split the payload if udp with header.
			{
				uint32 chunk_sz = static_cast<uint32>( m_buff_sz - sizeof(header) );
				header head = *reinterpret_cast<header *>(buff);
				uint32 left = static_cast< uint32 >( len );
				do
				{
					header * hdr = reinterpret_cast<header *>(buff);
					*hdr = h2n(head);
					l_Error = API::sendto( m_socket, buff, head.len + sizeof(header), 0, reinterpret_cast< sockaddr* >( &m_peer ), sizeof(sockaddr_in) );
					if (SOCKET_ERROR == l_Error)
					{
						Log("write " + head.tostr() + SVSocketError::GetErrorText(SVSocketError::GetLastSocketError()), true);
					}

					buff += chunk_sz;
					assert(left >= head.len); // sanity check
					left -= head.len;
					head.len = std::min(chunk_sz, left);
					++head.seq;
					//Sleep(1);
				}
				while((l_Error != SOCKET_ERROR) && (left != 0) );
			}
			else
			{
				u_long sz = htonl(static_cast<u_long>(len));
				l_Error = API::sendto( m_socket, reinterpret_cast<char *>(&sz), static_cast< int >( sizeof(u_long) ), 0, reinterpret_cast< sockaddr* >( &m_peer ), sizeof(sockaddr_in) );
				if (SOCKET_ERROR != l_Error)
				{
					l_Error = API::sendto( m_socket, buff, static_cast< int >( len ), 0, reinterpret_cast< sockaddr* >( &m_peer ), sizeof(sockaddr_in) );
				}
			}
			if ( l_Error == SOCKET_ERROR)
			{
				error = SVSocketError::GetLastSocketError();
			}
		}
		return error;
	}

	template<typename API>
	inline Err SVSocket<API>::Read(unsigned char* buffer, size_t len, size_t& amtRead, bool hasHeader)
	{
		// Currently, we can only read 2 billion bytes at a time, but Write takes 
		// len as a size_t because we want to allow for future functionality the 
		// possibility to read larger amounts.

		Err error = SVSocketError::Success;
		amtRead = 0;
		// get available data
		int amt = API::recv( m_socket,
			( char* )buffer, // no _cast can convert from const u_char* to char*
			static_cast< int >( len ), // send needs length as an int
			0 );
		if (amt < 0)
		{
			error = SVSocketError::GetLastSocketError();
		}
		else
		{
			amtRead = amt;
		}
		return error;
	}

	template<typename API>
	inline Err SVSocket<API>::ReadFrom(unsigned char * buffer, size_t len, size_t & amtRead, 
		sockaddr_in & from, size_t & fromLen, bool hasHeader)
	{
		// Currently, we can only read 2 billion bytes at a time, but Write takes 
		// len as a size_t because we want to allow for future functionality the 
		// possibility to read larger amounts.

		Err error = SvSol::SVSocketError::Success;
		amtRead = 0;
		int frLen = static_cast<int>(fromLen);
		// get available data
		int amt = API::recvfrom(m_socket, reinterpret_cast< char* >( buffer ), static_cast< int >( len ), 0, reinterpret_cast< sockaddr* >( &from ), &frLen);
		if (amt < 0)
		{
			error = SvSol::SVSocketError::GetLastSocketError();

			if ( error == SvSol::Err::ConnectionTimeout )
			{
				Log("ConnectionTimeout in ReadFrom", true);
			}
			else
			{
				Log(SvSol::SVSocketError::GetErrorText(error), true);
			}
		}
		else
		{
			amtRead = amt;
			fromLen = frLen;
		}
		return error;
	}

	template<typename FUN>
	struct scope_cleanup
	{
		FUN fun;
		scope_cleanup(const FUN & f): fun(f) {}
		~scope_cleanup() { fun(); }
	};

	template<typename API>
	inline Err SVSocket<API>::ReadAll(bytes & dest, u_long & sz, bool hasHeader)
	{
		const u_int rdTmout = 200;
		sz = 0;
		if (hasHeader)
		{
			header head;
			size_t len = sizeof(header);
			size_t dummy = sizeof(sockaddr_in);
			std::vector<u_char> tmp;
			tmp.resize( m_buff_sz );
			Err result = SetReadTimeout(rdTmout);
			boost::function<Err ()> tout = boost::bind(&SVSocket<API>::SetReadTimeout, this, 0);
			scope_cleanup<boost::function<Err ()> > sc(tout); // make sure we reset the timeout when exiting
			result = PeekFrom(&tmp[0], m_buff_sz, len, m_peer, dummy); // peek at the header
			if (Err::Success == result)
			{
				head = n2h(*reinterpret_cast<header *>(&tmp[0]));
				dest.reset(new BYTE[head.tot]);
				sz = head.tot;
				u_char * buf = dest.get();
				u_int idx = 0;
				u_int cnt = head.cnt;
				u_int id = head.id;
				if (Err::Success == result)
				{
					for (; idx < cnt;)
					{
						result = ReadFrom(&tmp[0], m_buff_sz, len, m_peer, dummy);
						u_int chnk_sz = static_cast<u_int>(m_buff_sz - sizeof(header));
						if (Err::Success == result && len >= sizeof(header))
						{
							head = n2h(*reinterpret_cast<header *>(&tmp[0]));
							// sanity check
							if (id != head.id)
							{
								char stray[80];
								memset(stray, 0, 80);
								sprintf(stray, "id mismatch: %d ~ %d\n", id, head.id);
								Log(stray, true); // should we log it?
							}
							else
							{
								memcpy(buf + chnk_sz * head.seq, &tmp[0] + sizeof(header), head.len);
								++idx;
							}
						}
						else
						{
							if ( result == Err::ConnectionTimeout )
							{
								Log("ConnectionTimeout", true);
							}

							return ( Err::Success != result ) ? result : Err::NoData;
						}
					}
				}
				else
				{
					Log("Unable to set timeout", true);
				}
			}
			return result;
		}
		else
		{
			return Err::ProtocolNotSupported;
		}
	}

	template<>
	inline Err SVSocket<TcpApi>::ReadAll(bytes & dest, u_long & sz, bool)
	{
		u_long len = 0;
		const u_int rdTmout = 500;
		Err result = SetReadTimeout(rdTmout);
		boost::function<Err ()> tout = boost::bind(&SVSocket<TcpApi>::SetReadTimeout, this, 0);
		scope_cleanup<boost::function<Err ()> > sc(tout); // make sure we reset the timeout when exiting
		int amt = TcpApi::recv(m_socket, reinterpret_cast< char* >( &len ), sizeof(u_long), 0);
		if (0 == amt)
		{
			return SVSocketError::ConnectionReset;
		}
		if (amt < 0)
		{
			return SVSocketError::GetLastSocketError();
		}
		if (amt > 0)
		{
			len = sz = ntohl(len);
			if (len > 0)
			{
				dest.reset(new BYTE[len]);
				BYTE * buff = dest.get();
				do
				{
					result = SetReadTimeout(rdTmout*5);
					amt = TcpApi::recv(m_socket, reinterpret_cast< char* >( buff ), len, 0);
					if (amt < 0)
					{
						return SVSocketError::GetLastSocketError();
					}
					if (amt > 0)
					{
						buff += amt;
						len -= amt;
					}
				} while (len > 0);
			}
		}
		return SVSocketError::Success;
	}

	template<typename API>
	inline Err SVSocket<API>::PeekFrom(unsigned char * buffer, size_t len, size_t & amtRead, 
		sockaddr_in & from, size_t & fromLen, bool hasHeader = false)
	{
		// Currently, we can only peek 2 billion bytes at a time, but Write takes 
		// len as a size_t because we want to allow for future functionality the 
		// possibility to peek larger amounts.

		Err error = SVSocketError::Success;
		amtRead = 0; // Initialize to 0 in case of an error.

		// need an int to use with recvfrom
		int frLen = static_cast< int >( fromLen );
		// get available data;
		int amt = API::recvfrom(m_socket, reinterpret_cast< char* >( buffer ), static_cast< int >( len ), MSG_PEEK, reinterpret_cast< sockaddr* >( &from ), &frLen);

		if (amt < 0)
		{
			error = SVSocketError::GetLastSocketError();
		}
		else
		{
			amtRead = amt;
			fromLen = frLen;
		}
		return error;
	}

	template<typename API>
	inline Err SVSocket<API>::WriteTo(const unsigned char * buffer, size_t len, 
		const sockaddr_in & to, size_t toLen, bool hasHeader = false)
	{
		// Currently, we can only write 2 billion bytes at a time, but Write takes 
		// len as a size_t because we want to allow for future functionality the 
		// possibility to write larger amounts.

		Err error = SVSocketError::Success;

		if (len > 0)
		{
			int l_Error = API::sendto(m_socket, reinterpret_cast< const char* >( buffer ), static_cast< int >( len ), 0, reinterpret_cast< sockaddr* >( &to ), static_cast< int >( toLen ) );
			if ( l_Error == SOCKET_ERROR )
			{
				error = SVSocketError::GetLastSocketError();
			}
		}
		return error;
	}

	template<typename API>
	inline Err SVSocket<API>::Select(int & nfds, FdSet_t * readfds, FdSet_t * writefds, FdSet_t * exceptfds, const timeval *timeout)
	{
		if ((nfds = API::select(nfds, readfds, writefds, exceptfds, timeout)) == SOCKET_ERROR)
		{
			return SVSocketError::GetLastSocketError();
		}
		else
		{
			return SVSocketError::Success;
		}
	}

	template<typename API>
	inline SVSocketError::ErrorEnum SVSocket<API>::Send( const std::string& data )
	{
		SVSocketError::ErrorEnum retVal = SVSocketError::Success;
		int status = API::send( m_socket, data.c_str(), static_cast< int >( data.size() ), 0 );
		if ( status == SOCKET_ERROR )
		{
			retVal = SVSocketError::GetLastSocketError();
		}
		return retVal;
	}
} /*namespace SVSocketLibrary*/ } /*namespace Seidenader*/

#pragma warning(pop)

