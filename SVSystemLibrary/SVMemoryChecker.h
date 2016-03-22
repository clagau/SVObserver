//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMemoryChecker
//* .File Name       : $Workfile:   SVMemoryChecker.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:52:30  $
//******************************************************************************

#pragma once

#pragma comment( lib, "Psapi" )

class SVMemoryChecker
{
public:
	static size_t GetFreeProcessMemorySize();

	virtual ~SVMemoryChecker();

private:
	SVMemoryChecker();

};


