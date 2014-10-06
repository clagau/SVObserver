//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVJsonStreamPacket
//* .File Name       : $Workfile:   SVJsonStreamPacket.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   02 Oct 2014 10:40:48  $
//******************************************************************************
#pragma once
#include <string>
#include <sstream>

class SVJsonStreamPacket
{
private:
	std::stringstream m_packet;
	std::string m_pending;
	int m_count;

public:
	SVJsonStreamPacket();
	void Add(const std::string& data);
	bool IsComplete() const;
	bool IsValid() const;
	std::istream& GetJsonDataStream();
	void Clear();
	void Reset();
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSocketLibrary\SVJsonStreamPacket.h_v  $
 * 
 *    Rev 1.0   02 Oct 2014 10:40:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  949
 * SCR Title:  Repeatedly Calling PutConfig Causes SVObserver to Stop Responding
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
