//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file stdafx.h
/// All Rights Reserved
//******************************************************************************
/// include file for standard system include files,
/// or project specific include files that are used frequently, but
/// are changed infrequently
//******************************************************************************

#pragma once

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#include <chrono>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <stdio.h>
#include <string>
#include <tchar.h>

#include <boost/array.hpp>
#pragma warning(push)
#pragma warning(disable : 4702)
#include <boost/asio.hpp>
#pragma warning(pop)
