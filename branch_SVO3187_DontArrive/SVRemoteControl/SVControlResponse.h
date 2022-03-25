//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVControlCommands
//* .File Name       : $Workfile:   SVControlResponse.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   16 Oct 2014 09:58:30  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <map>
#include <string>
#include <comdef.h>
#include "SVCommandStatus.h"
#pragma endregion Includes

struct SVControlResponse
{
	typedef std::map<std::string, CComVariant> ResultValues;

	SVControlResponse() : cmd_id( 0 ), cmd_name( "" ), results(), m_Status() {}
	SVControlResponse(const SVControlResponse& rhs) : cmd_id(rhs.cmd_id), cmd_name(rhs.cmd_name), results(rhs.results), m_Status(rhs.m_Status) {}
	~SVControlResponse() 
	{ 
		results.clear(); 
		cmd_name.clear(); 
	}
	SVControlResponse& operator=(const SVControlResponse& rhs)
	{
		if (this != &rhs)
		{
			cmd_id = rhs.cmd_id;
			cmd_name = rhs.cmd_name;
			results = rhs.results;
			m_Status = rhs.m_Status;
		}
		return *this;
	}

	int cmd_id;
	std::string cmd_name;
	ResultValues results;
	SVCommandStatus m_Status;
};
