//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonStreamPacket
//* .File Name       : $Workfile:   SVJsonStreamPacket.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   02 Oct 2014 10:40:48  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVJsonStreamPacket.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSocketLibrary
{
	SVJsonStreamPacket::SVJsonStreamPacket()
	: m_count(0)
	{
	}

	void SVJsonStreamPacket::Add(const std::string& data)
	{
		// match opening/closing braces
		for (std::string::const_iterator it = data.begin();it != data.end();++it)
		{
			if ((*it) == '{')
			{
				m_count++;
				m_packet << (*it);
			}
			else if ((*it) == '}')
			{
				m_count--;
				m_packet << (*it);
			}
			else if ((*it) != '\n')
			{
				m_packet << (*it);
			}
			if (m_count == 0)
			{
				// put the rest into the overflow bucket
				++it;
				if (it != data.end())
				{
					m_pending = data.substr(std::distance(data.begin(), it));
					if (m_pending.length() == 1 && m_pending[0] == '\n')
					{
						m_pending.clear();
					}
				}
				break;
			}
		}
	}

	bool SVJsonStreamPacket::IsComplete() const
	{
		int length = static_cast<int>(const_cast<std::stringstream *>(&m_packet)->tellp()); // why isn't tellp const?
		bool bComplete = (m_count == 0 &&  length > 0) ? true : false;
		return bComplete;
	}

	bool SVJsonStreamPacket::IsValid() const
	{
		bool bRetVal = true;
		// run it through a lexer...

		return bRetVal;
	}

	std::istream& SVJsonStreamPacket::GetJsonDataStream()
	{
		m_packet.clear(); // clear error/eof bits
		m_packet.seekg(0, std::ios_base::beg); // position at beginning for reading
		return m_packet;
	}

	void SVJsonStreamPacket::Clear()
	{
		m_packet.str(std::string());
		m_packet.clear();
	
		// copy from overflow buffer
		if (m_pending.length() > 0)
		{
			// match opening/closing braces
			for (std::string::iterator it = m_pending.begin();it != m_pending.end();++it)
			{
				if ((*it) == '{')
				{
					m_count++;
					m_packet << (*it);
				}
				else if ((*it) == '}')
				{
					m_count--;
					m_packet << (*it);
				}
				else if ((*it) != '\n')
				{
					m_packet << (*it);
				}
				if (m_count == 0)
				{
					if (it != m_pending.begin())
					{
						++it;
						// erase what was moved
						m_pending.erase(m_pending.begin(), it);
						break;
					}
				}
			}
		}
	}

	void SVJsonStreamPacket::Reset()
	{
		m_packet.str(std::string());
		m_packet.clear();
		m_pending.clear();
		m_count = 0;
	}

} /*namespace SVSocketLibrary*/ } /*namespace Seidenader*/
