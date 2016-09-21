//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once
#define NOMINMAX
#include "targetver.h"

#define VC_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define _CRT_SECURE_NO_WARNINGS
#include <afx.h>
#include <algorithm>
#include <assert.h>
#include <comdef.h>
#include <concrt.h>
#include <ctime>
#include <deque>
#include <map>
#include <ppl.h>
#include <set>
#include <string>

#include <sequential_tree.h>
#include <tchar.h>
#include <vector>
#include <boost/assign/list_of.hpp>
#include <boost/circular_buffer.hpp>
#include "SVOResource\resource.h"