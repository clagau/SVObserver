//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonPacket
//* .File Name       : $Workfile:   SVJsonPacket.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Jun 2014 15:48:04  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVJsonPacket.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSocketLibrary
{
	SVJsonPacket::SVJsonPacket()
	: m_count(0)
	{
	}

	void SVJsonPacket::Add(const std::string& data)
	{
		// match opening/closing braces
		for (std::string::const_iterator it = data.begin();it != data.end();++it)
		{
			if ((*it) == '{')
			{
				m_count++;
				m_packet += (*it);
			}
			else if ((*it) == '}')
			{
				m_count--;
				m_packet += (*it);
			}
			else if ((*it) != '\n')
			{
				m_packet += (*it);
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

	bool SVJsonPacket::IsComplete() const
	{
		bool bComplete = (m_count == 0 && m_packet.length() > 0) ? true : false;
		return bComplete;
	}

	bool SVJsonPacket::IsValid() const
	{
		bool bRetVal = true;
		// run it through a lexer...

		return bRetVal;
	}

	const std::string& SVJsonPacket::GetJsonData() const
	{
		return m_packet;
	}

	void SVJsonPacket::Clear()
	{
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
					m_packet += (*it);
				}
				else if ((*it) == '}')
				{
					m_count--;
					m_packet += (*it);
				}
				else if ((*it) != '\n')
				{
					m_packet += (*it);
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

	void SVJsonPacket::Reset()
	{
		m_packet.clear();
		m_pending.clear();
		m_count = 0;
	}

} /*namespace SVSocketLibrary*/ } /*namespace Seidenader*/
