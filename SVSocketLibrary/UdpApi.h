//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : UdpApi
//* .File Name       : $Workfile:   UdpApi.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   27 Oct 2014 09:39:02  $
//******************************************************************************

#pragma once

#include "TcpApi.h"

namespace Seidenader
{
	namespace Socket
	{
		typedef u_int uint32;
		typedef u_short uint16;

		class UdpApi : public TcpApi
		{
		public:
		   static int connect(Socket_t u, const sockaddr* name, int namelen) { return 0; }
		   static int listen(Socket_t u, int backlog) { return 0; }
		   static int shutdown(Socket_t u, int flags) { return 0; }
		   static int setKeepAliveValues(Socket_t u, int interval, int timeVal) { return 0; }
		};

		enum ctrl 
		{ 
			dscnct = 0, // disconnect request
			ack = 1, // acknowledged
			nack = 2, // not acknowledged
			maxsz = 3, // for max buffer size negotiation
			cmd = 4, // the packet contains a json command
			end = 15 // highest id value for control messages
		};

		struct header
		{
			uint32 id; // message id, normally higher than ctrl::end. id values from ctrl serve as control messages
			uint32 len; // packet payload length (datagram size - header size)
			uint32 tot; // total message payload size
			uint16 cnt; // number of datagrams in the message
			uint16 seq; // sequence number (0-based) of a datatagram in a message
			header(uint32 i, uint32 l, uint32 t, uint16 c, uint16 s): id(i), len(l), tot(t), cnt(c), seq(s) {}
			header() {};
			std::string tostr() const
			{
				char buf[128];

				#pragma warning (push)
				#pragma warning (disable: 4996)
				sprintf(buf, "{ id: %d, len: %d, tot: %d, cnt: %d, seq: %d } ", id, len, tot, cnt, seq);
				#pragma warning (pop)

				return buf;
			}
		};

		template<typename T>
		T h2n(const T & t);

		template<typename T>
		T n2h(const T & t);

		template<>
		inline header n2h(const header & h)
		{
			return header(ntohl(h.id), ntohl(h.len), ntohl(h.tot), ntohs(h.cnt), ntohs(h.seq));
		}

		template<>
		inline header h2n(const header & h)
		{
			return header(htonl(h.id), htonl(h.len), htonl(h.tot), htons(h.cnt), htons(h.seq));
		}
	}
}

typedef Seidenader::Socket::UdpApi UdpApi;

