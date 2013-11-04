//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonPacket
//* .File Name       : $Workfile:   SVJsonPacket.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:14:38  $
//******************************************************************************
#include "stdafx.h"
#include "SVJsonPacket.h"

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

	//m_packet += data;
}

bool SVJsonPacket::IsComplete() const
{
	bool bComplete = (m_count == 0 && m_packet.length() > 0) ? true : false;
	return bComplete;
/*
	int count = 0;
	// match opening/closing braces
	for (std::string::const_iterator it = m_packet.begin();it != m_packet.end();++it)
	{
		if ((*it) == '{')
		{
			count++;
		}
		else if ((*it) == '}')
		{
			count--;
		}
	}
	return (count == 0);
*/
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSocketLibrary\SVJsonPacket.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:14:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 Mar 2012 11:34:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added reset method to JSON Packet class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Feb 2012 14:20:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed issues with partial packets and transfer issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Dec 2011 13:54:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
