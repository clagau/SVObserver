//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   stdafx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   03 Oct 2013 14:43:26  $
//******************************************************************************

#pragma once

#pragma region Precompiled Headers
#include "targetver.h"
#include <atlenc.h>
#include <algorithm>
#include <comutil.h>		//Used by static library
#include <conio.h>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <list>
#include <map>
#include <process.h>
#include <set>
#include <signal.h>
#include <string>
#include <sstream>
#include <Psapi.h>
#include <stdio.h>
#include <set>				//Used by static library
#include <sstream>			//Used by static library
#include <string>
#include <tchar.h>
#include <tlhelp32.h>
#include <vector>
#include <utility>
#include <vector>
#include <Windows.h>
#include <boost/function.hpp>			//Used by static library

//avoid warning: 'identifier': decorated name length exceeded, name was truncated
//@Todo[MEC][8.00] [22.12.2017] remove this warning https://msdn.microsoft.com/de-de/library/074af4b6.aspx
#pragma warning( disable: 4503 ) 
#pragma endregion Precompiled Headers
